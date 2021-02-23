#include "ai_escaper.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <set>
#include <thread>
#include <condition_variable>

static const std::vector<EntityDirection> directions = { EntityDirection::UP
                                                       , EntityDirection::DOWN
                                                       , EntityDirection::RIGHT
                                                       , EntityDirection::LEFT};

template<typename T>
bool is_in(T value, std::vector<T> vct) {
    for (auto&& t : vct) {
        if (t == value) {
            return  true;
        }
    }
    return false;
}

void AIEscaper::update_loop() {
    for(;;) {
        std::unique_lock<std::mutex> l(cond_m);
        cond.wait(l);
        if (is_running && !new_pos_calculated) {
            BFS();
        } else if (!is_running) {
            break;
        }
    }
}

struct SGDTuple {
    GameMapLogic map;
    int score;
    int depth;
    EntityCoords pos;
    EntityDirection dir;
    int predecessor_index;
    SGDTuple(GameMapLogic map
            , int score
            , int depth
            , const EntityCoords& pos
            , EntityDirection dir
            , int predecessor_index)
            : map(map)
            , score(score)
            , depth(depth)
            , pos(pos)
            , dir(dir)
            , predecessor_index(predecessor_index) {}
    bool operator < (const SGDTuple& right) const {
        //std::cout << "Called comparator left.score : " << score << "; right.score : " << right.score << std::endl;
        return score < right.score;
    }
};

#define DEPTH 15
#define THREADS 4
#define PENALTY_CHANGE -5
#define REWARD_KEEP_DIRECTION 5
static const std::vector<EntityDirection> possible_moves = {
    EntityDirection::UP,
    EntityDirection::DOWN,
    EntityDirection::LEFT,
    EntityDirection::RIGHT
};

static void thread_BFS( std::priority_queue<SGDTuple>& q
                       , std::vector<SGDTuple>& predecessors
                       , std::set<EntityCoords>& explored
                       , std::mutex& q_m
                       , std::mutex& predecessors_m
                       , std::mutex& explored_m
                       , std::mutex& solution_m
                       , std::condition_variable& found_solution
                       , std::atomic<int>& index_of_solution
                       , float move_factor
                       , std::atomic<bool>& terminate
                       , std::atomic<bool>& is_running
                       , std::atomic<int>& running_threads
                       , PlayerEntity* this_ai) {
    sf::Clock clock;
    SGDTuple last(GameMapLogic(), 0, 0, EntityCoords(), EntityDirection::STATIC, -1);
    int index = -1;
    int last_index = -1;
    while (is_running && !terminate) {
        {
            std::unique_lock<std::mutex> l(solution_m);
            // the default of index_of_solution is -1
            // index_of_solution is set if a solution is found
            // then we want to exit from all of the threads
            if (index_of_solution >= 0) {
                break;
            }
            // we prun all the computations that exceed the limit
            // of 0.1f per step
            if (clock.getElapsedTime().asSeconds() >= 0.09f) {
                std::cout << "AIEscaper : couldn't find the right solution, just prunning!" << std::endl;
                index_of_solution = (last_index >= 0) ? last_index : 0;
                break;
            }
        }
        {
            std::unique_lock<std::mutex> l(q_m);
            // to enable greater parellelism we wait if there isn't anything
            // in the queue and it hasn't passed more than a half of the described
            // computation time
            if (q.empty()) {
                if (clock.getElapsedTime().asSeconds() >= 0.05f) {
                    break;
                } else {
                    continue;
                }
            }
            last = std::move(const_cast<SGDTuple&>(q.top()));
            q.pop();
        }
        {
            std::unique_lock<std::mutex> predecessors_lock(predecessors_m);
            predecessors.push_back(last);
            last_index = index;
            index = predecessors.size() - 1;
        }
        // if ai dies in the move, we don't consider the move at all
        if (last.map.check_damage(last.pos)) {
            continue;
        }
        if (last.depth == DEPTH) {
            std::unique_lock<std::mutex> l(solution_m);
            index_of_solution = index;
            break;
        }
        last.map.update(0.1f);
        for (auto&& direction : possible_moves) {
            auto position = last.pos;
            go(position, direction, move_factor);
            std::unique_lock<std::mutex> explored_lock(explored_m);
            // if (explored.find(position) != explored.end()) {
            //     continue;
            // }
            if (last.map.collision_checking(move_factor, position, direction) == Collision::NONE) {
                explored.emplace(position);
                explored_lock.unlock();
                float penalty = 0;
                if (last.depth == 0) {
                    if (static_cast<int>(last.dir) == static_cast<int>(direction)) {
                        penalty = REWARD_KEEP_DIRECTION;
                    }
                }
                if (static_cast<int>(opposite(last.dir)) == static_cast<int>(direction)){
                    penalty = PENALTY_CHANGE;
                } 
                {
                    std::unique_lock<std::mutex> l(q_m);
                    q.emplace(SGDTuple(last.map, last.score + penalty, last.depth + 1, position, direction, index));
                }
            }
        }
    }
    std::unique_lock<std::mutex> l(solution_m);
    if (running_threads != 1) {
        running_threads--;
    } else {
        found_solution.notify_all();
    }
}

void AIEscaper::BFS() {
    std::priority_queue<SGDTuple> q;
    std::mutex predecessors_m, explored_m, q_m;
    std::atomic<int> index_of_solution = -1, running_threads = THREADS;
    std::vector<SGDTuple> predecessors;
    std::set<EntityCoords> explored;
    std::atomic<bool> terminate = false;
    workers.clear();
    float mf = 0.f;
    {
        std::unique_lock<std::mutex> l(resources_m);
        q.emplace(map, 0, 0, actual_pos, direction, -1);
        mf = move_factor;
        for (int i = 0; i < THREADS; i++) {
            workers.emplace_back(thread_BFS, std::ref(q)
                                        , std::ref(predecessors)
                                        , std::ref(explored)
                                        , std::ref(q_m)
                                        , std::ref(predecessors_m)
                                        , std::ref(explored_m)
                                        , std::ref(solution_m)
                                        , std::ref(solution_found)
                                        , std::ref(index_of_solution)
                                        , move_factor
                                        , std::ref(terminate)
                                        , std::ref(is_running)
                                        , std::ref(running_threads)
                                        , this);
        }
    }
    {
        std::unique_lock<std::mutex> l(solution_m);
        using namespace std::chrono_literals;
        auto status = solution_found.wait_for(l, 200ms);
        terminate = true;
        if (status == std::cv_status::timeout) {
            std::cout << "AIEscaper : solution not found, time out on condition variable!" << std::endl;
        }
        if (index_of_solution == -1) {
            // all the workers should have finished by now
            index_of_solution = 0;
            std::cout << "AIEscaper : error - solution not found!" << std::endl;
        }
    }
    {
        std::unique_lock<std::mutex> l(resources_m);
        for (auto&& thread : workers) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    if (index_of_solution >= predecessors.size()) {
        std::cout << "AIEscaper : error - couldn't calculate solution because of wrong index!" << std::endl;
        return;
    }
    auto&& last = predecessors.at(index_of_solution);
    while (last.predecessor_index > 0) {
        last = predecessors.at(last.predecessor_index);
    }
    next_move = last.dir;
    new_pos_calculated = true;
}

#define BOMB_PLACE_TIME 1.4f
void AIEscaper::notify_new_bomb(const IDPos& idp) {
    std::unique_lock<std::mutex> l(resources_m);
    map.place_bomb(idp.second, BOMB_PLACE_TIME);
}

void AIEscaper::notify_sb_destroyed(int i) {
    std::unique_lock<std::mutex> l(resources_m);
    map.erase_soft_block(i);
}

/* Updates the time and map of AIEscaper
 * and checks if the next move was already
 * calculated. If yes, then the entity will move
 * in that direction.
 */
void AIEscaper::update(float dt) {
    ServerPlayerEntity::update(dt);
    {
        std::unique_lock<std::mutex> l(resources_m);
        map.update(dt);
        if (new_pos_calculated) {
            if (next_move != EntityDirection::STATIC) {
                direction = next_move;
                go(actual_pos, next_move, move_factor);
                map.collision_checking(move_factor, actual_pos, direction);
            } else {
                std::cout << "SERVER : ai wants to place a bomb at: " << actual_pos.first << "," << actual_pos.second << std::endl;
            }
            updated = true;
            new_pos_calculated = false;
        }
    }
    {
        std::unique_lock<std::mutex> l(cond_m);
        cond.notify_all();
    }
}

/* Sets the position of the entity to
 * the respawn location, in thread safe manner
 */
void AIEscaper::respawn() {
    std::unique_lock<std::mutex> l(resources_m);
    ServerPlayerEntity::respawn();
}

/* Updates the position of the entity in thread safe
 * manner
 */
void AIEscaper::update_pos_dir(EntityCoords&& c, EntityDirection d) {
    std::unique_lock<std::mutex> l(resources_m);
    ServerPlayerEntity::update_pos_dir(std::move(c), d);
}

/* Applies power up to the entity in thread safe
 * manner
 */
void AIEscaper::apply_power_up(PowerUpType p, const sf::Time& t) {
    std::unique_lock<std::mutex> l(resources_m);
    ServerPlayerEntity::apply_power_up(p, t);
}

AIEscaper::~AIEscaper() {
    terminate();
    if (updater.joinable()) {
        updater.join();
        std::cout << "AIEscaper : joined updater" << std::endl;
    } else {
        std::cout << "AIEscaper : already joined" << std::endl;
    }
}

/* Sets on the variables controlling
 * the update_loop and next move calculations
 */
void AIEscaper::terminate() {
    std::unique_lock<std::mutex> run_l(cond_m);
    if (!is_running) { return; }
    // update_loop goes until is_running = true
    is_running = false;

    // update_loop waits on cond to be notified if
    // the next move needs to be computed
    cond.notify_all();
}
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
        std::unique_lock<std::mutex> l(cond_mutex);
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
        return score < right.score;
    }
};

#define DEPTH 20
#define THREADS 3
#define PENALTY_MOVE (-10)
#define REWARD_KEEP_DIRECTION (+30)
static const std::vector<EntityDirection> possible_moves = {
    EntityDirection::UP,
    EntityDirection::DOWN,
    EntityDirection::LEFT,
    EntityDirection::RIGHT
};

static void thread_BFS( std::priority_queue<SGDTuple>& q
                       , std::vector<SGDTuple>& predecessors
                       , std::set<EntityCoords>& explored
                       , std::set<EntityCoords>& placed_bombs
                       , std::mutex& q_m
                       , std::mutex& predecessors_m
                       , std::mutex& explored_m
                       , std::mutex& placed_bombs_m
                       , std::mutex& solution_m
                       , std::condition_variable& found_solution
                       , std::atomic<int>& index_of_solution
                       , float move_factor
                       , std::atomic<bool>& terminate
                       , std::atomic<int>& running_threads) {
    sf::Clock clock;
    SGDTuple last(GameMapLogic(), 0, 0, EntityCoords(), EntityDirection::STATIC, -1);
    int index = -1;
    int last_index = -1;
    while (!terminate) {
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
            if (explored.find(position) != explored.end()) {
                continue;
            }
            if (last.map.collision_checking(move_factor, position, direction) == Collision::NONE) {
                explored.emplace(position);
                explored_lock.unlock();
                float penalty = PENALTY_MOVE;
                if (static_cast<int>(last.dir) == static_cast<int>(direction)) {
                    penalty = REWARD_KEEP_DIRECTION;
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
    std::mutex predecessors_m, explored_m, placed_bombs_m, q_m, solution_m;
    std::condition_variable found_solution;
    std::atomic<int> index_of_solution = -1, running_threads = THREADS;
    std::vector<SGDTuple> predecessors;
    std::set<EntityCoords> explored, placed_bombs;
    std::vector<std::thread> finding_threads;
    std::atomic<bool> terminate = false;
    float mf = 0.f;
    {
        std::unique_lock<std::mutex> l(resources_mutex);
        q.emplace(map, 0, 0, actual_pos, direction, -1);
        mf = move_factor;
    }
    for (int i = 0; i < THREADS; i++) {
        finding_threads.emplace_back(thread_BFS, std::ref(q)
                                               , std::ref(predecessors)
                                               , std::ref(explored)
                                               , std::ref(placed_bombs)
                                               , std::ref(q_m)
                                               , std::ref(predecessors_m)
                                               , std::ref(explored_m)
                                               , std::ref(placed_bombs_m)
                                               , std::ref(solution_m)
                                               , std::ref(found_solution)
                                               , std::ref(index_of_solution)
                                               , move_factor
                                               , std::ref(terminate)
                                               , std::ref(running_threads));
    }
    {
        std::unique_lock<std::mutex> l(solution_m);
        found_solution.wait(l);
        terminate = true;
        if (index_of_solution == -1) {
            index_of_solution = 0; // don't do anything, 0 is the index of actual position
            std::cout << "Solution not found!" << std::endl;
        }
    }
    for (auto&& thread : finding_threads) {
        thread.join();
    }
    auto&& last = predecessors.at(index_of_solution);
    while (last.predecessor_index > 0) {
        last = predecessors.at(last.predecessor_index);
    }
    next_move = last.dir;
    new_pos_calculated = true;
}

void AIEscaper::notify_new_bomb(const IDPos& idp) {
    std::unique_lock<std::mutex> l(resources_mutex);
    map.place_bomb(idp.second);
}

void AIEscaper::notify_sb_destroyed(int i) {
    std::unique_lock<std::mutex> l(resources_mutex);
    map.erase_soft_block(i);
}

void AIEscaper::update(float dt) {
    ServerPlayerEntity::update(dt);
    {
        std::unique_lock<std::mutex> l(resources_mutex);
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
        std::unique_lock<std::mutex> l(cond_mutex);
        cond.notify_all();
    }
}

void AIEscaper::respawn() {
    std::unique_lock<std::mutex> l(resources_mutex);
    ServerPlayerEntity::respawn();
}

void AIEscaper::update_pos_dir(EntityCoords&& c, EntityDirection d) {
    std::unique_lock<std::mutex> l(resources_mutex);
    ServerPlayerEntity::update_pos_dir(std::move(c), d);
}
void AIEscaper::apply_power_up(PowerUpType p, const sf::Time& t) {
    std::unique_lock<std::mutex> l(resources_mutex);
    ServerPlayerEntity::apply_power_up(p, t);
}

AIEscaper::~AIEscaper() {
    is_running = false;
    std::unique_lock<std::mutex> l(resources_mutex);
    cond.notify_all();
}
#include <algorithm>
#include <sstream>
#include <string>

bool is_valid_ip(const std::string& str) {
    size_t cnt = std::count(str.begin(), str.end(), '.');
    if (cnt == 3) {
        std::string quad;
        std::stringstream ss(str);
        cnt = 0;
        while(std::getline(ss,quad, '.')) {
            if ((quad.size() == 0) || (std::stoi(quad) >= 256)) {
                return false;
            }
            cnt++;
        }
        return cnt == 4;
    }
    return false;
}

bool is_valid_port(const std::string& str) {
    if (str.size() == 0) { return false; }
    for (auto&& c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    auto i = std::stoi(str);
    return (i == 0) || (i > 1024);
}
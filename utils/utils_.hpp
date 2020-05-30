//
// Created by eleonora on 27/05/20.
//

#include <sstream>

std::vector<int16_t> split(const std::string &str, const std::string &delim) {
    std::vector<int16_t> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(-(std::stoi(token)));
        prev = pos + delim.size() ;
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

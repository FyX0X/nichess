//
// Created by martin on 27/08/2026.
//

#ifndef NICHESS_STRING_UTILS_H
#define NICHESS_STRING_UTILS_H
#include <algorithm>
#include <string>
#include <vector>


namespace utility::strings {

    std::vector<std::string_view> Tokenize(std::string_view input, char separator);

    constexpr char ToLowerCase(char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
    constexpr char ToUpperCase(char c) { return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); }

    constexpr std::string ToLowerCase(std::string_view str) {
        std::string result(str);
        std::transform(str.begin(), str.end(), result.begin(), ::tolower);
        return result;
    }
    constexpr std::string ToUpperCase(std::string_view str) {
        std::string result(str);
        std::transform(str.begin(), str.end(), result.begin(), ::toupper);
        return result;
    }



}

#endif //NICHESS_STRING_UTILS_H

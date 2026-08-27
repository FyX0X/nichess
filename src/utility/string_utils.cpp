//
// Created by martin on 27/08/2026.
//

#include "string_utils.h"

namespace utility::strings {
    /**
     * Tokenizes a string.
     *
     * @param input a view of the string to tokenize
     * @param separator the character delimiting the tokens
     * @return a vector of string_views pointing into the original string_view.
     */
    std::vector<std::string_view> Tokenize(std::string_view input, char separator) {
        // split each sections into tokens
        size_t last = 0;
        size_t next = 0;
        std::vector<std::string_view> tokens;
        while ((next = input.find(separator, last)) != std::string::npos) {
            tokens.push_back(input.substr(last, next - last));
            last = next + 1;
        }
        tokens.push_back(input.substr(last));

        return tokens;
    }

}
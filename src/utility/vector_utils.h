//
// Created by martin on 30/08/2026.
//

#ifndef NICHESS_VECTOR_UTILS_H
#define NICHESS_VECTOR_UTILS_H
#include <algorithm>
#include <vector>
#include <random>
#include <iterator>


namespace utility::vectors {

    template <typename T>
    constexpr bool Contains(const std::vector<T>& vector, const T& element) {
        return std::find(vector.begin(), vector.end(), element) != vector.end();
    }

    // Source - https://stackoverflow.com/a/16421677
    // Posted by Christopher Smith, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-08-30, License - CC BY-SA 3.0

    template <typename T>
    const T& RandomChoice(const std::vector<T>& vec) {
        static std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<std::size_t> dist(0, vec.size() - 1);
        return vec[dist(rng)];
    }




} // utility::vectors

#endif //NICHESS_VECTOR_UTILS_H

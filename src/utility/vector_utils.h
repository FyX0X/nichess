//
// Created by martin on 30/08/2026.
//

#ifndef NICHESS_VECTOR_UTILS_H
#define NICHESS_VECTOR_UTILS_H
#include <algorithm>
#include <vector>


namespace utility::vectors {

    template <typename T>
    constexpr bool contains(const std::vector<T>& vector, const T& element) {
        return std::find(vector.begin(), vector.end(), element) != vector.end();
    }

} // utility::vectors

#endif //NICHESS_VECTOR_UTILS_H

//
// Created by martin on 30/08/2026.
//

#ifndef NICHESS_VECTOR_UTILS_H
#define NICHESS_VECTOR_UTILS_H
#include <algorithm>
#include <ranges>
#include <random>
#include <cassert>
#include <iterator>


namespace utility::ranges {

    template <std::ranges::range R, typename T>
    constexpr bool Contains(R&& range, const T& element) {
        return std::ranges::find(range, element) != std::ranges::end(range);
    }

    // Source - https://stackoverflow.com/a/16421677
    // Posted by Christopher Smith, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-08-30, License - CC BY-SA 3.0

    template <std::ranges::random_access_range R>
    const auto& RandomChoice(R&& range) {
        assert(!std::ranges::empty(range));

        static std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<std::size_t> dist(
            0, std::ranges::size(range) - 1
        );

        return range[dist(rng)];
    }



} // utility::vectors

#endif //NICHESS_VECTOR_UTILS_H

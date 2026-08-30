//
// Created by martin on 29/08/2026.
//

#ifndef NICHESS_COORDINATES_H
#define NICHESS_COORDINATES_H

#include <format>
#include <string>
#include <string_view>
#include <optional>

#include "utility/string_utils.h"

namespace chess {

    struct Coordinates {
        int row;
        int column;

        bool operator==(const Coordinates&) const = default;
        Coordinates operator+(const Coordinates& rhs) const {
            return { .row = row + rhs.row, .column = column + rhs.column};
        }
        Coordinates &operator+=(const Coordinates & rhs) {
            row += rhs.row;
            column += rhs.column;
            return *this;
        }

        [[nodiscard]] bool IsValid() const { return 0 <= row && row < 8 && 0 <= column && column < 8; }

        [[nodiscard]] std::string ToString() const; // declared later since formatter does not exist yet.

        static std::optional<Coordinates> FromAlgebraicSquareNotation(std::string_view algebraic_square_notation);
    };


    constexpr Coordinates kWhiteKingSquare = { .row = 0, .column = 4 };
    constexpr Coordinates kWhiteQueenSquare = { .row = 0, .column = 3 };
    constexpr Coordinates kWhiteRookKingsideSquare = { .row = 0, .column = 7 };
    constexpr Coordinates kWhiteRookQueensideSquare = { .row = 0, .column = 0 };
    constexpr Coordinates kWhiteKnightKingsideSquare = { .row = 0, .column = 6 };
    constexpr Coordinates kWhiteKnightQueensideSquare = { .row = 0, .column = 1 };
    constexpr Coordinates kWhiteBishopKingsideSquare = { .row = 0, .column = 5 };
    constexpr Coordinates kWhiteBishopQueensideSquare = { .row = 0, .column = 2 };

    constexpr Coordinates kBlackKingSquare = { .row = 7, .column = 4 };
    constexpr Coordinates kBlackQueenSquare = { .row = 7, .column = 3 };
    constexpr Coordinates kBlackRookKingsideSquare = { .row = 7, .column = 7 };
    constexpr Coordinates kBlackRookQueensideSquare = { .row = 7, .column = 0 };
    constexpr Coordinates kBlackKnightKingsideSquare = { .row = 7, .column = 6 };
    constexpr Coordinates kBlackKnightQueensideSquare = { .row = 7, .column = 1 };
    constexpr Coordinates kBlackBishopKingsideSquare = { .row = 7, .column = 5 };
    constexpr Coordinates kBlackBishopQueensideSquare = { .row = 7, .column = 2 };

    constexpr Coordinates kWhitePawnDirection = { .row = 1, .column = 0 };
    constexpr Coordinates kBlackPawnDirection = { .row = -1, .column = 0 };

} // chess

template <>
struct std::formatter<chess::Coordinates> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const chess::Coordinates& coordinates, std::format_context& ctx) const {

        if (!coordinates.IsValid()) {
            return std::format_to(ctx.out(), "[Invalid: ({},{})]", coordinates.row, coordinates.column);
        }

        char col_chr = static_cast<char>('a' + coordinates.column);
        return std::format_to(
            ctx.out(),
            "{}{}", col_chr, coordinates.row + 1
        );
    }
};


#endif //NICHESS_COORDINATES_H

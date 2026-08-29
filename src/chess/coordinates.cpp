//
// Created by martin on 29/08/2026.
//

#include "coordinates.h"

namespace chess {


    std::optional<Coordinates> Coordinates::FromAlgebraicSquareNotation(std::string_view algebraic_square_notation) {
        if (algebraic_square_notation.size() != 2) {
            return std::nullopt;
        }

        char column_char = utility::strings::ToLowerCase(algebraic_square_notation[0]);
        char row_char = algebraic_square_notation[1];

        int row = row_char - '1';
        int column = column_char - 'a';

        Coordinates coordinates{ .row = row, .column = column };
        if (coordinates.IsValid()) {
            return coordinates;
        }

        return std::nullopt;
    }



    // now the formatter exists
    std::string Coordinates::ToString() const {
        return std::format("{}", *this);
    }

} // chess
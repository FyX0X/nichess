//
// Created by martin on 30/08/2026.
//

#include "move.h"

#include <cassert>
#include <format>
#include <utility>
#include "utility/ranges_utils.h"


namespace chess {
    std::string MoveLAN::ToLongAlgebraicNotation() const {
        std::string piece_char = (moving_piece == PieceType::Pawn) ? "" : std::format("{}", std::to_underlying(moving_piece));
        char capture_char = (capture) ? 'x' : '-';
        std::string promotion_char = (promotion_type == PieceType::None) ? "" : std::format("{}", std::to_underlying(promotion_type));
        return std::format("{}{}{}{}{}", piece_char, from, capture_char, to, promotion_char);
    }


    std::optional<MoveLAN> MoveLAN::FromLongAlgebraicNotation(std::string_view lan_str) {
        PieceType piece_type = static_cast<PieceType>(lan_str[0]);
        PieceType promotion_type = PieceType::None;
        int index = 1;
        bool check_promotion = false;
        if (!utility::ranges::Contains(kNonPawnTypes, piece_type)) {
            piece_type = PieceType::Pawn;
            index = 0;
            check_promotion = true;
        }

        std::optional<Coordinates> from = Coordinates::FromAlgebraicSquareNotation(lan_str.substr(index, 2));
        index += 2;
        bool capture = lan_str[index++] == 'x';
        std::optional<Coordinates> to = Coordinates::FromAlgebraicSquareNotation(lan_str.substr(index, 2));
        index += 2;
        if (check_promotion && lan_str.size() > index) {
            promotion_type = static_cast<PieceType>(lan_str[index++]);
            assert(utility::ranges::Contains(kPromotableTypes, promotion_type) && "Promoting to illegal piece type.");
        }

        if (! (from.has_value() && to.has_value()) ) {
            return std::nullopt;
        }
        return MoveLAN{
            .moving_piece = piece_type,
            .from = from.value(),
            .to = to.value(),
            .capture = capture,
            .promotion_type = promotion_type
        };
    }
} // chess
//
// Created by martin on 30/08/2026.
//

#include "move.h"

#include <cassert>
#include <format>
#include <utility>
#include "utility/ranges_utils.h"


namespace chess {

    std::string Move::ToString() const {
        char capture_char = capture_type != PieceType::None ? 'x' : '-';
        std::string promotion_char = (promotion_type == PieceType::None) ? "" : std::format("{}", std::to_underlying(promotion_type));
        return std::format("{}{}{}{}", from, capture_char, to, promotion_char);
    }

    bool MoveLAN::Matches(const Move &move) const {
        return move.from == from && move.to == to 
            && (moving_piece == PieceType::None || ((move.capture_type != PieceType::None) == capture)) // do not check capture when moving_piece is None (for UCI notation)
            && move.promotion_type == promotion_type;
    }

    std::optional<Move> MoveLAN::GetCorrespondingMove(const std::vector<Move> &moves) const {

        for (const Move& move : moves) {
            if (Matches(move)) {
                return std::optional<Move>{move};
            }
        }

        return std::nullopt;
    }

    std::string MoveLAN::ToLongAlgebraicNotation() const {
        std::string piece_char = (moving_piece == PieceType::Pawn) ? "" : std::format("{}", std::to_underlying(moving_piece));
        char capture_char = (capture) ? 'x' : '-';
        std::string promotion_char = (promotion_type == PieceType::None) ? "" : std::format("{}", std::to_underlying(promotion_type));
        return std::format("{}{}{}{}{}", piece_char, from, capture_char, to, promotion_char);
    }

    std::string MoveLAN::ToUCINotation() const {
        std::string promotion_char = (promotion_type == PieceType::None) ? "" : std::format("{}", std::to_underlying(promotion_type));
        return std::format("{}{}{}", from, to, utility::strings::ToLowerCase(promotion_char));
    }


    std::optional<MoveLAN> MoveLAN::FromLongAlgebraicNotation(std::string_view lan_str) {
        if (lan_str.size() < 5) {
            return std::nullopt;
        }

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
            if (!utility::ranges::Contains(kPromotableTypes, promotion_type) || lan_str.size() != index) {
                return std::nullopt;
            }
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


    std::optional<MoveLAN> MoveLAN::FromUCINotation(std::string_view uci_move) {
        if (uci_move.size() != 4 && uci_move.size() != 5) {
            return std::nullopt;
        }

        std::optional<Coordinates> from = Coordinates::FromAlgebraicSquareNotation(uci_move.substr(0, 2));
        std::optional<Coordinates> to = Coordinates::FromAlgebraicSquareNotation(uci_move.substr(2, 2));
        PieceType promotion_type = PieceType::None;
        if (uci_move.size() > 4) {
            promotion_type = static_cast<PieceType>(utility::strings::ToUpperCase(uci_move[4]));
            if (!utility::ranges::Contains(kPromotableTypes, promotion_type)) {
                return std::nullopt;
            }
        }

        if (! (from.has_value() && to.has_value()) ) {
            return std::nullopt;
        }
        return MoveLAN{
            .moving_piece = PieceType::None, // This value is ignored when matching a MoveLAN to a Move, so we can leave it as None.
            .from = from.value(),
            .to = to.value(),
            .capture = false,                // We don't actually know this value Be sure to not check it when matching.
            .promotion_type = promotion_type
        };
    }
} // chess
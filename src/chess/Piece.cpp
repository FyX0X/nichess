//
// Created by martin on 27/08/2026.
//

#include "Piece.h"

#include <utility>

#include "utility/string_utils.h"

namespace chess {
    std::string PlayerToString(const Player player) {
        switch (player) {
            case Player::White:
                return "White";
            case Player::Black:
                return "Black";
            default:
                return "None";
        }
    }

    char Piece::ToChar() const {
        if (IsEmpty()) {
            return ' ';
        }

        const char pieceChr = std::to_underlying(type);
        if (IsWhite()) {
            return pieceChr;
        }

        return utility::strings::ToLowerCase(pieceChr);
    }

    Piece Piece::FromChar(const char c) {
        const char upper = utility::strings::ToUpperCase(c);

        PieceType type;
        switch (upper) {
            case 'P': type = PieceType::Pawn; break;
            case 'N': type = PieceType::Knight; break;
            case 'B': type = PieceType::Bishop; break;
            case 'R': type = PieceType::Rook; break;
            case 'Q': type = PieceType::Queen; break;
            case 'K': type = PieceType::King; break;
            default: type = PieceType::None; break;
        }

        const Player player = (c == upper) ? Player::White : Player::Black;

        return { .type = type, .player = player };
    }
} // chess
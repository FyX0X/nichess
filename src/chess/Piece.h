//
// Created by martin on 27/08/2026.
//

#ifndef NICHESS_PIECE_H
#define NICHESS_PIECE_H

#include <cstdint>

namespace chess {

    enum class PieceType : char {
        None = 0,
        Pawn = 'P',
        Knight = 'N',
        Bishop = 'B',
        Rook = 'R',
        Queen = 'Q',
        King = 'K'
    };

    enum class Player : std::uint8_t {
        None = 0,
        White,
        Black
    };

    struct Piece {
        PieceType type;
        Player player;

        [[nodiscard]] constexpr bool IsEmpty() const { return type == PieceType::None; }
        [[nodiscard]] constexpr bool IsWhite() const { return player == Player::White; }
        [[nodiscard]] constexpr bool IsBlack() const { return player == Player::Black; }

        [[nodiscard]] char ToChar() const;
        static Piece FromChar(char c);

        static constexpr Piece Empty() { return { .type = PieceType::None, .player = Player::None }; }
    };

} // chess

#endif //NICHESS_PIECE_H

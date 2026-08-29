//
// Created by martin on 27/08/2026.
//

#ifndef NICHESS_PIECE_H
#define NICHESS_PIECE_H

#include <cstdint>

#include "coordinates.h"

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

    std::string PlayerToString(Player player);

    struct Piece {
        PieceType type;
        Player player;

        [[nodiscard]] constexpr bool IsEmpty() const { return type == PieceType::None; }
        [[nodiscard]] constexpr bool IsWhite() const { return player == Player::White; }
        [[nodiscard]] constexpr bool IsBlack() const { return player == Player::Black; }

        [[nodiscard]] char ToChar() const;
        static Piece FromChar(char c);

        static constexpr Piece Empty() { return { .type = PieceType::None, .player = Player::None }; }


        bool operator==(const Piece&) const = default;

    };

    constexpr Piece kEmptyPiece = { .type = PieceType::None, .player = Player::None };

    constexpr Piece kWhitePawn = { .type = PieceType::Pawn, .player = Player::White };
    constexpr Piece kWhiteKnight = { .type = PieceType::Knight, .player = Player::White };
    constexpr Piece kWhiteBishop = { .type = PieceType::Bishop, .player = Player::White };
    constexpr Piece kWhiteRook = { .type = PieceType::Rook, .player = Player::White };
    constexpr Piece kWhiteQueen{ .type = PieceType::Queen, .player = Player::White };
    constexpr Piece kWhiteKing{ .type = PieceType::King, .player = Player::White };

    constexpr Piece kBlackPawn = { .type = PieceType::Pawn, .player = Player::Black };
    constexpr Piece kBlackKnight = { .type = PieceType::Knight, .player = Player::Black };
    constexpr Piece kBlackBishop = { .type = PieceType::Bishop, .player = Player::Black };
    constexpr Piece kBlackRook = { .type = PieceType::Rook, .player = Player::Black };
    constexpr Piece kBlackQueen{ .type = PieceType::Queen, .player = Player::Black };
    constexpr Piece kBlackKing{ .type = PieceType::King, .player = Player::Black };

} // chess

#endif //NICHESS_PIECE_H

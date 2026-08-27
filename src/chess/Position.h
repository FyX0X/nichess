//
// Created by martin on 27/08/2026.
//

#ifndef NICHESS_POSITION_H
#define NICHESS_POSITION_H

#include <string>
#include <array>
#include <vector>

#include "Piece.h"

namespace chess {

    struct CastlingRights {
        bool white_kingside = true;
        bool white_queenside = true;
        bool black_kingside = true;
        bool black_queenside = true;
    };

    struct Coordinates {
        uint8_t row;
        uint8_t column;

        bool operator==(const Coordinates&) const = default;
    };

    class Position {
    public:
        Position();
        explicit Position(const std::array<std::array<Piece, 8>, 8>& board);
        Position(const std::array<std::array<Piece, 8>, 8>& board,
                 Player active_player,
                 const CastlingRights& castling_rights,
                 std::string_view en_passant,
                 int half_move_clock,
                 int move_count);

        [[nodiscard]] std::string ToString() const;

        // getters
        [[nodiscard]] const std::array<std::array<Piece, 8>, 8>& GetBoard() const { return board_; }

        [[nodiscard]] const std::array<Piece, 8>& GetRow(uint8_t row) const { return board_[row]; }
        [[nodiscard]] Piece GetPiece(uint8_t row, uint8_t column) const { return board_[row][column]; }
        [[nodiscard]] Piece GetPiece(Coordinates coords) const { return board_[coords.row][coords.column]; }
        [[nodiscard]] Player GetActivePlayer() const { return active_player_; }

        [[nodiscard]] CastlingRights GetCastlingRights() const { return castling_rights_; }

        [[nodiscard]] int GetHalfMoveClock() const { return half_move_clock_; }
        [[nodiscard]] int GetMoveCount() const { return move_count_; }

        // setters


    private:

        void RecomputeRemainingPieces();

        // each char represents a board cell.
        // coordinates are row order (e.g: [0, 2] -> a3, [7][3] -> h4)
        std::array<std::array<Piece, 8>, 8> board_;
        Player active_player_ = Player::White;
        CastlingRights castling_rights_{};
        std::string en_passant_; // TODO implement this or a previous move field.
        int half_move_clock_ = 0;
        int move_count_ = 1;


        // duplicate data for faster iteration through pieces
        std::vector<Coordinates> piece_coordinates_ = {};

    };
} // chess

#endif //NICHESS_POSITION_H

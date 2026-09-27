//
// Created by martin on 27/08/2026.
//

#ifndef NICHESS_POSITION_H
#define NICHESS_POSITION_H

#include <string>
#include <array>
#include <format>
#include <vector>

#include "piece.h"
#include "coordinates.h"
#include "move.h"
#include "castling_rights.h"
#include "irreversible_aspects.h"

namespace chess {

    class Board {
    public:
        Board();
        // explicit Position(const std::array<std::array<Piece, 8>, 8>& board);
        Board(const std::array<std::array<Piece, 8>, 8>& board,
                 Player active_player,
                 const CastlingRights& castling_rights,
                 std::optional<Coordinates> en_passant,
                 int half_move_clock,
                 int move_count);

        [[nodiscard]] std::string ToString() const;

        [[nodiscard]] std::string GetTargetedSquaresString(Player player) const; // TODO remove

        // getters
        [[nodiscard]] const std::array<std::array<Piece, 8>, 8>& GetBoard() const { return board_; }

        [[nodiscard]] const std::array<Piece, 8>& GetRow(uint8_t row) const { return board_[row]; }
        [[nodiscard]] Piece GetPiece(int row, int column) const { return board_[row][column]; }
        [[nodiscard]] Piece GetPiece(Coordinates coords) const { return board_[coords.row][coords.column]; }
        [[nodiscard]] Player GetActivePlayer() const { return active_player_; }




        [[nodiscard]] const IrreversibleAspects& GetIrreversibleAspects() const { return irreversible_aspects_; }
        [[nodiscard]] const std::optional<Coordinates>& GetEnPassant() const { return irreversible_aspects_.en_passant; }
        [[nodiscard]] CastlingRights GetCastlingRights() const { return irreversible_aspects_.castling_rights; }

        [[nodiscard]] int GetHalfMoveClock() const { return irreversible_aspects_.half_move_clock; }
        [[nodiscard]] int GetMoveCount() const { return move_count_; }


        /**
         * Tries to perform a move (only if legal).
         * @param move The legal move to play.
         * @return True if the move was successfully played.
         */
        bool MakeLegalMove(const Move& move);

        // todo modify
        bool MakeLegalMoveLAN(const MoveLAN& move_lan, Move& actual_move);

        void UnmakeMove(const Move& move, const IrreversibleAspects& prev_aspects);



    private:

        void SetPiece(const Coordinates coordinates, const Piece piece) { board_[coordinates.row][coordinates.column] = piece; }

        void RecomputeRemainingPieces();

        void EnsureLegalEnPassantSquare();

        /** Limits the granted rights to what is possible with remaining pieces. */
        void EnsurePossibleCastlingRights();

#pragma region Modifiable Getters

        CastlingRights& GetCastlingRights() { return irreversible_aspects_.castling_rights; }
        std::optional<Coordinates>& GetEnPassant() { return irreversible_aspects_.en_passant; }

#pragma endregion



        void MakeMove(const Move& move);
        void UnmakeMoveInternal(const Move& move, const IrreversibleAspects& prev_aspects);


        // each char represents a board cell.
        // coordinates are row order (e.g: [0, 2] -> a3, [7][3] -> h4)
        std::array<std::array<Piece, 8>, 8> board_;
        Player active_player_ = Player::White;

        IrreversibleAspects irreversible_aspects_{};

        int move_count_ = 1;


        // duplicate data for faster iteration through pieces
        std::vector<Coordinates> white_piece_coordinates_ = {};
        std::vector<Coordinates> black_piece_coordinates_ = {};

    };
} // chess

#endif //NICHESS_POSITION_H

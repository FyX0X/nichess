//
// Created by martin on 27/08/2026.
//

#ifndef NICHESS_POSITION_H
#define NICHESS_POSITION_H

#include <string>
#include <array>
#include <format>
#include <vector>

#include "Piece.h"
#include "Coordinates.h"
#include "Move.h"
#include "CastlingRights.h"

namespace chess {

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

        [[nodiscard]] std::string GetReachableSquaresString(Player player) const;

        // getters
        [[nodiscard]] const std::array<std::array<Piece, 8>, 8>& GetBoard() const { return board_; }

        [[nodiscard]] const std::array<Piece, 8>& GetRow(uint8_t row) const { return board_[row]; }
        [[nodiscard]] Piece GetPiece(int row, int column) const { return board_[row][column]; }
        [[nodiscard]] Piece GetPiece(Coordinates coords) const { return board_[coords.row][coords.column]; }
        [[nodiscard]] Player GetActivePlayer() const { return active_player_; }
        [[nodiscard]] const std::vector<Coordinates>& GetPlayerOccupiedSquares(Player player) const {
            return (player == Player::White) ? white_piece_coordinates_ : black_piece_coordinates_;
        }
        [[nodiscard]] const std::array<std::array<bool, 8>, 8>& GetPlayerReachableSquares(Player player) const {
            return (player == Player::White) ? white_reachable_ : black_reachable_;
        }


        [[nodiscard]] CastlingRights GetCastlingRights() const { return castling_rights_; }

        [[nodiscard]] int GetHalfMoveClock() const { return half_move_clock_; }
        [[nodiscard]] int GetMoveCount() const { return move_count_; }


        [[nodiscard]] const std::vector<Move>& GetLegalMovesForPlayer(Player player) const;


        // setters

        // Moves
        [[nodiscard]] const std::vector<Move> &GetActivePlayerMoves() const;

    private:

        static int GetPawnStartingRank(Player player);
        static int GetPieceStartingRank(Player player);
        static Player GetOtherPlayer(Player player);
        static bool CouldPlayerTakePiece(Player player, const Piece& piece);

        void RecomputeRemainingPieces();

        /** Limits the granted rights to what is possible with remaining pieces. */
        void EnsurePossibleCastlingRights();

        std::vector<Move>& GetLegalMovesForPlayer(Player player); // modifiable version
        std::array<std::array<bool, 8>, 8>& GetPlayerReachableSquares(Player player) { // modifiable version
            return (player == Player::White) ? white_reachable_ : black_reachable_;
        }

        void ComputeMovesForPlayer(Player player);

        void AddMoveToPlayer(Move move, Player player);

        void AddMoveToPlayer(Coordinates from, Coordinates to, Player player);

        void GenerateLegalMovesFromCoordinates(const Coordinates& from, Player player);

        void GenerateTranslationMoves(const Coordinates& from, const std::vector<Coordinates>& directions, Player player);
        void GenerateDirectMoves(const Coordinates& from, const std::vector<Coordinates>& directions, Player player);

        void GeneratePawnMoves(const Coordinates& from, Player player);
        void GenerateKnightMoves(const Coordinates& from, Player player);
        void GenerateBishopMoves(const Coordinates& from, Player player);
        void GenerateRookMoves(const Coordinates& from, Player player);
        void GenerateQueenMoves(const Coordinates& from, Player player);
        void GenerateKingMoves(const Coordinates& from, Player player);
        void GenerateCastleMoves(Player player);

        [[nodiscard]] bool DoesPlayerTargetCoordinates(Player player, Coordinates coordinates) const;





        // each char represents a board cell.
        // coordinates are row order (e.g: [0, 2] -> a3, [7][3] -> h4)
        std::array<std::array<Piece, 8>, 8> board_;
        Player active_player_ = Player::White;
        CastlingRights castling_rights_{};
        std::string en_passant_; // TODO implement this or a previous move field.
        int half_move_clock_ = 0;
        int move_count_ = 1;


        // duplicate data for faster iteration through pieces
        std::vector<Coordinates> white_piece_coordinates_ = {};
        std::vector<Coordinates> black_piece_coordinates_ = {};

        std::array<std::array<bool, 8>, 8> white_reachable_ = {};
        std::array<std::array<bool, 8>, 8> black_reachable_ = {};

        std::vector<Move> white_moves_ = {};
        std::vector<Move> black_moves_ = {};

    };
} // chess

#endif //NICHESS_POSITION_H

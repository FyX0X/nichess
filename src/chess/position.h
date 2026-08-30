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

    class Position {
    public:
        Position();
        // explicit Position(const std::array<std::array<Piece, 8>, 8>& board);
        Position(const std::array<std::array<Piece, 8>, 8>& board,
                 Player active_player,
                 const CastlingRights& castling_rights,
                 std::optional<Coordinates> en_passant,
                 int half_move_clock,
                 int move_count);

        [[nodiscard]] std::string ToString() const;

        [[nodiscard]] std::string GetTargetedSquaresString(Player player) const;

        // getters
        [[nodiscard]] const std::array<std::array<Piece, 8>, 8>& GetBoard() const { return board_; }

        [[nodiscard]] const std::array<Piece, 8>& GetRow(uint8_t row) const { return board_[row]; }
        [[nodiscard]] Piece GetPiece(int row, int column) const { return board_[row][column]; }
        [[nodiscard]] Piece GetPiece(Coordinates coords) const { return board_[coords.row][coords.column]; }
        [[nodiscard]] Player GetActivePlayer() const { return active_player_; }
        [[nodiscard]] const std::vector<Coordinates>& GetPlayerOccupiedSquares(Player player) const {
            return (player == Player::White) ? white_piece_coordinates_ : black_piece_coordinates_;
        }
        [[nodiscard]] const std::array<std::array<bool, 8>, 8>& GetPlayerTargetedSquares(Player player) const {
            return (player == Player::White) ? white_targets_ : black_targets_;
        }


        [[nodiscard]] const IrreversibleAspects& GetIrreversibleAspects() const { return irreversible_aspects_stack_.back(); }
        [[nodiscard]] const std::optional<Coordinates>& GetEnPassant() const { return irreversible_aspects_stack_.back().en_passant; }
        [[nodiscard]] CastlingRights GetCastlingRights() const { return irreversible_aspects_stack_.back().castling_rights; }

        [[nodiscard]] int GetHalfMoveClock() const { return irreversible_aspects_stack_.back().half_move_clock; }
        [[nodiscard]] int GetMoveCount() const { return move_count_; }


        [[nodiscard]] const std::vector<Move>& GetLegalMovesForPlayer(Player player) const;


        // setters

        // Moves
        [[nodiscard]] const std::vector<Move> &GetActivePlayerMoves() const;

        /**
         * Tries to perform a move (only if legal).
         * @param move The legal move to play.
         * @return True if the move was successfully played.
         */
        bool MakeLegalMove(const Move& move);

        bool MakeLegalMoveLAN(const MoveLAN& move_lan);


        [[nodiscard]] bool IsInCheck() const { return is_in_check_; }


    private:

        static int GetPawnStartingRank(Player player);
        static int GetPieceStartingRank(Player player);
        static Player GetOtherPlayer(Player player);
        static bool CouldPlayerTakePiece(Player player, const Piece& piece);

        void SetPiece(const Coordinates coordinates, const Piece piece) { board_[coordinates.row][coordinates.column] = piece; }

        void RecomputeRemainingPieces();

        void EnsureLegalEnPassantSquare();

        /** Limits the granted rights to what is possible with remaining pieces. */
        void EnsurePossibleCastlingRights();

#pragma region Modifiable Getters

        std::vector<Move>& GetLegalMovesForPlayer(Player player); // modifiable version
        std::array<std::array<bool, 8>, 8>& GetPlayerTargetedSquares(const Player player) { // modifiable version
            return (player == Player::White) ? white_targets_ : black_targets_;
        }

        CastlingRights& GetCastlingRights() { return irreversible_aspects_stack_.back().castling_rights; }
        std::optional<Coordinates>& GetEnPassant() { return irreversible_aspects_stack_.back().en_passant; }

#pragma endregion


        void ComputeMovesForPlayer(Player player);

        void AddTargetedSquareToPlayer(Coordinates to, Player player);

        void AddMoveToPlayer(const Move &move, Player player);

        void GenerateLegalMovesFromCoordinates(const Coordinates& from, Player player);

        void GenerateTranslationMoves(const Coordinates& from, const std::vector<Coordinates>& directions, Player player);

        void GeneratePromotionMoves(const Move &move, Player player);

        void GenerateDirectMoves(const Coordinates& from, const std::vector<Coordinates>& directions, Player player);

        void GeneratePawnMoves(const Coordinates& from, Player player);
        void GenerateKnightMoves(const Coordinates& from, Player player);
        void GenerateBishopMoves(const Coordinates& from, Player player);
        void GenerateRookMoves(const Coordinates& from, Player player);
        void GenerateQueenMoves(const Coordinates& from, Player player);
        void GenerateKingMoves(const Coordinates& from, Player player);
        void GenerateCastleMoves(Player player);

        [[nodiscard]] bool DoesPlayerTargetCoordinates(Player player, Coordinates coordinates) const;


        void MakeMove(const Move& move);
        void UnmakeMove(const Move& move);

        bool ComputeIsInCheck();

        void PushIrreversibleAspects(const CastlingRights &castling_rights, const std::optional<Coordinates>& en_passant, int half_move_clock);

        void PushIrreversibleAspects(const IrreversibleAspects &irreversible_aspects);

        // each char represents a board cell.
        // coordinates are row order (e.g: [0, 2] -> a3, [7][3] -> h4)
        std::array<std::array<Piece, 8>, 8> board_;
        Player active_player_ = Player::White;

        std::vector<IrreversibleAspects> irreversible_aspects_stack_{};
        std::vector<Move> played_moves_{}; // TODO consider removing this if not needed ? for example PERFT already remembers moves in call stack.

        int move_count_ = 1;


        // duplicate data for faster iteration through pieces
        std::vector<Coordinates> white_piece_coordinates_ = {};
        std::vector<Coordinates> black_piece_coordinates_ = {};

        // contains the squares that the players can attack / target
        std::array<std::array<bool, 8>, 8> white_targets_ = {};
        std::array<std::array<bool, 8>, 8> black_targets_ = {};

        std::vector<Move> white_moves_ = {};
        std::vector<Move> black_moves_ = {};

        bool is_in_check_ = false;

    };
} // chess

#endif //NICHESS_POSITION_H

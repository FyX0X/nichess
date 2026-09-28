//
// Created by martin on 27/09/2026.
//

#ifndef NICHESS_ENGINE_H
#define NICHESS_ENGINE_H

#include "board.h"

namespace chess {
    class Engine {

    public:

        static std::vector<Move> GetPseudoLegalMoves(const Board& board);
        static std::vector<Move> GetLegalMoves(const Board& board); // TODO



        static bool IsPlayerInCheck(const Board& board, const Player& player);

        /* Returns if the player controls this square, by convention: a piece does not control it's own square. */
        static bool DoesPlayerTargetSquare(const Board& board, const Player& player, const Coordinates& target);
    private:


        static std::vector<Move> GeneratePseudoLegalMovesFromCoordinates(const Board& board, const Coordinates& from, Player player);
        static std::vector<Move> GenerateTranslationMoves(const Board& board, const Coordinates& from, const std::vector<Coordinates>& directions, Player player);
        static std::vector<Move> GeneratePromotionMoves(const Board &board, const Move &move, Player player);
        static std::vector<Move> GenerateDirectMoves(const Board& board, const Coordinates& from, const std::vector<Coordinates>& directions, Player player);
        static std::vector<Move> GeneratePawnMoves(const Board& board, const Coordinates& from, Player player);
        static std::vector<Move> GenerateKnightMoves(const Board& board, const Coordinates& from, Player player);
        static std::vector<Move> GenerateBishopMoves(const Board& board, const Coordinates& from, Player player);
        static std::vector<Move> GenerateRookMoves(const Board& board, const Coordinates& from, Player player);
        static std::vector<Move> GenerateQueenMoves(const Board& board, const Coordinates& from, Player player);
        static std::vector<Move> GenerateKingMoves(const Board& board, const Coordinates& from, Player player);
        static std::vector<Move> GenerateCastleMoves(const Board& board, Player player);


        static bool CanPawnTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player);
        static bool CanKnightTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player);
        static bool CanBishopTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player);
        static bool CanRookTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player);
        static bool CanQueenTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player);
        static bool CanKingTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player);

    };
} // chess

#endif //NICHESS_ENGINE_H

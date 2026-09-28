//
// Created by martin on 08/09/2026.
//

#include "perft.h"

#include <cassert>

#include "move.h"

#include <chrono>
#include <print>

#include "utility.h"
#include "chess/engine.h"

namespace chess {


    static int nodes_count = 0;
    static int en_passant_count = 0;
    static int castle_count = 0;
    static int promotion_count = 0;
    static int check_count = 0;
    static int capture_count = 0;

    uint64_t Perft::PerftAtDepth(int depth) {
        
        position_ = starting_position_;
        nodes_count = 0;
        en_passant_count = 0;
        castle_count = 0;
        promotion_count = 0;
        check_count = 0;
        capture_count = 0;
        return PerftRecursive(depth);
    }

    void Perft::PerformPerftAndPrintInfo(int depth) {

        std::print("Performing Perft at depth {}...\n", depth);
        auto start_time = std::chrono::high_resolution_clock::now();
        uint64_t nodes = PerftAtDepth(depth);
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = end_time - start_time;
        std::print("Perft completed:\nNodes:\t{}\nTime taken:\t{:.6f} seconds.\nNodes per second:\t{:.2f}\n", nodes,
                   elapsed_time.count(), static_cast<double>(nodes) / elapsed_time.count());

        std::println("Perft Statistics:\nEn Passant\t{}\nCastles \t{}\nPromotions\t{}\nchecks\t{}\ncaptures:\t{}",
            en_passant_count, castle_count, promotion_count, check_count, capture_count);

    }

    uint64_t Perft::PerftRecursive(int depth)
    {
        if (depth == 0) {
            nodes_count++;
            if ((nodes_count & (nodes_count-1)) == 0) {
                std::println("[INFO] Perft: Generated {} moves...", nodes_count);
            }
            return 1ULL;
        }

        std::vector<Move> move_list = Engine::GetPseudoLegalMoves(position_);
        int n_moves = move_list.size();
        uint64_t nodes = 0;

        IrreversibleAspects irreversible_aspects = position_.GetIrreversibleAspects();

        for (int i = 0; i < n_moves; i++) {
            Move move = move_list[i];
            position_.MakeMove(move);
            Player player = position_.GetActivePlayer();
            if (!Engine::IsPlayerInCheck(position_, GetOtherPlayer(player))) {
                nodes += PerftRecursive(depth - 1);
                capture_count += (move.capture_type != PieceType::None) ? 1 : 0;
                en_passant_count += (move.en_passant) ? 1 : 0;
                castle_count += (move.castle_kingside || move.castle_queenside) ? 1 : 0;
                promotion_count += (move.promotion_type != PieceType::None) ? 1 : 0;
                check_count += (Engine::IsPlayerInCheck(position_, player)) ? 1 : 0;

            } else {
                std::println("[Info] PerftRecursive: move {}->{} is not legal", move.from, move.to);
            }
            position_.UnmakeMove(move_list[i], irreversible_aspects);
        }

        return nodes;
    }

} // namespace chess

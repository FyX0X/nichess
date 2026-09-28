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


    static int move_count = 0;

    uint64_t Perft::PerftAtDepth(int depth) {
        
        position_ = starting_position_;
        move_count = 0;
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

    }

    uint64_t Perft::PerftRecursive(int depth)
    {
        if (depth == 0) {
            move_count++;
            if ((move_count & (move_count-1)) == 0) {
                std::println("[INFO] Perft: Generated {} moves...", move_count);
            }
            return 1ULL;
        }

        std::vector<Move> move_list = Engine::GetPseudoLegalMoves(position_);
        int n_moves = move_list.size();
        uint64_t nodes = 0;

        IrreversibleAspects irreversible_aspects = position_.GetIrreversibleAspects();

        for (int i = 0; i < n_moves; i++) {
            position_.MakeMove(move_list[i]);
            if (!Engine::IsPlayerInCheck(position_, GetOtherPlayer(position_.GetActivePlayer()))) {
                nodes += PerftRecursive(depth - 1);
            } else {
                Move move = move_list[i];
                std::println("[Info] PerftRecursive: move {}->{} is not legal", move.from, move.to);
            }
            position_.UnmakeMove(move_list[i], irreversible_aspects);
        }

        return nodes;
    }

} // namespace chess

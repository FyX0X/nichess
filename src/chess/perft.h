//
// Created by martin on 08/09/2026.
//

#ifndef NICHESS_PERFT_H
#define NICHESS_PERFT_H


#include "board.h"

namespace chess {

    class Perft {

    public:
        Perft(const Board& starting_position) :
            starting_position_(starting_position),
            position_(starting_position)
            {}

        uint64_t PerftAtDepth(int depth);
        
        void PerformPerftAndPrintInfo(int depth);

    private:
        
        uint64_t PerftRecursive(int depth);

        const Board starting_position_;
        Board position_;


    };
    

} // chess


#endif // NICHESS_PERFT_H
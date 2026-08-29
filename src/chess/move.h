//
// Created by martin on 29/08/2026.
//

#ifndef NICHESS_MOVE_H
#define NICHESS_MOVE_H


#include "coordinates.h"

namespace chess {

    // TODO use bitmask to shrink size of struct.
    struct Move {
        Coordinates from;
        Coordinates to;
        bool double_pawn = false;
        bool capture = false;
        bool en_passant = false;
        bool castle_kingside = false;
        bool castle_queenside = false;
        PieceType promotion_type = PieceType::None;

        Move(const Coordinates from, const Coordinates to) :
            from(from),
            to(to)
        {}

        bool operator==(const Move&) const = default;
    };

}

#endif //NICHESS_MOVE_H

//
// Created by martin on 29/08/2026.
//

#ifndef NICHESS_MOVE_H
#define NICHESS_MOVE_H


#include "Coordinates.h"

namespace chess {

    struct Move {
        Coordinates from;
        Coordinates to;

        bool operator==(const Move&) const = default;
    };

}

#endif //NICHESS_MOVE_H

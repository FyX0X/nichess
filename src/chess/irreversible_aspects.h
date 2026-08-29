//
// Created by martin on 29/08/2026.
//

#ifndef NICHESS_IRREVERSIBLE_ASPECTS_H
#define NICHESS_IRREVERSIBLE_ASPECTS_H

#include <optional>

#include "castling_rights.h"
#include "coordinates.h"

namespace chess {

    struct IrreversibleAspects {

        CastlingRights castling_rights{};
        std::optional<Coordinates> en_passant = std::nullopt;
        int half_move_clock = 0;

    };

} // chess

#endif //NICHESS_IRREVERSIBLE_ASPECTS_H

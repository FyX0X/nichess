//
// Created by martin on 27/09/2026.
//

#ifndef NICHESS_UTILITY_H
#define NICHESS_UTILITY_H

#include "piece.h"

namespace chess {
    int GetPawnStartingRank(Player player);
    int GetPieceStartingRank(Player player);
    Player GetOtherPlayer(Player player);
    bool CouldPlayerTakePiece(Player player, const Piece &piece);
    Coordinates GetPawnMoveDirection(Player player);

}


#endif //NICHESS_UTILITY_H

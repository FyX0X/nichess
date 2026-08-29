//
// Created by martin on 29/08/2026.
//

#ifndef NICHESS_CASTLING_RIGHTS_H
#define NICHESS_CASTLING_RIGHTS_H

namespace chess {

    struct CastlingRights {
        bool white_kingside = true;
        bool white_queenside = true;
        bool black_kingside = true;
        bool black_queenside = true;
    };

}

#endif //NICHESS_CASTLING_RIGHTS_H

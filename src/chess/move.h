//
// Created by martin on 29/08/2026.
//

#ifndef NICHESS_MOVE_H
#define NICHESS_MOVE_H


#include "coordinates.h"
#include "piece.h"


namespace chess {

    // TODO use bitmask to shrink size of struct.
    struct Move {
        Coordinates from;
        Coordinates to;
        bool double_pawn = false;
        bool en_passant = false;
        bool castle_kingside = false;
        bool castle_queenside = false;
        PieceType promotion_type = PieceType::None;
        PieceType capture_type = PieceType::None;

        Move(const Coordinates from, const Coordinates to) :
            from(from),
            to(to)
        {}

        bool operator==(const Move&) const = default;
    };


    /**
     * Represent LAN notation of a move that will be passed to a Position object to convert it to a real Move
     */
    struct MoveLAN {
        PieceType moving_piece;
        Coordinates from;
        Coordinates to;
        bool capture;
        PieceType promotion_type;


        /**
         * Compares a move with this object to check if they match.
         * TODO: currently does not compare 'moving_piece'.
         * @param move The real move object to compare this LAN move to.
         * @return True if the LAN matches 'move'.
         */
        [[nodiscard]] bool Matches(const Move& move) const;

        [[nodiscard]] std::string ToLongAlgebraicNotation() const;

        /**
         * <LAN move descriptor piece moves> ::= <Piece symbol><from square>['-'|'x']<to square>
         * <LAN move descriptor pawn moves>  ::= <from square>['-'|'x']<to square>[<promoted to>]
         * <Piece symbol> ::= 'N' | 'B' | 'R' | 'Q' | 'K'
         * @param lan_str a move in LAN format.
         * @return The move id represented the the given string.
         */
        static std::optional<MoveLAN> FromLongAlgebraicNotation(std::string_view lan_str);
    };

}

#endif //NICHESS_MOVE_H

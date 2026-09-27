//
// Created by martin on 27/09/2026.
//

#include "utility.h"


namespace chess {


    int GetPawnStartingRank(const Player player) {
        switch (player) {
            case Player::White:
                return 1; // #2
            case Player::Black:
                return 6; // #7
            default:
                return -1;
        }
    }

    int GetPieceStartingRank(const Player player) {
        switch (player) {
            case Player::White:
                return 0;
            case Player::Black:
                return 7;
            default:
                return -1;
        }
    }

    Player GetOtherPlayer(const Player player) {
        switch (player) {
            case Player::White:
                return Player::Black;
            case Player::Black:
                return Player::White;
            default:
                return Player::None;
        }
    }

    bool CouldPlayerTakePiece(const Player player, const Piece &piece) {
        return (piece.player == GetOtherPlayer(player) && piece.type != PieceType::King && piece.type != PieceType::None);
    }

    static constexpr Coordinates GetPawnMoveDirection(const Player player) {
        switch (player) {
            case Player::White:
                return kWhitePawnDirection;
            case Player::Black:
                return kBlackPawnDirection;
            default:
                return {};
        }
    }

}
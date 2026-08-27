//
// Created by martin on 27/08/2026.
//

#include "Position.h"

#include <iostream>
#include <sstream>
#include <ranges>


namespace chess {

    Position::Position() :
        board_()
    {}

    Position::Position(const std::array<std::array<Piece, 8>, 8> &board) :
        board_(board)
    {}

    Position::Position(const std::array<std::array<Piece, 8>, 8> &board,
                       Player active_player, const CastlingRights& castling_rights,
                       std::string_view en_passant, int half_move_clock, int move_count) :
        board_(board),
        active_player_(active_player),
        castling_rights_(castling_rights),
        en_passant_(en_passant),
        half_move_clock_(half_move_clock),
        move_count_(move_count)
    {}

    std::string Position::ToString() const {
        std::stringstream boardStream;
        // reverse order since row 0 is at bottom
        for (const auto& row : std::views::reverse(board_)) {
            for (const Piece& piece : row) {
                boardStream << piece.ToChar() << ' ';
            }
            boardStream << '\n';
        }
        return boardStream.str();
    }

} // chess
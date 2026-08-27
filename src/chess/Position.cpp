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
        board_(board) {
        RecomputeRemainingPieces();
    }

    Position::Position(const std::array<std::array<Piece, 8>, 8> &board,
                       Player active_player, const CastlingRights& castling_rights,
                       std::string_view en_passant, int half_move_clock, int move_count) :
        board_(board),
        active_player_(active_player),
        castling_rights_(castling_rights),
        en_passant_(en_passant),
        half_move_clock_(half_move_clock),
        move_count_(move_count) {
        RecomputeRemainingPieces();
    }

    std::string Position::ToString() const {
        std::stringstream boardStream;
        // reverse order since row 0 is at bottom
        boardStream << '\n' << (active_player_ == Player::White ? "White" : "Black") << " to play.\n";
        boardStream << "  _________________\n";
        for (int row_index = 7; row_index >= 0; --row_index) {
            const auto& row = board_[row_index];
            boardStream << row_index + 1 << "| ";
            for (const Piece& piece : row) {
                boardStream << piece.ToChar() << ' ';
            }
            boardStream << "|\n";
        }
        boardStream << "  -----------------\n";
        boardStream << "   a b c d e f g h \n";
        return boardStream.str();
    }


    void Position::RecomputeRemainingPieces() {
        piece_coordinates_.clear();
        for (uint8_t row = 0; row < 8; ++row) {
            for (uint8_t col = 0; col < 8; ++col) {
                Piece piece = board_[row][col];
                if (piece.IsEmpty()) {
                    continue;
                }
                piece_coordinates_.push_back({
                    .row = row,
                    .column = col
                });
            }
        }
    }
} // chess
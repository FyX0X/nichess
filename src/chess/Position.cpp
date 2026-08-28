//
// Created by martin on 27/08/2026.
//

#include "Position.h"

#include <iostream>
#include <sstream>
#include <ranges>


namespace chess {


#pragma region Constants

    constexpr std::vector<Coordinates> kOrthogonalDirections = {
        {.row = 1, .column = 0},
        {.row = 0, .column = 1},
        {.row = -1, .column = 0},
        {.row = 0, .column = -1}
    };

    constexpr std::vector<Coordinates> kDiagonalDirections = {
        {.row = 1, .column = 1},
        {.row = -1, .column = 1},
        {.row = -1, .column = -1},
        {.row = 1, .column = -1}
    };

    constexpr std::vector<Coordinates> kAllDirections = {
        {.row = 1, .column = 0},
        {.row = 0, .column = 1},
        {.row = -1, .column = 0},
        {.row = 0, .column = -1},
        {.row = 1, .column = 1},
        {.row = -1, .column = 1},
        {.row = -1, .column = -1},
        {.row = 1, .column = -1}
    };

    constexpr std::vector<Coordinates> kKnightDirections = {
        {.row = 2, .column = 1},
        {.row = 1, .column = 2},
        {.row = -1, .column = 2},
        {.row = -2, .column = 1},
        {.row = -2, .column = -1},
        {.row = -1, .column = -2},
        {.row = 1, .column = -2},
        {.row = 2, .column = -1}
    };


#pragma endregion

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


#pragma region Moves

    /**
     * @param piece The target piece
     * @return if the target piece could be taken if another piece was targeting it (not current player and not king).
     */
    bool Position::CouldPieceBeTaken(const Piece& piece) const {
        return (piece.player != active_player_ && piece.type != PieceType::King && piece.type != PieceType::None);
    }

    std::vector<Move> Position::GetLegalMoves() const {
        std::vector<Move> moves;
        for (const Coordinates& coord : piece_coordinates_) {
            moves.append_range(GetLegalMovesFromCoordinates(coord));
        }
    }


    std::vector<Move> Position::GetLegalMovesFromCoordinates(const Coordinates& from) const {
        Piece piece = GetPiece(from);
        if (piece.IsEmpty() || piece.player != active_player_) {
            return {};
        }

        switch (piece.type) {
            case PieceType::None:
                return {};
            case PieceType::Pawn:
                return GeneratePawnMoves(from);
            case PieceType::Knight:
                return GenerateKnightMoves(from);
            case PieceType::Bishop:
                return GenerateBishopMoves(from);
            case PieceType::Rook:
                return GenerateRookMoves(from);
            case PieceType::Queen:
                return GenerateQueenMoves(from);
            case PieceType::King:
                return GenerateKingMoves(from);
        }
    }

    std::vector<Move> Position::GenerateDirectMoves(const Coordinates &from, const std::vector<Coordinates> &directions) const {
        std::vector<Move> moves;
        for (const Coordinates& dir : directions) {
            Coordinates to = from + dir;
            Piece piece = GetPiece(to);
            if (piece.IsEmpty() || CouldPieceBeTaken(piece)) {
                moves.push_back({.from = from, .to = to});
            }
        }
        return moves;
    }

    std::vector<Move> Position::GenerateTranslationMoves(const Coordinates& from, const std::vector<Coordinates>& directions) const {
        std::vector<Move> moves;

        Player opponent = (active_player_ == Player::White) ? Player::Black : Player::White;

        int row = from.row;
        int col = from.column;


        // go in all directions
        for (const Coordinates& dir : directions) {
            for (Coordinates to = from + dir; to.IsValid(); to += dir ) {
                Piece piece = GetPiece(to);
                if (piece.IsEmpty()) {
                    moves.push_back({.from = from, .to = to});
                    continue;
                }
                if (CouldPieceBeTaken(piece)) {
                    moves.push_back( {.from = from, .to = to});
                    break;
                }
            }
        }
        return moves;
    }

    std::vector<Move> Position::GeneratePawnMoves(const Coordinates& from) const {
        // TODO
    }

    std::vector<Move> Position::GenerateKnightMoves(const Coordinates& from) const {
        return GenerateDirectMoves(from, kKnightDirections);
    }

    std::vector<Move> Position::GenerateBishopMoves(const Coordinates& from) const {
        return GenerateTranslationMoves(from, kDiagonalDirections);
    }

    std::vector<Move> Position::GenerateRookMoves(const Coordinates& from) const {
        return GenerateTranslationMoves(from, kOrthogonalDirections);
    }

    std::vector<Move> Position::GenerateQueenMoves(const Coordinates& from) const {
        return GenerateTranslationMoves(from, kAllDirections);
    }

    std::vector<Move> Position::GenerateKingMoves(const Coordinates& from) const {
        return GenerateDirectMoves(from, kAllDirections);

        // todo add castle
    }




#pragma endregion
} // chess
//
// Created by martin on 27/08/2026.
//

#include "Position.h"

#include <print>
#include <sstream>
#include <ranges>


namespace chess {


#pragma region Constants

    static const std::vector<Coordinates> kOrthogonalDirections = {
        {.row = 1, .column = 0},
        {.row = 0, .column = 1},
        {.row = -1, .column = 0},
        {.row = 0, .column = -1}
    };

    static const std::vector<Coordinates> kDiagonalDirections = {
        {.row = 1, .column = 1},
        {.row = -1, .column = 1},
        {.row = -1, .column = -1},
        {.row = 1, .column = -1}
    };

    static const std::vector<Coordinates> kAllDirections = {
        {.row = 1, .column = 0},
        {.row = 0, .column = 1},
        {.row = -1, .column = 0},
        {.row = 0, .column = -1},
        {.row = 1, .column = 1},
        {.row = -1, .column = 1},
        {.row = -1, .column = -1},
        {.row = 1, .column = -1}
    };

    static const std::vector<Coordinates> kKnightDirections = {
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

#pragma region Static Functions

    int Position::GetPawnStartingRank(Player player) {
        switch (player) {
            case Player::White:
                return 1; // #2
            case Player::Black:
                return 6; // #7
            default:
                return -1;
        }
    }

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
        std::println("[INFO] RecomputeRemainingPieces(): All pieces recomputed. ({} still on board)", piece_coordinates_.size());
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
            if (GetPiece(coord).player != active_player_) {
                continue;
            }
            moves.append_range(GetLegalMovesFromCoordinates(coord));
            std::println("generated {} moves.", GetLegalMovesFromCoordinates(coord).size()); // TODO remove
        }
        return moves;
    }


    std::vector<Move> Position::GetLegalMovesFromCoordinates(const Coordinates& from) const {
        Piece piece = GetPiece(from);
        if (piece.IsEmpty() || piece.player != active_player_) {
            return {};
        }

        std::println("generating moves from coord ({},{})", from.row, from.column);
        switch (piece.type) {
            case PieceType::Pawn:
                std::println("PAWN MOVES:");
                return GeneratePawnMoves(from);
            case PieceType::Knight:
                std::println("KNIGHT MOVES:");
                return GenerateKnightMoves(from);
            case PieceType::Bishop:
                std::println("BISHOP MOVES:");
                return GenerateBishopMoves(from);
            case PieceType::Rook:
                std::println("ROOK MOVES:");
                return GenerateRookMoves(from);
            case PieceType::Queen:
                std::println("QUEEN MOVES:");
                return GenerateQueenMoves(from);
            case PieceType::King:
                std::println("KING MOVES:");
                return GenerateKingMoves(from);
            default:
                return {};
        }
    }

    std::vector<Move> Position::GenerateDirectMoves(const Coordinates &from, const std::vector<Coordinates> &directions) const {
        std::vector<Move> moves;
        for (const Coordinates& dir : directions) {
            Coordinates to = from + dir;
            if (!to.IsValid()) {
                continue;
            }
            Piece piece = GetPiece(to);
            if (piece.IsEmpty() || CouldPieceBeTaken(piece)) {
                moves.push_back({.from = from, .to = to});
            }
        }
        return moves;
    }

    std::vector<Move> Position::GenerateTranslationMoves(const Coordinates& from, const std::vector<Coordinates>& directions) const {
        std::vector<Move> moves;
        // go in all directions

        for (const Coordinates& dir : directions) {
            // std::println("from: ({},{}), current dir: ({},{})", from.row, from.column, dir.row, dir.column);
            for (Coordinates to = from + dir; to.IsValid(); to += dir ) {
                // std::println("from: ({},{}), to: ({},{})", from.row, from.column, to.row, to.column);
                Piece piece = GetPiece(to);
                if (piece.IsEmpty()) {
                    moves.push_back({.from = from, .to = to});
                    // std::println("({},{}) is empty, adding move to list", to.row, to.column);
                    continue;
                }
                if (CouldPieceBeTaken(piece)) {
                    moves.push_back( {.from = from, .to = to});
                    // std::println("({},{}) is enemy, adding move to list", to.row, to.column);
                }
                break;
            }
        }
        return moves;
    }

    std::vector<Move> Position::GeneratePawnMoves(const Coordinates& from) const {
        // TODO
        std::println("pawn moves from ({},{}):", from.row, from.column);

        std::vector<Move> moves;

        // moving straight
        int dy = (active_player_ == Player::White) ? 1 : -1;
        Coordinates direction = {.row = dy, .column = 0};

        Coordinates to = from + direction;
        std::println("testing coord ({},{})", to.row, to.column);
        if (to.IsValid() && GetPiece(to).IsEmpty()) {
            moves.push_back({.from = from, .to = to});

            // second step if starting row and unobstructed.
            if (from.row == GetPawnStartingRank(active_player_)) {
                to += direction;
                std::println("testing coord ({},{})", to.row, to.column);
                if (to.IsValid() && GetPiece(to).IsEmpty()) {
                    moves.push_back({.from = from, .to = to});
                }
            }
        }

        // Taking pieces diagonally
        std::vector<Coordinates> take_directions = { {.row = dy, .column = -1}, {.row = dy, .column = 1}};
        for (Coordinates take_direction : take_directions) {
            to = from + take_direction;
            if (to.IsValid() && CouldPieceBeTaken(GetPiece(to))) {
                moves.push_back({.from = from, .to = to});
            }
        }

        std::println("pawn moves from ({},{}): count: {}", from.row, from.column, moves.size());
        return moves;
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
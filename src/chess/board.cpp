//
// Created by martin on 27/08/2026.
//

#include "board.h"

#include <cassert>
#include <print>
#include <sstream>
#include <ranges>
#include <utility>
#include "utility/ranges_utils.h"
#include "utility.h"

namespace chess {


    Board::Board() :
        irreversible_aspects_({}, std::nullopt, 0),
        board_()
    {
        EnsureLegalEnPassantSquare();
        EnsurePossibleCastlingRights();
        RecomputeRemainingPieces();
        // ComputeMovesForPlayer(Player::Black);

    }

    Board::Board(const std::array<std::array<Piece, 8>, 8> &board,
                       Player active_player, const CastlingRights& castling_rights,
                       std::optional<Coordinates> en_passant, int half_move_clock, int move_count) :

        irreversible_aspects_(castling_rights, en_passant, half_move_clock),
        board_(board),
        active_player_(active_player),
        move_count_(move_count) {


        EnsureLegalEnPassantSquare();
        EnsurePossibleCastlingRights();
        RecomputeRemainingPieces();
    }

    std::string Board::ToString() const {
        const std::optional<Coordinates>& en_passant = GetEnPassant();

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
        if (en_passant.has_value()) {
            boardStream << std::format("(en passant allowed at square: {})\n", en_passant.value());
        }
        return boardStream.str();
    }


    /* todo move to engine or delete
    std::string Board::GetTargetedSquaresString(Player player) const {
        std::stringstream boardStream;

        const auto& targets = GetPlayerTargetedSquares(player);

        // reverse order since row 0 is at bottom
        boardStream << '\n' << (player == Player::White ? "White" : "Black") << " Targets.\n";
        boardStream << "  _________________\n";
        for (int row_index = 7; row_index >= 0; --row_index) {
            const auto& row = targets[row_index];
            boardStream << row_index + 1 << "| ";
            for (const bool is_square_reachable : row) {
                boardStream << (is_square_reachable ?  "█" : " " ) << ' ';
            }
            boardStream << "|\n";
        }
        boardStream << "  -----------------\n";
        boardStream << "   a b c d e f g h \n";
        return boardStream.str();
    }*/


    void Board::RecomputeRemainingPieces() {
        white_piece_coordinates_.clear();
        black_piece_coordinates_.clear();
        for (uint8_t row = 0; row < 8; ++row) {
            for (uint8_t col = 0; col < 8; ++col) {
                Coordinates coordinates = { .row = row, .column = col };
                Piece piece = GetPiece(coordinates);
                if (piece.IsEmpty()) {
                    continue;
                }
                switch (piece.player) {
                    case Player::White:
                        white_piece_coordinates_.push_back(coordinates);
                        break;
                    case Player::Black:
                        black_piece_coordinates_.push_back(coordinates);
                        break;
                    default:
                        break;
                }
            }
        }
        // std::println("[INFO] RecomputeRemainingPieces(): All pieces recomputed. ({}/{}, w/b still on board)",
        //     white_piece_coordinates_.size(), black_piece_coordinates_.size());
    }

    void Board::EnsureLegalEnPassantSquare() {
        std::optional<Coordinates>& en_passant = GetEnPassant();
        if (!en_passant.has_value()) {
            return;
        }

        Coordinates coordinates = en_passant.value();
        Player player_who_moved;
        Coordinates moved_pawn_coords{-1, -1};
        if (coordinates.row == 2) {
            // white pawn
            player_who_moved = Player::White;
            moved_pawn_coords = { .row = 3, .column = coordinates.column };
        } else if (coordinates.row == 5) {
            player_who_moved = Player::Black;
            moved_pawn_coords = { .row = 4, .column = coordinates.column };
        } else {
            std::println(stderr, "[Warning] Position::EnsureLegalEnPassantSquare(): Invalid en_passant position: {}.", coordinates);
            en_passant.reset();
            return;
        }

        Piece pawn = GetPiece(moved_pawn_coords);
        if (pawn.type != PieceType::Pawn || pawn.player != player_who_moved) {
            std::println(stderr, "[Warning] Position::EnsureLegalEnPassantSquare(): Invalid en_passant piece: {}.", pawn.ToChar());
            en_passant.reset();
            return;
        }
    }

    void Board::EnsurePossibleCastlingRights() {
        CastlingRights& castling_rights = GetCastlingRights();
        if (GetPiece(kWhiteKingSquare) != kWhiteKing) {
            // std::println("White castling removed");
            castling_rights.white_kingside = false;
            castling_rights.white_queenside = false;
        }
        if (GetPiece(kWhiteRookKingsideSquare) != kWhiteRook) {
            //std::println("White king castling removed");
            castling_rights.white_kingside = false;
        }
        if (GetPiece(kWhiteRookQueensideSquare) != kWhiteRook) {

            //std::println("White queen castling removed");
            castling_rights.white_queenside = false;
        }

        if (GetPiece(kBlackKingSquare) != kBlackKing) {
            //std::println("Black castling removed");
            castling_rights.black_kingside = false;
            castling_rights.black_queenside = false;
        }
        if (GetPiece(kBlackRookKingsideSquare) != kBlackRook) {
            //std::println("Black king castling removed");
            castling_rights.black_kingside = false;
        }
        if (GetPiece(kBlackRookQueensideSquare) != kBlackRook) {
            //std::println("Black queen castling removed");
            castling_rights.black_queenside = false;
        }
    }



#pragma region Moves


    void Board::MakeMove(const Move &move) {
        // TODO
        IrreversibleAspects new_aspects = GetIrreversibleAspects();
        new_aspects.en_passant = std::nullopt;
        new_aspects.half_move_clock++;

        auto [from, to, double_pawn, en_passant, castle_kingside, castle_queenside, promotion_type, capture_type] = move;
        Piece moved_piece = GetPiece(from);
        Piece target_piece = GetPiece(to);

        // std::println("[INFO] MakeMove(): Making move: {}: {} -> {}", moved_piece.ToChar(), from, to);


        assert(moved_piece.player == active_player_);
        assert( ( move.capture_type == target_piece.type ) || ( move.en_passant && target_piece.IsEmpty() ) );

        SetPiece(from, kEmptyPiece);
        SetPiece(to, moved_piece);

        if (moved_piece.type == PieceType::Pawn) {
            new_aspects.half_move_clock = 0;
        }

        if (moved_piece.type == PieceType::King) {
            if (active_player_ == Player::White) {
                new_aspects.castling_rights.white_kingside = false;
                new_aspects.castling_rights.white_queenside = false;
            } else {
                new_aspects.castling_rights.black_kingside = false;
                new_aspects.castling_rights.black_queenside = false;
            }
        }
        if (moved_piece.type == PieceType::Rook) {
            if (from == kWhiteRookKingsideSquare) {
                new_aspects.castling_rights.white_kingside = false;
            } else if (from == kWhiteRookQueensideSquare) {
                new_aspects.castling_rights.white_queenside = false;
            } else if (from == kBlackRookKingsideSquare) {
                new_aspects.castling_rights.black_kingside = false;
            } else if (from == kBlackRookQueensideSquare) {
                new_aspects.castling_rights.black_queenside = false;
            }
        }

        if (double_pawn) {
            new_aspects.en_passant = from + GetPawnMoveDirection(active_player_); // one square from start pos.
        }

        if (capture_type != PieceType::None) {
            new_aspects.half_move_clock = 0;
        }

        if (en_passant) {
            // pawn taken en passant coords is on square after where we take.
            Coordinates pawn_taken_en_passant_coords = to + GetPawnMoveDirection(GetOtherPlayer(active_player_));
            assert(pawn_taken_en_passant_coords.IsValid() && GetPiece(pawn_taken_en_passant_coords).type == PieceType::Pawn);
            SetPiece(pawn_taken_en_passant_coords, kEmptyPiece);
        }

        if (castle_kingside) {
            Coordinates king_square{};
            Coordinates rook_square{};
            Coordinates new_rook_square{};
            if (active_player_ == Player::White) {
                king_square = kWhiteKingSquare;
                rook_square = kWhiteRookKingsideSquare;
                new_rook_square = kWhiteBishopKingsideSquare;

                new_aspects.castling_rights.white_kingside = false;
                new_aspects.castling_rights.white_queenside = false;
            } else {
                king_square = kBlackKingSquare;
                rook_square = kBlackRookKingsideSquare;
                new_rook_square = kBlackBishopKingsideSquare;

                new_aspects.castling_rights.black_kingside = false;
                new_aspects.castling_rights.black_queenside = false;
            }
            Piece rook_piece = GetPiece(rook_square);
            assert(from == king_square && rook_piece.type == PieceType::Rook);

            SetPiece(rook_square, kEmptyPiece);
            SetPiece(new_rook_square, rook_piece);
        }

        if (castle_queenside) {
            Coordinates king_square{};
            Coordinates rook_square{};
            Coordinates new_rook_square{};
            if (active_player_ == Player::White) {
                king_square = kWhiteKingSquare;
                rook_square = kWhiteRookQueensideSquare;
                new_rook_square = kWhiteQueenSquare;

                new_aspects.castling_rights.white_kingside = false;
                new_aspects.castling_rights.white_queenside = false;
            } else {
                king_square = kBlackKingSquare;
                rook_square = kBlackRookQueensideSquare;
                new_rook_square = kBlackQueenSquare;

                new_aspects.castling_rights.black_kingside = false;
                new_aspects.castling_rights.black_queenside = false;
            }
            Piece rook_piece = GetPiece(rook_square);
            assert(from == king_square && rook_piece.type == PieceType::Rook);

            SetPiece(rook_square, kEmptyPiece);
            SetPiece(new_rook_square, rook_piece);
        }

        if (promotion_type != PieceType::None) {
            SetPiece(to, { .type = promotion_type, .player = active_player_ });
        }
        irreversible_aspects_ = new_aspects;
        if (active_player_ == Player::Black) {
            move_count_++;
        }
        active_player_ = GetOtherPlayer(active_player_);

        RecomputeRemainingPieces();
    }


    void Board::UnmakeMove(const Move &move, const IrreversibleAspects& prev_aspects) {
        auto [from, to, double_pawn, en_passant, castle_kingside, castle_queenside, promotion_type, capture_type] = move;

        Player other_player = active_player_;
        active_player_ = GetOtherPlayer(active_player_);

        Piece moving_piece = GetPiece(to);
        /*std::println("[INFO] UnmakeMove(): Unmaking move: {} -> {}, moving piece: {}, capture type: {}, promotion type: {}, en_passant: {}, double_pawn: {}, castle_kingside: {}, castle_queenside: {}",
            from, to, moving_piece.ToChar(), std::to_underlying(capture_type), std::to_underlying(promotion_type), en_passant, double_pawn, castle_kingside, castle_queenside);
        if (active_player_ == Player::White) {
            std::println("[INFO] UnmakeMove(): active_player_ is White");
        } else {
            std::println("[INFO] UnmakeMove(): active_player_ is Black");
        }*/
        assert(moving_piece.player == active_player_);

        SetPiece(from, moving_piece);
        SetPiece(to, kEmptyPiece);

        // may be empty piece.
        Piece captured_piece = Piece{ .type = capture_type, .player = other_player};

        if (en_passant) {
            assert(capture_type == PieceType::Pawn);

            // pawn taken en passant coords is on square after where we take.
            Coordinates pawn_taken_en_passant_coords = to + GetPawnMoveDirection(other_player);
            assert(pawn_taken_en_passant_coords.IsValid() && GetPiece(pawn_taken_en_passant_coords).type == PieceType::None);
            SetPiece(pawn_taken_en_passant_coords, captured_piece);
        } else if (capture_type != PieceType::None) {
            SetPiece(to, captured_piece);
        }

        if (promotion_type != PieceType::None) {
            assert(moving_piece.type == promotion_type);

            // moved piece was a pawn before
            moving_piece.type = PieceType::Pawn;
            SetPiece(from, moving_piece);
        }

        if (castle_kingside) {

            Coordinates king_square{};
            Coordinates rook_square{};
            Coordinates new_rook_square{};
            if (active_player_ == Player::White) {
                king_square = kWhiteKingSquare;
                rook_square = kWhiteRookKingsideSquare;
                new_rook_square = kWhiteBishopKingsideSquare;
            } else {
                king_square = kBlackKingSquare;
                rook_square = kBlackRookKingsideSquare;
                new_rook_square = kBlackBishopKingsideSquare;
            }
            Piece rook_piece = GetPiece(new_rook_square);
            assert(from == king_square && rook_piece.type == PieceType::Rook);

            SetPiece(rook_square, rook_piece);
            SetPiece(new_rook_square, kEmptyPiece);
            // king already moved
        } else if (castle_queenside) {
            Coordinates king_square{};
            Coordinates rook_square{};
            Coordinates new_rook_square{};
            if (active_player_ == Player::White) {
                king_square = kWhiteKingSquare;
                rook_square = kWhiteRookQueensideSquare;
                new_rook_square = kWhiteQueenSquare;
            } else {
                king_square = kBlackKingSquare;
                rook_square = kBlackRookQueensideSquare;
                new_rook_square = kBlackQueenSquare;
            }
            Piece rook_piece = GetPiece(new_rook_square);
            assert(from == king_square && rook_piece.type == PieceType::Rook);

            SetPiece(rook_square, rook_piece);
            SetPiece(new_rook_square, kEmptyPiece);
            // king already moved
        }


        irreversible_aspects_ = prev_aspects;


        RecomputeRemainingPieces();
    }



#pragma endregion
} // chess
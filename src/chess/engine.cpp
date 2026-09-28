//
// Created by martin on 27/09/2026.
//

#include "engine.h"
#include "utility.h"

#include <cassert>
#include <print>
#include <utility>

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


#pragma region CheckingTargets

    bool Engine::IsPlayerInCheck(const Board &board, const Player &player) {

        Coordinates king_coordinates = {.row = -1, .column = -1};

        for (const Coordinates& coordinates : board.GetPlayerOccupiedSquares(player)) {
            if (board.GetPiece(coordinates).type == PieceType::King) {
                assert(board.GetPiece(coordinates).player == player); // should be always true if not there is a sync error with GetPlayerOccupiedSquares()
                king_coordinates = coordinates;
            }
        }
        assert(king_coordinates.IsValid() && "Player should always have a king!");

        return DoesPlayerTargetSquare(board, GetOtherPlayer(player), king_coordinates);
    }



    bool Engine::DoesPlayerTargetSquare(const Board& board, const Player& player, const Coordinates& target) {


        for (const Coordinates& coord : board.GetPlayerOccupiedSquares(player)) {
            bool does_piece_attack_target = false;
            Piece piece = board.GetPiece(coord);

            switch (piece.type) {
                case PieceType::Pawn:
                    does_piece_attack_target = CanPawnTargetSquare(board, coord, target, player);
                    break;
                case PieceType::Knight:
                    does_piece_attack_target = CanKnightTargetSquare(board, coord, target, player);
                    break;
                case PieceType::Bishop:
                    does_piece_attack_target = CanBishopTargetSquare(board, coord, target, player);
                    break;
                case PieceType::Rook:
                    does_piece_attack_target = CanRookTargetSquare(board, coord, target, player);
                    break;
                case PieceType::Queen:
                    does_piece_attack_target = CanQueenTargetSquare(board, coord, target, player);
                    break;
                case PieceType::King:
                    does_piece_attack_target = CanKingTargetSquare(board, coord, target, player);
                    break;
                case PieceType::None:
                default:
                    assert(false && "Invalid Piece in DoesPlayerTargetSquare()");
                    return false;
            }

            if (does_piece_attack_target) {
                return true;
            }

        }


        return false;
    }



    bool Engine::CanPawnTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player) {

    }
    bool Engine::CanKnightTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player) {
        const Coordinates diff = target - from;

        const int dy = abs(diff.row);
        const int dx = abs(diff.column);

        return std::max(dy, dx) == 2 && std::min(dy, dx) == 1;
    }
    bool Engine::CanBishopTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player) {

    }
    bool Engine::CanRookTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player) {

    }
    bool Engine::CanQueenTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player) {

    }

    bool Engine::CanKingTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, Player player) {
        Coordinates diff = target - from;

        return abs(diff.row) <= 1 && abs(diff.column) <= 1;
    }

#pragma endregion


#pragma region Move Generation


    std::vector<Move> Engine::GetLegalMoves(const Board& board) {
        std::vector<Move> moves;

        const Player player = board.GetActivePlayer();

        for (const Coordinates& coord : board.GetPlayerOccupiedSquares(player)) {
            assert(board.GetPiece(coord).player == player);
            moves.append_range(GeneratePseudoLegalMovesFromCoordinates(board, coord, player));
        }
        moves.append_range(GenerateCastleMoves(board, player));
        return moves;
    }



    std::vector<Move> Engine::GeneratePseudoLegalMovesFromCoordinates(const Board& board, const Coordinates &from, Player player) {

        Piece piece = board.GetPiece(from);
        assert(!piece.IsEmpty() && piece.player == player && "Invalid player or piece.");
        std::vector<Move> moves;
        switch (piece.type) {
            case PieceType::Pawn:
                // std::println("PAWN MOVES:");
                return GeneratePawnMoves(board, from, player);
            case PieceType::Knight:
                // std::println("KNIGHT MOVES:");
                return GenerateKnightMoves(board, from, player);
            case PieceType::Bishop:
                // std::println("BISHOP MOVES:");
                return GenerateBishopMoves(board, from, player);
            case PieceType::Rook:
                // std::println("ROOK MOVES:");
                return GenerateRookMoves(board, from, player);
            case PieceType::Queen:
                // std::println("QUEEN MOVES:");
                return GenerateQueenMoves(board, from, player);
            case PieceType::King:
                // std::println("KING MOVES:");
                return GenerateKingMoves(board, from, player);
            default:
                std::println(stderr, "[ERROR] Position::GeneratePseudoLegalMovesForPlayer(): Unrecognized piece type, {}", std::to_underlying(piece.type));
                return std::vector<Move>{};
        }
    }

    std::vector<Move> Engine::GenerateDirectMoves(const Board& board, const Coordinates &from, const std::vector<Coordinates> &directions,
                                                    Player player) {
        assert(board.GetPiece(from).player == player);

        std::vector<Move> moves;

        int count = 0;
        for (const Coordinates& dir : directions) {
            Coordinates to = from + dir;
            if (!to.IsValid()) {
                continue;
            }
            // AddTargetedSquareToPlayer(to, player);
            Piece piece = board.GetPiece(to);
            if (piece.IsEmpty() || CouldPlayerTakePiece(player, piece)) {
                Move move(from, to);
                move.capture_type = piece.type;
                moves.push_back(move);
                count++;
            }
        }
        // std::println("Added '{}' moves from {}", count, from);
        return moves;
    }

    std::vector<Move> Engine::GenerateTranslationMoves(const Board& board, const Coordinates &from,
                                                         const std::vector<Coordinates> &directions, Player player) {

        assert(board.GetPiece(from).player == player);

        std::vector<Move> moves;

        int count = 0;
        // go in all directions
        for (const Coordinates& dir : directions) {
            for (Coordinates to = from + dir; to.IsValid(); to += dir ) {
                // AddTargetedSquareToPlayer(to, player);
                Piece piece = board.GetPiece(to);
                if (piece.IsEmpty()) {
                    Move move(from, to);
                    moves.push_back(move);
                    count++;
                    continue;
                }
                if (CouldPlayerTakePiece(player, piece)) {
                    Move move(from, to);
                    move.capture_type = piece.type;
                    moves.push_back(move);
                    count++;
                }
                break;
            }
        }

        // std::println("Added '{}' moves from {}", count, from);
        return moves;
    }


    std::vector<Move> Engine::GeneratePromotionMoves(const Board& board, const Move &move, Player player) {
        assert(move.to.row == GetPieceStartingRank(GetOtherPlayer(player)));
        assert(!move.en_passant);
        assert(!move.double_pawn);

        std::vector<Move> moves;
        Move promotion = move;
        for (const PieceType promoted_type : kPromotableTypes) {
            promotion.promotion_type = promoted_type;
            moves.push_back(promotion);
        }
        if (move.capture_type != PieceType::None) {
            // AddTargetedSquareToPlayer(move.to, player);
        }
        return moves;
    }

    std::vector<Move> Engine::GeneratePawnMoves(const Board& board, const Coordinates &from, Player player) {

        assert(board.GetPiece(from).player == player);

        std::vector<Move> moves;

        int count = 0;
        // moving straight
        int dy = (player == Player::White) ? 1 : -1;
        Coordinates direction = {.row = dy, .column = 0};

        // if currently on row before last -> promotion when moving pawn.
        bool promotion = from.row == GetPawnStartingRank(GetOtherPlayer(player));

        Coordinates to = from + direction;
        if (to.IsValid() && board.GetPiece(to).IsEmpty()) {
            Move move(from, to);
            if (promotion) {
                moves.append_range(GeneratePromotionMoves(board, move, player));
                count += kPromotableTypes.size();
            } else {
                moves.push_back(move);
                count++;
            }

            // second step if starting row and unobstructed.
            if (from.row == GetPawnStartingRank(player)) {
                to += direction;
                if (to.IsValid() && board.GetPiece(to).IsEmpty()) {
                    move = Move(from, to);
                    move.double_pawn = true;
                    moves.push_back(move);
                    count++;
                }
            }
        }

        // Taking pieces diagonally
        const std::optional<Coordinates>& en_passant = board.GetEnPassant();
        std::vector<Coordinates> take_directions = { {.row = dy, .column = -1}, {.row = dy, .column = 1}};
        for (Coordinates take_direction : take_directions) {
            to = from + take_direction;
            if (!to.IsValid()) {
                break;
            }
            bool is_en_passant = en_passant.has_value() && en_passant.value() == to;
            Piece target_piece = board.GetPiece(to);
            if (CouldPlayerTakePiece(player, target_piece) || is_en_passant) {
                Move move(from, to);
                move.en_passant = is_en_passant;
                move.capture_type = is_en_passant ? PieceType::Pawn : target_piece.type;

                if (promotion) {
                    moves.append_range(GeneratePromotionMoves(board, move, player));
                    count += kPromotableTypes.size();
                } else {
                    moves.push_back(move);
                    // AddTargetedSquareToPlayer(to, player);
                    count++;
                }
            }
        }
        // std::println("Added '{}' moves from {}", count, from);
        return moves;
    }

    std::vector<Move> Engine::GenerateKnightMoves(const Board& board, const Coordinates& from, Player player) {
        return GenerateDirectMoves(board, from, kKnightDirections, player);
    }

    std::vector<Move> Engine::GenerateBishopMoves(const Board& board, const Coordinates& from, Player player) {
        return GenerateTranslationMoves(board, from, kDiagonalDirections, player);
    }

    std::vector<Move> Engine::GenerateRookMoves(const Board& board, const Coordinates& from, Player player) {
        return GenerateTranslationMoves(board, from, kOrthogonalDirections, player);
    }

    std::vector<Move> Engine::GenerateQueenMoves(const Board& board, const Coordinates& from, Player player) {
        return GenerateTranslationMoves(board, from, kAllDirections, player);
    }

    std::vector<Move> Engine::GenerateKingMoves(const Board& board, const Coordinates& from, Player player) {
        return GenerateDirectMoves(board, from, kAllDirections, player);
    }

    std::vector<Move> Engine::GenerateCastleMoves(const Board& board, const Player player) {
        // TODO handle castle moves correctly when implementing move function.

        std::vector<Move> moves;

        const CastlingRights& castling_rights = board.GetCastlingRights();

        Coordinates from = { .row = GetPieceStartingRank(player), .column = 4 };

        Piece king = board.GetPiece(from);

        // std::println("generate castle moves, current opponent reach = {}",
        //     GetReachableSquaresString(GetOtherPlayer(player)));

        if (player == Player::White && castling_rights.white_kingside
            || player == Player::Black && castling_rights.black_kingside) {

            assert(king.type == PieceType::King);
            assert(king.player == player);

            bool castle_available = true;
            for (int col = 5; col < 7; ++col) {
                Coordinates coords = { .row = from.row, .column = col };
                if (!board.GetPiece(coords).IsEmpty() || DoesPlayerTargetSquare(board, GetOtherPlayer(player), coords)) {
                    castle_available = false;
                    break;
                }
            }
            if (castle_available) {
                Move move(from, {.row = from.row, .column = from.column + 2});
                move.castle_kingside = true;
                moves.push_back(move);
                // std::println("Add kingside castle for player: {}", PlayerToString(player));
            }
        }
        // TODO try to remove duplicate code for kingside / queenside
        if (player == Player::White && castling_rights.white_queenside
            || player == Player::Black && castling_rights.black_queenside) {

            assert(king.type == PieceType::King);
            assert(king.player == player);

            bool castle_available = true;
            for (int col = 3; col > 0; --col) {
                Coordinates coords = { .row = from.row, .column = col };
                if (!board.GetPiece(coords).IsEmpty() || DoesPlayerTargetSquare(board, GetOtherPlayer(player), coords)) {
                    castle_available = false;
                    break;
                }
            }
            if (castle_available) {
                Move move(from, {.row = from.row, .column = from.column - 2});
                move.castle_queenside = true;
                moves.push_back(move);
                // std::println("Add queenside castle for player: {}", PlayerToString(player));
            }
        }

        return moves;
    }

#pragma endregion

} // chess
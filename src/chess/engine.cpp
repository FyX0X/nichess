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

    bool Engine::CanPawnTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, const Player player) {
        Coordinates left = {.row = 0, .column = -1};
        Coordinates right = {.row = 0, .column = 1};

        left += GetPawnMoveDirection(player);
        right += GetPawnMoveDirection(player);

        const Coordinates diff = target - from;
        return diff == left || diff == right;
    }

    bool Engine::CanKnightTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, const Player player) {
        const Coordinates diff = target - from;

        const int dy = abs(diff.row);
        const int dx = abs(diff.column);

        return std::max(dy, dx) == 2 && std::min(dy, dx) == 1;
    }

    bool Engine::CanBishopTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, const Player player) {

        const bool diagonal_up_left = (target.row + target.column) == (from.row + from.column);
        const bool diagonal_up_right = (target.row - target.column) == (from.row - from.column);

        if (diagonal_up_left == diagonal_up_right) { // either both false (cannot target) or both true (cannot protect own square).
            return  false;
        }

        Coordinates direction;

        if (diagonal_up_left) {
            const int dy = target.row > from.row ? 1 : -1;
            direction = {.row = dy, .column = -dy};
        } else {
            const int dy = target.row > from.row ? 1 : -1;
            direction = {.row = dy, .column = dy};
        }

        Coordinates square = from + direction;
        while (square.IsValid()) {

            if (square == target) {
                return true;
            }

            if (!board.GetPiece(square).IsEmpty()) {
                break;
            }

            square += direction;
        }
        return false;
    }

    bool Engine::CanRookTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, const Player player) {

        const bool same_column = target.column == from.column;
        const bool same_row = target.row == from.row;

        if (same_column == same_row) { // either both false (cannot target) or both true (cannot protect own square).
            return  false;
        }

        Coordinates direction;

        if (same_column) {
            const int dy = target.row > from.row ? 1 : -1;
            direction = {.row = dy, .column = 0};
        } else {
            const int dx = target.column > from.column ? 1 : -1;
            direction = {.row = 0, .column = dx};
        }

        Coordinates square = from + direction;
        while (square.IsValid()) {

            if (square == target) {
                return true;
            }

            if (!board.GetPiece(square).IsEmpty()) {
                break;
            }

            square += direction;
        }
        return false;
    }

    bool Engine::CanQueenTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, const Player player) {
        return CanRookTargetSquare(board, from, target, player) || CanBishopTargetSquare(board, from, target, player);
    }

    bool Engine::CanKingTargetSquare(const Board& board, const Coordinates& from, const Coordinates& target, const Player player) {
        Coordinates diff = target - from;

        return abs(diff.row) <= 1 && abs(diff.column) <= 1;
    }

#pragma endregion


#pragma region Move Generation


    std::vector<Move> Engine::GetPseudoLegalMoves(const Board& board) {
        std::vector<Move> moves;

        const Player player = board.GetActivePlayer();

        for (const Coordinates& coord : board.GetPlayerOccupiedSquares(player)) {
            assert(board.GetPiece(coord).player == player);
            moves.append_range(GeneratePseudoLegalMovesFromCoordinates(board, coord, player));
        }
        moves.append_range(GenerateCastleMoves(board, player));
        return moves;
    }

    std::vector<Move> Engine::GetLegalMoves(const Board& board) {
        const std::vector<Move> pseudo_legal_moves = GetPseudoLegalMoves(board);
        std::vector<Move> legal_moves;
        legal_moves.reserve(pseudo_legal_moves.size());

        Board copy = board;

        Player other = GetOtherPlayer(copy.GetActivePlayer());
        IrreversibleAspects aspects = copy.GetIrreversibleAspects();

        for (const Move move : pseudo_legal_moves)
        {
            copy.MakeMove(move);
            if (!Engine::IsPlayerInCheck(copy, other)) { 
                legal_moves.push_back(move);
            }
            copy.UnmakeMove(move, aspects);
        }
        return legal_moves;
    }

    std::vector<Move> Engine::GetOpponentLegalMoves(const Board &board) {
        Board copy = board;
        copy.SwitchPlayer();
        return GetPseudoLegalMoves(copy);
    }


    Move Engine::SearchBestMove(const Board &board) {
        const std::vector<Move> moves = GetLegalMoves(board); // TODO switch to pseudo ?

        Move best_move;
        float best_evaluation = -std::numeric_limits<float>::infinity();

        Board copy = board;

        const IrreversibleAspects aspects = copy.GetIrreversibleAspects();

        for (const Move& move : moves) {
            copy.MakeMove(move);
            const float evaluation = Negamax(copy, 4);
            copy.UnmakeMove(move, aspects);

            if (evaluation > best_evaluation) {
                best_evaluation = evaluation;
                best_move = move;
            }
        }

        return best_move;
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
                continue;
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
        if (DoesPlayerTargetSquare(board, GetOtherPlayer(player), from)) {
            return moves;
        }

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
                if (!board.GetPiece(coords).IsEmpty() || (col > 1 && DoesPlayerTargetSquare(board, GetOtherPlayer(player), coords))) {
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


#pragma region Search

    /*function negamax(node, depth) is
    if depth = 0 or node is a terminal node then
        return evaluatePosition() // From current player's perspective
    value := −∞
    for each child of node do
        value := max(value, −negamax(child, depth − 1))
    return value*/

    float Engine::Negamax(Board &node, const int depth) {

        const std::vector<Move> moves = GetLegalMoves(node);

        if (moves.empty()) {
            return EvaluateEndScore(node);
        }
        if (depth == 0) {
            return EvaluateHeuristic(node);
        }

        float value = -std::numeric_limits<float>::infinity();
        const IrreversibleAspects aspects = node.GetIrreversibleAspects();
        for (const Move& move : moves) {
            node.MakeMove(move);
            value = std::max(value, -Negamax(node, depth - 1));
            node.UnmakeMove(move, aspects);
        }
        return value;
    }

    /**
     * Compute the position score from the current player POV at the end of the game
     * @param board
     * @return
     */
    float Engine::EvaluateEndScore(const Board &board) {
        if (IsPlayerInCheck(board, board.GetActivePlayer())) {
            return -std::numeric_limits<float>::infinity(); // in check and no available move => just got checkmated
        }

        return 0; // stalemate

    }

    /**
     * Compute the position score from the current player POV using following
     * Claude Shannon's heuristic (modified):
     * f(p) = 9(Q-Q')
     *      + 5(R-R')
     *      + 3(B-B' + N-N')
     *      + 1(P-P')
     *      - 0.5(D-D' + S-S' + I-I')
     *      + 0.1(M-M') + ...
     *
     * KQRBNP = number of kings, queens, rooks, bishops, knights and pawns
     * D,S,I = doubled, backward and isolated pawns
     * M = Mobility (the number of legal moves)
     * @param board The board to evaluate
     * @return the computed score.
     */
    float Engine::EvaluateHeuristic(const Board &board) {
        const Player player = board.GetActivePlayer();
        const Player other_player = GetOtherPlayer(player);

        float score = 0;

        for (const Coordinates& coord : board.GetPlayerOccupiedSquares(player)) {
            score += static_cast<float>(board.GetPiece(coord).GetValue());
        }
        for (const Coordinates& coord : board.GetPlayerOccupiedSquares(player)) {
            score -= static_cast<float>(board.GetPiece(coord).GetValue());
        }

        score -= 0.5f * static_cast<float>(DoubledBackwardIsolatedCount(board, player) - DoubledBackwardIsolatedCount(board, other_player));
        score += 0.1f * static_cast<float>(GetLegalMoves(board).size() - GetOpponentLegalMoves(board).size());

        return score;
    }

    int Engine::DoubledBackwardIsolatedCount(const Board &board, const Player player) {
        int pawns_per_col[8] = {0};
        int least_advanced_row[8];
        for (int &row : least_advanced_row) {
            row = player == Player::White ? 7 : 0;
        }

        const Coordinates dir = GetPawnMoveDirection(player);

        std::vector<Coordinates> pawns_coords;

        for (const Coordinates& coord : board.GetPlayerOccupiedSquares(player)) {
            if (board.GetPiece(coord).type != PieceType::Pawn) {
                continue;
            }

            pawns_per_col[coord.column]++;
            pawns_coords.push_back(coord);

            if (player == Player::White) {
                least_advanced_row[coord.column] = std::min(least_advanced_row[coord.column], coord.row);
            } else {
                least_advanced_row[coord.column] = std::max(least_advanced_row[coord.column], coord.row);
            }
        }

        int doubled = 0;
        for (const int pawns : pawns_per_col) {
            doubled += pawns > 1;
        }


        int backward = 0;
        for (const Coordinates& coord : pawns_coords) {

            // check if the pawn is blocked.
            const Coordinates front = coord + dir;
            if (!board.GetPiece(front).IsEmpty() || DoesPlayerTargetSquare(board, GetOtherPlayer(player), front)) {
                // pawn is blocked.

                // is it backward ?
                const int row = coord.row;

                const bool does_left_pawn_exists = coord.column > 0 && pawns_per_col[coord.column - 1] > 0; // must exist to be ahead
                const bool does_right_pawn_exists = coord.column < 7 && pawns_per_col[coord.column + 1] > 0; // must exist to be ahead
                bool is_left_ahead;
                bool is_right_ahead;
                if (player == Player::White) {
                    is_left_ahead = does_left_pawn_exists && least_advanced_row[coord.column - 1] > row;
                    is_right_ahead = does_right_pawn_exists && least_advanced_row[coord.column + 1] > row;
                } else {
                    is_left_ahead = does_left_pawn_exists && least_advanced_row[coord.column - 1] < row;
                    is_right_ahead = does_right_pawn_exists && least_advanced_row[coord.column + 1] < row;
                }

                if (is_left_ahead && is_right_ahead) {
                    backward++;
                }
            }
        }

        int isolated = 0;
        isolated += (pawns_per_col[0] > 0 && pawns_per_col[1] == 0) ? pawns_per_col[0] : 0;
        isolated += (pawns_per_col[7] > 0 && pawns_per_col[6]== 0) ? pawns_per_col[7] : 0;
        for (int col = 1; col < 7; ++col) {
            isolated += (pawns_per_col[col] > 0 && pawns_per_col[col - 1] == 0 && pawns_per_col[col + 1] == 0) ? pawns_per_col[col] : 0;
        }

        return doubled + backward + isolated;
    }

#pragma endregion

} // chess
//
// Created by martin on 27/08/2026.
//

#include "position.h"

#include <cassert>
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

    int Position::GetPieceStartingRank(Player player) {
        switch (player) {
            case Player::White:
                return 0;
            case Player::Black:
                return 7;
            default:
                return -1;
        }
    }

    Player Position::GetOtherPlayer(Player player) {
        switch (player) {
            case Player::White:
                return Player::Black;
            case Player::Black:
                return Player::White;
            default:
                return Player::None;
        }
    }

    /**
     * @param piece The target piece
     * @return if the target piece could be taken if another piece was targeting it (not current player and not king).
     */
    bool Position::CouldPlayerTakePiece(const Player player, const Piece& piece) {
        return (piece.player == GetOtherPlayer(player) && piece.type != PieceType::King && piece.type != PieceType::None);
    }

#pragma endregion

    Position::Position() :
        board_()
    {
        EnsureLegalEnPassantSquare();
        EnsurePossibleCastlingRights();
        RecomputeRemainingPieces();
        ComputeMovesForPlayer(Player::White);
        ComputeMovesForPlayer(Player::Black);

    }

    Position::Position(const std::array<std::array<Piece, 8>, 8> &board,
                       Player active_player, const CastlingRights& castling_rights,
                       std::optional<Coordinates> en_passant, int half_move_clock, int move_count) :
        board_(board),
        active_player_(active_player),
        castling_rights_(castling_rights),
        en_passant_(en_passant),
        half_move_clock_(half_move_clock),
        move_count_(move_count) {

        EnsureLegalEnPassantSquare();
        EnsurePossibleCastlingRights();
        RecomputeRemainingPieces();
        ComputeMovesForPlayer(Player::White);
        ComputeMovesForPlayer(Player::Black);
        ComputeMovesForPlayer(Player::White); // recompute with updated info // todo change this
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
        if (en_passant_.has_value()) {
            boardStream << std::format("(en passant allowed at square: {})\n", en_passant_.value());
        }
        return boardStream.str();
    }


    std::string Position::GetReachableSquaresString(Player player) const {
        std::stringstream boardStream;

        const auto& reachable = GetPlayerReachableSquares(player);

        // reverse order since row 0 is at bottom
        boardStream << '\n' << (player == Player::White ? "White" : "Black") << " Reach.\n";
        boardStream << "  _________________\n";
        for (int row_index = 7; row_index >= 0; --row_index) {
            const auto& row = reachable[row_index];
            boardStream << row_index + 1 << "| ";
            for (const bool is_square_reachable : row) {
                boardStream << (is_square_reachable ?  "█" : " " ) << ' ';
            }
            boardStream << "|\n";
        }
        boardStream << "  -----------------\n";
        boardStream << "   a b c d e f g h \n";
        return boardStream.str();
    }


    void Position::RecomputeRemainingPieces() {
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
        std::println("[INFO] RecomputeRemainingPieces(): All pieces recomputed. ({}/{}, w/b still on board)",
            white_piece_coordinates_.size(), black_piece_coordinates_.size());
    }

    void Position::EnsureLegalEnPassantSquare() {
        if (!en_passant_.has_value()) {
            return;
        }

        Coordinates coordinates = en_passant_.value();
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
            en_passant_.reset();
            return;
        }

        Piece pawn = GetPiece(moved_pawn_coords);
        if (pawn.type != PieceType::Pawn || pawn.player != player_who_moved) {
            std::println(stderr, "[Warning] Position::EnsureLegalEnPassantSquare(): Invalid en_passant piece: {}.", pawn.ToChar());
            en_passant_.reset();
            return;
        }
    }

    void Position::EnsurePossibleCastlingRights() {
        if (GetPiece(kWhiteKingSquare) != kWhiteKing) {
            std::println("White castling removed");
            castling_rights_.white_kingside = false;
            castling_rights_.white_queenside = false;
        }
        if (GetPiece(kWhiteRookKingsideSquare) != kWhiteRook) {
            std::println("White king castling removed");
            castling_rights_.white_kingside = false;
        }
        if (GetPiece(kWhiteRookQueensideSquare) != kWhiteRook) {

            std::println("White queen castling removed");
            castling_rights_.white_queenside = false;
        }

        if (GetPiece(kBlackKingSquare) != kBlackKing) {
            std::println("Black castling removed");
            castling_rights_.black_kingside = false;
            castling_rights_.black_queenside = false;
        }
        if (GetPiece(kBlackRookKingsideSquare) != kBlackRook) {
            std::println("Black king castling removed");
            castling_rights_.black_kingside = false;
        }
        if (GetPiece(kBlackRookQueensideSquare) != kBlackRook) {
            std::println("Black queen castling removed");
            castling_rights_.black_queenside = false;
        }
    }



    std::vector<Move>& Position::GetLegalMovesForPlayer(const Player player) {
        switch (player) {
            case Player::White:
                return white_moves_;
            case Player::Black:
                return black_moves_;
            default:
                break;
        }

        std::println(stderr, "[ERROR] Position::GetLegalMovesForPlayer(): Invalid player");
        std::unreachable();
    }

    const std::vector<Move>& Position::GetLegalMovesForPlayer(const Player player) const {
        switch (player) {
            case Player::White:
                return white_moves_;
            case Player::Black:
                return black_moves_;
            default:
                break;
        }

        std::println(stderr, "[ERROR] Position::GetLegalMovesForPlayer(): Invalid player");
        std::unreachable();
    }




#pragma region Moves

    const std::vector<Move>& Position::GetActivePlayerMoves() const {
        assert(active_player_ != Player::None && "[ERROR] Position::GetLegalMoves() Active player was None");
        return GetLegalMovesForPlayer(active_player_);
    }



    void Position::ComputeMovesForPlayer(Player player) {

        GetLegalMovesForPlayer(player).clear();
        GetPlayerReachableSquares(player).fill(std::array<bool, 8>{false});

        for (const Coordinates& coord : GetPlayerOccupiedSquares(player)) {
            assert(GetPiece(coord).player == player);
            GenerateLegalMovesFromCoordinates(coord, player);
        }
        GenerateCastleMoves(player);


    }

    // TODO consider changing args to P, std::vector<Move>& player_moves, std::array<std::array<bool, 8>, 8>& player_reachable instead of just Player playre
    void Position::AddMoveToPlayer(const Move move, const Player player) {
        GetLegalMovesForPlayer(player).push_back(move);
        GetPlayerReachableSquares(player)[move.to.row][move.to.column] = true;
    }

    void Position::AddMoveToPlayer(const Coordinates from, const Coordinates to, const Player player) {
        GetLegalMovesForPlayer(player).push_back({ .from = from, .to = to });
        GetPlayerReachableSquares(player)[to.row][to.column] = true;
    }


    void Position::GenerateLegalMovesFromCoordinates(const Coordinates& from, Player player) {

        Piece piece = GetPiece(from);
        assert(!piece.IsEmpty() && piece.player == player && "Invalid player or piece.");

        switch (piece.type) {
            case PieceType::Pawn:
                // std::println("PAWN MOVES:");
                return GeneratePawnMoves(from, player);
            case PieceType::Knight:
                // std::println("KNIGHT MOVES:");
                return GenerateKnightMoves(from, player);
            case PieceType::Bishop:
                // std::println("BISHOP MOVES:");
                return GenerateBishopMoves(from, player);
            case PieceType::Rook:
                // std::println("ROOK MOVES:");
                return GenerateRookMoves(from, player);
            case PieceType::Queen:
                // std::println("QUEEN MOVES:");
                return GenerateQueenMoves(from, player);
            case PieceType::King:
                // std::println("KING MOVES:");
                return GenerateKingMoves(from, player);
            default:
                std::println(stderr, "[ERROR] Position::GenerateLegalMovesForPlayer(): Unrecognized piece type, {}", std::to_underlying(piece.type));
                return;
        }
    }

    void Position::GenerateDirectMoves(const Coordinates &from, const std::vector<Coordinates> &directions, Player player) {
        Piece moving_piece = GetPiece(from);
        assert(GetPiece(from).player == player);
        int count = 0;
        for (const Coordinates& dir : directions) {
            Coordinates to = from + dir;
            if (!to.IsValid()) {
                continue;
            }

            Piece piece = GetPiece(to);
            if (piece.IsEmpty() || CouldPlayerTakePiece(player, piece)) {
                AddMoveToPlayer(from, to, player);
                count++;
            }
        }
        // std::println("Added '{}' moves from {}", count, from);
    }

    void Position::GenerateTranslationMoves(const Coordinates& from, const std::vector<Coordinates>& directions, Player player) {

        assert(GetPiece(from).player == player);

        int count = 0;
        // go in all directions
        for (const Coordinates& dir : directions) {
            for (Coordinates to = from + dir; to.IsValid(); to += dir ) {
                Piece piece = GetPiece(to);
                if (piece.IsEmpty()) {
                    AddMoveToPlayer(from, to, player);
                    count++;
                    continue;
                }
                if (CouldPlayerTakePiece(player, piece)) {
                    AddMoveToPlayer( from, to, player);
                    count++;
                }
                break;
            }
        }

        // std::println("Added '{}' moves from {}", count, from);
    }

    void Position::GeneratePawnMoves(const Coordinates& from, Player player) {
        // TODO

        assert(GetPiece(from).player == player);
        int count = 0;
        // moving straight
        int dy = (player == Player::White) ? 1 : -1;
        Coordinates direction = {.row = dy, .column = 0};

        Coordinates to = from + direction;
        if (to.IsValid() && GetPiece(to).IsEmpty()) {
            AddMoveToPlayer(from, to, player);
            count++;

            // second step if starting row and unobstructed.
            if (from.row == GetPawnStartingRank(player)) {
                to += direction;
                if (to.IsValid() && GetPiece(to).IsEmpty()) {
                    AddMoveToPlayer(from, to, player);
                    count++;
                }
            }
        }

        // Taking pieces diagonally
        std::vector<Coordinates> take_directions = { {.row = dy, .column = -1}, {.row = dy, .column = 1}};
        for (Coordinates take_direction : take_directions) {
            to = from + take_direction;
            if (to.IsValid() && CouldPlayerTakePiece(player, GetPiece(to)) || (en_passant_.has_value() && en_passant_.value() == to)) {
                AddMoveToPlayer(from, to, player);
                count++;
            }
        }

        // std::println("Added '{}' moves from {}", count, from);
    }

    void Position::GenerateKnightMoves(const Coordinates& from, Player player) {
        GenerateDirectMoves(from, kKnightDirections, player);
    }

    void Position::GenerateBishopMoves(const Coordinates& from, Player player) {
        GenerateTranslationMoves(from, kDiagonalDirections, player);
    }

    void Position::GenerateRookMoves(const Coordinates& from, Player player) {
        GenerateTranslationMoves(from, kOrthogonalDirections, player);
    }

    void Position::GenerateQueenMoves(const Coordinates& from, Player player) {
        GenerateTranslationMoves(from, kAllDirections, player);
    }

    void Position::GenerateKingMoves(const Coordinates& from, Player player) {
        GenerateDirectMoves(from, kAllDirections, player);

        // todo add castle
    }

    void Position::GenerateCastleMoves(const Player player) {
        // TODO handle castle moves correctly when implementing move function.

        Coordinates from = { .row = GetPieceStartingRank(player), .column = 4 };

        Piece king = GetPiece(from);

        // std::println("generate castle moves, current opponent reach = {}",
        //     GetReachableSquaresString(GetOtherPlayer(player)));

        if (player == Player::White && castling_rights_.white_kingside
            || player == Player::Black && castling_rights_.black_kingside) {

            assert(king.type == PieceType::King);
            assert(king.player == player);

            bool castle_available = true;
            for (int col = 5; col < 7; ++col) {
                Coordinates coords = { .row = from.row, .column = col };
                std::println("testing {}", coords);
                if (!GetPiece(coords).IsEmpty() || DoesPlayerTargetCoordinates(GetOtherPlayer(player), coords)) {
                    castle_available = false;
                    break;
                }
            }
            if (castle_available) {
                AddMoveToPlayer(from, {.row = from.row, .column = from.column + 2}, player);
                std::println("Add kingside castle for player: {}", PlayerToString(player));
            }
        }
        // TODO try to remove duplicate code for kingside / queenside
        if (player == Player::White && castling_rights_.white_queenside
            || player == Player::Black && castling_rights_.black_queenside) {

            assert(king.type == PieceType::King);
            assert(king.player == player);

            bool castle_available = true;
            for (int col = 3; col > 0; --col) {
                Coordinates coords = { .row = from.row, .column = col };
                if (!GetPiece(coords).IsEmpty() || DoesPlayerTargetCoordinates(GetOtherPlayer(player), coords)) {
                    castle_available = false;
                    break;
                }
            }
            if (castle_available) {
                AddMoveToPlayer(from, {.row = from.row, .column = from.column - 2}, player);
                std::println("Add queenside castle for player: {}", PlayerToString(player));
            }
        }
    }

    bool Position::DoesPlayerTargetCoordinates(const Player player, const Coordinates coordinates) const {
        const bool is_targeted = GetPlayerReachableSquares(player)[coordinates.row][coordinates.column];
        // std::println("does {} target {} = {}", PlayerToString(player), coordinates, is_targeted);
        return is_targeted;
    }




#pragma endregion
} // chess
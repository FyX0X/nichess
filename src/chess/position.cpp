//
// Created by martin on 27/08/2026.
//

#include "position.h"

#include <cassert>
#include <print>
#include <sstream>
#include <ranges>
#include <utility>
#include "utility/ranges_utils.h"


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
     * @param player The player that checks if could take some piece
     * @param piece The target piece
     * @return if the target piece could be taken if another piece was targeting it (not current player and not king).
     */
    bool Position::CouldPlayerTakePiece(const Player player, const Piece& piece) {
        return (piece.player == GetOtherPlayer(player) && piece.type != PieceType::King && piece.type != PieceType::None);
    }

#pragma endregion

    Position::Position() :
        irreversible_aspects_({}, std::nullopt, 0),
        board_()
    {
        EnsureLegalEnPassantSquare();
        EnsurePossibleCastlingRights();
        RecomputeRemainingPieces();
        ComputeMovesForPlayer(Player::White);
        // ComputeMovesForPlayer(Player::Black);

    }

    Position::Position(const std::array<std::array<Piece, 8>, 8> &board,
                       Player active_player, const CastlingRights& castling_rights,
                       std::optional<Coordinates> en_passant, int half_move_clock, int move_count) :

        irreversible_aspects_(castling_rights, en_passant, half_move_clock),
        board_(board),
        active_player_(active_player),
        move_count_(move_count) {


        EnsureLegalEnPassantSquare();
        EnsurePossibleCastlingRights();
        RecomputeRemainingPieces();
        ComputeMovesForPlayer(active_player);
        // ComputeMovesForPlayer(Player::Black);
        // ComputeMovesForPlayer(Player::White); // recompute with updated info // todo change this
    }

    std::string Position::ToString() const {
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


    std::string Position::GetTargetedSquaresString(Player player) const {
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
        // std::println("[INFO] RecomputeRemainingPieces(): All pieces recomputed. ({}/{}, w/b still on board)",
        //     white_piece_coordinates_.size(), black_piece_coordinates_.size());
    }

    bool Position::IsPositionCheckLegal() const {


        Player other = GetOtherPlayer(active_player_);
        Coordinates target = {-1, -1};

        for (Coordinates coordinates : GetPlayerOccupiedSquares(other)) {
            if (GetPiece(coordinates).type == PieceType::King) {
                target = coordinates;
                break;
            }
        }

        assert(target.IsValid());

        for (const Move& move : GenerateMovesForPlayer(other)) {
            if (move.to == target) {
                return false;
            }
        }

        return true;


    }

    void Position::EnsureLegalEnPassantSquare() {
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

    void Position::EnsurePossibleCastlingRights() {
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


    bool Position::MakeLegalMove(const Move &move) {

        const auto& legal_moves = GetLegalMovesForPlayer(active_player_);

        if (!utility::ranges::Contains(legal_moves, move)) {
            return false;
        }
        MakeMove(move);

        ComputeMovesForPlayer(active_player_);
        return true;
    }

    bool Position::MakeLegalMoveLAN(const MoveLAN &move_lan, Move& actual_move) {
        const auto& legal_moves = GetLegalMovesForPlayer(active_player_);
        for (const Move& move : legal_moves) {
            if (move_lan.Matches(move)) {
                std::println("MakeLEgalMove(): {} -> {}", move.from, move.to);
                actual_move = move;
                MakeMove(move);
                ComputeMovesForPlayer(active_player_);
                return true;
            }
        }
        return false;
    }


    std::vector<Move> Position::GenerateMovesForPlayer(Player player) const {
        std::vector<Move> moves;


        for (const Coordinates& coord : GetPlayerOccupiedSquares(player)) {
            assert(GetPiece(coord).player == player);
            moves.append_range(GeneratePseudoLegalMovesFromCoordinates(coord, player));
        }
        moves.append_range(GenerateCastleMoves(player));
        return moves;
    }

    void Position::ComputeMovesForPlayer(Player player) {

        GetPlayerKingTargeted(GetOtherPlayer(player)) = false;

        GetLegalMovesForPlayer(player).clear();
        GetPlayerTargetedSquares(player).fill(std::array<bool, 8>{false});

        for (const Move& move : GenerateMovesForPlayer(player)) {
            AddMoveToPlayer(move, player);
        }
    }

    void Position::AddTargetedSquareToPlayer(const Coordinates to, Player player) {
        GetPlayerTargetedSquares(player)[to.row][to.column] = true;
    }

    // TODO consider changing args to P, std::vector<Move>& player_moves, std::array<std::array<bool, 8>, 8>& player_reachable instead of just Player player
    void Position::AddMoveToPlayer(const Move& move, const Player player) {
        assert(player == active_player_);
        // make move
        IrreversibleAspects irreversible_aspects = GetIrreversibleAspects();
        MakeMove(move);
        // check legal
        if (IsPositionCheckLegal()) {
            // add move
            // std::println("info: successfully added a move");
            GetLegalMovesForPlayer(player).push_back(move);
            GetPlayerTargetedSquares(player)[move.to.row][move.to.column] = true;
        }
        // unmake move
        UnmakeMoveInternal(move, irreversible_aspects);
    }


    std::vector<Move> Position::GeneratePseudoLegalMovesFromCoordinates(const Coordinates &from, Player player) const {

        Piece piece = GetPiece(from);
        assert(!piece.IsEmpty() && piece.player == player && "Invalid player or piece.");
        std::vector<Move> moves;
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
                std::println(stderr, "[ERROR] Position::GeneratePseudoLegalMovesForPlayer(): Unrecognized piece type, {}", std::to_underlying(piece.type));
                return std::vector<Move>{};
        }
    }

    std::vector<Move> Position::GenerateDirectMoves(const Coordinates &from, const std::vector<Coordinates> &directions,
                                                    Player player) const {
        assert(GetPiece(from).player == player);

        std::vector<Move> moves;

        int count = 0;
        for (const Coordinates& dir : directions) {
            Coordinates to = from + dir;
            if (!to.IsValid()) {
                continue;
            }
            // AddTargetedSquareToPlayer(to, player);
            Piece piece = GetPiece(to);
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

    std::vector<Move> Position::GenerateTranslationMoves(const Coordinates &from,
                                                         const std::vector<Coordinates> &directions, Player player) const {

        assert(GetPiece(from).player == player);

        std::vector<Move> moves;

        int count = 0;
        // go in all directions
        for (const Coordinates& dir : directions) {
            for (Coordinates to = from + dir; to.IsValid(); to += dir ) {
                // AddTargetedSquareToPlayer(to, player);
                Piece piece = GetPiece(to);
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


    std::vector<Move> Position::GeneratePromotionMoves(const Move &move, Player player) {
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

    std::vector<Move> Position::GeneratePawnMoves(const Coordinates &from, Player player) const {

        assert(GetPiece(from).player == player);

        std::vector<Move> moves;

        int count = 0;
        // moving straight
        int dy = (player == Player::White) ? 1 : -1;
        Coordinates direction = {.row = dy, .column = 0};

        // if currently on row before last -> promotion when moving pawn.
        bool promotion = from.row == GetPawnStartingRank(GetOtherPlayer(player));

        Coordinates to = from + direction;
        if (to.IsValid() && GetPiece(to).IsEmpty()) {
            Move move(from, to);
            if (promotion) {
                moves.append_range(GeneratePromotionMoves(move, player));
                count += kPromotableTypes.size();
            } else {
                moves.push_back(move);
                count++;
            }

            // second step if starting row and unobstructed.
            if (from.row == GetPawnStartingRank(player)) {
                to += direction;
                if (to.IsValid() && GetPiece(to).IsEmpty()) {
                    move = Move(from, to);
                    move.double_pawn = true;
                    moves.push_back(move);
                    count++;
                }
            }
        }

        // Taking pieces diagonally
        const std::optional<Coordinates>& en_passant = GetEnPassant();
        std::vector<Coordinates> take_directions = { {.row = dy, .column = -1}, {.row = dy, .column = 1}};
        for (Coordinates take_direction : take_directions) {
            to = from + take_direction;
            if (!to.IsValid()) {
                break;
            }
            bool is_en_passant = en_passant.has_value() && en_passant.value() == to;
            Piece target_piece = GetPiece(to);
            if (CouldPlayerTakePiece(player, target_piece) || is_en_passant) {
                Move move(from, to);
                move.en_passant = is_en_passant;
                move.capture_type = is_en_passant ? PieceType::Pawn : target_piece.type;

                if (promotion) {
                    moves.append_range(GeneratePromotionMoves(move, player));
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

    std::vector<Move> Position::GenerateKnightMoves(const Coordinates& from, Player player) const {
        return GenerateDirectMoves(from, kKnightDirections, player);
    }

    std::vector<Move> Position::GenerateBishopMoves(const Coordinates& from, Player player) const {
        return GenerateTranslationMoves(from, kDiagonalDirections, player);
    }

    std::vector<Move> Position::GenerateRookMoves(const Coordinates& from, Player player) const {
        return GenerateTranslationMoves(from, kOrthogonalDirections, player);
    }

    std::vector<Move> Position::GenerateQueenMoves(const Coordinates& from, Player player) const {
        return GenerateTranslationMoves(from, kAllDirections, player);
    }

    std::vector<Move> Position::GenerateKingMoves(const Coordinates& from, Player player) const {
        return GenerateDirectMoves(from, kAllDirections, player);
    }

    std::vector<Move> Position::GenerateCastleMoves(const Player player) const {
        // TODO handle castle moves correctly when implementing move function.

        std::vector<Move> moves;

        const CastlingRights& castling_rights = GetCastlingRights();

        Coordinates from = { .row = GetPieceStartingRank(player), .column = 4 };

        Piece king = GetPiece(from);

        // std::println("generate castle moves, current opponent reach = {}",
        //     GetReachableSquaresString(GetOtherPlayer(player)));

        if (player == Player::White && castling_rights.white_kingside
            || player == Player::Black && castling_rights.black_kingside) {

            assert(king.type == PieceType::King);
            assert(king.player == player);

            bool castle_available = true;
            for (int col = 5; col < 7; ++col) {
                Coordinates coords = { .row = from.row, .column = col };
                if (!GetPiece(coords).IsEmpty() || DoesPlayerTargetCoordinates(GetOtherPlayer(player), coords)) {
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
                if (!GetPiece(coords).IsEmpty() || DoesPlayerTargetCoordinates(GetOtherPlayer(player), coords)) {
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

    bool Position::DoesPlayerTargetCoordinates(const Player player, const Coordinates coordinates) const {
        const bool is_targeted = GetPlayerTargetedSquares(player)[coordinates.row][coordinates.column];
        // std::println("does {} target {} = {}", PlayerToString(player), coordinates, is_targeted);
        return is_targeted;
    }


    void Position::MakeMove(const Move &move) {
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
        // DO not do this here, instead outside of this function
        /*ComputeMovesForPlayer(Player::White);
        ComputeMovesForPlayer(Player::Black);*/
    }


    void Position::UnmakeMove(const Move &move, const IrreversibleAspects& prev_aspects) {

        UnmakeMoveInternal(move, prev_aspects);

        RecomputeRemainingPieces();
        ComputeMovesForPlayer(active_player_);

    }

    void Position::UnmakeMoveInternal(const Move &move, const IrreversibleAspects& prev_aspects) {
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
        // ComputeMovesForPlayer(Player::White);
        // ComputeMovesForPlayer(Player::Black);
    }



    // TODO REMOVE THIS
    bool Position::ComputeIsInCheck() {

        Player other = GetOtherPlayer(active_player_);
        const auto& active_player_coords = GetPlayerOccupiedSquares(active_player_);
        Coordinates king_coordinates = {-1, -1};
        for (const auto coord : active_player_coords) {
            if (GetPiece(coord).type == PieceType::King) {
                king_coordinates = coord;
                break;
            }
        }
        assert(king_coordinates.IsValid() && "No king Was Found!");

        // update other player target squares
        ComputeMovesForPlayer(other);

        is_in_check_ = DoesPlayerTargetCoordinates(other, king_coordinates);
        return is_in_check_;
    }



#pragma endregion
} // chess
//
// Created by martin on 27/08/2026.
//

#include "fen.h"
#include <sstream>
#include <format>
#include <vector>
#include <print>
#include "utility/string_utils.h"


namespace notation::fen {

#pragma region Forward Declarations

    static std::string getPlacementRow(const std::array<chess::Piece, 8>& row);
    static std::string GetPlacementSection(const chess::Board& Board);
    static char GetActivePlayerSection(const chess::Board& Board);
    static std::string GetCastlingSection(const chess::Board& Board);
    static std::string GetEnPassantSection(const chess::Board& Board);

    static std::array<chess::Piece, 8> ParsePlacementRow(std::string_view placement_row);
    static std::array<std::array<chess::Piece, 8>, 8> ParsePlacementSection(std::string_view placement_section);
    static chess::Player ParseActivePlayerSection(std::string_view active_player_section);
    static chess::CastlingRights ParseCastlingSection(std::string_view castling_section);
    static std::optional<chess::Coordinates> ParseEnPassantSection(std::string_view en_passant_section);

#pragma endregion

    std::string Encode(const chess::Board& Board) {
        return std::format("{} {} {} {} {} {}",
            GetPlacementSection(Board),
            GetActivePlayerSection(Board),
            GetCastlingSection(Board),
            GetEnPassantSection(Board),
            Board.GetHalfMoveClock(),
            Board.GetMoveCount()
            );
    }

    std::optional<chess::Board> Decode(std::string_view fen) {
        // split each sections into tokens
        std::vector<std::string_view> tokens = utility::strings::Tokenize(fen, ' ');

        // there should be 6 tokens/sections
        if (tokens.size() != 6) {
            return std::nullopt;
        }

        chess::Board Board {
            ParsePlacementSection(tokens[0]),
            ParseActivePlayerSection(tokens[1]),
            ParseCastlingSection(tokens[2]),
            ParseEnPassantSection(tokens[3]),
            std::stoi(std::string(tokens[4])),
            std::stoi(std::string(tokens[5]))
        };

        return std::make_optional(Board);
    }


#pragma region Private Functions

    // encoding

    static std::string getPlacementRow(const std::array<chess::Piece, 8>& row) {
        std::stringstream row_stream;
        int current_empty_count = 0;
        for (std::size_t i = 0; i < 8; ++i) {
            chess::Piece piece = row[i];
            if (piece.IsEmpty()) {
                current_empty_count++;
                continue;
            }

            if (current_empty_count > 0) {
                row_stream << current_empty_count;
                current_empty_count = 0;
            }
            row_stream << piece.ToChar();
        }
        if (current_empty_count > 0) {
            row_stream << current_empty_count;
        }
        return row_stream.str();
    }

    static std::string GetPlacementSection(const chess::Board& Board) {

        std::stringstream section_stream;
        section_stream << getPlacementRow(Board.GetRow(7));
        for (int i = 6; i >= 0; --i) {
            section_stream << '/' << getPlacementRow(Board.GetRow(i));
        }

        return section_stream.str();
    }

    static char GetActivePlayerSection(const chess::Board& Board) {
        return Board.GetActivePlayer() == chess::Player::White ? 'w' : 'b';
    }

    static std::string GetCastlingSection(const chess::Board& Board) {
        std::string castling_section = "";
        chess::CastlingRights castling_rights = Board.GetCastlingRights();
        if (castling_rights.white_kingside) {
            castling_section += 'K';
        }
        if (castling_rights.white_queenside) {
            castling_section += 'Q';
        }
        if (castling_rights.black_kingside) {
            castling_section += 'k';
        }
        if (castling_rights.black_queenside) {
            castling_section += 'q';
        }

        if (castling_section.empty()) {
            return "-";
        }
        return castling_section;
    }

    static std::string GetEnPassantSection(const chess::Board& Board) {
        std::optional<chess::Coordinates> en_passant = Board.GetEnPassant();
        if (en_passant.has_value()) {
            return en_passant.value().ToString();
        }

        return "-";
    }


    // decoding

    static std::array<chess::Piece, 8> ParsePlacementRow(std::string_view placement_row) {
        std::array<chess::Piece, 8> row{};

        int column_index = 0;

        for (char c : placement_row) {

            if (column_index >= 8) {
                std::println(stderr, "[ERROR] FEN::ParsePlacementRow() Too many characters in row: {}", placement_row);
                break;
            }

            if (std::isdigit(c)) {
                column_index += c - '0'; // todo check if correct.
                continue;
            }

            chess::Piece piece = chess::Piece::FromChar(c);
            if (piece.IsEmpty()) {
                std::println(stderr, "[ERROR] FEN::ParsePlacementRow() Unknown piece '{}' in row: {}", c, placement_row);
                column_index++;
            } else {
                row[column_index++] = piece;
            }
        }

        return row;
    }

    static std::array<std::array<chess::Piece, 8>, 8> ParsePlacementSection(std::string_view placement_section) {
        std::array<std::array<chess::Piece, 8>, 8> board{};
        const std::vector<std::string_view> rows = utility::strings::Tokenize(placement_section, '/');

        if (rows.size() != 8) {
            std::println(stderr, "[ERROR] FEN::ParsePlacementSection() Expected 8 rows but found '{}'"
                 " inside placement section: {}", rows.size(), placement_section);
            return board;
        }

        for (size_t row = 0; row < 8; ++row) {
            board[row] = ParsePlacementRow(rows[7 - row]);
        }

        return board;
    }

    static chess::Player ParseActivePlayerSection(std::string_view active_player_section) {
        if (active_player_section == "w") {
            return chess::Player::White;
        }
        if (active_player_section == "b") {
            return chess::Player::Black;
        }

        std::println(stderr, "[ERROR] FEN::ParseActivePlayerSection() unrecognized color '{}', expected 'w' or 'b'.",
            active_player_section);
        return chess::Player::None;

    }


    static chess::CastlingRights ParseCastlingSection(std::string_view castling_section) {
        return {
            .white_kingside = castling_section.contains('K'),
            .white_queenside = castling_section.contains('Q'),
            .black_kingside = castling_section.contains('k'),
            .black_queenside = castling_section.contains('q')
        };
    }


    static std::optional<chess::Coordinates> ParseEnPassantSection(std::string_view en_passant_section) {
        if (en_passant_section == "-") {
            return std::nullopt;
        }
        return chess::Coordinates::FromAlgebraicSquareNotation(en_passant_section);
    }

#pragma endregion

} // notation::FEN
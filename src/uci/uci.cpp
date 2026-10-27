//
// Created by martin on 27/10/2026.
//


#include "uci.h"

#include <print>
#include <iostream>
#include <ranges>
#include <string_view>
#include <cassert>


#include "chess/engine.h"
#include "chess/board.h"
#include "notation/fen.h"

namespace uci {


    static chess::Board ParsePositionCommand(std::vector<std::string_view> arguments);
    static void ParseGoCommand(std::vector<std::string_view> arguments, chess::Board& board);
    static void PrintHelp();


    void RunUCI() {
        std::println("Starting UCI engine...");
        std::string input;


        chess::Board board = chess::Board();

        while (true) {
            std::getline(std::cin, input);
            std::vector<std::string_view> tokens = utility::strings::Tokenize(input, ' ');
            std::string_view command = tokens.empty() ? "" : tokens[0];
            std::vector<std::string_view> args(tokens.begin() + 1, tokens.end());

            if (command == "quit") {
                break;
            } else if (command == "uci") {
                std::println("id name nichess");
                std::println("id author Martin");
                std::println("uciok");
            } else if (command == "isready") {
                std::println("readyok");
            } else if (command == "position") {
                board = ParsePositionCommand(args);
            } else if (command == "go") {
                ParseGoCommand(args, board);
            } else if (command == "ucinewgame") {
                board = chess::Board();
            } else if (command == "d") {
                std::println("Current position:\n{}", board.ToString());
            } else if (command == "help") {
                PrintHelp();
            } else {
                std::println("Unknown command: {}", input);
            }
        }
    }

    static chess::Board ParsePositionCommand(std::vector<std::string_view> arguments) {
        
        assert(!arguments.empty());

        chess::Board board;

        int index = 0;

        if (arguments[0] == "startpos") {
            board = notation::fen::Decode(notation::fen::kDefaultFEN).value();
            index = 1;
        } else if (arguments[0] == "fen") {
            if (arguments.size() < 7) {
                std::println("Error: 'position fen' command requires six FEN fields.");
                return chess::Board();
            }
            std::string fen_string;
            for (size_t i = 1; i < 7; ++i) {
                if (i > 1) {
                    fen_string += ' ';
                }
                fen_string += arguments[i];
            }
            board = notation::fen::Decode(fen_string).value_or(chess::Board());
            index = 7;
        } else {
            std::println("Error: Unrecognized position command: {}", arguments[0]);
            return chess::Board();
        }

        if (arguments.size() > index && arguments[index] == "moves") {
            for (size_t i = index + 1; i < arguments.size(); ++i) {
                std::string_view move_str = arguments[i];
                std::optional<chess::MoveLAN> move_lan = chess::MoveLAN::FromUCINotation(move_str);
                if (!move_lan.has_value()) {
                    std::println("Unrecognized move in position command: {}", move_str);
                    continue;
                }
                std::vector<chess::Move> legal_moves = chess::Engine::GetLegalMoves(board); // TODO use pseudo legal and the check manually not in check.
                std::optional<chess::Move> corresponding_move = move_lan.value().GetCorrespondingMove(legal_moves);
                if (!corresponding_move.has_value()) {
                    std::println("Illegal move in position command: {}", move_str);
                    continue;
                }
                board.MakeMove(corresponding_move.value());
            }
        }

        return board;
    }


    static void ParseGoCommand(std::vector<std::string_view> arguments, chess::Board& board) {
        std::vector<chess::Move> legal_moves = chess::Engine::GetLegalMoves(board);
        if (legal_moves.empty()) {
            std::println("No legal moves available.");
            return;
        }
        chess::Move best_move = legal_moves[0]; // TODO implement search for best move.
        std::println("bestmove {}", chess::MoveLAN{.from = best_move.from, .to = best_move.to, .capture = best_move.capture_type != chess::PieceType::None, .promotion_type = best_move.promotion_type}.ToUCINotation());
        // bestmove <move> ponder <expected> // pondering not implemented yet.
    }


    static void PrintHelp() {
        std::println("Available commands:");
        std::println("uci - Initialize the engine in UCI mode.");
        std::println("isready - Check if the engine is ready.");
        std::println("position <startpos|fen> [FEN string] [moves <move1> <move2> ...] - Set the board position.");
        std::println("go - Start the engine's search for the best move.");
        std::println("ucinewgame - Start a new game.");
        std::println("d - Display the current board position.");
        std::println("help - Display this help message.");
        std::println("quit - Exit the UCI engine.");
    }

}
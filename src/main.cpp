#include <iostream>
#include <print>

#include "chess/board.h"
#include "notation/fen.h"
#include "utility/ranges_utils.h"
#include "chess/perft.h"
#include "chess/engine.h"

constexpr bool kDoPerft = true;
constexpr bool kDoPlayGame = true;


static void GameInfo(const chess::Board& position) {
    std::println("Game Info: \n\n{}", position.ToString());
    std::println("Encoded FEN: {}", notation::fen::Encode(position));
    const std::vector<chess::Move>& moves =  chess::Engine::GetLegalMoves(position);
    std::println("move count= {}", moves.size());

    /*std::println("{}", position.GetTargetedSquaresString(chess::Player::White));
    std::println("{}", position.GetTargetedSquaresString(chess::Player::Black));*/
}

static void GameInfoFEN(std::string_view fen) {
    GameInfo(notation::fen::Decode(fen).value());
}

static void TestCoordinatesConvertion() {

    std::vector<std::string> coords = { "a1", "b2", "e8", "h1" };
    for (const auto& coord : coords) {
        chess::Coordinates decoded_coords = chess::Coordinates::FromAlgebraicSquareNotation(coord).value();
        if (decoded_coords.ToString() != coord) {
            std::println("Coordinates are incorrect: {} -> {}", coord, decoded_coords);
        }
    }

    std::vector<std::string> invalid_coords = { "a0", "b22", "ee", "h", "f9", " f"};
    for (const auto& coord : invalid_coords) {
        std::optional<chess::Coordinates> decoded_coords = chess::Coordinates::FromAlgebraicSquareNotation(coord);
        if (decoded_coords.has_value()) {
            std::println("Coordinates should not be correct: {} -> {}", coord, decoded_coords.value());
        }
    }
}

static void TestPerft() {
    chess::Board position = notation::fen::Decode(notation::fen::kDefaultFEN).value();
    chess::Perft perft(position);
    perft.PerformPerftAndPrintInfo(2);
}

static void PlayGame() {

    chess::Board position = notation::fen::Decode(notation::fen::kDefaultFEN).value();

    chess::Move move(chess::Coordinates{}, chess::Coordinates{});
    std::vector<chess::Move> played_moves;
    std::vector<chess::IrreversibleAspects> irreversible_stack;
    irreversible_stack.push_back(position.GetIrreversibleAspects());

    std::println("{}", position.ToString());
    std::string input;
    while (true) {
        const std::vector<chess::Move>& moves = chess::Engine::GetLegalMoves(position);
        if (moves.empty()) {
            std::println("no more playable moves");
            break;
        }

        std::println("Enter a move in LAN format: ");
        std::cin >> input;
        std::string lower_case = utility::strings::ToLowerCase(input);
        if (lower_case == "q") {
            std::println("Exiting...");
            break;
        }
        if (lower_case == "undo") {
            if (played_moves.empty()) {
                std::println("No moves to undo.");
                continue;
            }
            position.UnmakeMove(played_moves.back(), irreversible_stack.back());
            played_moves.pop_back();
            irreversible_stack.pop_back();

            std::println("Undoing move...\n{}", position.ToString());
            continue;
        }
        if (lower_case == "list") {
            std::println("Displaying all moves:");
            for (const chess::Move& available_move : moves) {
                std::println("{}", available_move.ToString());
            }
        }

        std::optional<chess::MoveLAN> move_lan = chess::MoveLAN::FromLongAlgebraicNotation(input);
        if (!move_lan.has_value()) {
            std::println("Unrecognized lan format: {}\t\t('q' to quit)", input);
            continue;
        }
        if ( !position.MakeLegalMoveLAN(move_lan.value(), move)) {
            std::println("Illegal move: {}", move_lan.value().ToLongAlgebraicNotation());
            continue;
        }
        std::println("main(): move: {} -> {}", move.from, move.to);
        played_moves.push_back(move);
        irreversible_stack.push_back(position.GetIrreversibleAspects());

        std::println("Playing move: {}\n{}", move_lan.value().ToLongAlgebraicNotation(), position.ToString());

    }
}


int main() {

    /*
    TestCoordinatesConvertion();

    chess::Position position;
    GameInfo(position);

    GameInfoFEN(notation::fen::kDefaultFEN);


    std::string hikaru_fen = "8/6pp/p1r1R3/8/P3N2P/3k1P2/6PK/8 b - - 0 39";
    GameInfoFEN(hikaru_fen);


    GameInfoFEN("8/8/4N3/8/8/2K5/8/8 w - - 0 1");

    std::println("castle check");

    GameInfoFEN("r3k3/8/8/8/6B1/2n5/8/R3K2R w KQq - 0 1");


    std::println("check en passant");
    GameInfoFEN("k7/8/8/2pPPpP1/8/8/8/K7 w - f6 0 1");*/


    if (kDoPerft) {
        TestPerft();
    }

    if (kDoPlayGame) {
        PlayGame();
    }

    return 0;


}
#include <iostream>
#include <print>

#include "chess/position.h"
#include "notation/fen.h"

static void GameInfo(const chess::Position& position) {
    std::println("Game Info: \n\n{}", position.ToString());
    std::println("Encoded FEN: {}", notation::fen::Encode(position));
    const std::vector<chess::Move>& moves = position.GetActivePlayerMoves();
    std::println("move count= {}", moves.size());

    std::println("{}", position.GetReachableSquaresString(chess::Player::White));
    std::println("{}", position.GetReachableSquaresString(chess::Player::Black));
}

static void GameInfoFEN(std::string_view fen) {
    GameInfo(notation::fen::Decode(fen).value());
}



int main() {

    chess::Position position;
    GameInfo(position);

    GameInfoFEN(notation::fen::kDefaultFEN);


    std::string hikaru_fen = "8/6pp/p1r1R3/8/P3N2P/3k1P2/6PK/8 b - - 0 39";
    GameInfoFEN(hikaru_fen);


    GameInfoFEN("8/8/4N3/8/8/2K5/8/8 w - - 0 1");

    std::println("castle check");

    GameInfoFEN("r3k3/8/8/8/6B1/2n5/8/R3K2R w KQq - 0 1");


    return 0;


}
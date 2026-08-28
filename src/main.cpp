#include <iostream>
#include <print>

#include "chess/Position.h"
#include "notation/FEN.h"


static const chess::Position kDefaultPosition = notation::FEN::Decode(notation::FEN::kDefaultFEN).value_or(chess::Position{});

int main() {

    chess::Position position;

    std::println("{}", position.ToString());

    std::println("{}", notation::FEN::Encode(position));

    std::println("default position: \n{}", kDefaultPosition.ToString());

    std::println("default FEN:  {}", notation::FEN::kDefaultFEN);
    std::println("computed FEN: {}", notation::FEN::Encode(kDefaultPosition));

    std::vector<chess::Move> moves = kDefaultPosition.GetLegalMoves();

    std::println("starting move count= {}", moves.size());


    std::string hikaru_fen = "8/6pp/p1r1R3/8/P3N2P/3k1P2/6PK/8 b - - 0 39";
    chess::Position hikaru_position = notation::FEN::Decode(hikaru_fen).value();
    std::println("hikaru: {}, \n{}", hikaru_fen, hikaru_position.ToString());

    if (hikaru_fen == notation::FEN::Encode(hikaru_position)) {
        std::println("encoded FEN matches decoded FEN!");
    } else {
        std::println("decoded FEN matches encoded FEN!: {}", notation::FEN::Encode(hikaru_position));
    }

    moves = hikaru_position.GetLegalMoves();
    std::println("hikaru move count= {}", moves.size());
    return 0;


}
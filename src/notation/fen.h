//
// Created by martin on 27/08/2026.
//

#ifndef NICHESS_FEN_H
#define NICHESS_FEN_H

#include <optional>
#include <string>
#include "chess/position.h"

namespace notation::fen {

    constexpr std::string_view kDefaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    std::string Encode(const chess::Position& position);
    std::optional<chess::Position> Decode(std::string_view fen);

} // notation::FEN

#endif //NICHESS_FEN_H

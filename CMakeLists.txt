cmake_minimum_required(VERSION 3.28)
project(nichess)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(nichess src/main.cpp
        src/chess/board.cpp
        src/chess/piece.h
        src/chess/piece.cpp
        src/notation/fen.cpp
        src/notation/fen.h
        src/utility/string_utils.h
        src/utility/string_utils.cpp
        src/chess/coordinates.h
        src/chess/move.h
        src/chess/castling_rights.h
        src/chess/coordinates.cpp
        src/chess/irreversible_aspects.h
        src/utility/ranges_utils.cpp
        src/utility/ranges_utils.h
        src/chess/move.cpp
        src/chess/perft.cpp
        src/chess/perft.h
        src/chess/engine.cpp
        src/chess/engine.h
        src/chess/utility.cpp
        src/chess/utility.h
        src/chess/utility.cpp
)


target_include_directories(nichess PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

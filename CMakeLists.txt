cmake_minimum_required(VERSION 4.3)
project(nichess)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(nichess src/main.cpp
        src/chess/position.cpp
        src/chess/position.h
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
)


target_include_directories(nichess PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

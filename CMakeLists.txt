cmake_minimum_required(VERSION 4.3)
project(nichess)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(nichess src/main.cpp
        src/chess/Position.cpp
        src/chess/Position.h
        src/chess/Piece.h
        src/chess/Piece.cpp
        src/notation/FEN.cpp
        src/notation/FEN.h
        src/utility/string_utils.h
        src/utility/string_utils.cpp
        src/chess/Coordinates.h
        src/chess/Move.h
        src/chess/CastlingRights.h
)


target_include_directories(nichess PRIVATE
        ${PROJECT_SOURCE_DIR}/src)

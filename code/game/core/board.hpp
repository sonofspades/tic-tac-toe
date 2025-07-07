#pragma once

#include "piece_type.hpp"

class Board
{
public:
    auto check_win(int32_t row, int32_t column, piece_type type) -> bool;

private:
    auto check_row   (int32_t row,    piece_type type) -> bool;
    auto check_column(int32_t column, piece_type type) -> bool;

    auto check_diagonals(int32_t piece_type) -> bool;

    piece_type _tiles[3][3] { };
};
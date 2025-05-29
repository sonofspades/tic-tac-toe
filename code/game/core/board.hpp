#pragma once

class Board
{
public:
    auto check_win(int32_t row, int32_t column, int32_t type) -> bool;

private:
    auto check_row   (int32_t row,    int32_t type) -> bool;
    auto check_column(int32_t column, int32_t type) -> bool;

    auto check_diagonals(int32_t type) -> bool;

    int32_t _tiles[3][3] { };
};
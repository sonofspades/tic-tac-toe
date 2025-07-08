#include "board.hpp"

auto Board::check_row(const int32_t row, const piece_type type) const -> bool
{
    return pieces[row][0].type == type &&
           pieces[row][1].type == type &&
           pieces[row][2].type == type;
}

auto Board::check_col(const int32_t col, const piece_type type) const -> bool
{
    return pieces[0][col].type == type &&
           pieces[1][col].type == type &&
           pieces[2][col].type == type;
}
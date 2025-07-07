#include "board.hpp"

auto Board::check_row(const int32_t row, const piece_type type) const -> bool
{
    return pieces[row][0].type == type &&
           pieces[row][1].type == type &&
           pieces[row][2].type == type;
}
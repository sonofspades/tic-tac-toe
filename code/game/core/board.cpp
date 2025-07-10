#include "board.hpp"

auto Board::reset() -> void
{
    for (auto row = 0; row < 3; row++)
    {
        for (auto col = 0; col < 3; col++)
        {
            pieces[row][col].type = piece_type::none;
        }
    }
}

auto Board::check_win(const int32_t row, const int32_t col, const piece_type type) const -> bool
{
    return check_row(row, type) || check_col(col, type) || check_diagonals(type);
}

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

auto Board::check_diagonals(const piece_type type) const -> bool
{
    return pieces[1][1].type == type &&
         ((pieces[0][0].type == type && pieces[2][2].type == type) || (pieces[0][2].type == type && pieces[2][0].type == type));
}
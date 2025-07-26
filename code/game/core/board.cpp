#include "board.hpp"

namespace core
{
    auto Board::reset() -> void
    {
        for (auto& [type, position] : _data)
        {
            type = piece_type::none;
        }
    }

    auto Board::check_win(const data::grid_position& position, const piece_type type) const -> bool
    {
        return check_row(position.row, type) || check_col(position.col, type) || check_diagonals(type);
    }

    auto Board::check_row(const int32_t row, const piece_type type) const -> bool
    {
        return at({ row, 0 }).type == type &&
               at({ row, 1 }).type == type &&
               at({ row, 2 }).type == type;
    }

    auto Board::check_col(const int32_t col, const piece_type type) const -> bool
    {
        return at({ 0, col }).type == type &&
               at({ 1, col }).type == type &&
               at({ 2, col }).type == type;
    }

    auto Board::check_diagonals(const piece_type type) const -> bool
    {
        return at({ 1, 1 }).type == type &&
             ((at({ 0, 0 }).type == type && at({ 2, 2 }).type == type) || (at({ 0, 2 }).type == type && at({ 2, 0 }).type == type));
    }
}
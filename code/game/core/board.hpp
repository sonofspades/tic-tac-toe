#pragma once

#include "piece.hpp"

namespace core
{
    class Board final : public data::Grid<piece, 3, 3>
    {
    public:
        auto reset() -> void;

        auto check_win(const data::grid_position& position, piece_type type) const -> bool;

    private:
        auto check_row(int32_t row, piece_type type) const -> bool;
        auto check_col(int32_t col, piece_type type) const -> bool;

        auto check_diagonals(piece_type type)        const -> bool;
    };
}
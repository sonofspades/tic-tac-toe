#pragma once

#include "piece.hpp"

class Board
{
public:
    auto reset() -> void;

    auto check_win(int32_t row, int32_t col, piece_type type) const -> bool;

    piece pieces[3][3] { };

private:
    auto check_row(int32_t row, piece_type type) const -> bool;
    auto check_col(int32_t col, piece_type type) const -> bool;

    auto check_diagonals(piece_type type) const -> bool;
};
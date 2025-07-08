#pragma once

#include "piece_type.hpp"

struct piece
{
     glm::vec3 position;
    piece_type type { };

    // TODO move the position a little closer to the center
    // TODO make the winner row/column white? and maybe blinking
};
#pragma once

#include "piece_type.hpp"

namespace core
{
    struct piece
    {
        piece_type type;

         glm::vec3 position;

        // TODO move the position a little closer to the center
        // TODO make the winner row/column white? and maybe blinking
    };
}
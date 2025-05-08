//========================================================================
// GLFW 3.5 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2018 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "internal.h"

//////////////////////////////////////////////////////////////////////////
//////                       GLFW internal API                      //////
//////////////////////////////////////////////////////////////////////////

static const struct
{
    int ID;
    GLFWbool (*connect)(int,_GLFWplatform*);
} supportedPlatforms[] =
{
#if defined(_GLFW_WIN32)
    { GLFW_PLATFORM_WIN32, _glfwConnectWin32 },
#endif
};

GLFWbool _glfwSelectPlatform(int desiredID, _GLFWplatform* platform)
{
    const size_t count = sizeof(supportedPlatforms) / sizeof(supportedPlatforms[0]);

    if (desiredID != GLFW_ANY_PLATFORM &&
        desiredID != GLFW_PLATFORM_WIN32 &&
        desiredID != GLFW_PLATFORM_NULL)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid platform ID 0x%08X", desiredID);
        return GLFW_FALSE;
    }

    if (desiredID == GLFW_ANY_PLATFORM)
    {
        // If there is exactly one platform available for auto-selection, let it emit the
        // error on failure as the platform-specific error description may be more helpful
        if (count == 1)
            return supportedPlatforms[0].connect(supportedPlatforms[0].ID, platform);
    }

    for (size_t i = 0;  i < count;  i++)
    {
        if (supportedPlatforms[i].ID == desiredID)
            return supportedPlatforms[i].connect(desiredID, platform);
    }

    _glfwInputError(GLFW_PLATFORM_UNAVAILABLE, "The requested platform is not supported");

    return GLFW_FALSE;
}

//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

int glfwPlatformSupported(int platformID)
{
    const size_t count = sizeof(supportedPlatforms) / sizeof(supportedPlatforms[0]);

    if (platformID != GLFW_PLATFORM_WIN32 &&
        platformID != GLFW_PLATFORM_NULL)
    {
        _glfwInputError(GLFW_INVALID_ENUM, "Invalid platform ID 0x%08X", platformID);
        return GLFW_FALSE;
    }

    if (platformID == GLFW_PLATFORM_NULL)
        return GLFW_TRUE;

    for (size_t i = 0;  i < count;  i++)
    {
        if (platformID == supportedPlatforms[i].ID)
            return GLFW_TRUE;
    }

    return GLFW_FALSE;
}
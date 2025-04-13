add_library(glfw ${GLFW_LIBRARY_TYPE}
                 "${GLFW_SOURCE_DIR}/include/GLFW/glfw3.h"
                 "${GLFW_SOURCE_DIR}/include/GLFW/glfw3native.h"
                 internal.h platform.h context.c init.c input.c monitor.c platform.c window.c)

# The time, thread and module code is shared between all backends on a given OS,
# including the null backend, which still needs those bits to be functional
if (WIN32)
    target_sources(glfw PRIVATE win32_time.h win32_thread.h win32_module.c win32_time.c win32_thread.c)
endif()

if (GLFW_BUILD_WIN32)
    target_compile_definitions(glfw PRIVATE _GLFW_WIN32)
    target_sources(glfw PRIVATE win32_platform.h win32_init.c win32_monitor.c win32_window.c wgl_context.c)
endif()

if (GLFW_BUILD_X11 OR GLFW_BUILD_WAYLAND)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_sources(glfw PRIVATE linux_joystick.h linux_joystick.c)
    endif()
    target_sources(glfw PRIVATE posix_poll.h posix_poll.c)
endif()

if (WIN32 AND GLFW_BUILD_SHARED_LIBRARY)
    configure_file(glfw.rc.in glfw.rc @ONLY)
    target_sources(glfw PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/glfw.rc")
endif()

if (UNIX AND GLFW_BUILD_SHARED_LIBRARY)
    # On Unix-like systems, shared libraries can use the soname system.
    set(GLFW_LIB_NAME glfw)
else()
    set(GLFW_LIB_NAME glfw3)
endif()
set(GLFW_LIB_NAME_SUFFIX "")

set_target_properties(glfw PROPERTIES
                      OUTPUT_NAME ${GLFW_LIB_NAME}
                      VERSION ${GLFW_VERSION_MAJOR}.${GLFW_VERSION_MINOR}
                      SOVERSION ${GLFW_VERSION_MAJOR}
                      POSITION_INDEPENDENT_CODE ON
                      C_STANDARD 99
                      C_EXTENSIONS OFF
                      DEFINE_SYMBOL _GLFW_BUILD_DLL
                      FOLDER "GLFW3")

target_include_directories(glfw PUBLIC
                           "$<BUILD_INTERFACE:${GLFW_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_include_directories(glfw PRIVATE
                           "${GLFW_SOURCE_DIR}/src"
                           "${GLFW_BINARY_DIR}/src")
target_link_libraries(glfw PRIVATE Threads::Threads)

if (GLFW_BUILD_WIN32)
    list(APPEND glfw_PKG_LIBS "-lgdi32")
endif()

if (GLFW_BUILD_COCOA)
    target_link_libraries(glfw PRIVATE "-framework Cocoa"
                                       "-framework IOKit"
                                       "-framework CoreFoundation"
                                       "-framework QuartzCore")

    set(glfw_PKG_DEPS "")
    set(glfw_PKG_LIBS "-framework Cocoa -framework IOKit -framework CoreFoundation -framework QuartzCore")
endif()

if (WIN32)
    if (GLFW_USE_HYBRID_HPG)
        target_compile_definitions(glfw PRIVATE _GLFW_USE_HYBRID_HPG)
    endif()
endif()

# Enable a reasonable set of warnings
# NOTE: The order matters here, Clang-CL matches both MSVC and Clang
if (MSVC)
    target_compile_options(glfw PRIVATE "/W3")
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
        CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
        CMAKE_C_COMPILER_ID STREQUAL "AppleClang")

    target_compile_options(glfw PRIVATE "-Wall")
endif()

if (GLFW_BUILD_WIN32)
    target_compile_definitions(glfw PRIVATE UNICODE _UNICODE)
endif()

# HACK: When building on MinGW, WINVER and UNICODE need to be defined before
# the inclusion of stddef.h (by glfw3.h), which is itself included before
# win32_platform.h.  We define them here until a saner solution can be found
# NOTE: MinGW-w64 and Visual C++ do /not/ need this hack.
if (MINGW)
    target_compile_definitions(glfw PRIVATE WINVER=0x0501)
endif()

# Workaround for the MS CRT deprecating parts of the standard library
if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    target_compile_definitions(glfw PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Workaround for -std=c99 on Linux disabling _DEFAULT_SOURCE (POSIX 2008 and more)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(glfw PRIVATE _DEFAULT_SOURCE)
endif()

list(JOIN glfw_PKG_DEPS " " deps)
list(JOIN glfw_PKG_LIBS " " libs)

set(GLFW_PKG_CONFIG_REQUIRES_PRIVATE "${deps}" CACHE INTERNAL
    "GLFW pkg-config Requires.private")
set(GLFW_PKG_CONFIG_LIBS_PRIVATE "${libs}" CACHE INTERNAL
    "GLFW pkg-config Libs.private")

if (GLFW_INSTALL)
    install(TARGETS glfw
            EXPORT glfwTargets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endif()


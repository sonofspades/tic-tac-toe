link_libraries(glfw)

include_directories("${GLFW_SOURCE_DIR}/deps")

if (MATH_LIBRARY)
    link_libraries("${MATH_LIBRARY}")
endif()

# Workaround for the MS CRT deprecating parts of the standard library
if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h")
set(GETOPT "${GLFW_SOURCE_DIR}/deps/getopt.h" "${GLFW_SOURCE_DIR}/deps/getopt.c")
set(TINYCTHREAD "${GLFW_SOURCE_DIR}/deps/tinycthread.h" "${GLFW_SOURCE_DIR}/deps/tinycthread.c")

add_executable(allocator allocator.c ${GLAD_GL})
add_executable(clipboard clipboard.c ${GETOPT} ${GLAD_GL})
add_executable(events events.c ${GETOPT} ${GLAD_GL})
add_executable(msaa msaa.c ${GETOPT} ${GLAD_GL})
add_executable(glfwinfo glfwinfo.c ${GETOPT} ${GLAD_GL})
add_executable(iconify iconify.c ${GETOPT} ${GLAD_GL})
add_executable(monitors monitors.c ${GETOPT} ${GLAD_GL})
add_executable(reopen reopen.c ${GLAD_GL})
add_executable(cursor cursor.c ${GLAD_GL})

add_executable(empty WIN32 MACOSX_BUNDLE empty.c ${TINYCTHREAD} ${GLAD_GL})
add_executable(gamma WIN32 MACOSX_BUNDLE gamma.c ${GLAD_GL})
add_executable(icon WIN32 MACOSX_BUNDLE icon.c ${GLAD_GL})
add_executable(inputlag WIN32 MACOSX_BUNDLE inputlag.c ${GETOPT} ${GLAD_GL})
add_executable(tearing WIN32 MACOSX_BUNDLE tearing.c ${GLAD_GL})
add_executable(threads WIN32 MACOSX_BUNDLE threads.c ${TINYCTHREAD} ${GLAD_GL})
add_executable(timeout WIN32 MACOSX_BUNDLE timeout.c ${GLAD_GL})
add_executable(title WIN32 MACOSX_BUNDLE title.c ${GLAD_GL})
add_executable(window WIN32 MACOSX_BUNDLE window.c ${GLAD_GL})

target_link_libraries(empty Threads::Threads)
target_link_libraries(threads Threads::Threads)
if (RT_LIBRARY)
    target_link_libraries(empty "${RT_LIBRARY}")
    target_link_libraries(threads "${RT_LIBRARY}")
endif()

set(GUI_ONLY_BINARIES empty gamma icon inputlag tearing threads timeout title window)
set(CONSOLE_BINARIES allocator clipboard events msaa glfwinfo iconify monitors reopen cursor)

set_target_properties(${GUI_ONLY_BINARIES} ${CONSOLE_BINARIES} PROPERTIES C_STANDARD 99 FOLDER "GLFW3/Tests")

if (MSVC)
    # Tell MSVC to use main instead of WinMain
    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    # Tell Clang using MS CRT to use main instead of WinMain
    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif()
cmake_minimum_required(VERSION 3.22)

project(av24tr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Find Qt6 components for GUI and SerialPort
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui SerialPort)

# Explicitly list sources for desktop client
add_executable(desktop_client
    desktop/client/main.cpp
    desktop/client/src/canvas.cpp
    desktop/client/src/window.cpp
    desktop/client/src/comservice.cpp
    desktop/client/src/tcpservice.cpp
    desktop/client/src/uartservice.cpp
)

target_include_directories(desktop_client PUBLIC
    ${CMAKE_SOURCE_DIR}/desktop/client/include
    ${CMAKE_SOURCE_DIR}/shared
)

target_link_libraries(desktop_client PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::SerialPort
)

# Explicitly list sources for desktop server
add_executable(desktop_server
    desktop/server/main.cpp
    desktop/server/src/window.cpp
    desktop/server/src/comservice.cpp
    desktop/server/src/tcpservice.cpp
    desktop/server/src/uartservice.cpp
)

target_include_directories(desktop_server PRIVATE
    ${CMAKE_SOURCE_DIR}/desktop/server/include
    ${CMAKE_SOURCE_DIR}/shared
)

target_link_libraries(desktop_server PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::SerialPort
)

# Add a custom target to build client
add_custom_target(client
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target desktop_client
    DEPENDS desktop_client
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Add a custom target to build server
add_custom_target(server
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target desktop_server
    DEPENDS desktop_server
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Handle MaterialIcons.ttf font install
# === Install MaterialIcons.ttf from local res folder ===
set(FONT_NAME "MaterialIcons.ttf")
set(FONT_SOURCE_PATH "${CMAKE_SOURCE_DIR}/desktop/client/res/${FONT_NAME}")
set(FONT_DEST_DIR "$ENV{HOME}/.local/share/fonts")
set(FONT_DEST_PATH "${FONT_DEST_DIR}/${FONT_NAME}")

# Only install if it's not already installed in ~/.local/share/fonts
if(NOT EXISTS "${FONT_DEST_PATH}")
    message(STATUS "MaterialIcons.ttf not found in user fonts. Installing from local res...")

    if(NOT EXISTS "${FONT_SOURCE_PATH}")
        message(FATAL_ERROR "Local font file not found: ${FONT_SOURCE_PATH}")
    endif()

    file(MAKE_DIRECTORY "${FONT_DEST_DIR}")
    file(COPY "${FONT_SOURCE_PATH}" DESTINATION "${FONT_DEST_DIR}")

    execute_process(
        COMMAND fc-cache -f -v "${FONT_DEST_DIR}"
        RESULT_VARIABLE FC_RESULT
        OUTPUT_VARIABLE FC_OUTPUT
        ERROR_VARIABLE FC_ERROR
    )

    if(NOT FC_RESULT EQUAL 0)
        message(WARNING "fc-cache failed: ${FC_ERROR}")
    else()
        message(STATUS "fc-cache succeeded:\n${FC_OUTPUT}")
    endif()
endif()

# Dummy file to hook into build system
set(FONT_DUMMY_FILE "${CMAKE_BINARY_DIR}/.font_installed_stamp")

add_custom_command(
    OUTPUT "${FONT_DUMMY_FILE}"
    COMMAND ${CMAKE_COMMAND} -E touch "${FONT_DUMMY_FILE}"
    COMMENT "Marking font installed"
    VERBATIM
)

add_custom_target(install_fonts
    DEPENDS "${FONT_DUMMY_FILE}"
    
)

add_dependencies(desktop_client install_fonts)
add_dependencies(desktop_server install_fonts)
add_dependencies(client install_fonts)
add_dependencies(server install_fonts)

cmake_minimum_required(VERSION 3.22)

project(av24tr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
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
set(FONT_NAME "MaterialIcons.ttf")
set(FONT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
set(FONT_SOURCE_PATH "${FONT_SOURCE_DIR}/${FONT_NAME}")
set(FONT_DEST_DIR "$ENV{HOME}/.local/share/fonts")
set(FONT_DEST_PATH "${FONT_DEST_DIR}/${FONT_NAME}")
set(FONT_URL "https://github.com/google/material-design-icons/raw/master/font/MaterialIcons-Regular.ttf")

# Download font if it's missing in source tree
if(NOT EXISTS "${FONT_SOURCE_PATH}")
    message(STATUS "MaterialIcons.ttf not found in project. Downloading...")
    file(MAKE_DIRECTORY "${FONT_SOURCE_DIR}")
    file(DOWNLOAD "${FONT_URL}" "${FONT_SOURCE_PATH}" SHOW_PROGRESS STATUS DOWNLOAD_STATUS)

    list(GET DOWNLOAD_STATUS 0 DOWNLOAD_CODE)
    if(NOT DOWNLOAD_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download MaterialIcons.ttf from ${FONT_URL}")
    endif()
endif()

# Create custom target for installing the font system-wide for user
add_custom_command(
    OUTPUT "${FONT_DEST_PATH}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${FONT_DEST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${FONT_SOURCE_PATH}" "${FONT_DEST_PATH}"
    COMMAND fc-cache -f -v "${FONT_DEST_DIR}"
    COMMENT "Installing MaterialIcons.ttf to ${FONT_DEST_DIR} and updating font cache"
    VERBATIM
)

add_custom_target(install_fonts
    DEPENDS "${FONT_DEST_PATH}"
    COMMENT "Trigger font installation"
)

# Automatically install font before building binaries or custom targets
add_dependencies(desktop_client install_fonts)
add_dependencies(desktop_server install_fonts)
add_dependencies(client install_fonts)
add_dependencies(server install_fonts)

cmake_minimum_required(VERSION 3.22)

project(av24tr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Find Qt6 components for GUI and SerialPort
find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort)

# Handle MaterialIcons.ttf font install
# === Install MaterialIcons.ttf from local res folder ===
set(FONT_NAME "MaterialIcons.ttf")
set(FONT_SOURCE_PATH "${CMAKE_SOURCE_DIR}/desktop/client/res/${FONT_NAME}")
set(FONT_DEST_DIR "$ENV{HOME}/.local/share/fonts")
set(FONT_DEST_PATH "${FONT_DEST_DIR}/${FONT_NAME}")

# Only install if it's not already installed in ~/.local/share/fonts
if (NOT EXISTS "${FONT_DEST_PATH}")
    message(STATUS "MaterialIcons.ttf not found in user fonts. Installing from local res...")

    if (NOT EXISTS "${FONT_SOURCE_PATH}")
        message(FATAL_ERROR "Local font file not found: ${FONT_SOURCE_PATH}")
    endif ()

    file(MAKE_DIRECTORY "${FONT_DEST_DIR}")
    file(COPY "${FONT_SOURCE_PATH}" DESTINATION "${FONT_DEST_DIR}")

    execute_process(
            COMMAND fc-cache -f -v "${FONT_DEST_DIR}"
            RESULT_VARIABLE FC_RESULT
            OUTPUT_VARIABLE FC_OUTPUT
            ERROR_VARIABLE FC_ERROR
    )

    if (NOT FC_RESULT EQUAL 0)
        message(WARNING "fc-cache failed: ${FC_ERROR}")
    else ()
        message(STATUS "fc-cache succeeded:\n${FC_OUTPUT}")
    endif ()
endif ()

# Explicitly list sources for desktop client using tcp
add_executable(client_tcp
    desktop/client/main.cpp
    desktop/client/src/canvas.cpp
    desktop/client/src/window.cpp
    desktop/client/src/comservice.cpp
    desktop/client/src/tcpservice.cpp
)

target_include_directories(client_tcp PUBLIC
    ${CMAKE_SOURCE_DIR}/desktop/client/include
    ${CMAKE_SOURCE_DIR}/shared
)

target_link_libraries(client_tcp PUBLIC
    Qt6::Core
    Qt6::Widgets
)

# Explicitly list sources for desktop client using uart
add_executable(client_uart
    desktop/client/main.cpp
    desktop/client/src/canvas.cpp
    desktop/client/src/window.cpp
    desktop/client/src/comservice.cpp
    desktop/client/src/uartservice.cpp
)

target_compile_definitions(client_uart PRIVATE UARTCOM)

target_include_directories(client_uart PUBLIC
    ${CMAKE_SOURCE_DIR}/desktop/client/include
    ${CMAKE_SOURCE_DIR}/shared
)

target_link_libraries(client_uart PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::SerialPort
)

target_compile_definitions(client_uart PRIVATE UARTCOM)

# Explicitly list sources for desktop server using tcp
add_executable(server_tcp
    desktop/server/main.cpp
    desktop/server/src/window.cpp
    desktop/server/src/comservice.cpp
    desktop/server/src/tcpservice.cpp
    
)

target_include_directories(server_tcp PRIVATE
    ${CMAKE_SOURCE_DIR}/desktop/server/include
    ${CMAKE_SOURCE_DIR}/shared
)

target_link_libraries(server_tcp PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Explicitly list sources for desktop server using uart
add_executable(server_uart
    desktop/server/main.cpp
    desktop/server/src/window.cpp
    desktop/server/src/comservice.cpp
    desktop/server/src/uartservice.cpp
    
)

target_compile_definitions(server_uart PRIVATE UARTCOM)

target_include_directories(server_uart PRIVATE
    ${CMAKE_SOURCE_DIR}/desktop/server/include
    ${CMAKE_SOURCE_DIR}/shared
)

target_link_libraries(server_uart PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::SerialPort
   
)

target_compile_definitions(server_uart PRIVATE UARTCOM)
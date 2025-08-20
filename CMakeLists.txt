cmake_minimum_required(VERSION 3.22)

project(av24tr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

set(CLIENT_DIR desktop/client)
set(SERVER_DIR desktop/server)

# Find Qt6 components for GUI and SerialPort
find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort Multimedia)

set(CLIENT_HEADERS
    shared/setting.h
    ${CLIENT_DIR}/include/window.h
    ${CLIENT_DIR}/include/canvas.h
    ${CLIENT_DIR}/include/comservice.h)
set(CLIENT_SOURCES
    ${CLIENT_DIR}/main.cpp
    ${CLIENT_DIR}/src/canvas.cpp
    ${CLIENT_DIR}/src/window.cpp
    ${CLIENT_DIR}/src/comservice.cpp)
set(CLIENT_LIBRARIES Qt6::Core Qt6::Widgets Qt6::Multimedia)


set(SERVER_HEADERS
    shared/setting.h
    ${SERVER_DIR}/include/window.h
    ${SERVER_DIR}/include/comservice.h)
set(SERVER_SOURCES
    ${SERVER_DIR}/main.cpp
    ${SERVER_DIR}/src/window.cpp
    ${SERVER_DIR}/src/comservice.cpp)
set(SERVER_LIBRARIES Qt6::Core Qt6::Widgets)

set(RES_DIR "${CMAKE_SOURCE_DIR}/desktop/client/res")

# Handle MaterialIcons.ttf font install
# === Install MaterialIcons.ttf from local res folder ===
set(FONT_NAME "MaterialIcons.ttf")
set(FONT_SOURCE_PATH "${RES_DIR}/${FONT_NAME}")
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

# Handle sound.wav file, setting up a variable for the name and making sure it's in build folder
set(WAV_FILE "sound.wav")
if (NOT EXISTS "${CMAKE_BINARY_DIR}/${WAV_FILE}")
    file(COPY "${RES_DIR}/${WAV_FILE}" DESTINATION ${CMAKE_BINARY_DIR})
endif()

if (UARTCOM)
    set(SERVER_UPORT "/dev/ttyUSB0")
    set(CLIENT_UPORT "/dev/ttyUSB1")
    add_compile_definitions(UARTCOM UART_SPORT="${SERVER_UPORT}" UART_CPORT="${CLIENT_UPORT}")

    set(CLIENT_HEADERS ${CLIENT_HEADERS} ${CLIENT_DIR}/include/uartservice.h)
    set(CLIENT_SOURCES ${CLIENT_SOURCES} ${CLIENT_DIR}/src/uartservice.cpp)
    set(CLIENT_LIBRARIES ${CLIENT_LIBRARIES} Qt6::SerialPort)

    set(SERVER_HEADERS ${SERVER_HEADERS} ${SERVER_DIR}/include/uartservice.h)
    set(SERVER_SOURCES ${SERVER_SOURCES} ${SERVER_DIR}/src/uartservice.cpp)
    set(SERVER_LIBRARIES ${SERVER_LIBRARIES} Qt6::SerialPort)

    add_custom_target(upload_client cd ${CMAKE_SOURCE_DIR}/esp32/client && pio run --target upload --upload-port ${CLIENT_UPORT})

    add_custom_target(upload_server cd ${CMAKE_SOURCE_DIR}/esp32/server && pio run --target upload --upload-port ${SERVER_UPORT})

    else()

    set(CLIENT_HEADERS ${CLIENT_HEADERS} ${CLIENT_DIR}/include/tcpservice.h)
    set(CLIENT_SOURCES ${CLIENT_SOURCES} ${CLIENT_DIR}/src/tcpservice.cpp)

    set(SERVER_HEADERS ${SERVER_HEADERS} ${SERVER_DIR}/include/tcpservice.h)
    set(SERVER_SOURCES ${SERVER_SOURCES} ${SERVER_DIR}/src/tcpservice.cpp)

endif()

add_executable(client ${CLIENT_HEADERS} ${CLIENT_SOURCES})
target_link_libraries(client PUBLIC ${CLIENT_LIBRARIES})
target_include_directories(client PUBLIC shared ${CLIENT_DIR}/include)

add_executable(server ${SERVER_HEADERS} ${SERVER_SOURCES})
target_link_libraries(server PUBLIC ${SERVER_LIBRARIES})
target_include_directories(server PUBLIC shared ${SERVER_DIR}/include)

add_custom_target(use_uart cd ${CMAKE_BINARY_DIR} && cmake .. -DUARTCOM:BOOL=ON)
add_custom_target(use_tcpip cd ${CMAKE_BINARY_DIR} && cmake .. -DUARTCOM:BOOL=OFF)
cmake_minimum_required(VERSION 3.22)

project(battleships VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  set(CMAKE_CXX_FLAGS "/std:c++23preview /EHsc")
  # EHsc: warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
else()
  set(CMAKE_CXX_FLAGS "-std=c++23 -Wall -Wextra -pedantic -Wconversion -Wcast-align -Wunused -Wshadow -Wold-style-cast -lgcov")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif ()

# -----------------------------
#         uWebSockets + needed externals
# -----------------------------
set(FETCHCONTENT_QUIET FALSE)
add_subdirectory(externals)
# -----------------------------

# -----------------------------
#              Qt
# -----------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui WebSockets Core Network Concurrent)
set(CMAKE_AUTOMOC ON)
qt_standard_project_setup()
# -----------------------------

# ------------------------------------------
#              Battleships Client
# ------------------------------------------
set(BATTLESHIPS_CLIENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/battleships_client/src)
set(BATTLESHIPS_CLIENT_SOURCE_FILES ${BATTLESHIPS_CLIENT_SOURCE_DIR}/main.cpp
                                    ${BATTLESHIPS_CLIENT_SOURCE_DIR}/mainwindow.h
                                    ${BATTLESHIPS_CLIENT_SOURCE_DIR}/mainwindow.cpp
                                    ${BATTLESHIPS_CLIENT_SOURCE_DIR}/mainwindow.ui
)

qt_add_executable(battleships_client ${BATTLESHIPS_CLIENT_SOURCE_FILES})
target_link_libraries(battleships_client PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Network Qt6::WebSockets Qt::Concurrent)
# ===============================================================


# ------------------------------------------
#              Battleships Server
# ------------------------------------------
set(BATTLESHIPS_SERVER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/battleships_server/src)
set(BATTLESHIPS_SERVER_SOURCE_FILES
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/main.cpp
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/game/board.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/game/game_hub.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/game/game.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/game/match.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/game/message.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/game/lobby.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/net/websocket_server.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/net/publisher.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/net/connection.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/net/client.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/util/thread_pool.h
  ${BATTLESHIPS_SERVER_SOURCE_DIR}/util/non_owning_ptr.h
)

include_directories(${BATTLESHIPS_SERVER_SOURCE_DIR})
add_executable(battleships_server ${BATTLESHIPS_SERVER_SOURCE_FILES})
target_link_libraries(battleships_server PRIVATE uWebSockets)
target_precompile_headers(battleships_server PUBLIC ${BATTLESHIPS_SERVER_SOURCE_DIR}/stdafx.h)

if (MSVC)
  # Copy generated ZLIB library into project output directory()
  add_custom_command(TARGET battleships_server
                    POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                    copy "${zlib_BINARY_DIR}/${CMAKE_BUILD_TYPE}/zlibd.dll"
                          "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
                    COMMENT "Copying ZLIB library into build folder")
endif()
# ===============================================================

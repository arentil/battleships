cmake_minimum_required(VERSION 3.22)

project(battleships VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets Gui WebSockets Core Network Concurrent)
set(CMAKE_AUTOMOC ON)
qt_standard_project_setup()
# ===============================================================


if (MSVC)
  set(CMAKE_CXX_FLAGS "/std:c++23preview /bigobj")
else()
  set(CMAKE_CXX_FLAGS "-std=c++23 -Wall -Wextra -pedantic -Wconversion -Wcast-align -Wunused -Wshadow -Wold-style-cast -lgcov")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif ()


# ------------------------------------------
#              Battleships
# ------------------------------------------
set(BATTLESHIPS_CLIENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/battleships_client/src)
set(BATTLESHIPS_CLIENT_SOURCE_FILES ${BATTLESHIPS_CLIENT_SOURCE_DIR}/main.cpp)

qt_add_executable(battleships_client ${BATTLESHIPS_CLIENT_SOURCE_FILES}
  battleships_client/src/mainwindow.h battleships_client/src/mainwindow.cpp battleships_client/src/mainwindow.ui)
target_link_libraries(battleships_client PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Network Qt6::WebSockets Qt::Concurrent)
# ===============================================================

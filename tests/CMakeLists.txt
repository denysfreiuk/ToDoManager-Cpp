enable_language(C)
enable_language(CXX)

set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER} CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER} CACHE FILEPATH "" FORCE)

cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0111 OLD)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include(FetchContent)

# ======================================================
# === GoogleTest Integration
# ======================================================
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# ======================================================
# === SQLite Auto-Download & Build
# ======================================================
set(SQLITE_PATH "${CMAKE_BINARY_DIR}/sqlite")
set(SQLITE_LOCAL "${CMAKE_CURRENT_BINARY_DIR}/sqlite3.c")

if(NOT EXISTS "${SQLITE_LOCAL}")
    message(STATUS "Downloading SQLite amalgamation for tests...")
    file(MAKE_DIRECTORY "${SQLITE_PATH}")
    file(DOWNLOAD
            "https://www.sqlite.org/2024/sqlite-amalgamation-3460000.zip"
            "${SQLITE_PATH}/sqlite.zip"
            SHOW_PROGRESS
    )
    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf "${SQLITE_PATH}/sqlite.zip"
            WORKING_DIRECTORY "${SQLITE_PATH}"
    )
    file(GLOB SQLITE_SRC "${SQLITE_PATH}/sqlite-amalgamation-*/sqlite3.c")
    file(COPY "${SQLITE_SRC}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

if(NOT TARGET sqlite3)
    add_library(sqlite3 STATIC "${SQLITE_LOCAL}")
    target_include_directories(sqlite3 PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# ======================================================
# === Qt Libraries
# ======================================================
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia Test)

# ======================================================
# === Project Sources
# ======================================================
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE CORE_SOURCES
        "${CMAKE_SOURCE_DIR}/databaseManager/*.cpp"
        "${CMAKE_SOURCE_DIR}/accounts/*.cpp"
        "${CMAKE_SOURCE_DIR}/tasks/*.cpp"
        "${CMAKE_SOURCE_DIR}/logger/*.cpp"
)
file(GLOB_RECURSE UI_SOURCES
        "${CMAKE_SOURCE_DIR}/authWindow/*.cpp"
        "${CMAKE_SOURCE_DIR}/mainWindow/*.cpp"
        "${CMAKE_SOURCE_DIR}/settings/*.cpp"
        "${CMAKE_SOURCE_DIR}/windowEdit/*.cpp"
)
set(RESOURCES_QRC "${CMAKE_SOURCE_DIR}/resources.qrc")

# ======================================================
# === Test Executable
# ======================================================
add_executable(QtEx_tests
        ${TEST_SOURCES}
        ${CORE_SOURCES}
        ${UI_SOURCES}
        ${RESOURCES_QRC}
)

# ======================================================
# === Link Dependencies
# ======================================================
target_link_libraries(QtEx_tests PRIVATE
        gtest
        sqlite3
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::Test
)

target_include_directories(QtEx_tests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/databaseManager
        ${CMAKE_SOURCE_DIR}/databaseManager/SQLUtilities
        ${CMAKE_SOURCE_DIR}/accounts
        ${CMAKE_SOURCE_DIR}/tasks
        ${CMAKE_SOURCE_DIR}/logger
        ${CMAKE_SOURCE_DIR}/mainWindow
        ${CMAKE_SOURCE_DIR}/settings
        ${CMAKE_SOURCE_DIR}/authWindow
        ${CMAKE_SOURCE_DIR}/windowEdit
)

# ======================================================
# === Enable GTest Discovery
# ======================================================
include(GoogleTest)
gtest_discover_tests(QtEx_tests)
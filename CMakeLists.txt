cmake_minimum_required(VERSION 3.16)
project(QtEx VERSION 0.1 LANGUAGES CXX)

# ======================================================
# === Основні налаштування проекту
# ======================================================
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ======================================================
# === Шляхи до Qt і бібліотек
# ======================================================
set(CMAKE_PREFIX_PATH "C:/Qt/6.10.0/mingw_64/lib/cmake")

# ======================================================
# === Пошук бібліотек Qt
# ======================================================
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

# ======================================================
# === Джерельні файли
# ======================================================
set(PROJECT_SOURCES
        main.cpp

        # --- Main window & core UI ---
        mainWindow/mainwindow.cpp
        mainWindow/mainwindow.h
        mainWindow/mainwindow.ui
        mainWindow/taskEditorWindow.cpp
        mainWindow/taskEditorWindow.h
        mainWindow/taskEditorWindow.ui
        mainWindow/taskItemWidget.cpp
        mainWindow/taskItemWidget.h
        mainWindow/taskItemWidget.ui

        # --- Accounts & Auth ---
        accounts/account.cpp
        accounts/account.h
        accounts/authManager.cpp
        accounts/authManager.h
        authWindow/loginWindow.cpp
        authWindow/loginWindow.h
        authWindow/loginWindow.ui
        authWindow/registerWindow.cpp
        authWindow/registerWindow.h
        authWindow/registerWindow.ui

        # --- Database ---
        databaseManager/SQLUtilities/SQLUtils.cpp
        databaseManager/SQLUtilities/SQLUtils.h
        databaseManager/accountRepository.cpp
        databaseManager/accountRepository.h
        databaseManager/databaseManager.cpp
        databaseManager/databaseManager.h
        databaseManager/TaskRepository.cpp
        databaseManager/TaskRepository.h

        # --- Tasks ---
        tasks/task.cpp
        tasks/task.h
        tasks/TaskManager.cpp
        tasks/TaskManager.h

        # --- Settings ---
        settings/settingsWindow.cpp
        settings/settingsWindow.h
        settings/settingsWindow.ui
        settings/appSettings.h
        settings/appSettings.cpp

        # --- Logging ---
        logger/logger.cpp
        logger/logger.h
        logger/globalLogger.cpp
        logger/globalLogger.h

        # --- Frameless Window & Snap ---
        windowEdit/framelessWindow.cpp
        windowEdit/framelessWindow.h
        windowEdit/snapPreviewWindow.cpp
        windowEdit/snapPreviewWindow.h

        # --- Resources ---
        resources.qrc
)

# ======================================================
# === Створення виконуваного файлу
# ======================================================
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(QtEx
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
    )
else()
    add_executable(QtEx ${PROJECT_SOURCES})
endif()

# ======================================================
# === Підключення бібліотек
# ======================================================
target_link_libraries(QtEx PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Multimedia
)

# ======================================================
# === SQLite (вбудована статична збірка)
# ======================================================
set(SQLITE_PATH "${CMAKE_BINARY_DIR}/sqlite")

if(NOT EXISTS "${SQLITE_PATH}/sqlite3.c")
    message(STATUS "Downloading SQLite amalgamation for main build...")
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
    file(COPY "${SQLITE_SRC}" DESTINATION "${SQLITE_PATH}")
endif()

add_library(sqlite3 STATIC "${SQLITE_PATH}/sqlite3.c")
target_include_directories(sqlite3 PUBLIC "${SQLITE_PATH}")
target_link_libraries(QtEx PRIVATE sqlite3)

# ======================================================
# === Властивості додатку (Windows / macOS)
# ======================================================
set_target_properties(QtEx PROPERTIES
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QtEx
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

# ======================================================
# === Автоматичний запуск windeployqt (Windows only)
# ======================================================
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

    add_custom_command(TARGET QtEx POST_BUILD
            COMMAND "${QT_BIN_DIR}/windeployqt.exe"
            --no-translations
            --multimedia
            "$<TARGET_FILE:QtEx>"
            COMMENT "Running windeployqt automatically after build..."
    )
endif()

# ======================================================
# === Налаштування компіляції для MinGW  | OPTIONAL(access to console)
# ======================================================
if(MINGW)
    set_target_properties(QtEx PROPERTIES LINK_FLAGS "-Wl,-subsystem,console")
endif()

# ======================================================
# === Завершення для Qt6
# ======================================================
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QtEx)
endif()

# ======================================================
# === Інсталяційні шляхи (CMake install)
# ======================================================
include(GNUInstallDirs)
install(TARGETS QtEx
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ======================================================
# === Підключення тестів
# ======================================================
option(BUILD_TESTS "Build unit tests" ON)

if (BUILD_TESTS)
    if (EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
        message(STATUS "Including GoogleTests...")
        add_subdirectory(tests)
    else()
        message(WARNING "Tests directory not found, skipping tests.")
    endif()
endif()
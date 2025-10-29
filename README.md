# ToDoManager-Cpp
ToDo Manager is a cross-platform desktop application developed in C++ with Qt 6. It provides a simple yet powerful interface for managing daily tasks with support for user accounts, secure authentication, and data persistence through SQLite.

# Run Application
To run application you need to add CMakeLists.txt and compile with CMake

Toolset: QT - Bundled MinGW

CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.16)

project(QtEx VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.10.0/mingw_64/lib/cmake")
set(SQLITE_PATH "C:/libs/sqlite")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp
        mainWindow/mainwindow.cpp
        mainWindow/mainwindow.h
        mainWindow/mainwindow.ui
        tasks/task.h
        tasks/task.cpp
        accounts/account.h
        accounts/account.cpp
        accounts/authManager.h
        accounts/authManager.cpp
        databaseManager/SQLUtilities/SQLUtils.cpp
        databaseManager/SQLUtilities/SQLUtils.h
        databaseManager/accountRepository.cpp
        databaseManager/accountRepository.h
        databaseManager/databaseManager.cpp
        databaseManager/databaseManager.h
        logger/logger.h
        logger/logger.cpp
        logger/globalLogger.h
        logger/globalLogger.cpp
        authWindow/loginWindow.h
        authWindow/loginWindow.cpp
        authWindow/loginWindow.ui
        authWindow/registerWindow.h
        authWindow/registerWindow.cpp
        authWindow/registerWindow.ui
        mainWindow/taskEditorWindow.h
        mainWindow/taskEditorWindow.cpp
        mainWindow/taskEditorWindow.ui
        databaseManager/TaskRepository.h
        databaseManager/TaskRepository.cpp
        tasks/TaskManager.h
        tasks/TaskManager.cpp
        mainWindow/taskItemWidget.h
        mainWindow/taskItemWidget.cpp
        mainWindow/taskItemWidget.ui
        settings/settingsWindow.h
        settings/settingsWindow.cpp
        settings/settingsWindow.ui
        settings/appSettings.h

        resources.qrc
        windowEdit/snapPreviewWindow.h
        windowEdit/snapPreviewWindow.cpp
        windowEdit/framelessWindow.h
        windowEdit/framelessWindow.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QtEx
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}

    )
else()
    if(ANDROID)
        add_library(QtEx SHARED
                ${PROJECT_SOURCES}
        )
    else()
        add_executable(QtEx
                ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(QtEx PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(QtEx PRIVATE Qt6::Widgets Qt6::Multimedia)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QtEx)
endif()
set_target_properties(QtEx PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QtEx
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QtEx)
endif()

target_include_directories(QtEx PUBLIC
        ${SQLITE_PATH}
)

target_link_libraries(QtEx PUBLIC
        "${SQLITE_PATH}/sqlite3.dll"
)

add_custom_command(TARGET QtEx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SQLITE_PATH}/sqlite3.dll"
        $<TARGET_FILE_DIR:QtEx>
)

if (MINGW)
    set_target_properties(QtEx PROPERTIES LINK_FLAGS "-Wl,-subsystem,console")
endif()
```

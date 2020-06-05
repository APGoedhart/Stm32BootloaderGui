cmake_minimum_required(VERSION 3.5)

project(BootloaderGui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS SerialPort REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)


if(ANDROID)
  add_library(BootloaderGui SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(BootloaderGui
    src/main.cpp
    src/mainwindow.cpp
    src/logView.cpp
    src/filelogger.cpp
    src/SerialSetup.cpp
    src/SerialSource.cpp
    src/softwareimage.cpp
    src/commands/command.cpp
    src/commands/commandstep.cpp
    src/commands/commandSequence.cpp
    src/mainwindow.ui
    src/logView.ui
    include/mainwindow.h
    include/filelogger.h
    include/logView.h
    include/SerialSetup.h
    include/SerialSource.h
    include/softwareimage.h
    include/commands/command.h
    include/commands/commandstep.h
    include/commands/commandSequence.h
    include/commands/RxPacket.h
      )
endif()

target_include_directories(BootloaderGui PUBLIC
    "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(BootloaderGui
    PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::SerialPort
 )

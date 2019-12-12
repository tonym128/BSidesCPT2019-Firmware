cmake_minimum_required(VERSION 3.0.0)
project(BSides2019CPT VERSION 0.1.0)
include_directories( ${MY_SOURCE_DIR} )

add_executable(${PROJECT_NAME} 
    tjpgd.c
    TJpg_Decoder.cpp
    qrcodegen.cpp
    parson.c
    game.cpp 
    myfont.cpp 
    mypigfont.cpp 
    myGame.cpp 
    platform_core.cpp
    fire.cpp
    voxel.cpp
    rotozoomer.cpp
    raycaster.cpp
    tunnel.cpp
    bt.cpp
    scroller.cpp
    asteroids.cpp
    feelings.cpp
    name.cpp
    cryptoRunner.cpp
    bleKeyboard.cpp
    wifiScanner.cpp
    setupScreen.cpp
    matrixscroller.cpp
    about.cpp
    schedule.cpp
    pong.cpp
    achievements.cpp
    )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

if (WIN32)
    find_package(SDL2 CONFIG REQUIRED)

    if (SDL2_FOUND)
        find_package(SDL2 CONFIG REQUIRED)
        add_definitions(-DSDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2::SDL2 SDL2::SDL2main)
    endif(SDL2_FOUND)
endif (WIN32)

if (UNIX)
    find_package(SDL2 REQUIRED)
    if (SDL2_FOUND)
        add_definitions(-DSDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    endif (SDL2_FOUND)
endif (UNIX)

include(CPack)

cmake_minimum_required(VERSION 3.16)
project(OrbRush LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# SFML 3 (Graphics, Window, System, Audio).
# No macOS com Homebrew a SFML costuma ser "keg-only"; ajudamos o CMake a acha-la.
if(APPLE AND EXISTS "/opt/homebrew/opt/sfml")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/sfml")
elseif(APPLE AND EXISTS "/usr/local/opt/sfml")
    list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/sfml")
endif()

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(OrbRush ${SOURCES})
target_include_directories(OrbRush PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(OrbRush PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio)

# Copia a pasta de assets para junto do executavel, para que o jogo
# encontre os recursos ao rodar de dentro do diretorio de build.
add_custom_command(TARGET OrbRush POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:OrbRush>/assets")

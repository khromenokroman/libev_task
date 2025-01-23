cmake_minimum_required(VERSION 3.10)
project(libev_task)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)

message(STATUS "CXX Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX Compiler name: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CXX Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

# Парсилка версии
function(parse_version)
    file(READ Version versionRaw)
    message(STATUS "Полученная версия из файла: ${versionRaw}")
    string(STRIP "${versionRaw}" version)
    string(REPLACE "." " " version ${version})    # Заменяем точки на пробел

    separate_arguments(versionSeparatedArgs NATIVE_COMMAND "${version}")

    list(GET versionSeparatedArgs 0 versionMajor)
    list(GET versionSeparatedArgs 1 versionMinor)
    list(GET versionSeparatedArgs 2 versionPatch)
    list(GET versionSeparatedArgs 3 versionTweak)

    message(STATUS "Мажорная версия: ${versionMajor}")
    message(STATUS "Минорная версия: ${versionMinor}")
    message(STATUS "Патч версия: ${versionPatch}")
    message(STATUS "Твик версия: ${versionTweak}")

    set(VERSION_MAJOR ${versionMajor} CACHE STRING "Major version number" FORCE)
    set(VERSION_MINOR ${versionMinor} CACHE STRING "Minor version number" FORCE)
    set(VERSION_PATCH ${versionPatch} CACHE STRING "Patch version number" FORCE)
    set(VERSION_TWEAK ${versionTweak} CACHE STRING "Tweak version number" FORCE)
endfunction()

parse_version()

add_definitions("-DVERSION_MAJOR=${VERSION_MAJOR}")
add_definitions("-DVERSION_MINOR=${VERSION_MINOR}")
add_definitions("-DVERSION_PATCH=${VERSION_PATCH}")
add_definitions("-DVERSION_TWEAK=${VERSION_TWEAK}")

# Опции компилятора для вывода ошибок и предупреждений
add_compile_options(
        -Wall
        -Wextra
        -Werror
        -Wsign-conversion
        -Wno-format-truncation
        -Wnon-virtual-dtor
        -Wno-stringop-overflow
        -Wno-stringop-truncation
)

# Добавляем исполняемый файл
add_executable(server
        src/app.cpp
        src/server.cpp
)

add_custom_target(version
        -DVERSION_MAJOR="${VERSION_MAJOR}"
        -DVERSION_MINOR="${VERSION_MINOR}"
        -DVERSION_PATCH="${VERSION_PATCH}"
        -DVERSION_TWEAK="${VERSION_TWEAK}"
)

# Подключаем библиотеку libev
target_link_libraries(server ev)

set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libev4 (>= 1:4.33)")
set(CPACK_PACKAGE_VENDOR "Khromenok Roman")
set(CPACK_PACKAGE_DESCRIPTION "Эхо сервер")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "roma55592@yandex.ru")
install(TARGETS server DESTINATION /usr/bin)
include(CPack)
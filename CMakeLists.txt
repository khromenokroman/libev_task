cmake_minimum_required(VERSION 3.10)
project(libev_task)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)

# Опции компилятора для вывода ошибок и предупреждений
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# Добавляем исполняемый файл
add_executable(server server.cpp)

# Подключаем библиотеку libev
target_link_libraries(server ev)
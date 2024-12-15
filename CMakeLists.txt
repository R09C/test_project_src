# /root/test_project_src/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(BMPUtils)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

# Библиотека bmp
add_library(bmp STATIC src/bmp/bmp.c)

# Установка директории с заголовочными файлами для библиотеки bmp
target_include_directories(bmp PUBLIC src/bmp)

# Утилита converter
add_executable(converter src/converter/converter.c)
# Указание директории с заголовочными файлами для converter
target_include_directories(converter PRIVATE src/bmp)
target_link_libraries(converter bmp)

# Утилита comparer
add_executable(comparer src/comparer/comparer.c)
# Указание директории с заголовочными файлами для comparer
target_include_directories(comparer PRIVATE src/bmp)
target_link_libraries(comparer bmp)

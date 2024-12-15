cmake_minimum_required(VERSION 3.10)
project(bmp_utils C)

# Определяем библиотеку bmp
add_library(bmp SHARED src/bmp/bmp.c)
target_include_directories(bmp PUBLIC src/bmp)

# Добавляем исполняемый файл converter
add_executable(converter src/converter/converter.c)
target_link_libraries(converter PRIVATE bmp)
target_include_directories(converter PRIVATE src)

# Добавляем исполняемый файл comparer
add_executable(comparer src/comparer/comparer.c)
target_link_libraries(comparer PRIVATE bmp)
target_include_directories(comparer PRIVATE src)

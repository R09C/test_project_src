#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIFFS 100

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: comparer image1.bmp image2.bmp\n");
        return 1; // Некорректные аргументы
    }

    const char *image1_file = argv[1];
    const char *image2_file = argv[2];

    BMPImage image1 = {0}, image2 = {0};
    int read_result1 = bmp_read(image1_file, &image1);
    if (read_result1 != 0) {
        fprintf(stderr, "Error reading BMP image '%s'.\n", image1_file);
        bmp_free(&image1);
        return 1; // Ошибка при чтении первого изображения
    }

    int read_result2 = bmp_read(image2_file, &image2);
    if (read_result2 != 0) {
        fprintf(stderr, "Error reading BMP image '%s'.\n", image2_file);
        bmp_free(&image1);
        bmp_free(&image2);
        return 1; // Ошибка при чтении второго изображения
    }

    // Проверка совпадения битности
    if (image1.info.biBitCount != image2.info.biBitCount) {
        fprintf(stderr, "Images have different bit depths.\n");
        bmp_free(&image1);
        bmp_free(&image2);
        return 1; // Несовпадение битности
    }

    // Проверка совпадения размеров
    if (image1.info.biWidth != image2.info.biWidth || image1.info.biHeight != image2.info.biHeight) {
        fprintf(stderr, "Images have different dimensions.\n");
        bmp_free(&image1);
        bmp_free(&image2);
        return 1; // Несовпадение размеров
    }

     // Обработка порядка строк (top-down vs bottom-up)
     // Приведение обоих изображений к top-down для унификации
    if (image1.info.biHeight < 0) {
         // Переворачиваем данные изображения 1
       int height = abs(image1.info.biHeight);
      int row_size = image1.info.biWidth * (image1.info.biBitCount / 8) + ((4 - (image1.info.biWidth * (image1.info.biBitCount / 8)) % 4) % 4);
     for (int y = 0; y < height / 2; y++) {
          uint8_t *row_top = image1.data + y * row_size;
         uint8_t *row_bottom = image1.data + (height - 1 - y) * row_size;
          // Меняем строки местами
        uint8_t *temp = (uint8_t *)malloc(row_size);
         if (!temp) {
              fprintf(stderr, "Memory allocation failed during image processing.\n");
           bmp_free(&image1);
            bmp_free(&image2);
           return 1;
          }
       memcpy(temp, row_top, row_size);
      memcpy(row_top, row_bottom, row_size);
      memcpy(row_bottom, temp, row_size);
       free(temp);
        }
    image1.info.biHeight = height; // Теперь положительное
   }
    
    if (image2.info.biHeight < 0) {
        // Переворачиваем данные изображения 2
       int height = abs(image2.info.biHeight);
       int row_size = image2.info.biWidth * (image2.info.biBitCount / 8) + ((4 - (image2.info.biWidth * (image2.info.biBitCount / 8)) % 4) % 4);
       for (int y = 0; y < height / 2; y++) {
            uint8_t *row_top = image2.data + y * row_size;
          uint8_t *row_bottom = image2.data + (height - 1 - y) * row_size;
         // Меняем строки местами
           uint8_t *temp = (uint8_t *)malloc(row_size);
          if (!temp) {
              fprintf(stderr, "Memory allocation failed during image processing.\n");
           bmp_free(&image1);
          bmp_free(&image2);
              return 1;
           }
        memcpy(temp, row_top, row_size);
      memcpy(row_top, row_bottom, row_size);
      memcpy(row_bottom, temp, row_size);
            free(temp);
        }
        image2.info.biHeight = height; // Теперь положительное
   }
    


    // Сравнение палитры для 8-битных изображений
     if (image1.info.biBitCount == 8 && image2.info.biBitCount == 8) {
        size_t palette_size = (image1.info.biClrUsed != 0) ? image1.info.biClrUsed : 256;
      if (memcmp(image1.palette, image2.palette, palette_size * 4) != 0) {
           // Палитры отличаются
           fprintf(stderr, "Palettes differ.\n");
           bmp_free(&image1);
         bmp_free(&image2);
          return 2; // Изображения различны
        }
     }
    


    // Сравнение пиксельных данных
   int bytes_per_pixel = image1.info.biBitCount / 8;
   int padding = (4 - (image1.info.biWidth * bytes_per_pixel) % 4) % 4;
  int row_size = image1.info.biWidth * bytes_per_pixel + padding;
   int height = abs(image1.info.biHeight);

   int diffs_found = 0;
   fprintf(stderr, "Next pixels are different:\n");
  for (int y = 0; y < height && diffs_found < MAX_DIFFS; y++) {
       for (int x = 0; x < image1.info.biWidth && diffs_found < MAX_DIFFS; x++) {
            uint8_t *pixel1 = image1.data + y * row_size + x * bytes_per_pixel;
           uint8_t *pixel2 = image2.data + y * row_size + x * bytes_per_pixel;

            if (memcmp(pixel1, pixel2, bytes_per_pixel) != 0) {
                fprintf(stderr, "x%-6d y%-6d\n", x, y);
              diffs_found++;
           }
        }
   }

    if (diffs_found > 0) {
        bmp_free(&image1);
        bmp_free(&image2);
        return 2; // Изображения различны
    }

    // Изображения совпадают
   printf("Images are same\n");
    bmp_free(&image1);
   bmp_free(&image2);
  return 0; // Изображения совпадают
}
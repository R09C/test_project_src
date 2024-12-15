#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int silent_mode = 0; // Флаг для отключения отладочных сообщений

    // Проверка наличия флага --silent или -s
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--silent") == 0 || strcmp(argv[i], "-s") == 0) {
            silent_mode = 1;
            break;
        }
    }
    

    if (argc < 3) {
          fprintf(stderr, "Usage: converter [--silent | -s] input.bmp output.bmp\n");
        return 1; // Некорректные аргументы
   }


  
  const char *input_file = argv[argc-2];
   const char *output_file = argv[argc-1];
    BMPImage image = {0};
    // Чтение входного BMP-файла
    if (bmp_read(input_file, &image) != 0) {
        fprintf(stderr, "Error reading BMP file '%s'.\n", input_file);
       bmp_free(&image);
      return 1; // Ошибка при чтении
  }

      // Отладочное сообщение о начальных параметрах изображения
     if (!silent_mode) {
         fprintf(stderr, "Converting image '%s' to negative.\n", input_file);
         fprintf(stderr, "Image width: %d, height: %d, bit count: %d\n",
            image.info.biWidth, image.info.biHeight, image.info.biBitCount);
          if (image.info.biBitCount == 8) {
             size_t palette_size = (image.info.biClrUsed != 0) ? image.info.biClrUsed : 256;
              fprintf(stderr, "Palette size: %zu\n", palette_size);
           }
   }



   // Конвертация в негатив
    if (image.info.biBitCount == 8) {
          // Определение размера палитры
         size_t palette_size = (image.info.biClrUsed != 0) ? image.info.biClrUsed : 256;
    
        // Проверка, что палитра существует
        if (image.palette == NULL) {
            fprintf(stderr, "Error: Palette is not initialized.\n");
            bmp_free(&image);
           return 1;
          }
           //  Вывод палитры перед инверсией
         if (!silent_mode) {
            fprintf(stderr, "Palette before inversion:\n");
                for (size_t i = 0; i < palette_size; i++) {
                    fprintf(stderr, "[%zu] B=%u G=%u R=%u\n", i, image.palette[i * 4 + 0], image.palette[i * 4 + 1], image.palette[i * 4 + 2]);
                 }
        }


     // Инвертируем палитру в памяти
        for(size_t i = 0; i < palette_size; i++){
             image.palette[i * 4 + 0] = 255 -  image.palette[i * 4 + 0];
              image.palette[i * 4 + 1] = 255 - image.palette[i * 4 + 1];
             image.palette[i * 4 + 2] = 255 - image.palette[i * 4 + 2];
            }


           //  Вывод палитры после инверсии
      if (!silent_mode) {
           fprintf(stderr, "Palette after inversion:\n");
                 for (size_t i = 0; i < palette_size; i++) {
                      fprintf(stderr, "[%zu] B=%u G=%u R=%u\n", i, image.palette[i * 4 + 0], image.palette[i * 4 + 1], image.palette[i * 4 + 2]);
               }
       }


    }
   else if (image.info.biBitCount == 24) {
        // Проверка, что данные изображения существуют
       if (image.data == NULL) {
            fprintf(stderr, "Error: Image data is not initialized.\n");
            bmp_free(&image);
            return 1;
        }
           // Инвертирование пикселей
     size_t bytes_per_pixel = 3;
     size_t padding = (4 - (image.info.biWidth * bytes_per_pixel) % 4) % 4;
    size_t row_size = image.info.biWidth * bytes_per_pixel + padding;

      for (int y = 0; y < abs(image.info.biHeight); y++) {
                uint8_t *row = image.data + y * row_size;
             for (int x = 0; x < image.info.biWidth; x++) {
                   uint8_t *pixel = row + x * bytes_per_pixel;
                  pixel[0] = 255 - pixel[0]; // Blue
                 pixel[1] = 255 - pixel[1]; // Green
                pixel[2] = 255 - pixel[2]; // Red
               }
         }
      }
    else {
        fprintf(stderr, "Unsupported BMP format (bit count: %d).\n", image.info.biBitCount);
         bmp_free(&image);
        return 1; // Неподдерживаемый формат
   }
  


  // Запись конвертированного BMP-файла
  if (bmp_write(output_file, &image) != 0) {
      fprintf(stderr, "Error writing BMP file '%s'.\n", output_file);
    bmp_free(&image);
        return 1; // Ошибка при записи
     }
       // Отладочное сообщение об успешной конвертации
     if (!silent_mode) {
          fprintf(stderr, "Successfully converted '%s' to '%s'.\n", input_file, output_file);
       }

   // Освобождение памяти
     bmp_free(&image);
   return 0; // Успех
}
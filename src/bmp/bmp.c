#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Функция для чтения BMP-файла
int bmp_read(const char *filename, BMPImage *image) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Cannot open file '%s'.\n", filename);
        return 1; // Ошибка открытия файла
    }

    // Чтение BMPFileHeader
    if (fread(&image->header, sizeof(BMPFileHeader), 1, file) != 1) {
        fprintf(stderr, "Failed to read BMPFileHeader from '%s'.\n", filename);
        fclose(file);
        return 1;
    }

    // Проверка типа файла ('BM')
    if (image->header.bfType != 0x4D42) { // 'BM' в little endian
        fprintf(stderr, "File '%s' is not a valid BMP file.\n", filename);
        fclose(file);
        return 1;
    }

    // Чтение BMPInfoHeader
    if (fread(&image->info, sizeof(BMPInfoHeader), 1, file) != 1) {
        fprintf(stderr, "Failed to read BMPInfoHeader from '%s'.\n", filename);
        fclose(file);
        return 1;
    }

    // Проверка размера заголовка
    if (image->info.biSize != 40) {
        fprintf(stderr, "Unsupported BMP header size (%u) in '%s'.\n", image->info.biSize, filename);
        fclose(file);
        return 1;
    }

    // Проверка количества бит на пиксель
    if (image->info.biBitCount != 8 && image->info.biBitCount != 24) {
        fprintf(stderr, "Unsupported BMP bit count (%d) in '%s'. Only 8-bit and 24-bit are supported.\n", image->info.biBitCount, filename);
        fclose(file);
        return 1;
    }

    // Проверка типа сжатия
    if (image->info.biCompression != 0) {
        fprintf(stderr, "Unsupported BMP compression (%u) in '%s'. Only uncompressed BMP is supported.\n", image->info.biCompression, filename);
        fclose(file);
        return 1;
    }

     // Чтение палитры для 8-битных изображений
    if (image->info.biBitCount == 8) {
        size_t palette_size = (image->info.biClrUsed != 0) ? image->info.biClrUsed : 256;
        image->palette = (uint8_t *)malloc(palette_size * 4);
           if (!image->palette) {
            fprintf(stderr, "Memory allocation failed for palette in '%s'.\n", filename);
            fclose(file);
            return 1;
         }

       if (fread(image->palette, 1, palette_size * 4, file) != palette_size * 4) {
              fprintf(stderr, "Failed to read palette from '%s'.\n", filename);
             free(image->palette);
           fclose(file);
              return 1;
          }
    } else {
        image->palette = NULL;
    }


    // Расчет паддинга и размера пиксельных данных
    size_t bytes_per_pixel = image->info.biBitCount / 8;
    size_t padding = (4 - (image->info.biWidth * bytes_per_pixel) % 4) % 4;
    size_t row_size = image->info.biWidth * bytes_per_pixel + padding;
    size_t data_size = row_size * (size_t)(abs(image->info.biHeight));

       // Проверка размера файла (для избежания переполнения буфера)
      if (image->header.bfSize < image->header.bfOffBits + data_size){
         fprintf(stderr, "Image size is bigger than file size in '%s'\n", filename);
        if(image->palette){
             free(image->palette);
         }
        fclose(file);
           return 1;
       }
    

     // Выделение памяти для пиксельных данных
     image->data = (uint8_t *)malloc(data_size);
        if (!image->data) {
          fprintf(stderr, "Memory allocation failed for pixel data in '%s'.\n", filename);
           if(image->palette){
              free(image->palette);
            }
            fclose(file);
            return 1;
        }


    // Перемещение указателя файла на начало пиксельных данных
    if (fseek(file, image->header.bfOffBits, SEEK_SET) != 0) {
         fprintf(stderr, "Failed to seek to pixel data in '%s'.\n", filename);
            if(image->palette){
              free(image->palette);
            }
          free(image->data);
          fclose(file);
         return 1;
    }

    // Чтение всех пиксельных данных
      if (fread(image->data, 1, data_size, file) != data_size) {
           fprintf(stderr, "Failed to read pixel data from '%s'.\n", filename);
           if(image->palette){
               free(image->palette);
           }
           free(image->data);
          fclose(file);
          return 1;
      }
    

    fclose(file);
    return 0; // Успех
}

// Функция для записи BMP-файла
int bmp_write(const char *filename, const BMPImage *image) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Cannot open file '%s' for writing.\n", filename);
        return 1; // File open error
    }

    // Calculate padding and data size
    size_t bytes_per_pixel = image->info.biBitCount / 8;
    size_t padding = (4 - (image->info.biWidth * bytes_per_pixel) % 4) % 4;
    size_t row_size = image->info.biWidth * bytes_per_pixel + padding;
    size_t data_size = row_size * (size_t)(abs(image->info.biHeight));


     // Update headers
    BMPFileHeader header = image->header; // Copy for modification
    BMPInfoHeader info = image->info;    // Copy for modification

      if (info.biBitCount == 8) {
         info.biClrUsed = (info.biClrUsed != 0) ? info.biClrUsed : 256;
    }

    info.biSizeImage = data_size; // Calculate correct image size including padding

    header.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + (info.biClrUsed * 4);
    header.bfSize = header.bfOffBits + data_size;

    // Write BMPFileHeader
    if (fwrite(&header, sizeof(BMPFileHeader), 1, file) != 1) {
        fprintf(stderr, "Failed to write BMPFileHeader to '%s'.\n", filename);
        fclose(file);
        return 1;
    }

    // Write BMPInfoHeader
    if (fwrite(&info, sizeof(BMPInfoHeader), 1, file) != 1) {
        fprintf(stderr, "Failed to write BMPInfoHeader to '%s'.\n", filename);
        fclose(file);
        return 1;
    }
    
       // Write palette for 8-bit images
    if (info.biBitCount == 8) {
      size_t palette_size = (info.biClrUsed != 0) ? info.biClrUsed : 256;
        if (fwrite(image->palette, 1, palette_size * 4, file) != palette_size * 4) {
            fprintf(stderr, "Failed to write palette to '%s'.\n", filename);
           fclose(file);
            return 1;
         }
    }
  

    // Write pixel data
   for (size_t y = 0; y < (size_t)(abs(info.biHeight)); y++) {
      if (fwrite(image->data + y * row_size, 1, row_size, file) != row_size) {
            fprintf(stderr, "Failed to write pixel data to '%s'.\n", filename);
            fclose(file);
           return 1;
         }
    }

    fclose(file);
    return 0; // Success
}

// Функция для освобождения памяти BMPImage
void bmp_free(BMPImage *image) {
    if (image->palette) {
        free(image->palette);
        image->palette = NULL;
    }
    if (image->data) {
        free(image->data);
        image->data = NULL;
    }
}
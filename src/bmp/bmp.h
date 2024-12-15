#ifndef BMP_H
#define BMP_H

#include <stdint.h>

// Структуры для BMP
#pragma pack(push, 1) // Убираем выравнивание структуры
typedef struct {
    uint16_t bfType;        // Тип файла (должно быть 0x4D42)
    uint32_t bfSize;        // Размер файла
    uint16_t bfReserved1;  // Зарезервировано
    uint16_t bfReserved2;  // Зарезервировано
    uint32_t bfOffBits;    // Смещение до начала пиксельных данных
} BMPFileHeader;

typedef struct {
    uint32_t biSize;           // Размер структуры (40 для BITMAPINFOHEADER)
    int32_t biWidth;           // Ширина изображения
    int32_t biHeight;          // Высота изображения (положительное для bottom-up, отрицательное для top-down)
    uint16_t biPlanes;         // Количество цветовых плоскостей (должно быть 1)
    uint16_t biBitCount;      // Биты на пиксель (8 или 24)
    uint32_t biCompression;    // Тип сжатия (0 для отсутствия сжатия)
    uint32_t biSizeImage;      // Размер изображения (включая паддинг)
    int32_t biXPelsPerMeter;  // Горизонтальное разрешение
    int32_t biYPelsPerMeter;  // Вертикальное разрешение
    uint32_t biClrUsed;        // Количество используемых цветов
    uint32_t biClrImportant;  // Количество важных цветов
} BMPInfoHeader;
#pragma pack(pop)

typedef struct {
    BMPFileHeader header;
    BMPInfoHeader info;
    uint8_t *palette;        // Палитра (только для 8-битных изображений)
    uint8_t *data;           // Пиксельные данные
} BMPImage;

// Прототипы функций
int bmp_read(const char *filename, BMPImage *image);
int bmp_write(const char *filename, const BMPImage *image);
void bmp_free(BMPImage *image);

#endif // BMP_H
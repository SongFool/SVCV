#include "stdint.h"
#include "stdbool.h"
typedef enum {
    MAT_U8 = 0,   // unsigned 8-bit
    MAT_U16,      // unsigned 16-bit  
    MAT_S16,      // signed 16-bit
    MAT_S32,      // signed 32-bit
    MAT_F32,      // float 32-bit
    MAT_F64       // double 64-bit
} MatType;

typedef struct {
    void *data;           // 数据指针
    int rows;            // 行数
    int cols;            // 列数
    int channels;        // 通道数
    MatType type;        // 数据类型
    size_t step;         // 行步长（字节）
    size_t elem_size;    // 每个元素的大小（字节）
} Mat;

typedef struct 
{
   uint8_t * (*read)(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);     
   int (*write)(const char *filename, int width, int height, int comp, const void *data);
}Image_ops;

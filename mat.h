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

typedef enum {
    COLOR_BGR2GRAY,

}COLOR_TYPE;

#pragma pack(push, 1)
typedef struct { unsigned char r, g, b; } RGB;
typedef struct {
    double h; // 0-360
    double s; // 0-1
    double v; // 0-1
} HSV;
#pragma pack(pop)


#define MAT_AS_2D(type, mat) ((type(*)[(mat)->cols])((mat)->data))
#define AT_2D(type, mat, i, j) (MAT_AS_2D(type, mat)[i][j])
#define at(type, mat, i, j) AT_2D(type, mat, i, j)
#define at_rgb(mat, i, j) at(RGB, mat, i, j)


#define DECLARE_MAT_2D(type, name, mat) type(*name)[(mat)->cols] = MAT_AS_2D(type, mat)


#define DECLARE_RGB_MAT(name, mat) DECLARE_MAT_2D(RGB, name, mat)
#define DECLARE_R_MAT(name, mat) DECLARE_MAT_2D(uint8_t, name, mat)
#define DECLARE_HSV_MAT(name, mat) DECLARE_MAT_2D(HSV, name, mat)





// #define MAT_AUTO_TYPE(name,mat) \
//     typedef struct { \
//         union { \
//             uint8_t (*chr)[(mat)->cols]; \
//             RGB (*rgb)[(mat)->cols]; \
//             HSV (*hsv)[(mat)->cols]; \
//         } data; \
//     } name##_t; \



typedef struct 
{
   uint8_t * (*read)(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);     
   int (*write)(const char *filename, int width, int height, int comp, const void *data);
}Image_ops;

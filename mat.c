#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "mat.h"

int write_image(const char *filename, int width, int height, int comp, const void *data) {
    const char *ext = strrchr(filename, '.');
    if (!ext) {
        printf("错误: 文件名缺少扩展名\n");
        return 0;
    }
    
    if (strcmp(ext, ".png") == 0) {
        return stbi_write_png(filename, width, height, comp, data, width * comp);
    }
    else if (strcmp(ext, ".bmp") == 0) {
        return stbi_write_bmp(filename, width, height, comp, data);
    }
    else if (strcmp(ext, ".tga") == 0) {
        return stbi_write_tga(filename, width, height, comp, data);
    }
    else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return stbi_write_jpg(filename, width, height, comp, data, 90); // 质量 90
    }
    else if (strcmp(ext, ".hdr") == 0) {
        return stbi_write_hdr(filename, width, height, comp, (const float*)data);
    }
    else {
        printf("错误: 不支持的格式: %s\n", ext);
        return 0;
    }
}

static const Image_ops _ops = {
    .read = stbi_load,
    .write = write_image,
};

Mat * imread(const char *filename,int comp){
    Mat * mat = (Mat *)malloc(sizeof(Mat));
    if (!mat) return NULL;
    
    // 使用 stb_image 加载图像
    mat->data = _ops.read(filename, &mat->rows, &mat->cols, &mat->channels, comp);
    mat->type = MAT_U8;
    mat->elem_size = 1;
    return mat;
}
int imwrite(Mat * mat,const char *filename,int comp)
{
    return _ops.write(filename,mat->rows,mat->cols,comp,mat->data);
} 

int main()
{
    SetConsoleOutputCP(65001);
    imwrite(imread("output.jpg",3),"test_out.jpg",3);
}
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
    mat->data = _ops.read(filename, &mat->cols, &mat->rows, &mat->channels, comp);
    mat->type = MAT_U8;
    mat->elem_size = 1;
    printf("mat->channels:%d\r\n",mat->channels);
    return mat;
}
int imwrite(Mat * mat,const char *filename,int comp)
{
    return _ops.write(filename,mat->cols,mat->rows,comp,mat->data);
} 

Mat * mat_create(int rows,int cols,int channels,MatType type){
    Mat * p_mat = malloc(sizeof(Mat));
    switch(type){
    case MAT_U8 :  
        p_mat->data = malloc(rows * cols * channels);
    break;
    case MAT_F64 :     
        p_mat->data = malloc(rows * cols * channels * 8);
    break;
    }
    
    p_mat->cols = cols;
    p_mat->rows = rows;
    p_mat->channels = channels;
    p_mat->elem_size = 1;
    p_mat->type = type;
    p_mat->step = 1;
    return p_mat;
}
int sv_rgb_to_hsv(Mat* input, Mat * output)
{
    if(input == NULL || output == NULL){
        return 1;
    }
    DECLARE_RGB_MAT(p_input,input);
    DECLARE_HSV_MAT(p_output,output);
    HSV hsv;

    for(int i = 0; i < output->rows; i++){
        for(int j = 0; j < input->cols; j++){
            float R = p_input[i][j].r / 255.0f;
            float G = p_input[i][j].g  / 255.0f;
            float B = p_input[i][j].b  / 255.0f;
            float cmax = fmaxf(R, fmaxf(G, B));
            float cmin = fminf(R, fminf(G, B));
            float delta = cmax - cmin;
            // Hue
            if (delta == 0)
                hsv.h = 0;
            else if (cmax == R)
                hsv.h = fmodf((60 * ((G - B) / delta) + 360), 360.0f);
            else if (cmax == G)
                hsv.h = fmodf((60 * ((B - R) / delta) + 120), 360.0f);
            else
                hsv.h = fmodf((60 * ((R - G) / delta) + 240), 360.0f);

            // Saturation
            hsv.s = (cmax == 0) ? 0 : (delta / cmax);

            // Value
            hsv.v = cmax;
        }
    }
    return 0;
}
int sv_rgb_to_gray(Mat* input, Mat * output)
{
    if(input == NULL || output == NULL){
        return 1;
    }
    DECLARE_RGB_MAT(p_input,input);
    DECLARE_R_MAT(p_output,output);
    for(int i = 0; i < output->rows; i++){
        for(int j = 0; j < input->cols; j++){
            uint8_t gray = (uint8_t)((p_input[i][j].r * 77 +  p_input[i][j].g * 150 +  p_input[i][j].b * 29) >> 8);
            p_output[i][j] = gray;
        }
    }
    return 0;
}

int sv_cvtColor(Mat* input, Mat * output,COLOR_TYPE type)
{

}

int sv_crop(Mat* src, int x, int y, int w, int h)
{

    // 分配新内存
    void* crop_data = malloc(src->cols *  src->rows * src->channels);
    if (!crop_data) { printf("内存失败");return 3;}

    // 使用正确的类型访问
    DECLARE_RGB_MAT(src_array,src);
    RGB(*dst_array)[src->cols] = (RGB(*)[src->cols])crop_data;
    printf("尺寸%dx%d\n", src->cols, src->rows);
    // 正确的循环顺序：行→列
    for (int i = 0; i < h; i++) {          // 行循环 (高度方向)
        for (int j = 0; j < w; j++) {      // 列循环 (宽度方向)
            dst_array[i][j] = src_array[i][j];
        }
    }

    // 更新图像信息
    free(src->data);
    src->data = crop_data;
    src->cols = src->cols;
    src->rows = src->rows;
    src->step = w * sizeof(RGB);

    printf("裁剪完成: 新尺寸%dx%d\n", w, h);
    return 0;
}

Mat* sv_mat_add(Mat* a, Mat* b)
{
    if(a->data == NULL || b->data == NULL)
        return NULL;
    
    
}
int main()
{
    SetConsoleOutputCP(65001);
    Mat * p_mat = imread("test_1.jpg",3);
    Mat * p_out = mat_create(p_mat->rows,p_mat->cols,1,0);
    sv_crop(p_mat,0,0,600,200);
    imwrite(p_mat,"test_out.jpg",3);
}
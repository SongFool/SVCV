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
int sv_crop_1(Mat* src, int x, int y, int w, int h)
{
    // 检查边界
    if (x < 0 || y < 0 || w <= 0 || h <= 0 ||
        x + w > src->cols || y + h > src->rows) {
        return 1; // 参数错误
    }

    // 计算裁剪后的尺寸
    int crop_cols = w;
    int crop_rows = h;
    size_t crop_size = crop_cols * crop_rows * src->channels;
    
    // 分配新内存
    uint8_t* crop_data = (uint8_t*)malloc(crop_size);
    if (!crop_data) {
        return 2; // 内存分配失败
    }

    // 计算源图像的行字节数
    int src_row_bytes = src->cols * src->channels;
    int crop_row_bytes = crop_cols * src->channels;;

    // 逐行拷贝数据
    for (int i = 0; i < crop_rows; i++) {
        uint8_t* src_row = src->data + ((y + i) * src_row_bytes) + (x * src->channels);
        uint8_t* dst_row = crop_data + (i * crop_row_bytes);
        memcpy(dst_row, src_row, crop_row_bytes);
    }

#if 0
// 方法2：数组方式
uint8_t (*src_2d)[src_row_bytes] = (uint8_t (*)[src_row_bytes])src->data;
uint8_t (*dst_2d)[crop_row_bytes] = (uint8_t (*)[crop_row_bytes])crop_data;

for (int i = 0; i < crop_rows; i++) {
    uint8_t* src_row = &src_2d[y + i][x * src->channels];
    uint8_t* dst_row = &dst_2d[i][0];
    memcpy(dst_row, src_row, crop_row_bytes);
}

#endif

    // 更新源图像信息
    free(src->data);
    src->data = crop_data;
    src->cols = crop_cols;
    src->rows = crop_rows;

    return 0; // 成功
}
int sv_mat_add(Mat* a, Mat* b)
{
    #if 1
    if(a->data == NULL || b->data == NULL)
        return 1;
    if(a->channels != b->channels || a->cols != b->cols || a->rows != b->rows)
        return 2;

    switch(a->type){
        case MAT_U8:
        {      
            DECLARE_RGB_MAT(p_a, a);
            DECLARE_RGB_MAT(p_b, b);
            for(int i = 0; i < a->rows; i++){
                for(int j = 0; j < a->cols; j++){
                    uint16_t r = p_a[i][j].r + p_b[i][j].r;
                    uint16_t g = p_a[i][j].g + p_b[i][j].g;
                    uint16_t b_val = p_a[i][j].b + p_b[i][j].b;

                    p_a[i][j].r = (r > 255) ? 255 : r;
                    p_a[i][j].g = (g > 255) ? 255 : g;
                    p_a[i][j].b = (b_val > 255) ? 255 : b_val;
                }
            }
            break;
        }
    }
    return 0;
    #endif
}
int sv_mat_sub(Mat* a, Mat* b)
{
    #if 1
    if(a->data == NULL || b->data == NULL)
        return 1;
    if(a->channels != b->channels || a->cols != b->cols || a->rows != b->rows)
        return 2;

    switch(a->type){
        case MAT_U8:
        {      
            DECLARE_RGB_MAT(p_a, a);
            DECLARE_RGB_MAT(p_b, b);
            for(int i = 0; i < a->rows; i++){
                for(int j = 0; j < a->cols; j++){
                    int16_t r = p_a[i][j].r - p_b[i][j].r;
                    int16_t g = p_a[i][j].g - p_b[i][j].g;
                    int16_t b_val = p_a[i][j].b - p_b[i][j].b;

                    p_a[i][j].r = (r < 0) ? 0 : r;
                    p_a[i][j].g = (g < 0) ? 0 : g;
                    p_a[i][j].b = (b_val < 0) ? 0 : b_val;
                }
            }
            break;
        }
    }
    return 0;
    #endif
}

int sv_binarize(Mat* input, Mat * output,int threshold)
{
    if(input == NULL || output == NULL){
        return 1;
    }
    DECLARE_R_MAT(p_input,input);
    DECLARE_R_MAT(p_output,output);
    for(int i = 0; i < output->rows; i++){                 // 高   y    
        for(int j = 0; j < input->cols; j++){              //宽     x
            if(p_input[i][j] >= threshold){
                p_output[i][j] = 255;
            }else{
                p_output[i][j] = 0;
            }
        }
    }
    return 0;
}
int sv_binarize_1(Mat* input, Mat * output,int x, int y,int w,int h)
{
    if(input == NULL || output == NULL){
        return 1;
    }
    DECLARE_R_MAT(p_input,input);
    DECLARE_R_MAT(p_output,output);
    for(int i = 0; i < output->rows; i++){                 // 高   y    
        for(int j = 0; j < input->cols; j++){              //宽     x
            if(j >= x && i <= x + w && i >= h && y <= )
        }
    }
    return 0;
}
int main()
{
    SetConsoleOutputCP(65001);
    Mat * p_mat = imread("test_1.jpg",3);
    Mat * p_out = mat_create(p_mat->rows,p_mat->cols,1,0);
    //memset(p_out->data,0x30,p_out->channels * p_out->cols * p_out->rows);
    sv_rgb_to_gray(p_mat,p_out);
    sv_binarize(p_out,p_out,100);
    imwrite(p_out,"test_out.jpg",1);
}

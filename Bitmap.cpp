#include "Bitmap.h"
#include <stdio.h>
#include <stdlib.h>

Bitmap::Bitmap()
{

}

Bitmap::~Bitmap()
{
    free(bitmap_data);
}

int Bitmap::loadFromFile(char* filename)
{
        // 打开输入文件
    FILE* pFile = fopen(filename, "rb");
    if( pFile == 0 )
    {
        printf("加载图像文件失败");
        exit(0);
    }

    // 读取图象的大小信息
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, sizeof(width), 1, pFile);
    fread(&height, sizeof(height), 1, pFile);

    // 计算像素数据长度
    GLint    PixelLength;
    PixelLength = width * 3;
    while( PixelLength % 4 != 0 )
        ++PixelLength;
    PixelLength *= height;

    // 读取像素数据
    GLubyte* PixelData;
    PixelData = (GLubyte*)malloc(PixelLength);
    if( PixelData == 0 )
    {
        printf("内存不够用");
        exit(0);
    }

    fseek(pFile, 54, SEEK_SET);
    fread(PixelData, PixelLength, 1, pFile);

    // 关闭文件
    fclose(pFile);
    bitmap_data = PixelData;
    return 0;

}

#ifndef BITMAP_H
#define BITMAP_H

#include <GL/gl.h>

class Bitmap
{
    public:
        GLubyte* bitmap_data;
        GLint width;
        GLint height;

        Bitmap();
        ~Bitmap();
        int loadFromFile(char* filename);

    protected:
    private:
};

#endif // BITMAP_H

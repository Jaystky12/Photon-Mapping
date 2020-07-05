#ifndef IMAGE_H
#define IMAGE_H

#include "../Math/math_calc.h"
#include "../Math/vector.h"
using namespace std;

/*------------------------------------------------------------------
| Struct holds pixels/colors of rendered image
------------------------------------------------------------------*/
struct Image {
    int width, height;
    Color *pixels;

    Image(int _w, int _h) : width(_w), height(_h) {
        pixels = new Color[width * height];
    }

    Color getColor(int x, int y) {
        int index = (height - y - 1) * width + x;
        return pixels[index];
    }

    void setColor(int x, int y, const Color &c) {
        int index = (height - y - 1) * width + x;
        pixels[index] = c;
    }

    void addColor(int x, int y, const Color &c) {
        int index = (height - y - 1) * width + x;
        pixels[index] = pixels[index] + c;
    }

    int toInteger(double x) {
        /* Clamp to [0,1] */
        if (x < 0.0)
            x = 0.0;

        if (x > 1.0)
            x = 1.0;

        /* Apply gamma correction and convert to integer */
        int ret = int(pow(x, 1 / 2.2) * 255 + .5);
        if (ret < 0) {
            cout << x << endl;
            ret = 0;
        }
        return ret;
    }

    void Save(const string &filename) {
        /* Save image in PPM format */
        FILE *f = fopen(filename.c_str(), "wb");
        fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
        for (int i = 0; i < width * height; i++)
            fprintf(f, "%d %d %d ", toInteger(pixels[i].x),
                    toInteger(pixels[i].y),
                    toInteger(pixels[i].z));
        fclose(f);
    }
};

#endif
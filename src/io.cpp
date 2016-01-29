#include "io.h"

#include <string>
#include <QImage>
#include <QColor>

using std::string;

using std::tuple;
using std::make_tuple;
using std::tie;
#define DEBUG false

Image load_image(const char *path)
{

    if (!path)
        throw string("Error reading file ") + string(path);
    QImage in(path);
    if (in.isNull())
    {
        if (DEBUG)
            std::cout<<path<<std::endl;
        throw string("Error reading file ") + string(path);
    }
    if (DEBUG)
        std::cout<<"Ok\n";
    Image res(in.height(), in.width());

    for (uint i = 0; i < res.n_rows; ++i) {
        for (uint j = 0; j < res.n_cols; ++j) {
            QRgb p = in.pixel(j, i);
            res(i, j) = make_tuple(qRed(p), qGreen(p), qBlue(p));
        }
    }
    return res;
}

QImage save_image(const Image& im, const char *path)
{
    QImage out(im.n_cols, im.n_rows, QImage::Format_RGB32);
    uint r, g, b;
    QRgb p;
    for (uint i = 0; i < im.n_rows; ++i) {
        for (uint j = 0; j < im.n_cols; ++j) {
            tie (r,g,b)= im(i, j);
            p = qRgb(r,g,b);

            out.setPixel(j, i, p);
        }
    }

    if (!out.save(path))
        throw string("Error writing file ") + string(path);
    return out;
}

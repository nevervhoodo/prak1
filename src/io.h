#pragma once

#include "matrix.h"

#include <tuple>
#include <QImage>

typedef unsigned int uint;
typedef Matrix<uint> Bimg;
typedef Matrix<std::tuple<uint, uint, uint>> Image;

Image load_image(const char*);
QImage save_image(const Image&, const char*);

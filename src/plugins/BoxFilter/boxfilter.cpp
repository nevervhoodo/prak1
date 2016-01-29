#include "boxfilter.h"
#include <QString>
#include "io.h"
#include <iostream>
#include "matrix.h"

using std::cout;
using std::endl;

using std::tuple;
using std::get;
using std::tie;
using std::make_tuple;

class BoxFilterOp
{
public:
    static const int radius = 1;
    tuple<uint, uint, uint> operator () (const Image &m) const;
};

QString BoxFilter::Name()const
{
    return QString("BoxFilter");
}

Image BoxFilter::filterImage(const Image &image)
{

    return image.unary_map(BoxFilterOp());
}

tuple<uint, uint, uint> BoxFilterOp::operator () (const Image &m) const
{
    uint size = 2 * radius + 1;
    uint r, g, b, sum_r = 0, sum_g = 0, sum_b = 0;
    for (uint i = 0; i < size; ++i) {
        for (uint j = 0; j < size; ++j) {
            // Tie is useful for taking elements from tuple
            tie(r, g, b) = m(i, j);
            sum_r += r;
            sum_g += g;
            sum_b += b;
        }
    }
    auto norm = size * size;
    sum_r /= norm;
    sum_g /= norm;
    sum_b /= norm;
    return make_tuple(sum_r, sum_g, sum_b);
}


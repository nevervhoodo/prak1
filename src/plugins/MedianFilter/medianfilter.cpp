#include "medianfilter.h"
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

class MedianFilterOp
{
public:
    static const int radius = 1;
    tuple<uint, uint, uint> operator () (const Image &m) const;
};

QString MedianFilter::Name()const
{
    return QString("MedianFilter");
}

Image MedianFilter::filterImage(const Image &image)
{

    return image.unary_map(MedianFilterOp());
}

tuple<uint, uint, uint> MedianFilterOp::operator () (const Image &m) const
{
    uint size = 2 * radius + 1;
    uint r, g, b;
    std::vector<uint> vec_r;
    std::vector<uint> vec_g;
    std::vector<uint> vec_b;

    for (uint i = 0; i < size; ++i)
        for (uint j = 0; j < size; ++j)
        {
            tie(r, g, b) = m(i, j);
            vec_r.push_back(r);
            vec_g.push_back(g);
            vec_b.push_back(b);
        }

    std::sort(vec_r.begin(),vec_r.end());
    std::sort(vec_g.begin(),vec_g.end());
    std::sort(vec_b.begin(),vec_b.end());

    uint s = (vec_r.size() + 1)/2;

    return make_tuple(vec_r[s],vec_g[s],vec_b[s]);
}


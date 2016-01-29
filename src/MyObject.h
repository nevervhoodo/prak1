#pragma once

#include <fstream>
#include <tuple>

using std::tuple;
using std::ostream;
using std::endl;
using std::tie;

struct IObject
{
    IObject(tuple<int, int> in_location = tuple<int, int>(0, 0))
        : location(in_location)
    {}

    virtual ~IObject() {}
    virtual void Write(ostream &io_stream) = 0;
	virtual unsigned int GetSmallRad() = 0;
	virtual unsigned int GetBigRad() = 0;
    tuple<int, int> location;
};

struct Gear
    : public IObject
{
    /// radius of the inscribed circle
    unsigned int min_r;
    /// radis of the circumscribed circle 
    unsigned int max_r;
    /// is the gear is broken?
    bool is_broken;
    /// number of cogs
    int num_cogs;

    Gear(tuple<int, int> in_location = tuple<int, int>(0, 0), 
		unsigned int in_min_r = 0, unsigned int in_max_r = 0, 
		bool in_is_broken = true, int in_num_cogs = 0)
        : IObject(in_location), min_r(in_min_r), max_r(in_max_r),
        is_broken(in_is_broken), num_cogs(in_num_cogs)
    {}
    
    void Write(ostream &io_stream)
    {
        int x, y;
        tie(x, y) = location;
        io_stream << "Gear " << y << ' ' << x << ' ' << max_r << ' ' <<
            min_r << ' ' << (is_broken ? "TRUE " : "FALSE ") <<  num_cogs << endl;
    }
    unsigned int GetSmallRad()
    {
		return min_r;
	}
    unsigned int GetBigRad()
    {
		return max_r;
	}
};

struct Axis
    : public IObject
{
    Axis(tuple<int, int> in_location = tuple<int, int>(0, 0))
        : IObject(in_location)
    {}

	unsigned int GetSmallRad()
    {
		return 0;
	}
    unsigned int GetBigRad()
    {
		return 0;
	}
	
    void Write(ostream &io_stream)
    {
        int x, y;
        tie(x, y) = location;
        io_stream << "Axis " << y << ' ' << x << endl;
    }
};

#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#include "medianfilter_global.h"
#include "filterinterface.h"
#include <QtPlugin>
#include "io.h"

class MedianFilter:
        public QObject, public FilterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Filter")
    Q_INTERFACES(FilterInterface)
public:
    virtual ~MedianFilter() {}
    virtual QString Name() const;
    virtual Image filterImage(const Image &image);
};

#endif // MEDIANFILTER_H

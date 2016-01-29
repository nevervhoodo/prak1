#ifndef BOXFILTER_H
#define BOXFILTER_H

#include "boxfilter_global.h"
#include "filterinterface.h"
#include <QtPlugin>
#include "io.h"

class BoxFilter:
        public QObject, public FilterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Filter")
    Q_INTERFACES(FilterInterface)
public:
    virtual ~BoxFilter() {}
    virtual QString Name() const;
    virtual Image filterImage(const Image &image);
};

#endif

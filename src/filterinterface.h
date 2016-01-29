#ifndef FILTERINTERFACE_H
#define FILTERINTERFACE_H

#include <QtPlugin>
#include "io.h"

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
class QWidget;
class QPainterPath;
class QPoint;
class QRect;
class QString;
class QStringList;
QT_END_NAMESPACE

class FilterInterface
{
public:
    virtual ~FilterInterface() {}

    virtual QString Name() const = 0;
    virtual Image filterImage(const Image &image) = 0;
};

QT_BEGIN_NAMESPACE
#define FilterInterface_iid "Filter"
Q_DECLARE_INTERFACE(FilterInterface, FilterInterface_iid)
QT_END_NAMESPACE

#endif

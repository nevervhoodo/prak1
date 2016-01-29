#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "matrix.h"
#include "io.h"


class controller : public QObject
{
    Q_OBJECT
    char *imginpath=0;
public:
    explicit controller(QObject *parent = 0);

signals:
    void StartProcess();
    void ImgTaken (Image,int);
    void PrintStr(const char*);
public slots:
    void Run(char**);
    void Mend();
};

#endif // CONTROLLER_H

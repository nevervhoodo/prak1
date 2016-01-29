#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "matrix.h"
#include "io.h"
#include <QDir>
#include <QPluginLoader>
#include "filterinterface.h"
#include <vector>

class controller : public QObject
{
    Q_OBJECT
    char *imginpath;
    int filter = 0;
    QDir pluginsDir;
    QStringList pluginFileNames;
    std::vector <FilterInterface*> plugs;
    bool filt = false;
public:
    explicit controller(uint i, char **args, QObject *parent = 0);
    void Run();
signals:
    void StartProcess();
    void ChooseFilter();
    void ImgTaken (Image,int);
    void FilterFound(const QString&);
    void Filter(FilterInterface*);
public slots:
    void FilterChosen(int);
};

#endif // CONTROLLER_H

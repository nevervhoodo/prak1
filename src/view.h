#ifndef VIEW_H
#define VIEW_H

#include "MyObject.h"
#include "io.h"
#include <QObject>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <vector>
#include <tuple>
#include <math.h>
#include <memory>
#include <ctime>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;
using std::shared_ptr;

class view : public QObject
{
    Q_OBJECT
    char *imgoutpath;
    ofstream fout;
    int nfilters = 0;
public:
    explicit view(char *,char *,QObject *parent = 0);

signals:
    void FilterChosen(int);
public slots:
    void SaveImg(Image);
    void Output(int n);
    void PrintObjs(vector<shared_ptr<IObject>>&);
    void Prints(const char *);
    void FilterFound(const QString&);
    void ChooseFilter();
};

#endif // VIEW_H

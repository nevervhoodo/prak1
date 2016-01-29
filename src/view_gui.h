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
    const char *imgoutpath;
    ofstream fout;
    time_t start;
public:
    explicit view(const char *,const char *,QObject *parent = 0);

signals:
    void ShowImg(QImage);
    void PrintStr(const char*);
    void PrintN(int);
public slots:
    void SaveImg(Image);
    void Output(int n);
    void PrintObjs(vector<shared_ptr<IObject>>&);
    void Prints(const char *);
};

#endif // VIEW_H

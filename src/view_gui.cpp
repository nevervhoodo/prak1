#include "view_gui.h"
#include "io.h"
#include "matrix.hpp"
#include "matrix.h"
#include "MyObject.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <cstdio>
#include <QApplication>
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"

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

#define DEBUG false

view::view(const char *argv_2,const char *argv_3,QObject *parent) :
    QObject(parent),fout (argv_3)
{
    imgoutpath = argv_2;
    start = time(NULL);
}

void view::SaveImg(Image out)
{
    QImage img = save_image(out, imgoutpath);
    emit ShowImg(img);
}

void view::Output(int n)
{
    fout << n << endl;
    emit PrintN(n);
    if (DEBUG)
        cout<<n<<endl;
}

void view::Prints(const char *str)
{
    emit PrintStr(str);
    if (DEBUG)
        cout << str << endl;
//    time_t now = time(NULL);
//    double t=difftime(start, now);
//    printf("%f\n",t);
//    float c = clock()/CLOCKS_PER_SEC;
//    printf("%f  %ld\n",c,CLOCKS_PER_SEC);

}

void view::PrintObjs(vector<shared_ptr<IObject>>& object_array)
{
    std::stringstream ss;
    if (DEBUG)
        cout<<"PrintObjs "<<object_array.size()<<"\n";
    for (uint i=0;i<object_array.size();i++)
        object_array[i]->Write(ss);
    emit PrintStr(QString(ss.str().c_str()).replace("\n", "<br/>").toStdString().c_str());
}

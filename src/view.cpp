#include "view.h"
#include "io.h"
#include "matrix.hpp"
#include "matrix.h"
#include "MyObject.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>

using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::ofstream;
using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;
using std::shared_ptr;

#define DEBUG false

view::view(char *argv_2,char *argv_3,QObject *parent) :
    QObject(parent),fout (argv_3)
{
    imgoutpath = argv_2;
}

void view::SaveImg(Image out)
{
    save_image(out, imgoutpath);
}

void view::Output(int n)
{
    fout << n << endl;

    if (DEBUG)
        cout<<n<<endl;
}

void view::Prints(const char *str)
{
    cout << str << endl;
    time_t now = time(NULL);
    cout<<ctime(&now)<<endl;
//    float c = clock()/CLOCKS_PER_SEC;
//    printf("%f  %ld\n",c,CLOCKS_PER_SEC);
    if (DEBUG)
        cout<<str<<endl;
}

void view::PrintObjs(vector<shared_ptr<IObject>>& object_array)
{
    if (DEBUG)
        cout<<"PrintObjs "<<object_array.size()<<"\n";
    for (uint i=0;i<object_array.size();i++)
        object_array[i]->Write(fout);
}

void view::FilterFound(const QString &str)
{
    if (!nfilters)
        cout<<"Choose filter:"<<endl<<"none - [0]"<<endl;
    nfilters++;
    cout<<str.toStdString()<<" - ["<<nfilters<<"]"<<endl;
}

void view::ChooseFilter()
{
    int n=0;
    if (nfilters)
        cin>>n;
    emit FilterChosen(n);
}

#include "controller_gui.h"
#include "io.h"
#include <QString>
#include <QImage>
#include <string>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QFileDialog>
#include <ui_mainwindow.h>


using std::cout;
using std::endl;
#define DEBUG false
char *change_name(char *str, uint n)
{
    uint i,len;
    for (i=0;str[i];i++);
    len = i++;

    char *new_str = new char [len+32];
    size_t place=0;
    for (size_t i=0;i<len;i++)
        if (str[i]=='.')
            place=i;
    for (size_t i=0;i<place;i++)
        new_str[i]=str[i];
    new_str[place]=0;
    //~ new_str[place+1]=(char)('0'+n);
    snprintf(&new_str[place], 32, "_%d.bmp", n);
    return new_str;
}

controller::controller(QObject *parent) :
    QObject(parent)
{  
}

void controller::Run(char **in)
{
    uint len = strlen(*in);
    imginpath = new char[len];
    for (uint i=0;((*in)[i])&&(i<len-1);i++)
        imginpath[i] = (*in)[i];
    imginpath[len-1]=0;
    if (DEBUG)
        std::cout<<"Run"<<std::endl;
    char **in2=in;
    if (DEBUG)
        emit PrintStr("\niii");
    if (DEBUG)
        emit PrintStr(imginpath);
    Image img = load_image(*in2);
    if (DEBUG)
        emit PrintStr("\nt");
    if (DEBUG)
        emit PrintStr(*in);
    emit ImgTaken(img,0);
    for (int i=0;i<3;i++)
    {
        if (DEBUG)
            emit PrintStr("\nl");
        if (DEBUG)
            emit PrintStr(imginpath);
        if (DEBUG)
            emit PrintStr("\n");
        char *arg=change_name(imginpath,i+1);
        if (DEBUG)
            emit PrintStr("\nl");
        if (DEBUG)
            emit PrintStr(arg);
        if (DEBUG)
            emit PrintStr("\n");
        emit ImgTaken(load_image(arg),i+1);
    }
}

void controller::Mend()
{
    emit StartProcess();
}


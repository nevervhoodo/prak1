#include "mainwindow.h"
#include "controller_gui.h"
#include "filterinterface.h"
#include "model.h"
#include "view_gui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);
        MainWindow w;
        controller ctr;
        model m;
        view v("./res.bmp","./ans.txt");

        QObject::connect(&ctr,SIGNAL(StartProcess()),&m,SLOT(Process()));
        QObject::connect(&m, SIGNAL(SaveImg(Image)),&v,SLOT(SaveImg(Image)));
        QObject::connect(&m, SIGNAL(Output(int)),&v,SLOT(Output(int)));
        QObject::connect(&m, SIGNAL(PrintObjs(vector<shared_ptr<IObject> >&)),&v,SLOT(PrintObjs(vector<shared_ptr<IObject> >&)));
        QObject::connect(&ctr, SIGNAL(ImgTaken(Image,int)),&m,SLOT(ReceiveImgs(Image,int)));
        QObject::connect(&m, SIGNAL(Prints(const char*)),&v,SLOT(Prints(const char*)));

        QObject::connect(&w,SIGNAL(Run(char**)),&ctr,SLOT(Run(char**)));
        QObject::connect(&ctr,SIGNAL(PrintStr(const char*)),&w,SLOT(PrintStr(const char*)));
        QObject::connect(&v,SIGNAL(PrintN(int)),&w,SLOT(PrintN(int)));
        QObject::connect(&v,SIGNAL(PrintStr(const char*)),&w,SLOT(PrintStr(const char*)));
        QObject::connect(&w,SIGNAL(Mend()),&ctr,SLOT(Mend()));
        QObject::connect(&v,SIGNAL(ShowImg(QImage)),&w,SLOT(ShowImg(QImage)));
        QObject::connect(&w, SIGNAL(Ex()), qApp, SLOT(closeAllWindows()));
        QObject::connect(&w, SIGNAL(Filter(FilterInterface*)),&m,SLOT(Filter(FilterInterface*)));
        w.show();
        return a.exec();
    }
    catch (const char *s)
    {
        std::cout<<"ERROR "<<s<<std::endl;
    }
    catch (...){
        std::cout<<"ERROR "<<std::endl;
    }
}

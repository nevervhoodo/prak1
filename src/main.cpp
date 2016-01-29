#include <QCoreApplication>
#include <QObject>
#include "filterinterface.h"
#include "model.h"
#include "controller.h"
#include "view.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    try
    {
        //QCoreApplication a(argc, argv);
        model m;
        controller ctr(argc,argv);
        view v(argv[2],argv[3]);

        QObject::connect(&ctr,SIGNAL(StartProcess()),&m,SLOT(Process()));
        QObject:: connect(&m, SIGNAL(SaveImg(Image)),&v,SLOT(SaveImg(Image)));
        QObject::connect(&m, SIGNAL(Output(int)),&v,SLOT(Output(int)));
        QObject::connect(&m, SIGNAL(PrintObjs(vector<shared_ptr<IObject> >&)),&v,SLOT(PrintObjs(vector<shared_ptr<IObject> >&)));
        QObject::connect(&ctr, SIGNAL(ImgTaken(Image,int)),&m,SLOT(ReceiveImgs(Image,int)));
        QObject::connect(&m, SIGNAL(Prints(const char*)),&v,SLOT(Prints(const char*)));
        QObject::connect(&ctr,SIGNAL(Filter(FilterInterface*)),&m,SLOT(Filter(FilterInterface*)));
        QObject::connect(&ctr,SIGNAL(FilterFound(const QString&)),&v,SLOT(FilterFound(const QString&)));
        QObject::connect(&ctr,SIGNAL(ChooseFilter()),&v,SLOT(ChooseFilter()));
        QObject::connect(&v,SIGNAL(FilterChosen(int)),&ctr,SLOT(FilterChosen(int)));
        ctr.Run();
        //return a.exec();
    }
    catch (const char *s)
    {
        std::cout<<"ERROR "<<s<<std::endl;
    }
    catch (...){
        std::cout<<"ERROR "<<std::endl;
    }
}

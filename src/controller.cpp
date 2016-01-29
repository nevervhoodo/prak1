#include "controller.h"
#include "filterinterface.h"
#include "io.h"
#include <QDir>
#include <QPluginLoader>
#include <QtCore>

using std::cout;
using std::endl;
#define DEBUG false
char *change_name(const char *str, uint n)
{
    uint i,len;
    for (i=0;str[i];i++);
    len = i;
    char *new_str = new char [len+2];
    size_t place=0;
    for (size_t i=0;i<len;i++)
        if (str[i]=='.')
            place=i;
    for (size_t i=0;i<place;i++)
        new_str[i]=str[i];
    new_str[place]=0;
    //~ new_str[place+1]=(char)('0'+n);
    sprintf(new_str,"%s_%d.bmp",new_str,n);
    return new_str;
}

controller::controller(uint i, char **args, QObject *parent) :
    QObject(parent)
{
    if (i<4)
    {
        cout << "Usage: " << endl << args[0]
             << " <in_image.bmp> <out_image.bmp> <out_result.txt>" << endl;
        throw ("Error\n");
    }
    imginpath = args[1];
    if (i>4)
    {
        filt = true;
        //        foreach (QObject *plugin, QPluginLoader::staticInstances())
        //                populateMenus(plugin);

    }
}

void controller::Run()
{

    emit ImgTaken(load_image(imginpath),0);
    for (int i=0;i<3;i++)
    {
        char *arg=change_name(imginpath,i+1);
        emit ImgTaken(load_image(arg),i+1);
    }
    emit StartProcess();
    if (filt)
    {
        pluginsDir = QDir(qApp->applicationDirPath());
        pluginsDir.cd("plugins");
        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin) {
                pluginFileNames += fileName;
            }
            FilterInterface *iFilter = qobject_cast<FilterInterface *>(plugin);
            if (iFilter)
            {
                plugs.push_back(iFilter);
                emit FilterFound(iFilter->Name());
            }
        }
        emit ChooseFilter();
    }

}

void controller::FilterChosen(int n)
{
    if (n)
        emit Filter(plugs[n-1]);
}




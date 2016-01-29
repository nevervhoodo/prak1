#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGraphicsScene>
#include "qlayout.h"
#include <ctime>
#include <cstdlib>

#include "filterinterface.h"

#include <QPluginLoader>
#include <QTimer>

#include <QScrollArea>
#include <QMessageBox>
#include <QActionGroup>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QColorDialog>
#include <QInputDialog>
#include <QApplication>
#define DEBUG false

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowTitle(tr("Mending gears mechanism"));
    ui->setupUi(this);
    connect (ui->actionReload_Plugins,SIGNAL(triggered()),this,SLOT(on_actionReload_Plugins_triggered()));
    connect(ui->menuPlugins, SIGNAL(triggered(QAction*)), this, SLOT(applyFilter(QAction*)));
    on_actionReload_Plugins_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    if (DEBUG)
        ui->label->setText("hello\n");
    QString FileName=QFileDialog::getOpenFileName(this, "Choose an image",QDir::currentPath(),"Images (*.bmp);;All files (*.*)");
    if (FileName.isEmpty())
    {
        ui->label->setText(ui->label->text());
        throw "aaaaaa\n";
    }
    if (DEBUG)
        ui->label->setText(ui->label->text()+FileName);
    QImage Image = QImage(FileName);
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addPixmap(QPixmap::fromImage(Image));
    ui->graphicsView->setScene(scene);

    const char *str = FileName.toStdString().c_str();
    if (DEBUG)
        ui->label->setText(ui->label->text()+"\n"+str);
    char *s = (char*)str;
    if (DEBUG)
        ui->label->setText(ui->label->text()+"\nsd"+s);
    fileloaded = true;
    ui->label->setText("Log:\n");
    emit Run(&s);
}

void MainWindow::on_actionExit_triggered()
{
    emit Ex();
}

void MainWindow::on_actionMend_triggered()
{
    if (fileloaded)
        emit Mend();
}

void MainWindow::ShowImg(QImage out)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addPixmap(QPixmap::fromImage(out));
    ui->graphicsView->setScene(scene);
}

void MainWindow::PrintStr(const char* str)
{

    ui->label->setText(ui->label->text()+QString("<br/>")+str);
    time_t t;
    time(&t);
    char *s = ctime(&t);
    ui->label->setText(ui->label->text()+QString("<br/>")+s);
}

void MainWindow::PrintN(int n)
{
    char buf[32] = {0};
    snprintf(buf, 31, "%d\n", n);
    ui->label->setText(ui->label->text()+QString("<br/>")+QString(buf));

}

void MainWindow::clearPlugins()
{
    foreach (QAction *a, ui->menuPlugins->actions()) {
        if (a != ui->actionReload_Plugins) {
            ui->menuPlugins->removeAction(a);
        }
    }
}


void MainWindow::on_actionReload_Plugins_triggered()
{

    clearPlugins();
    pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();

        FilterInterface *iFilter = qobject_cast<FilterInterface *>(plugin);
        if (iFilter)
        {
            ui->menuPlugins->addAction(iFilter->Name());
            plugs.push_back(iFilter);
        }
    }
}

void MainWindow:: applyFilter(QAction* a)
{
    if (!fileloaded)
        return;

    if (a!=ui->actionReload_Plugins)
    {
        for (uint i=0;i<plugs.size();i++)
        {
            if (a->text()==plugs[i]->Name())
            {
                emit Filter(plugs[i]);
                return ;
            }
        }
    }
}

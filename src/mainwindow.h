#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QStringList>
#include "filterinterface.h"

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    bool fileloaded = false;
    std::vector <FilterInterface*> plugs;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void Run(char**);
    void Ex();
    void Mend();
    void Filter(FilterInterface*);
private slots:
    void on_actionOpen_triggered();
    void on_actionExit_triggered();
    void on_actionMend_triggered();
    void on_actionReload_Plugins_triggered();
    void ShowImg(QImage);
    void applyFilter(QAction*);
    void PrintStr(const char*);
    void PrintN(int);
private:
    Ui::MainWindow *ui;
private:
    void createActions();
    void createMenus();
    void clearPlugins();
    QDir pluginsDir;
};

#endif // MAINWINDOW_H

#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include "io.h"
#include "matrix.h"
#include "filterinterface.h"

#include "MyObject.h"

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

enum type_of_task
{
    none,axis,broken
};

typedef tuple<uint,uint> Coordinates;

class model : public QObject
{
    Q_OBJECT
    Image src;
    Image test[3];
    uint result_idx = 0;
    vector<shared_ptr<IObject>> object_array;
    type_of_task task_type = none;
public:
    explicit model(QObject *parent = 0);

signals:
    void SaveImg(Image );
    void Output(int n);
    void PrintObjs(vector<shared_ptr<IObject>> &object_array);
    void Prints(const char *);
public slots:
    void Process();
    void ReceiveImgs(Image ,int);
    void Filter(FilterInterface*);
};

#endif // MODEL_H

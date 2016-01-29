#include "model.h"

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
#include "filterinterface.h"

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

#define SIZE_OF_HYST 256
#define PRECISION 0.05
#define DEBUG false
#define DEBUG_SEG false
#define DEBUG_DT false
#define DEBUG_RADS false
#define DEBUG_COGS false
#define DEBUG_CHECK false
#define DEBUG_NEIS false
#define DEBUG_TEST false
#define DEBUG_GET false
#define DEBUG2 false
#define DEBUG3 false
#define DEBUGMVC false
#define NOTCOUNTTHRES true
#define SETTHRES 50

#include "io.h"
#include "matrix.h"

#include "MyObject.h"

//enum type_of_task
//{
//    none,axis,broken
//};

typedef tuple<uint,uint> Coordinates;

bool operator!= (Coordinates coord1, Coordinates coord2)
{
    int x1,x2,y1,y2;
    tie (y1,x1) = coord1;
    tie (y2,x2) = coord2;
    if ((abs(y2-y1)>=2)||(abs(x2-x1)>=2))
        return true;
    return false;
}

bool cmprads(uint a, uint b)
{
    if (a>b)
        if ((a-b)<=3)
            return true;
        else
            return false;
    else
        if ((b-a)<=3)
            return true;
        else
            return false;
}

class FindGears
{
    Image src_img;
    Bimg bin, bin_dt;
    uint threshold = 0;
    Coordinates insertplace = make_tuple(0,0);
    vector <Coordinates> centers;
    vector <Coordinates> cents_dt;
    vector <shared_ptr<IObject>> objects;
    vector <uint> small_rads;
    vector <uint> small_rads_dt;
    vector <uint> big_rads;
    uint n_parts=0;
    tuple <uint,uint> space_for_gear = make_tuple(0,0);
    uint iplace = 0;
    enum type_of_task task = none;
public:
    FindGears (Image &m): src_img(m.deep_copy()), bin(m.n_rows, m.n_cols),
        bin_dt(m.n_rows,m.n_cols),centers(0),cents_dt(0),objects(0),
        small_rads(0),small_rads_dt(0),big_rads(0)
    {
        if (DEBUGMVC)
            cout<<"in find_gears_class\n"<<src_img.n_cols<<" "<<src_img.n_rows<<endl;
        uint r,g,b;
        uint hystog[256];
        for (uint i=0;i<256;i++)
            hystog[i]=0;
        uint bright;
        if (DEBUG2)
            for (uint i=0;i<m.n_cols;i++)
            {
                uint r,g,b;
                tie (r,g,b) = m(i,i);
                if (g||r||b)
                    cout<<"src2 "<<r<<" "<<g<<" "<<b<<" "<<endl;
            }
        for (uint i=0;i<m.n_rows;i++)
            for (uint j=0;j<m.n_cols;j++)
            {
                tie (r,g,b) = m(i,j);
                bright = (r+g+b)/3;
                bin (i,j) = bright;
                hystog [bright] ++;
            }
        if (DEBUG2)
            for (uint i=0; i<256;i++)
                cout<<"h["<<i<<"]="<<hystog[i]<<endl;
        if (NOTCOUNTTHRES)
            threshold = SETTHRES;
        else
            threshold = CountThreshold(hystog);
        if (DEBUG2)
            cout<<threshold<<endl;
        for (uint i=0;i<m.n_rows;i++)
            for (uint j=0;j<m.n_cols;j++)
            {
                if (bin(i,j)<threshold)
                    bin(i,j)=bin_dt(i,j)=0;
                else
                    bin(i,j)=bin_dt(i,j)=1;
            }
    }
    void Segmentation ()
    {
        uint a,b,c;
        uint counter = 1;
        vector <uint> arr;
        arr.push_back(0);
        for (uint i=0;i<bin.n_rows;i++)
            for (uint j=0;j<bin.n_cols;j++)
            {
                a=bin(i,j);
                TakeTwoNeighs(i,j,&b,&c);
                if (a)
                {
                    if ((!b)&(!c))
                    {
                        bin(i,j)=counter;
                        arr.push_back(counter);
                        counter++;
                    }
                    else if ((b!=0)^(c!=0))
                        bin(i,j)=b+c;
                    else
                    {
                        if (b==c)
                            bin(i,j)=b;
                        else if (b>c)
                            bin(i,j-1)=bin(i,j)=arr[b]=c;
                        else
                            bin(i-1,j)=bin(i,j)=arr[c]=b;
                    }
                }
                else {}
            }
        if (DEBUG_SEG)
        {
            cout<<"lalal"<<endl;
            for (uint i=0;i<arr.size();i++)
                if (arr[i]!=1)
                    cout<<"arr["<<i<<"]="<<arr[i]<<' ';
        }
        if (DEBUG_SEG)
            cout<<"arr["<<counter-1<<"]="<<arr[counter-1]<<' ';

        n_parts = OrganizeSegArray(&arr,counter);

        if (DEBUG_SEG)
            cout<<"lalal"<<n_parts<<endl;

        FindMassCenters(arr);
        DistanceTransform();

        if (DEBUG_SEG)
        {
            cout<<"lalal"<<endl;
            for (uint i=0;i<arr.size();i++)
                cout<<"arr["<<i<<"]="<<arr[i]<<' ';
            cout<<"prolog"<<endl;
        }

    }
    void CountRads()
    {
        if (DEBUG_RADS)
            cout<<"n_parts"<<n_parts<<endl;
        uint *neis = new uint [4];
        uint *min = new uint [n_parts];
        uint *max = new uint [n_parts];
        uint max_len = Distance(bin.n_rows,bin.n_cols,0,0);
        for (uint i=0;i<n_parts;i++)
        {
            max [i] = 0;
            min [i] = max_len;
        }
        if (DEBUG_RADS)
            cout<<"init"<<endl;

        bool flag;
        uint x,x1,y1,h1,h2,c;
        for (uint i=0;i<bin.n_rows;i++)
            for (uint j=0;j<bin.n_cols;j++)
            {
                x = bin(i,j);
                if (x)
                {
                    flag = false;
                    TakeFourNeighs(i,j,&neis);
                    for (uint k=0;k<4;k++)
                    {
                        if (!neis[k])
                        {
                            flag = true;
                            h1 = (((k+1)%2)*((-1)*(k==0)+(k==2)));
                            h2 = ((k%2)*((-1)*(k==3)+(k==1)));
                            if (centers[x-1]!=cents_dt[x-1])
                                tie(y1,x1) = cents_dt[x-1];
                            else
                                tie(y1,x1) = centers[x-1];
                            c = Distance(i+h1,j+h2,y1,x1);
                            if (c<min[x-1])
                                min[x-1] = c;
                        }
                    }
                    if (flag)
                    {
                        if (centers[x-1]!=cents_dt[x-1])
                            tie(y1,x1) = cents_dt[x-1];
                        else
                            tie(y1,x1) = centers[x-1];
                        c = Distance(i,j,y1,x1);
                        if (c>max[x-1])
                            max[x-1]=c;
                    }
                }
            }
        for (uint k=0;k<n_parts;k++)
        {
            small_rads.push_back(min[k]);
            big_rads.push_back(max[k]);
            if (DEBUG_RADS)
                cout<<"rad["<<k<<"] : "<<min[k]<<" "<<max[k]<<endl;
        }
    }
    type_of_task Analysis()
    {
        uint min = Distance(bin.n_rows,bin.n_cols,0,0);
        uint space;
        uint cogs = 0;
        uint space_for_small_rad=Distance(bin.n_rows,bin.n_cols,0,0);
        uint space_for_big_rad=Distance(bin.n_rows,bin.n_cols,0,0);
        bool flag;
        for (uint i=0;i<n_parts;i++,flag = true)
            if ((big_rads[i]-small_rads[i])<3)//or the gear is broken
            {
                task = axis;
                iplace=i;
                insertplace= centers[i];
                objects.emplace_back(new Axis(centers[i]));
            }
            else
            {
                cogs = CountCogs2(i,&flag);
                if ((centers[i]!=cents_dt[i])||(!flag))
                {
                    objects.emplace_back(new Gear(cents_dt[i],small_rads_dt[i],big_rads[i],true,cogs));
                    iplace=i;
                    insertplace= cents_dt[i];
                    task = broken;
                }
                //Gear g(centers[i],small_rads[i],big_rads[i]);
                else
                {
                    objects.emplace_back(new Gear(cents_dt[i],small_rads_dt[i],big_rads[i],false,cogs));
                }
            }

        if (task == axis)
        {
            for (uint i=0;i<n_parts;i++)
            {
                if ((Distance(centers[iplace],centers[i])<min)&&(i!=iplace))
                {
                    min = Distance(centers[iplace],centers[i]);
                    space = min - objects[i]->GetBigRad();
                    if (space<space_for_small_rad)
                        space_for_small_rad = space;
                    space = min - objects[i]->GetSmallRad();
                    if (space<space_for_big_rad)
                        space_for_big_rad = space;
                }
            }
            if ((iplace == 0)||(iplace==n_parts-1))
            {
                uint i,j;
                tie (i,j) = centers[iplace];
                if (iplace==n_parts-1)
                {
                    i = bin.n_cols - i;
                    j = bin.n_rows - j;
                }
                if (i<space_for_big_rad)
                    space_for_big_rad = i;
                if (i<space_for_big_rad)
                    space_for_big_rad = i;
            }
            space_for_gear = make_tuple(space_for_small_rad, space_for_big_rad);
        }
        return task;
    }

    tuple <uint,uint>  GetSpace(){return space_for_gear;}
    Coordinates  GetPlace(){return insertplace;}
    vector <shared_ptr<IObject>> &GetObjects(){return objects;}
    tuple <uint,uint>  GetRads()
    {
        if (DEBUG_GET)
            cout<<"getter"<<endl;
        if (n_parts == 1)
            return make_tuple(objects[0]->GetSmallRad(),objects[0]->GetBigRad());
        else if (task == broken)
            return make_tuple(objects[iplace]->GetSmallRad(),objects[iplace]->GetBigRad());
        else
            return make_tuple(0,0);

    }
static const int radius = 0;

private:
    uint Distance (uint x1, uint y1, uint x2, uint y2)
    {
        return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    }
    uint Distance (Coordinates coord1, Coordinates coord2)
    {
        uint x1,y1,x2,y2;
        tie(x1,y1) = coord1;
        tie(x2,y2) = coord2;
        return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    }
    uint FindMax (uint size, uint *arr)
    {
        uint max = 0;
        uint ind = 0;
        for (uint i=0;i<size;i++)
            if (arr[i]>max)
            {
                max = arr[i];
                ind = i;
            }
        return ind;
    }
    uint CountThreshold (uint *hystog)
    {
        uint i, h_p;
        uint h_max  = FindMax (SIZE_OF_HYST, hystog);
        if (DEBUG3)
            cout<<h_max<<endl;
        uint sum_max = 0;
        uint sum = 0;
        for (i=0; i<=h_max; i++)
            sum_max+=hystog[i];
        sum_max *=  PRECISION;
        for (i=0; sum < sum_max; i++)
            sum+=hystog[i];
        h_p = i-2;
        return (h_max + (h_max - h_p));
    }
    void TakeTwoNeighs(uint i, uint j, uint *left, uint *up, bool dt = false)
    {
        if (!dt)
        {
            if (i==0)
                *up = 0;
            else
                *up = bin(i-1,j);
            if (j==0)
                *left = 0;
            else
                *left = bin(i,j-1);
        }
        else
        {
            if (i==0)
                *up = 0;
            else
                *up = bin_dt(i-1,j);
            if (j==0)
                *left = 0;
            else
                *left = bin_dt(i,j-1);
        }
    }
    void TakeFourNeighs(uint i, uint j, uint **neighs)
    {
        if (DEBUG_NEIS)
            cout<<"in"<<endl;
        if (i==0)
            (*neighs)[0] = 0;
        else
            (*neighs)[0] = bin(i-1,j);
        if (DEBUG_NEIS)
            cout<<"0"<<endl;
        if (j==0)
            (*neighs)[3] = 0;
        else
            (*neighs)[3] = bin(i,j-1);
        if (DEBUG_NEIS)
            cout<<"3"<<endl;
        if (i==bin.n_rows-1)
            (*neighs)[2] = 0;
        else
            (*neighs)[2] = bin(i+1,j);
        if (DEBUG_NEIS)
            cout<<"2"<<endl;
        if (j==bin.n_cols-1)
            (*neighs)[1] = 0;
        else
            (*neighs)[1] = bin(i,j+1);
        if (DEBUG_NEIS)
            cout<<"1"<<endl;
    }
    uint OrganizeSegArray(vector <uint> *arr, uint size)
    {
        uint num=0;
        uint x;
        for (uint i=0; i<size; i++)
        {
            x = (*arr)[i];
            if ((*arr)[i]!=i)
            {
                (*arr)[i]=(*arr)[x];
            }
            else
            {
                if (i!=num)
                    (*arr)[i]=num;
                num++;
            }
        }
        return num-1;
    }
    void FindMassCenters(vector <uint> arr)
    {
        //And recolor binary image accordint to segmentation
        uint *arr_i = new uint [n_parts];
        uint *arr_j = new uint [n_parts];
        uint *arr_is = new uint [n_parts];
        uint *arr_js = new uint [n_parts];
        for (uint i=0; i<n_parts;i++)
            arr_i[i]=arr_j[i]=arr_is[i]=arr_js[i]=0;
        uint x;
        for (uint i=0;i<bin.n_rows;i++)
            for (uint j=0;j<bin.n_cols;j++)
            {
                x = bin(i,j);
                if (x)
                {
                    bin(i,j)=arr[x];
                    x=arr[x];
                    arr_i[x-1]+=i;
                    arr_j[x-1]+=j;
                    arr_is[x-1]++;
                    arr_js[x-1]++;
                }
            }
        for (uint i=0; i<n_parts;i++)
        {
            if (arr_is[i])
                arr_i[i]=arr_i[i]/arr_is[i];
            if (arr_js[i])
                arr_j[i]=arr_j[i]/arr_js[i];
            centers.push_back(make_tuple(arr_i[i],arr_j[i]));
        }
    }
    void DistanceTransform()
    {
        uint x,b,c,p,q;
        bool flag;
        small_rads_dt.resize(n_parts);
        cents_dt.resize(n_parts);
        for (uint i=0;i<n_parts;i++)
        {
            cents_dt[i] = make_tuple(0,0);
            small_rads_dt[i] = 0;
        }

        for (uint i=0;i<bin.n_rows;i++)
            for (uint j=0;j<bin.n_cols;j++)
            {
                x = bin (i,j);
                if (x)
                {
                    TakeTwoNeighs(i,j,&b,&c, true);
                    if (!b||!c)
                        bin_dt(i,j) = 1;
                    else if (b == c)
                    {
                        if (DEBUG_DT)
                            cout<<"b is "<<b<<" c is "<<c<<endl;
                        flag = true;
                        for (p = 1-b;(flag&&(p!=b-1)&&((j+p)>0)&&((j+p)<bin.n_cols));p++)
                        {
                            q = sqrt((b-1)*(b-1) - p*p);
                            if (i+q<bin.n_rows)
                                if (!bin(i+q,j+p))
                                {
                                    bin_dt(i,j)=b-1;
                                    flag = false;
                                }
                            if (i-q<bin.n_rows)
                                if (!bin(i-q,j+p))
                                {
                                    bin_dt(i,j)=b-1;
                                    flag = false;
                                }
                        }
                        if (flag)
                            for (p =b;(flag&&(p!=b)&&((j+p)>0)&&((j+p)<bin.n_cols));p++)
                            {
                                q = sqrt((b)*(b) - p*p);
                                if (i+q<bin.n_rows)
                                    if (!bin(i+q,j+p))
                                    {
                                        bin_dt(i,j)=b;
                                        flag = false;
                                    }
                                if (i-q<bin.n_rows)
                                    if (!bin(i-q,j+p))
                                    {
                                        bin_dt(i,j)=b;
                                        flag = false;
                                    }
                            }
                        if (flag)
                        {
                            bin_dt(i,j)=b+1;
                            if (b+1>small_rads_dt[x-1])
                            {
                                small_rads_dt[x-1] = b+1;
                                cents_dt[x-1] = make_tuple(i,j);
                            }
                            if (DEBUG_DT)
                                if (i==4)
                                {
                                    cout<<"3"<<endl;
                                    cout<<"bin_dt("<<i<<" "<<j<<") "<<bin_dt(i,j)<<endl;
                                }
                        }
                    }
                    else
                    {

                        if (b>c)
                        {
                            if (b-c==1)
                            {
                                flag  = true;

                                for (p = c;(flag&&(p!=c)&&((j+p)>0)&&((j+p)<bin.n_cols));p++)
                                {
                                    q = sqrt((c)*(c) - p*p);
                                    if (i+q<bin.n_rows)
                                        if (!bin(i+q,j+p))
                                        {
                                            bin_dt(i,j)=c;
                                            flag = false;
                                        }
                                    if (i-q<bin.n_rows)
                                        if (!bin(i-q,j+p))
                                        {
                                            bin_dt(i,j)=c;
                                            flag = false;
                                        }
                                }
                                if (flag)
                                    bin_dt(i,j)=b;
                            }
                            else
                            {
                                bin_dt(i,j)=b-1;
                            }
                        }
                        else
                        {
                            if (c-b==1)
                            {
                                flag  = true;

                                for (p = b;(flag&&(p!=b)&&((j+p)>0)&&((j+p)<bin.n_cols));p++)
                                {
                                    q = sqrt((b)*(b) - p*p);
                                    if (i+q<bin.n_rows)
                                        if (!bin(i+q,j+p))
                                        {
                                            bin_dt(i,j)=b;
                                            flag = false;
                                        }
                                    if (i-q<bin.n_rows)
                                        if (!bin(i-q,j+p))
                                        {
                                            bin_dt(i,j)=b;
                                            flag = false;
                                        }
                                }
                                if (flag)
                                    bin_dt(i,j)=c;
                            }
                            else
                            {
                                bin_dt(i,j)=c-1;
                            }
                        }
                    }
                    if (DEBUG_DT)
                        cout<<"bin_dt("<<i<<" "<<j<<") "<<bin_dt(i,j)<<endl;
                }
            }
        for (uint i=0;i<n_parts;i++)
        {
            small_rads_dt[i]--;

        }
        if (DEBUG_DT)
            for (uint i=0;i<n_parts;i++)
            {
                tie (c,b) = cents_dt[i];
                cout<<"cent is "<<b<<" "<<c<<" rad is "<<small_rads_dt[i]<<endl;
            }

    }
    uint CountCogs(uint n)
    {
        int middle_r = (small_rads_dt[n]+big_rads[n])/2;
        uint cen_y,cen_x;
        int y,x;
        uint counter = 0;
        bool cog = true;
        if (DEBUG_COGS)
            cout<<"in the func "<<middle_r<<endl;
        tie (cen_y,cen_x) = cents_dt [n];
        if (DEBUG_COGS)
            cout<<"center is "<<cen_y<<" "<<cen_x<<endl;
        for (x = -middle_r;x<=middle_r;x++)
        {
            y = sqrt((middle_r)*(middle_r) - x*x);
            if (DEBUG_COGS)
                cout<<"("<<cen_y+y<<","<<cen_x+x<<")"<<
                bin(cen_y+y,cen_x+x)<<" c="<<counter<<" fl="<<cog<<endl;
            if ((bin (cen_y+y,cen_x+x)==n+1)&&(!cog))
            {
                counter++;
                cog = true;
                if (DEBUG_COGS)
                    cout<<"counter "<<counter<<" "<<y<<" "<<x<<endl;
            }
            else if (bin (cen_y+y,cen_x+x)!=(n+1))
                cog = false;
        }
        for (x = middle_r;x>=-middle_r;x--)
        {
            y = sqrt((middle_r)*(middle_r) - x*x);
            if (DEBUG_COGS)
                cout<<"("<<cen_y-y<<","<<cen_x+x<<")"<<
                bin(cen_y-y,cen_x+x)<<" c="<<counter<<" fl="<<cog<<endl;
            if ((bin (cen_y-y,cen_x+x)==n+1)&&(!cog))
            {
                counter++;
                cog = true;
                if (DEBUG_COGS)
                    cout<<"counter "<<counter<<" "<<y<<" "<<x<<endl;
            }
            else if (bin (cen_y-y,cen_x+x)!=(n+1))
                cog = false;
        }
        if (DEBUG_COGS)
            cout<<"cogs are "<<counter<<endl;
        return counter;
    }
    bool CheckGear(uint n, uint ncogs)
    {
        float y,x;
        float angle = 360/ncogs;
        float phi;
        float phi1 = 0;
        uint cen_y,cen_x;
        tie (cen_y,cen_x) = cents_dt[n];
        uint midr = (small_rads_dt[n]+big_rads[n])/2;
        for (phi = 0;phi<360;phi++)
        {
            y = midr*sin(2*M_PI*phi/360);
            x = midr*cos(2*M_PI*phi/360);
            if (bin(cen_y+y,cen_x+x)==n+1)
            {
                phi1 = phi++;
                if (DEBUG_CHECK)
                    cout<<"cog is found "<<bin(cen_y+y,cen_x+x)<<endl;
                break;
            }
        }
        for (phi = phi1; phi < 360 + phi1; phi+=angle)
        {
            y = midr*sin(2*M_PI*phi/360);
            x = midr*cos(2*M_PI*phi/360);
            if (DEBUG_CHECK)
                cout<<"check is here"<<bin(cen_y+y,cen_x+x)<<endl;
            if (bin(cen_y+y,cen_x+x)!=n+1)
                return false;
        }
        return true;
    }
    uint CountCogs2(uint n, bool *whole)
    {
        int middle_r = (small_rads_dt[n]+big_rads[n])/2;
        uint cen_y,cen_x;
        float angle =0;
        float angl1 =0;
        float delta = 0;
        int y,x;
        bool flag = false;
        uint counter = 0;
        bool cog = true;
        if (DEBUG_COGS)
            cout<<"in the func "<<middle_r<<endl;
        tie (cen_y,cen_x) = cents_dt [n];
        if (DEBUG_COGS)
            cout<<"center is "<<cen_y<<" "<<cen_x<<endl;
        *whole = true;
        for (float phi = 0;phi<360;phi++)
        {
            y = middle_r*sin(2*M_PI*phi/360);
            x = middle_r*cos(2*M_PI*phi/360);
            if ((bin (cen_y+y,cen_x+x)==n+1)&&(!cog))
            {
                if (!counter)
                {
                    angle = phi;
                    angl1 = phi;
                }
                else if  (counter == 1)
                {
                    delta = phi - angle;
                    angle = phi;
                    if (DEBUG_COGS)
                        cout<<"phi "<<phi<<" ang "<<angle<<" del "<<delta<<" c "<<counter<<endl;
                }
                else
                {
                    if (DEBUG_COGS)
                        cout<<"phi "<<phi<<" ang "<<angle<<" del "<<delta<<" c "<<counter<<endl;
                    if (abs(phi - angle - delta) <=3)
                    {
                        angle = phi;
                    }
                    else
                    {
                        if (!flag)
                        {
                            *whole = false;
                            flag = true;
                        }
                    }
                }
                counter++;

                cog = true;

            }
            else if (bin (cen_y+y,cen_x+x)!=(n+1))
                cog = false;
            else
                cog = true;
        }
        if (!flag)
            if (abs(angl1+360 - angle - delta) > 3)
                *whole = false;
        if (DEBUG_COGS)
            cout<<"cogs are "<<counter<<endl;
        return counter;
    }
};



model::model(QObject *parent) :
    QObject(parent)
{
}

void model::ReceiveImgs(Image img, int n)
{
    if (!n)
    {
        src = img;
        if (DEBUGMVC)
        {
            cout<<src.n_cols<<" "<<src.n_rows<<endl;
           /* for (uint i=0;i<src.n_cols;i++)
            {
                uint r,g,b;
                tie (r,g,b) = src(i,i);
                if (g||r||b)
                    cout<<"src "<<r<<" "<<g<<" "<<b<<" "<<endl;
            }*/
        }
    }
    else
    {
        test[n-1] = img;
        if (DEBUGMVC)
            cout<<test[n-1].n_cols<<" "<<test[n-1].n_rows<<endl;
        /*for (uint i=0;i<src.n_cols;i++)
        {
            uint r,g,b;
            tie (r,g,b) = src(i,i);
            if (g||r||b)
                cout<<"src "<<r<<" "<<g<<" "<<b<<" "<<endl;
        }*/
    }
}

void model::Process()
{

    // Base: return array of found objects and index of the correct gear
    // Bonus: return additional parameters of gears
    if (DEBUGMVC)
        cout<<"Process started\n";
    FindGears main_img(src);
    emit Prints("Binarization done without any filter");
    if (DEBUG)
        cout<<"step1"<<endl;
    main_img.Segmentation();
    emit Prints("All segments are devided");
    if (DEBUG)
        cout<<"step2"<<endl;
    main_img.CountRads();
    if (DEBUG)
        cout<<"step3"<<endl;
    task_type = main_img.Analysis();
    emit Prints("The number of cogs counted");
    if (DEBUG)
        cout<<"step4"<<endl;
    object_array = main_img.GetObjects();
    if (DEBUG)
        emit PrintObjs(object_array);
    uint space[2];
    if (task_type == axis)
    {
        emit Prints("The axis is found");
        tie (space[0],space[1])=main_img.GetSpace();
    }
    else if (task_type == broken)
    {
        emit Prints("The broken gear is found");
        tie (space[0],space[1])=main_img.GetRads();
    }
    uint delta[2];
    uint radius = 0;
    delta[0]=src.n_rows;
    delta[1]=src.n_rows;
    for(uint ntest=1;ntest<=3;ntest++)
    {
        FindGears test_img(test[ntest-1]);
        test_img.Segmentation();
        test_img.CountRads();
        test_img.Analysis();
        if (DEBUG_TEST)
            cout<<"test"<<endl;
        uint rads[2];
        tie (rads[0],rads[1]) = test_img.GetRads();
        if (DEBUG_TEST)
            cout<<"rads are"<<rads[0]<<" "<<rads[1]<<endl;
        if (DEBUG_TEST)
            cout<<"space is"<<space[0]<<" "<<space[1]<<endl;
        if (task_type == axis)
            if ((rads[0]<=space[0])&&(rads[1]<=space[1]))
            {
                if (DEBUG_TEST)
                    cout<<"matches"<<endl;
                bool flag = false;
                for (uint k=0; k<2;k++)
                {
                    uint x = space[k]-rads[k];
                    if (x<delta[k])
                    {
                        delta[k] = x;
                        if (flag)
                        {
                            result_idx = ntest;
                            radius = rads [1];
                        }
                        else
                            flag = true;
                    }
                    else
                        flag = false;
                }
            }
            else {}
        else if (task_type == broken)
        {
            if ((cmprads(space[0],rads[0]))&&(cmprads(space[0],rads[0])))
            {
                result_idx = ntest;
                radius = rads[1];
            }
        }
    }

    if (DEBUG)
        cout<<"right img is "<<result_idx<<endl;
    if (DEBUG)
        cout<<"radius "<<radius<<endl;
    uint r,g,b,r1,g1,b1;
    uint x,y;
    tie (y,x) = main_img.GetPlace();
    if (DEBUG)
        cout<<"place"<<y<<" "<<x<<endl;
    for (uint i = y - radius; (i<= y + radius)&&(i<src.n_rows); i++)
        for (uint j = x - radius; (j<= x+radius)&&(i<src.n_cols); j++)
        {
            tie(r,g,b) = (src) (i,j);
            tie (r1,g1,b1) = (test[result_idx-1]) (i-(y - radius),j-(x - radius));
            if (r>100)
                (src) (i,j) = make_tuple(0,0,0);
            if (g1>100)
                (src) (i,j) = make_tuple(0,g1,0);
        }
    if (DEBUG)
        save_image(src,"debres.bmp");
    emit Prints("Mechanism is mended");
    emit SaveImg(src);
    emit Output(result_idx);
    emit Output(object_array.size());
    emit PrintObjs(object_array);
}

void model::Filter(FilterInterface* iFilter)
{
    Image image = iFilter->filterImage(src);
    emit SaveImg(image);
    Prints(QString(QString("Filter applied: ") + iFilter->Name()).toStdString().c_str());
}

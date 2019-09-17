#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <ctime>
#include <cstdlib>

using namespace cv;
using namespace std;

Vec2i passThreePoint(int seed,int p1, int p2, int jml_kolom){
    Point a(seed%jml_kolom,seed/jml_kolom);
    Point b(p1%jml_kolom,p1/jml_kolom);
    Point c(p2%jml_kolom,p2/jml_kolom);
    Vec2i res;

    Mat A = (Mat_<float >(2,2) << 2*(b.x-a.x) , 2*(b.y-a.y) , 2*(c.x-a.x) , 2*(c.y-a.y));
    Mat B = (Mat_<float >(2,1) << b.x*b.x - a.x*a.x + b.y*b.y - a.y*a.y , c.x*c.x - a.x*a.x + c.y*c.y - a.y*a.y);
    Mat X = A.inv()*B;
    Point pst(X.at<float > (0) , X.at<float > (1));
    res[0]= pst.x + pst.y*jml_kolom;
    res[1]=sqrt((a.x - pst.x)*(a.x - pst.x) + (a.y - pst.y)*(a.y - pst.y));

//    cout<<a<<b<<c<<endl;
//    cout<<pst<<res[1]<<endl;
    return res;
}

int cekPool(vector<Vec2i > &pool,int idx_target){
    for(size_t i=0;i<pool.size();i++){
        if(pool[i][0]==idx_target)return (int)i;
    }
    return -1;
}

void FRHT(Mat in,Mat &out, int iterasi,int min_score){
    int jml_kolom=in.cols;
    int jml_baris=in.rows;
    int w = 21;
    int maks_rad = jml_baris + jml_kolom;
    vector<int > edges;//himpunan edges atau D
    //Mat pool_accum = Mat::zeros(in.size(),CV_32FC2);
    out = Mat::zeros(in.size(),CV_8UC1);

    //index
    //score
    vector<Vec2i> pool;

    uchar* in_data = in.data;
//    uchar* out_data = out.data;

//    cout<<in.at<uchar>(0)<<endl;

//    cout<<in.total()<<endl;
    for(int i=0;i<jml_kolom*jml_baris;i++){
        if(in_data[i]>0){
//            cout<<in_data[i]<<endl;
//            cout<<out_data[i]<<endl;
            edges.push_back(i);
        }
    }

    srand(time(0));
//    cout<<edges.size()<<endl;
    for(int iter=0;
        (iter < iterasi) || (edges.size() > 1);
        iter++){

        int no_edges=rand()%(int)(edges.size()-1);
        int seed = edges[no_edges];//random seed point
        //cout<<no_edges<<";"<<seed<<endl;
        vector<int > cell;//sel setiap kandidat
        for(int i=0;i<maks_rad;i++){
            cell.push_back(-1);
        }

        Rect r(0,0,(seed%jml_kolom),(seed/jml_kolom));
        Mat roi(in,r);// w x w Windows
//        imshow("ROI",roi);
//        cout<<"CEK:"<<in_data[seed]<<endl;
        uchar* roi_data = roi.data;
        int x_seed = roi.cols-1,y_seed = roi.rows-1;

        for(size_t i=0;i<roi.total()-1;i++){
            if(roi_data[i]){
                int x = i%roi.cols, y = i/roi.cols;
                int dist = sqrt((x_seed - x)*(x_seed - x) + (y_seed - y)*(y_seed - y));
                if(cell[dist]==-1){
                    cell[dist] = (int)i;
                }else {
                    Vec2i kand = passThreePoint(seed,cell[dist],(int)i,jml_kolom);
//                    waitKey(0);
                    int idx_pool = kand[0] + maks_rad*kand[1];
                    int pos_pool = cekPool(pool,idx_pool);
                    if(pos_pool == -1){
                        Vec2i baru;baru[0] = idx_pool;baru[1] = 1;
                        pool.push_back(baru);
                    }else{
                        pool[pos_pool][1]++;
                    }
                    cell[dist] = -1;
                }
            }
        }
        edges.erase(edges.begin() + no_edges);
    }

    for(size_t i=0;i<pool.size();i++){
        if(pool[i][1]>min_score){
            int temp = pool[i][0]%maks_rad;
            int rad = pool[i][0]/maks_rad;
            Point ctr(temp%jml_kolom,temp/jml_kolom);
            circle(out,ctr,rad,Scalar(255),1);
        }
    }
}

int main(){
    Mat tes = Mat::zeros(480,640,CV_8UC1);
//    cout<<"CEK:"<<tes.at<uchar>(0)<<endl;
    Mat out;
    ellipse(tes,Point(200,200),Size(100,125),0.0,0.0,360.0,Scalar(255),1);

    FRHT(tes,out,100,10);

    imshow("CEK",tes);
    imshow("OUT",out);
    waitKey(0);
    return 0;
}


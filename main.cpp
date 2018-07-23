#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;



vector<Mat> pyrl, pyrr, pyrm;

vector<Mat> resultpyr;

int level;

void calLog(const Mat &src, vector<Mat> &logVec) {
    Mat down, up, current, logimg;
    current = src;
    for(int i=0; i<level; ++i) {
        cv::pyrDown(current, down);
        cv::pyrUp(down, up);
        logimg = current - up;
        logVec.push_back(logimg.clone());
        current = down;
        //imwrite("test" + to_string(i) + ".jpg", logimg);
    }
    logVec.push_back(current);
}

void calDog(const Mat &src){
    Mat down, current;
    cvtColor(src, current, CV_GRAY2BGR);
    pyrm.push_back(current);
    current = src.clone();
    for(int i=0; i<level; ++i) {
        Mat rgb_mask;
        cv::pyrDown(current, down);
        cvtColor(down, rgb_mask, CV_GRAY2BGR);
        pyrm.push_back(rgb_mask.clone());
        current = down;
        //imwrite("test" + to_string(i) + ".jpg", rgb_mask);
    }
}

void calBlend() {
    Mat ll ,rr;
    Mat blend;

    for (int i=0;i<level;i++){
        //cout << pyrl[i].size() << " " << pyrm[i].size() << endl;

        ll = pyrl[i].mul(pyrm[i]);

        //cout << ll.ptr<Vec3b>(257,0)[0] << " " << pyrl[i].ptr<Vec3b>(257,0)[0] << " " << pyrm[i].ptr<Vec3b>(257,0)[0] << endl;
        rr = pyrr[i].mul(Scalar(1.0,1.0,1.0)-pyrm[i]);
        blend = ll + rr;
        resultpyr.push_back(blend);
        //imwrite("test" + to_string(i) + ".jpg", blend);
    }

    Mat resulthighest = pyrl.back().mul(pyrm.back()) + (pyrr.back().mul(Scalar(1.0,1.0,1.0)-pyrm.back()));
    resultpyr.push_back(resulthighest);

}


void pyrBlend(Mat &dst) {
    Mat current,up;
    current = resultpyr.back();
    for (int i=level-1;i>=0;i--){
        pyrUp(current,up,resultpyr[i].size());
        //cout << up.type() << " " << resultpyr[i].type() << endl;
        //up.convertTo(up_f, CV_32F);
        //imwrite("test" + to_string(i) + ".jpg", up);
        current = up + resultpyr[i];
        //imwrite("test" + to_string(i) + ".jpg", current);
    }

    dst = current.clone();
}

int main() {
    Mat left, right, mask;
    left = imread("./left.jpg");
    right = imread("./right.jpg");
    mask = imread("./mask.jpg");
    level = 4;

    cv::resize(right, right, left.size());

    cvtColor(mask, mask, CV_BGR2GRAY);

    Mat left_f, right_f, mask_f;

    left.convertTo(left_f,CV_32F,1.0/255.0);
    right.convertTo(right_f,CV_32F,1.0/255.0);
    mask.convertTo(mask_f,CV_32F,1.0/255.0);
    //cout << left.type() << " " << mask.type() << endl;

    calLog(left_f, pyrl);
    calLog(right_f, pyrr);
    calDog(mask_f);
    calBlend();

    Mat result;
    pyrBlend(result);

    imshow("left",left);
    imshow("right",right);
    imshow("result",result);


    waitKey(0);
    return 0;
}
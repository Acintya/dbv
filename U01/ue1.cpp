#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


void saltandpepper(Mat & image, int border)
{
    for (int k = 0; k < border; k++)
    {
        int i = rand() % image.cols;
        int j = rand() % image.rows;
        if (image.channels() == 1)
            image.at<uchar>(j,i) = 255;
        else if (image.channels() == 3){
            image.at<cv::Vec3b>(j,i)[0] = 255;
            image.at<cv::Vec3b>(j,i)[1] = 255;
            image.at<cv::Vec3b>(j,i)[2] = 255;
        }
    }

    for (int k = 0; k < border; k++)
    {
        int i = rand() % image.cols;
        int j = rand() % image.rows;
        if (image.channels() == 1)
            image.at<uchar>(j,i) = 0;
        else if (image.channels() == 3){
            image.at<cv::Vec3b>(j,i)[0] = 0;
            image.at<cv::Vec3b>(j,i)[1] = 0;
            image.at<cv::Vec3b>(j,i)[2] = 0;
        }
    }

    imwrite("salted.jpg", image);
}


void changesize(Mat & image, double scale_i, double scale_j)
{
    Mat skal_pic;
    resize(image, skal_pic, Size(image.cols * scale_i, image.rows * scale_j));
    imwrite("scaled_pic.jpg",skal_pic);
    //imshow("size_changed:",skal_pic);
}

void pic_filter(Mat & image)
{
    //convert pic to grayscale
    Mat gray;
    cvtColor(image, gray, CV_RGB2GRAY);

    //imwrite("grayscale.jpg", gray);
    imshow("grayscale", gray);

    Mat normal_blur_pic;
    blur(image,normal_blur_pic,Size(5,5),Point(-1,-1));
    imshow("Normalized Block Filter:", normal_blur_pic);

    Mat median_blur_pic;
    medianBlur(image,median_blur_pic,7);
    imshow("Median Filter:", median_blur_pic);

    Mat gaus_blur_pic;
    GaussianBlur(image,gaus_blur_pic,Size(5,5),3,3);
    imshow("Gaussian Filter:", gaus_blur_pic);
}

void applyMedianFilter(Mat & srcImg, int rowCord, int colCord, int filterSize)
{
    if (filterSize == 0 || filterSize % 2 == 0)
    {
        cout << "Error: The size of filter has to be positive odd.";
        return;
    }

    int cordOffset = filterSize / 2;
    int upperLeftX = rowCord - cordOffset;
    int upperLeftY = colCord - cordOffset;

    vector<uchar> vecPixel;
    for (int i = upperLeftX; i < upperLeftX + filterSize; i++)
    {
        for (int j = upperLeftY; j < upperLeftY + filterSize; j++)
        {
            //if (srcImg.channels() == 1)
                vecPixel.push_back(srcImg.at<uchar>(i,j));

            // else if (srcImg.channels() == 3){
            //     srcImg.at<cv::Vec3b>(j,i)[0] = 0;
            //     srcImg.at<cv::Vec3b>(j,i)[1] = 0;
            //     srcImg.at<cv::Vec3b>(j,i)[2] = 0;
            // }
        }
    }

    sort(vecPixel.begin(), vecPixel.end());
    int medianIndex = filterSize * filterSize / 2;
    srcImg.at<uchar>(rowCord, colCord) = vecPixel[medianIndex];
}

void m_medianBlur(Mat & srcImg, Mat & dstImg, int filterSize)
{
    int offset = filterSize / 2;
    cout << offset;
    for (int i = offset; i < srcImg.rows - offset; i++)
    {
        for (int j = offset; j < srcImg.cols - offset; j++)
        {
            applyMedianFilter(srcImg, i, j, filterSize);
        }
    } 
}

// void yu_medianBlur(Mat & srcImg, int rowX, int colY, int fensterSize)
// {
//     if (fensterSize == 0 || fensterSize % 2 == 0)
//     {
//         cout << "Error: the given number of fensterSize has to be postive odd!";
//         return;
//     }
//     int fensterRowMiddel = rowX + fensterSize/2;
//     int fensterColMiddel = colY + fensterSize/2;
//     int numHalf = (fensterSize * fensterSize) / 2;

//     vector<uchar> vecPixel;


//     for (int i = rowX; i < rowX + fensterSize; i++)
//     {
//         Vec3b *p;
//         p = srcImg.ptr<Vec3b>(i);
        
//         for (int j = colY; j < colY + fensterSize; j++)
//         {
//             // (vecPixel).push_back(& p(i)[j]);
//         }
//     }
//     sort(vecPixel.begin(),vecPixel.begin()+numHalf,vecPixel.end());
//     srcImg.at<Vec3b>(fensterRowMiddel,fensterColMiddel) = vecPixel[numHalf];

// }

// void yu_applymedianBlur(Mat & srcImg, Mat & dstImg, int fensterSize)
// {
//     int rowI = srcImg.rows;
//     int colJ = srcImg.cols * srcImg.channels();

//     if (srcImg.isContinuous())
//     {
//         colJ = rowI * colJ;
//         rowI = 1;
//     }

//     for (int i = 0; i < rowI; i++)
//     { 
//         for (int j = 0; j < colJ; j++)
//         {
//             yu_medianBlur(srcImg, i, j, fensterSize);
//         }
//     }
// }

int main()
{
    cout << "da";
    Mat src_pic = imread("../01.jpg");
    imshow("origin_picture", src_pic);

    changesize(src_pic, 0.5, 0.5);

    saltandpepper(src_pic, 1000);

    Mat grayImg;
    cvtColor(src_pic, grayImg, CV_BGR2GRAY); 
    imshow("salted_picture", src_pic);

    double t1 = (double)getTickCount();
    //certain event
    Mat medianBlurimg;
    medianBlur(grayImg, medianBlurimg,3);
    t1 = ((double)getTickCount() - t1)/ getTickFrequency(); //number of ticks/ times that CPU emits a tick in 1s
    cout << "openCV median: Times passt in seconds:" << t1 << endl;

    double t = (double)getTickCount();
    //certain event
    Mat medianDst;
    m_medianBlur(grayImg, medianDst, 3);
    t = ((double)getTickCount() - t)/ getTickFrequency(); //number of ticks/ times that CPU emits a tick in 1s
    cout << "my median: Times passt in seconds:" << t << endl;
    imshow("median", grayImg);
    //imshow("median", medianDst);


    // imwrite("fil_pic.jpg",gaus_blur_pic);
    // imshow("Gaussian Filter:", gaus_blur_pic);

    while(1){
        if (waitKey(1) == 'q')
        {
            break;
        }
    }

    return 0;
}



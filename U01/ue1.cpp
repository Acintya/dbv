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

        }
    }

    sort(vecPixel.begin(), vecPixel.end());
    int medianIndex = filterSize * filterSize / 2;
    srcImg.at<uchar>(rowCord, colCord) = vecPixel[medianIndex];

	// visualization
	/*Point upperLeft = Point(colCord - cordOffset, rowCord - cordOffset);
	Point lowerRight = Point(colCord + cordOffset, rowCord + cordOffset);
	Mat winImg = srcImg.clone();
	rectangle(winImg, upperLeft, lowerRight, Scalar(255, 0, 0));
	imwrite(".\\output\\window" + to_string(rowCord) + "_" + to_string(colCord) + ".png", winImg);*/
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

int getMediaValue(const int hist[], int thresh) {
	int sum = 0;
	for (int i = 0; i < 256; i++) {
		sum += hist[i];
		if (sum >= thresh) {
			return i;
		}
	}
	return 255;
}
//������ֵ�˲����Ҷ�ͼ
Mat histMedianBlur(Mat src, int diameter) {
	int row = src.rows;
	int col = src.cols;
	Mat dst(row, col, CV_8UC1);
	int Hist[256] = { 0 };
	int radius = (diameter - 1) / 2;
	int windowSize = diameter * diameter;
	int threshold = windowSize / 2 + 1;
	uchar* srcData = src.data;
	uchar* dstData = dst.data;
	int right = col - radius;
	int bot = row - radius;
	for (int j = radius; j < bot; j++) {
		for (int i = radius; i < right; i++) {
			//ÿһ�е�һ�����˲�Ԫ�ؽ���ֱ��ͼ
			if (i == radius) {
				memset(Hist, 0, sizeof(Hist));
				for (int y = j - radius; y <= min(j + radius, row); y++) {
					for (int x = i - radius; x <= min(i + radius, col); x++) {
						uchar val = srcData[y * col + x];
						Hist[val]++;
					}
				}
			}
			else {
				int L = i - radius - 1;
				int R = i + radius;
				for (int y = j - radius; y <= min(j + radius, row); y++) {
					//�������һ��
					Hist[srcData[y * col + L]]--;
					//�����ұ�һ��
					Hist[srcData[y * col + R]]++;
				}
			}
			uchar medianVal = getMediaValue(Hist, threshold);
			dstData[j * col + i] = medianVal;
		}
	}
	//�߽�ֱ�Ӹ�ֵ
	for (int i = 0; i < col; i++) {
		for (int j = 0; j < radius; j++) {
			int id1 = j * col + i;
			int id2 = (row - j - 1) * col + i;
			dstData[id1] = srcData[id1];
			dstData[id2] = srcData[id2];
		}
	}
	for (int i = radius; i < row - radius - 1; i++) {
		for (int j = 0; j < radius; j++) {
			int id1 = i * col + j;
			int id2 = i * col + col - j - 1;
			dstData[id1] = srcData[id1];
			dstData[id2] = srcData[id2];
		}
	}
	return dst;
}


int main()
{
    Mat src_pic = imread("../01.jpg");
    imshow("origin_picture", src_pic);

    changesize(src_pic, 0.5, 0.5);

    saltandpepper(src_pic, 1000);

    Mat grayImg;
    cvtColor(src_pic, grayImg, CV_BGR2GRAY); 
    imshow("salted_picture", src_pic);

    double t1 = (double)getTickCount();
    ////certain event
    Mat medianBlurimg;
    medianBlur(grayImg, medianBlurimg,3);
    t1 = ((double)getTickCount() - t1)/ getTickFrequency(); //number of ticks/ times that CPU emits a tick in 1s
    cout << "openCV median: Times passt in seconds:" << t1 << endl;

    double t = (double)getTickCount();
    //certain event
    Mat medianDst;
	grayImg = histMedianBlur(grayImg, 3);
    t = ((double)getTickCount() - t)/ getTickFrequency(); //number of ticks/ times that CPU emits a tick in 1s
    cout << "my median: Times passt in seconds:" << t << endl;
	/*Point upperLeft = Point(0, 0);
	Point lowerRight = Point(30, 30);
	rectangle(grayImg, upperLeft, lowerRight, Scalar(255, 0, 0));*/
    imshow("median", grayImg);


     //imwrite(".\\output\\fil_pic.jpg", grayImg);
    // imshow("Gaussian Filter:", gaus_blur_pic);

    while(1){
        if (waitKey(1) == 'q')
        {
            break;
        }
    }

    return 0;
}



#include "ExposureFusion.h"

using namespace cv;

Mat QualityMeasures::getContrastMeasure(Mat src)
{
	Mat dst(src.size(), CV_16UC1);
	Mat lptemp(src.size(), CV_16SC1);	
	Laplacian(src, lptemp, CV_16SC1, 3);
		
	Mat temp8(src.size(), CV_8UC1);
	for (int y = 0; y < temp8.rows; y++)
		for (int x = 0; x < temp8.cols; x++)
			temp8.at<uchar>(y, x) = (lptemp.at<short>(y, x) < 0) ? -lptemp.at<short>(y, x) : lptemp.at<short>(y, x);
	
	dst = temp8.clone();
	return dst;
}

Mat QualityMeasures::getSaturationMeasure(Mat src)
{
	Mat img = src;
	Mat stdDevImg(src.size(), CV_8UC1, CV_RGB(0, 0, 0));
	int nch = src.channels();
	float mean = 0;
	float variance = 0;
	float stdDev = 0;

#if MODE==GRAY
	for (int y = 0; y < src.rows; y++)
	{		
		for (int x = 0; x < src.cols; x++)
		{
			mean = (img.at<Vec3b>(y, x)[0] + img.at<Vec3b>(y, x)[1] + img.at<Vec3b>(y, x)[2]) / 3.0;
			variance = 0;
			for(int i=0; i<nch; i++)
			{
				variance += (mean - img.at<Vec3b>(y, x)[i]) * (mean - img.at<Vec3b>(y, x)[i]) / 3.0;
			}
			stdDev = sqrt(variance);
			stdDevImg.at<uchar>(y, x) = (uchar)(stdDev+0.5);
		}
	}
#endif

	Mat dst = stdDevImg.clone();
	return dst;
}

Mat QualityMeasures::getWellExposednessMeasure(Mat src)
{
	Mat dst(src.size(), CV_32FC1);
	
	Mat wellexpoimg(src.size(), CV_32FC1);
	double normalizedPixval = 0;
	double newPixval = 0;
	double gaussCurveWeight = 0;
	
#if MODE==GRAY
	Mat temp(src.size(), CV_32FC1);
	Mat gray(src.size(), CV_8UC1);
	
	Mat t(src.size(), CV_8UC1);

	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			
			gaussCurveWeight = LUTWEN[src.at<uchar>(y, x)];
			newPixval = gaussCurveWeight * src.at<uchar>(y, x);
			temp.at<float>(y, x) = newPixval;			
		
			wellexpoimg.at<float>(y, x) = (float)newPixval;			
		}
	}	

	/*imshow("src", src);
	imshow("wellexpoimg", wellexpoimg);
	imshow("t", t);
	waitKey();*/
	dst = wellexpoimg.clone();
#endif
	return dst;
}

Mat QualityMeasures::getWeightMapImage()
{	
	Mat C = getterContrast();
	Mat S = getterSaturation();
	Mat E = getterWellExposedness();
	
	float pix = 0;

	// float
	//Mat WeightMap(C.size(), CV_32FC1);
	Mat WeightMap(C.size(), CV_32SC1);
	for (int y = 0; y < C.rows; y++)
	{
		for (int x = 0; x < C.cols; x++)
		{
			pix = C.at<uchar>(y, x) * S.at<uchar>(y, x);
			WeightMap.at<int>(y, x) = pix;	
		}
	}
		
	return WeightMap.clone();
}
#include "QualityMeasures.h"
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;

Mat QualityMeasures::getContrastMeasure(Mat src)
{
	Mat dst(src.size(), CV_16UC1);
	//Mat dst(src.size(), CV_8UC1);
	Mat lptemp(src.size(), CV_16SC1);	
	Laplacian(src, lptemp, CV_16SC1, 3);
		
	/*Mat temp16(src.size(), CV_16UC1);
	for (int y = 0; y < temp16.rows; y++)
		for (int x = 0; x < temp16.cols; x++)
			temp16.at<ushort>(y, x) = (lptemp.at<short>(y, x) < 0) ? -lptemp.at<short>(y, x) : lptemp.at<short>(y, x);*/
	

	Mat temp8(src.size(), CV_8UC1);
	for (int y = 0; y < temp8.rows; y++)
		for (int x = 0; x < temp8.cols; x++)
			temp8.at<uchar>(y, x) = (lptemp.at<short>(y, x) < 0) ? -lptemp.at<short>(y, x) : lptemp.at<short>(y, x);
	
	// 일단은 연산을 하기위해 uchar를 영상을 반환하는데;
	// ushort영상이 훨씬 디테일하니, 나중에 수정할 수 도 있음
	dst = temp8.clone();
	
	/*resize(temp8, temp8, Size(temp8.cols*0.5, temp8.rows*0.5));
	imshow("Constrast8U", temp8);
	resize(temp16, temp16, Size(temp16.cols*0.5, temp16.rows*0.5));
	imshow("Constrast16U", temp16);	
	waitKey();*/
		
	return dst;
}

Mat QualityMeasures::getSaturationMeasure(Mat src)
{
	Mat img = src;
	Mat stdDevImg(src.size(), CV_8UC1, CV_RGB(0, 0, 0));
	//Mat stdDevImg(src.size(), CV_32FC1, CV_RGB(0, 0, 0));
	int nch = src.channels();
	float mean = 0;
	float variance = 0;
	float stdDev = 0;


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
			//stdDevImg.at<float>(y, x) = stdDev; // float로 저장
			stdDevImg.at<uchar>(y, x) = (uchar)(stdDev+0.5);
		}
	}


	Mat dst = stdDevImg.clone();
	return dst;
}

Mat QualityMeasures::getWellExposednessMeasure(Mat src)
{
	//Mat dst(src.size(), CV_8UC1);
	Mat dst(src.size(), CV_32FC1);
	
	
	Mat wellexpoimg(src.size(), CV_32FC1);
	double normalizedPixval = 0;
	double newPixval = 0;
	double gaussCurveWeight = 0;
	
	// uchar
	/*for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			for (int nch = 0; nch < src.channels(); nch++)
			{
				gaussCurveWeight = LUTWEN[src.at<Vec3b>(y, x)[nch]];
				//newPixval = gaussCurveWeight * src.at<Vec3b>(y, x)[nch] * 255;
				newPixval = gaussCurveWeight * src.at<Vec3b>(y, x)[nch];
				//temp.at<Vec3b>(y, x)[nch] = (uchar)newPixval;
				temp.at<Vec3f>(y, x)[nch] = newPixval;
			}
			newPixval = temp.at<Vec3f>(y, x)[0] * temp.at<Vec3f>(y, x)[1] * temp.at<Vec3f>(y, x)[2];
			newPixval = (255 < newPixval) ? 255 : newPixval;
			newPixval = (0 > newPixval) ? 0 : newPixval;
			wellexpoimg.at<uchar>(y,x) = (uchar)newPixval;
			//wellexpoimg.at<float>(y, x) = (float)newPixval;
		}
	}	
	dst = wellexpoimg.clone();*/

	// float


	//Mat temp(src.size(), CV_32FC3);
	//for (int y = 0; y < src.rows; y++)
	//{
	//	for (int x = 0; x < src.cols; x++)
	//	{
	//		for (int nch = 0; nch < src.channels(); nch++)
	//		{
	//			gaussCurveWeight = LUTWEN[src.at<Vec3b>(y, x)[nch]];
	//			newPixval = gaussCurveWeight * src.at<Vec3b>(y, x)[nch];
	//			temp.at<Vec3f>(y, x)[nch] = newPixval;
	//		}
	//		newPixval = (temp.at<Vec3f>(y, x)[0]/255.) * (temp.at<Vec3f>(y, x)[1]/255.) * (temp.at<Vec3f>(y, x)[2]/255.);
	//		wellexpoimg.at<float>(y, x) = (float)newPixval;
	//		//cout << newPixval << endl;
	//	}
	//}	

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

			/*int val = newPixval;
			val = (val > 255) ? 255 : val;
			val = (val < 0) ? 0 : val;
			t.at<uchar>(y, x) = (uchar)val;*/
		}
	}	

	/*imshow("src", src);
	imshow("wellexpoimg", wellexpoimg);
	imshow("t", t);
	waitKey();*/
	dst = wellexpoimg.clone();
	

	/*resize(wellexpoimg, wellexpoimg, Size(wellexpoimg.cols*0.5, wellexpoimg.rows*0.5));
	imshow("Well Exposedness", wellexpoimg);
	Mat ER(dst.size(), CV_8UC1);
	Mat R(dst.size(), CV_8UC1);
	Mat EG(dst.size(), CV_8UC1);
	Mat G(dst.size(), CV_8UC1);
	Mat EB(dst.size(), CV_8UC1);
	Mat B(dst.size(), CV_8UC1);
	float tempVal = 0;
	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			tempVal = temp.at<Vec3f>(y, x)[0];
			tempVal = (tempVal > 255) ? 255 : tempVal;
			tempVal = (tempVal < 0) ? 0 : tempVal;
			EB.at<uchar>(y, x) = (uchar)tempVal;
			B.at<uchar>(y, x) = src.at<Vec3b>(y, x)[0];

			tempVal = temp.at<Vec3f>(y, x)[1];
			tempVal = (tempVal > 255) ? 255 : tempVal;
			tempVal = (tempVal < 0) ? 0 : tempVal;
			EG.at<uchar>(y, x) = (uchar)tempVal;
			G.at<uchar>(y, x) = src.at<Vec3b>(y, x)[1];

			tempVal = temp.at<Vec3f>(y, x)[2];
			tempVal = (tempVal > 255) ? 255 : tempVal;
			tempVal = (tempVal < 0) ? 0 : tempVal;
			ER.at<uchar>(y, x) = (uchar)tempVal;
			R.at<uchar>(y, x) = src.at<Vec3b>(y, x)[2];
		}
	}
	resize(EB, EB, Size(EB.cols*0.5, EB.rows*0.5));
	resize(EG, EG, Size(EG.cols*0.5, EG.rows*0.5));
	resize(ER, ER, Size(ER.cols*0.5, ER.rows*0.5));
	resize(B, B, Size(B.cols*0.5, B.rows*0.5));
	resize(G, G, Size(G.cols*0.5, G.rows*0.5));
	resize(R, R, Size(R.cols*0.5, R.rows*0.5));
	imshow("EB", EB);
	imshow("B", B);
	imshow("EG", EG);
	imshow("G", G);
	imshow("ER", ER);
	imshow("R", R);
	waitKey();*/

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
			//pix = C.at<uchar>(y, x) * S.at<float>(y, x) * E.at<float>(y, x);
			//pix = C.at<uchar>(y, x) * S.at<uchar>(y, x) * E.at<float>(y, x);
			pix = C.at<uchar>(y, x) * S.at<uchar>(y, x);
			/*pix = (pix > 255) ? 255 : pix;
			pix = (pix < 0) ? 0 : pix;
			WeightMap.at<float>(y, x) = pix;*/
			WeightMap.at<int>(y, x) = pix;	// uchar이나 flaot는 hole 같은 것이 생김
		}
	}
		
	return WeightMap.clone();
}
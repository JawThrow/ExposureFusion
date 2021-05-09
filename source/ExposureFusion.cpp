#include "ExposureFusion.h"



void ExposureFusion::QualityMeasuresProcessing()
{
	Mat weightMap;
	Mat contrast;
	Mat saturation;
	Mat wellexposedness;
	Mat originalGray;
	Mat originalColor;
	time_t tok, tic = clock();
	for (int nfrm = 0; nfrm < nframes; nfrm++)
	{
		system("cls");
		cout << "Quality measure processing - Frame number: " << nfrm + 1;
		
		QualityMeasures *qm = new QualityMeasures(inputImages[nfrm].clone(), inputGrayImages[nfrm].clone());
		
		WeightMaps.push_back(qm->getterWeightMap());
		delete(qm);
	}
	tok = clock();
	cout << endl << "processing time of QualitymeasureProcessing: " << (float)(tok - tic) / CLOCKS_PER_SEC << endl;
	cout << endl;
}

void ExposureFusion::FusionProcessing()
{
	int nframes = getnframes();
	int rows = inputImages[0].rows;
	int cols = inputImages[0].cols;
	int pyramidDepth = 4;
	
	setNormalizedWeightMaps();
	
	vector<Mat> bgr;
	
	for (int i = 0; i < 3; i++)
	{
		bgr.push_back(setResultByPyramid(i));		
	}

	Mat dst;
	merge(bgr, dst);
	/*if(dst.rows > 1000)
		resize(dst, dst, Size(dst.cols*0.5, dst.rows*0.5));*/
		
	imshow("Exposure Fusion", dst);	
	resultimage = dst.clone();
	waitKey();
	/*if (resultimage.cols % 4)
		resize(resultimage, resultimage, Size(resultimage.cols - (resultimage.cols % 4), resultimage.rows));*/
}

void ExposureFusion::setNormalizedWeightMaps()
{
	int nframes = getnframes();
	int rows = inputImages[0].rows;
	int cols = inputImages[0].cols;

	float sumPix = 0;
#if MODE==GRAY	
	for (int nfrm = 0; nfrm < nframes; nfrm++)
	{
		Mat NorWeightMap(inputImages[0].rows, inputImages[0].cols, CV_32FC1);
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				sumPix = 0;
				
				for (int n = 0; n < nframes; n++)
				{
					sumPix += WeightMaps[n].at<float>(y, x);
				}
				NorWeightMap.at<float>(y, x) = WeightMaps[nfrm].at<float>(y, x) / sumPix;
			}
		}
		NorWeightMaps.push_back(NorWeightMap);
	}	
#endif
}

Mat ExposureFusion::setResultByPyramid(int nch)
{
	int pyramidDepth = 4;
	vector<Mat> gaussianPyramid;
	vector<vector<Mat>> gaussianWeightMapPyramid;
	vector<vector<Mat>> laplacianImagePyramid;
	vector<Mat> fusedPyramid;
	vector<vector<Mat>> fusedPyramidColor;
	int nframes = getnframes();
	Mat src;
	Mat ucharMap;
	Mat laplimg;
	Mat upGauss;
	Mat cuGauss;
	Mat pvGauss;
	Mat laplacianResult;
	Mat fuseimg;
	Mat rsLaplac;
	Mat result;
	float pix = 0;
		
#if MODE==GRAY
	vector<Mat> BGR;
	for (int nfrm = 0; nfrm < nframes; nfrm++)
	{
		split(inputImages[nfrm], BGR);
		src = BGR[nch].clone();
		laplacianImagePyramid.push_back(vector<Mat>());
		gaussianWeightMapPyramid.push_back(vector<Mat>());
		buildPyramid(src, gaussianPyramid, pyramidDepth);		
		
		ucharMap = Mat(NorWeightMaps[nfrm].size(), CV_8UC1);
		for (int y = 0; y < ucharMap.rows; y++)
		{
			for (int x = 0; x < ucharMap.cols; x++)
			{
				pix = NorWeightMaps[nfrm].at<float>(y, x) * 255;
				pix = (pix > 255) ? 255 : pix;
				pix = (pix < 0) ? 0 : pix;
				ucharMap.at<uchar>(y, x) = (uchar)pix;
			}
		}
		buildPyramid(ucharMap, gaussianWeightMapPyramid[nfrm], pyramidDepth);

		for (int i = 1; i < gaussianPyramid.size(); i++)
		{
			Mat prev = gaussianPyramid[i-1].clone();
			Mat crnt = gaussianPyramid[i].clone();
			

			pyrUp(crnt, crnt, prev.size());
			laplimg = Mat(prev.size(), CV_8SC1);

			for (int y = 0; y < prev.rows; y++)
			{
				for (int x = 0; x < prev.cols; x++)
				{
					laplimg.at<char>(y, x) = prev.at<uchar>(y, x) - crnt.at<uchar>(y, x);
				}
			}
			
			laplacianImagePyramid[nfrm].push_back(laplimg.clone());
			prev.release();
			crnt.release();
			laplimg.release();
		}

		laplacianImagePyramid[nfrm].push_back(gaussianPyramid[pyramidDepth].clone());
		ucharMap.release();
	}
	cout << "Set laplacian image pyramid " << endl << "Set gaussian weight map pyramid" << endl;


	for(int l=0; l<pyramidDepth; l++)	
	{
		//cout << "pyramid depth: " << l << endl;
		laplacianResult = Mat(laplacianImagePyramid[0][l].size(), CV_32SC1);
		for(int y = 0; y < laplacianResult.rows; y++)
		{
			for (int x = 0; x < laplacianResult.cols; x++)
			{
				pix = 0;
				for (int nfrm = 0; nfrm < nframes; nfrm++)
				{
					pix += gaussianWeightMapPyramid[nfrm][l].at<uchar>(y, x) * laplacianImagePyramid[nfrm][l].at<char>(y, x);
				} 
				laplacianResult.at<int>(y, x) = pix/255;
			}
		}
		fusedPyramid.push_back(laplacianResult.clone());
		laplacianResult.release();
	}


	laplacianResult = Mat(laplacianImagePyramid[0][pyramidDepth].size(), CV_8UC1);
	for (int y = 0; y < laplacianResult.rows; y++)
	{
		for (int x = 0; x < laplacianResult.cols; x++)
		{
			pix = 0;
			for (int nfrm = 0; nfrm < nframes; nfrm++)
			{
				pix += gaussianWeightMapPyramid[nfrm][pyramidDepth].at<uchar>(y, x) * (laplacianImagePyramid[nfrm][pyramidDepth].at<uchar>(y, x));
			}
			
			laplacianResult.at<uchar>(y, x) = (uchar)(pix/255);	// �� uchar�̳� char�� �ϸ� �̻��ϰ� ������; int���� ���͵��� �� �����ѵ�
		}
	}
	fusedPyramid.push_back(laplacianResult.clone());
		
	cout << "Set fused pyramid" << endl;
	

	int ipix = 0;	
	Mat temp = fusedPyramid[pyramidDepth].clone();
	Mat fusedLaplacianImage = fusedPyramid[pyramidDepth-1].clone();
	int rows = fusedLaplacianImage.rows;
	int cols = fusedLaplacianImage.cols;

	Mat sumimg(Size(cols, rows), CV_8UC1);
	pyrUp(temp, temp, fusedLaplacianImage.size());
	
	for (int y = 0; y < temp.rows; y++)
	{
		for (int x = 0; x < temp.cols; x++)
		{
			ipix = temp.at<uchar>(y, x) + fusedLaplacianImage.at<int>(y, x);
			ipix = (ipix > 255) ? 255 : ipix;
			ipix = (ipix < 0) ? 0 : ipix;
			sumimg.at<uchar>(y, x) = (uchar)ipix;
		}
	}
	
	for (int i = pyramidDepth - 2; i >= 0; i--)
	{
		fusedLaplacianImage = fusedPyramid[i].clone();

		pyrUp(sumimg, sumimg, fusedLaplacianImage.size());
		for (int y = 0; y < sumimg.rows; y++)
		{
			for (int x = 0; x < sumimg.cols; x++)
			{
				ipix = fusedLaplacianImage.at<int>(y, x) + sumimg.at<uchar>(y, x);
				ipix = (ipix > 255) ? 255 : ipix;
				ipix = (ipix < 0) ? 0 : ipix;
				sumimg.at<uchar>(y,x) = (uchar)ipix;
			}
		}
	}
	
	result = sumimg.clone();
#endif

	return result;
}
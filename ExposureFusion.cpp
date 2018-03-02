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
		
		QualityMeasures *qm = new QualityMeasures(inputImages[nfrm].clone(), inputGrayImages[nfrm].clone());	// delete �ؾ���
		
		//originalGray = inputGrayImages[nfrm].clone();
		//originalColor = inputImages[nfrm].clone();
		//contrast = qm->getterContrast();
		//saturation = qm->getterSaturation();
		//wellexposedness = qm->getterWellExposedness();
		//weightMap = qm->getterWeightMap();
		//
		//if (originalColor.rows > 1000)
		//{
		//	resize(originalColor, originalColor, Size(originalColor.cols*0.5, originalColor.rows*0.5));
		//	resize(originalGray, originalGray, Size(originalGray.cols*0.5, originalGray.rows*0.5));
		//	resize(contrast, contrast, Size(contrast.cols*0.5, contrast.rows*0.5));
		//	resize(saturation, saturation, Size(saturation.cols*0.5, saturation.rows*0.5));
		//	resize(wellexposedness, wellexposedness, Size(wellexposedness.cols*0.5, wellexposedness.rows*0.5));			
		//}
		////imshow("originalGray", originalGray);		
		////imshow("originalColor", originalColor);
		//imshow("EF Contrast", abs(contrast));
		//imshow("EF Saturation", saturation);
		////imshow("Well exposedness", wellexposedness);
		////namedWindow("WeightMap", CV_WINDOW_FREERATIO);
		//imshow("WeightMap", weightMap);
		//waitKey();

		/*originalGray = inputGrayImages[nfrm].clone();
		char wnd[256];
		sprintf(wnd, "original gray image %d", nfrm + 1);		
		imshow(wnd, originalGray);*/

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

	/*for (int i = 0; i < nframes; i+=nframes-1)
	{
		char wnd[256];
		sprintf(wnd, "Exposure fusion weight map %d", i);
		namedWindow(wnd, CV_WINDOW_KEEPRATIO);
		imshow(wnd, WeightMaps[i]);
	}
	cout << "Set normalized weight map" << endl;
	waitKey();*/

	// normalized weight map Ȯ��
	/*char c = 0;
	int i = 0;
	int crnt = 0;
	while (c != 27)
	{
		Mat normalizedWeightMap;
		resize(NorWeightMaps[crnt], normalizedWeightMap, Size(NorWeightMaps[crnt].cols*0.5, NorWeightMaps[crnt].rows*0.5));
		Mat wtMap;
		resize(WeightMaps[crnt], wtMap, Size(WeightMaps[crnt].cols*0.5, WeightMaps[crnt].rows*0.5));

		imshow("Normalized weight map", normalizedWeightMap);
		imshow("weight Map", wtMap);
		c = waitKey();

		if (c == '.')      i += 1;
		else if (c == ',') i -= 1;
		if (i < 0)         i = NorWeightMaps.size() - i;

		crnt = i%NorWeightMaps.size();
	}*/

	//seam image Ȯ��//
	/*Mat resultWithSeam(rows, cols, CV_8UC1);
	float pixVal = 0;
	float sumVal = 0;
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			pixVal = 0;
			for (int nfrm = 0; nfrm < nframes; nfrm++)
			{
				pixVal += NorWeightMaps[nfrm].at<float>(y, x) * (float)inputGrayImages[nfrm].at<uchar>(y, x);
			}
			pixVal = (pixVal > 255) ? 255 : pixVal;
			pixVal = (pixVal < 0) ? 0 : pixVal;
			resultWithSeam.at<uchar>(y, x) = (uchar)pixVal;
		}
	}
	Mat tempseam = resultWithSeam.clone();
	resize(tempseam, tempseam, Size(tempseam.cols*0.5, tempseam.rows*0.5));
	imshow("result", tempseam);
	waitKey();*/

	vector<Mat> bgr;
	
	for (int i = 0; i < 3; i++)
	{
		bgr.push_back(setResultByPyramid(i));
		/*char wnd[256];
		sprintf(wnd, "Destnation %d", i + 1);
		imshow(wnd, bgr[i]);*/
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
	vector<Mat> gaussianPyramid;	// laplacian image pyramid�� ���� gaussian image pyramid
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
					laplimg.at<char>(y, x) = prev.at<uchar>(y, x) - crnt.at<uchar>(y, x);	// �� ������ ������ ����?
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


	for(int l=0; l<pyramidDepth; l++)	// laplacianImagePyramid�� ������ ������ char������ �ƴ� uchar������;
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
				laplacianResult.at<int>(y, x) = pix/255;	// �� 255�� �����ϱ� ����ε� ������ ������? nframes�� ������ ���� �ƴϰ�?
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
				//pix += gaussianWeightMapPyramid[nfrm][pyramidDepth].at<uchar>(y, x) * (laplacianImagePyramid[nfrm][pyramidDepth].at<uchar>(y, x)/255.f);
				pix += gaussianWeightMapPyramid[nfrm][pyramidDepth].at<uchar>(y, x) * (laplacianImagePyramid[nfrm][pyramidDepth].at<uchar>(y, x));
			}
			
			laplacianResult.at<uchar>(y, x) = (uchar)(pix/255);	// �� uchar�̳� char�� �ϸ� �̻��ϰ� ������; int���� ���͵��� �� �����ѵ�
		}
	}
	fusedPyramid.push_back(laplacianResult.clone());

	/*for (int i = 0; i < fusedPyramid.size(); i++)
	{
		char wnd[256];
		sprintf(wnd, "fused pyramid %d", i + 1);
		Mat t(fusedPyramid[i].size(), CV_8UC1);
		for (int y = 0; y < t.rows; y++)
		{
			for (int x = 0; x < t.cols; x++)
			{
				if(i==fusedPyramid.size()-1) t.at<uchar>(y, x) = fusedPyramid[i].at<uchar>(y, x);
				else t.at<uchar>(y, x) = fusedPyramid[i].at<int>(y, x) + 127;
			}
		}
		imshow(wnd, t);
	}
	waitKey();*/
	//int idx = fusedPyramid.size() ;
	
	cout << "Set fused pyramid" << endl;
	

	int ipix = 0;	
	Mat temp = fusedPyramid[pyramidDepth].clone();
	Mat fusedLaplacianImage = fusedPyramid[pyramidDepth-1].clone();
	int rows = fusedLaplacianImage.rows;
	int cols = fusedLaplacianImage.cols;

	Mat sumimg(Size(cols, rows), CV_8UC1);
	//resize(temp, temp, fusedLaplacianImage.size());	// opencv bug; int���϶� �����߻�
	//pyrUp(temp, temp, Size(cols, rows));	// �Ƹ��� pyrUp ���ο��� resize�� �����ϴµ� ���� ���� ���׶����� �߻��ϴ� ���� ����.
	//resize(temp, temp, fusedLaplacianImage.size());
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
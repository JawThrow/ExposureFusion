#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <time.h>
#include <windows.h>
using namespace cv;

using namespace std;

#define GRAY 1
#define COLOR1 2
#define COLOR2 3

#define MODE GRAY

#define BLOCKROWS 32
#define BLOCKCOLS 24


class ExposureFusion
{
private:
	vector<Mat> inputImages;
	vector<Mat> inputGrayImages;
	vector<Mat> WeightMaps;
	vector<vector<Mat>> WeightMapsColor;	
	vector<Mat> NorWeightMaps;
	vector<vector<Mat>> NorWeightMapsColor;
	Mat ResultImage;
	int nframes;
public:
	ExposureFusion(char* seqPath)
	{
		char seqfname[256];
		char nfrmfname[256];
		int nfrm;
		sprintf(nfrmfname, "%s\\nframes.txt", seqPath);
		FILE* fp = fopen(nfrmfname, "rt");
		if (fp == NULL)
		{
			cout << "fail to read nframe txt" << endl;
			exit(-1);
		}
		fscanf(fp, "%d", &nfrm);
		this->nframes = nfrm;

		for(int n=0; n<nframes; n++)
		{
			sprintf(seqfname, "%s\\ExposureStep_%d.jpg", seqPath, n+1);
			Mat inputimg = imread(seqfname, IMREAD_UNCHANGED);	// flag 변경요망
			if (!inputimg.data)
			{
				cout << "fail to read image" << endl;
				exit(-1);
			}

			if (inputimg.rows > 1000)
			{
				do
				{
					Size sz(inputimg.cols*0.5, inputimg.rows*0.5);
					if ((int)(inputimg.cols*0.5) % BLOCKCOLS == 0 || (int)(inputimg.rows*0.5) % BLOCKROWS == 0)
						sz = Size(inputimg.cols*0.5 + 1, inputimg.cols*0.5 + 1);
					resize(inputimg, inputimg, sz);
				} while (inputimg.rows > 1000);
			}

			Mat gray(inputimg.size(), CV_8UC1);
			cvtColor(inputimg, gray, CV_BGR2GRAY);
			inputImages.push_back(inputimg);
			inputGrayImages.push_back(gray);
		}
		cout << "finish to read Image Sequence " << endl;
	}
	void QualityMeasuresProcessing(void);		
	void FusionProcessing(void);
	
	void setNormalizedWeightMaps(); // Quality measure 안으로?
	Mat setResultByPyramid(int nch);

/*******************************************
*getter
********************************************/
public:	
	Mat getinputImage(const int i) { return inputImages[i].clone(); }
	vector<Mat> getinputImages() const { return inputImages; }
	Mat getWeightMap(const int i) { return WeightMaps[i].clone(); }
	vector<Mat> getWeightMaps() const { return WeightMaps; }
	Mat getNorWeightMap(const int i) { return NorWeightMaps[i].clone(); }
	vector<Mat> getNorWeightMaps() const { return NorWeightMaps; }
	Mat getResultImage() { return ResultImage.clone(); }
	int getnframes() const { return nframes; }
	
	bool SaveImageBMP(const char* filename)
	{
		if (!strcmp(".bmp", &filename[strlen(filename) - 4]))
		{
			FILE* pFile = NULL;
			fopen_s(&pFile, filename, "wb");
			if (!pFile)
				return false;

			int m_nChannels = ResultImage.channels();
			int m_nHeight = ResultImage.rows;
			int m_nWidth = ResultImage.cols;
			int m_nWStep = (m_nWidth*m_nChannels * sizeof(uchar) + 3)&~3;

			BITMAPFILEHEADER fileHeader;
			fileHeader.bfType = 0x4D42; // 'BM'
			fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + m_nWStep*m_nHeight + (m_nChannels == 1) * 1024;
			fileHeader.bfReserved1 = 0;
			fileHeader.bfReserved2 = 0;
			fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (m_nChannels == 1) * 256 * sizeof(RGBQUAD);

			fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);

			BITMAPINFOHEADER infoHeader;
			infoHeader.biSize = sizeof(BITMAPINFOHEADER);
			infoHeader.biWidth = m_nWidth;
			infoHeader.biHeight = m_nHeight;
			infoHeader.biPlanes = 1;
			infoHeader.biBitCount = m_nChannels * 8;
			infoHeader.biCompression = BI_RGB;
			infoHeader.biSizeImage = m_nWStep*m_nHeight;
			infoHeader.biClrImportant = 0;
			infoHeader.biClrUsed = 0;
			infoHeader.biXPelsPerMeter = 0;
			infoHeader.biYPelsPerMeter = 0;

			fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

			if (m_nChannels == 1)
			{
				for (int l = 0; l<256; l++)
				{
					RGBQUAD GrayPalette = { l, l, l, 0 };
					fwrite(&GrayPalette, sizeof(RGBQUAD), 1, pFile);
				}
			}

			int r;
			for (r = m_nHeight - 1; r >= 0; r--)
			{
				fwrite(&ResultImage.data[r*m_nWStep], sizeof(BYTE), m_nWStep, pFile);
			}

			fclose(pFile);
			return true;
		}
		else
		{
			return false;
		}
	}
};


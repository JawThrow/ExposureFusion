#include "ExposureFusion.h"

using namespace std;
using namespace cv;

int main(void)
{
	//char *seqPath = "..\\..\\..\\..\\HDR\\Difference_Exposure_Images\\seq";
	char *seqPath = "..\\..\\..\\..\\HDR\\Difference_Exposure_Images\\time\\seq";
	char seqtoppath[256];

	char *resPath = "..\\..\\..\\..\\HDR\\Difference_Exposure_Images\\time\\res";
	char resfname[256];
	//FILE* fp = fopen("eftime.txt", "at");
	for (int i = 1; i <= 21; i++)
	{
		sprintf(seqtoppath, "%s%d", seqPath, i);
		time_t tok, tic = clock();
		ExposureFusion EF(seqtoppath);
		EF.QualityMeasuresProcessing();
		cout << "finish to QualityMeasuresProcessing" << endl;
		EF.FusionProcessing();
		cout << "finish to FusionProcessing" << endl;
		tok = clock();
		cout << endl << "total processing time : " << (float)(tok - tic) / CLOCKS_PER_SEC << endl;
		//fprintf(fp, "%.2f\n", (float)(tok - tic) / CLOCKS_PER_SEC);		
		
		/*sprintf(resfname, "%s\\EF_%d.bmp", resPath, i);
		if (!EF.SaveImageBMP(resfname))
		{
			cout << "fail to save result image" << endl;
			return -1;
		}*/
		//waitKey();
	} 
	//fclose(fp);

	return 0;
}
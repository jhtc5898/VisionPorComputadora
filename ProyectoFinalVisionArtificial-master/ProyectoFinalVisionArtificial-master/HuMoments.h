#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/videoio/videoio.hpp>

#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include <math.h>

using namespace std;
using namespace cv;

using namespace cv::xfeatures2d;

class HuMomentsExtractor {

private:
	string outDir;
	int hMin, sMin, vMin;
	int hMax, sMax, vMax;
	int operationMode;

	Mat imageThreshold;

	// Hu Moments previously extracted for Red, Blue, and Green colors:

	double basehumoments[3][7] = { {0.282695,0.0267101,0.000600215,0.000177191,5.75241e-08,-2.83437e-05,5.48647e-09},//ROJO
			{0.354126,0.0522643,0.0215943,0.0145461,0.000255355,0.00320318,3.54495e-05},//AZUL
			{0.250394,0.00304636,3.28906e-05,0.00178699,-2.74854e-07,4.80174e-05,3.34878e-07} }; //VERDE

	// Red: hsv-min (0,53,162)    hsv-max (12,192,244)
	// Blue: hsv-min (83,125,183)   hsv-max (137,186,232)
	// Green: hsv-min (36,85,134) hsv-max (53,196,201)




	static void huFunc(int, void*);
	void printHSV();


	int euclideanDistance(vector<double>, int);

	void refreshImg();
	/*
	static void hMax(int,void*);
	static void sMax(int,void*);
	static void vMax(int,void*);
	static void hMin(int,void*);
	static void sMin(int,void*);
	static void vMin(int,void*);
	*/

public:
	HuMomentsExtractor(string = "fichero.txt");
	vector<double> extractHuMoments(Mat);
	vector<double> extractHuMoments(Mat, int, int, int, int, int, int);
	void capture();
	void setOperationMode(int);
};
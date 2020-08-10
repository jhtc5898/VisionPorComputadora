#include "HuMoments.h"

HuMomentsExtractor::HuMomentsExtractor(string outDir) {
	this->outDir = outDir;
	this->operationMode = 1;
	this->hMax = 180;
	this->sMax = 255;
	this->vMax = 255;
}

vector<double> HuMomentsExtractor::extractHuMoments(Mat imageO) {

	Mat image;
	Mat hsv;
	Moments _moments;
	double huMoments[7];
	vector<double> huMomentsV;

	int clipLimit = 2;

	//Suavizado BGR
	GaussianBlur(imageO, image, Size(5, 5), 3);

	//Convertir HSV
	cvtColor(image, hsv, COLOR_BGR2HSV);

	std::vector<cv::Mat> lab_planes(3);
	cv::split(hsv, lab_planes);

	Ptr<cv::CLAHE> clahe = cv::createCLAHE();

	clahe->setClipLimit(clipLimit);
	cv::Mat dst;
	clahe->apply(lab_planes[2], dst);

	dst.copyTo(lab_planes[2]);
	cv::merge(lab_planes, hsv);

	imshow("hsv", hsv);

	inRange(hsv, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), imageThreshold);
	imshow("threshold", imageThreshold);

	_moments = moments(imageThreshold, true);
	HuMoments(_moments, huMoments);

	for (int i = 0; i < 7; i++) {
		huMomentsV.push_back(0);
		huMomentsV[i] = huMoments[i];
		cout << huMoments[i] << ",";
	}
	cout << endl;

	return huMomentsV;
}

vector<double> HuMomentsExtractor::extractHuMoments(Mat imageO, int hmin, int smin, int vmin, int hmax, int smax, int vmax) {

	Mat hsv;
	Mat image;
	Moments _moments;
	double huMoments[7];
	vector<double> huMomentsV;

	int clipLimit = 2;

	//Suavizado BGR
	GaussianBlur(imageO, image, Size(5, 5), 3);

	//Convertir HSV
	cvtColor(image, hsv, COLOR_BGR2HSV);

	std::vector<cv::Mat> lab_planes(3);
	cv::split(hsv, lab_planes);

	Ptr<cv::CLAHE> clahe = cv::createCLAHE();

	clahe->setClipLimit(clipLimit);
	cv::Mat dst;
	clahe->apply(lab_planes[2], dst);

	dst.copyTo(lab_planes[2]);
	cv::merge(lab_planes, hsv);

	inRange(hsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), imageThreshold);
	imshow("threshold", imageThreshold);

	_moments = moments(imageThreshold, true);
	HuMoments(_moments, huMoments);

	for (int i = 0; i < 7; i++) {
		huMomentsV.push_back(0);
		huMomentsV[i] = huMoments[i];
		cout << huMoments[i] << "@";
	}
	huMomentsV.push_back(_moments.m10 / _moments.m00);
	huMomentsV.push_back(_moments.m01 / _moments.m00);
	cout << endl;

	return huMomentsV;
}

void HuMomentsExtractor::huFunc(int v, void* p) {
	HuMomentsExtractor* hu = reinterpret_cast<HuMomentsExtractor*> (p);
	hu->refreshImg();
}

void HuMomentsExtractor::refreshImg() {
	imshow("threshold", imageThreshold);
	printHSV();
}

void HuMomentsExtractor::printHSV() {
	cout << "hsv-min (" << hMin << "," << sMin << "," << vMin << ")" << "hsv-max (" << hMax << "," << sMax << "," << vMax << ")" << endl;
}

int HuMomentsExtractor::euclideanDistance(vector<double> moms, int i) {
	double d = 0.0;
	int index = -1;

	double thresholdm = 0.3;

	for (int j = 0; j < 7; j++) {
		//cout << "m: " << moms[j] << " humoments: " << basehumoments[i][j] << ",";
		d += ((moms[j] - basehumoments[i][j]) * (moms[j] - basehumoments[i][j]));
	}
	cout << endl;
	d = sqrt(d);
	//cout << "Distance: " << " i: " << i << " :: " << d << endl;

	//if (i == 2)
		//thresholdm = 0.41;

	if (d < thresholdm)
		return i;

	return -1;
}

void HuMomentsExtractor::setOperationMode(int m) {
	this->operationMode = m;
}
void eventoTrack(int v, void* p) {
	//cout << "Valor: " << v << endl;
}


void HuMomentsExtractor::capture() {


	char key = 'a';
	int framecount = 0;

	Mat object = imread("ingles.jpg", IMREAD_GRAYSCALE);
	resize(object, object, Size(200, 200));

	Mat object2 = imread("libro.jpg", IMREAD_GRAYSCALE);
	resize(object2, object2, Size(200, 200));

	Mat object3 = imread("cuaderno.jpg", IMREAD_GRAYSCALE);
	resize(object3, object3, Size(200, 200));


	VideoCapture video(0);
	int opciones = 1;
	if (video.isOpened()) {
		video.set(CAP_PROP_FRAME_WIDTH, 800);
		video.set(CAP_PROP_FRAME_HEIGHT, 600);

		Mat frame;
		

		namedWindow("video", WINDOW_AUTOSIZE);
		namedWindow("hsv", WINDOW_AUTOSIZE);
		namedWindow("threshold", WINDOW_AUTOSIZE);


		

		createTrackbar("HMin", "video", &hMin, 180, HuMomentsExtractor::huFunc, static_cast<void *>(this));
		createTrackbar("SMin", "video", &sMin, 255, HuMomentsExtractor::huFunc, static_cast<void *>(this));
		createTrackbar("VMin", "video", &vMin, 255, HuMomentsExtractor::huFunc, static_cast<void *>(this));

		createTrackbar("HMax", "video", &hMax, 180, HuMomentsExtractor::huFunc, static_cast<void *>(this));
		createTrackbar("SMax", "video", &sMax, 255, HuMomentsExtractor::huFunc, static_cast<void *>(this));
		createTrackbar("VMax", "video", &vMax, 255, HuMomentsExtractor::huFunc, static_cast<void *>(this));

		vector<double> huMoments;
		vector<double> huMoments1;
		vector<double> huMoments2;
		int indexRed = -1;
		int indexBlue = -1;
		int indexGreen = -1;

		while (1 == 1) {

			video >> frame;
			flip(frame, frame, 1);
			//resize(frame, frame, Size(400, 200));

			createTrackbar("Opciones", "video", &opciones, 1, eventoTrack, NULL);
			if (opciones == 1)
			{
				cout << "Global: " << opciones << endl;
				if (this->operationMode == 1) { // In this mode you can use the trackbars to determine the HSV segmentation range
					huMoments = this->extractHuMoments(frame);
				}
				else if (this->operationMode == 2) { // In this mode you can use test the values selected to perform the segmentation and Hu Moments extraction
					huMoments = this->extractHuMoments(frame, 117, 139, 106, 180, 255, 255);
					indexRed = this->euclideanDistance(huMoments, 0);

					huMoments1 = this->extractHuMoments(frame, 99, 123, 123, 117, 255, 255);
					indexBlue = this->euclideanDistance(huMoments1, 1);

					huMoments2 = this->extractHuMoments(frame, 30, 160, 60, 101, 255, 255);
					indexGreen = this->euclideanDistance(huMoments2, 2);
					if (indexRed != -1) {
						cout << "Red object: " << indexRed << endl;
						putText(frame, "Object 1 Detected - Red", Point(huMoments[7], huMoments[8]), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 10, 143), 2);
					}
					if (indexBlue != -1) {
						cout << "Blue object: " << indexBlue << endl;
						putText(frame, "Object 2 Detected - Blue", Point(huMoments1[7], huMoments1[8]), FONT_HERSHEY_DUPLEX, 1, Scalar(143, 10, 0), 2);
					}
					if (indexGreen != -1) {
						cout << "Object 3 Detected - Green: " << indexGreen << endl;
						putText(frame, "Object 3 Detected - Green: ", Point(huMoments2[7], huMoments2[8]), FONT_HERSHEY_DUPLEX, 1, Scalar(10, 143, 3), 2);
					}
				}
				imshow("video", frame);

			}
			if(opciones == 0)
			{
				


               

                    Ptr<SIFT>  descriptor = SIFT::create();
                    FlannBasedMatcher matcher;
                    FlannBasedMatcher matcher2;
                    FlannBasedMatcher matcher3;

                    Mat frame, des_object, des_object2, des_object3, image;
                    Mat des_image, img_matches, img_matches2, img_matches3, H, H2, H3;

                    vector<KeyPoint> kp_object;
                    vector<KeyPoint> kp_object2;
                    vector<KeyPoint> kp_object3;


                    vector<Point2f> obj_corners(4);
                    vector<Point2f> obj_corners2(4);
                    vector<Point2f> obj_corners3(4);

                    vector<KeyPoint> kp_image;
                    vector<vector<DMatch > > matches;
                    vector<vector<DMatch > > matches2;
                    vector<vector<DMatch > > matches3;



                    vector<DMatch > matches_filtrados;
                    vector<DMatch > matches_filtrados2;
                    vector<DMatch > matches_filtrados3;

                    vector<Point2f> obj;
                    vector<Point2f> obj2;
                    vector<Point2f> obj3;

                    vector<Point2f> scene;
                    vector<Point2f> scene2;
                    vector<Point2f> scene3;

                    vector<Point2f> scene_corners(4);
                    vector<Point2f> scene_corners2(4);
                    vector<Point2f> scene_corners3(4);




                    descriptor->detect(object, kp_object);
                    descriptor->detect(object2, kp_object2);
                    descriptor->detect(object3, kp_object3);



                    descriptor->compute(object, kp_object, des_object);
                    descriptor->compute(object2, kp_object2, des_object2);
                    descriptor->compute(object3, kp_object3, des_object3);


                    obj_corners[0] = Point(0, 0);
                    obj_corners[1] = Point(object.cols, 0);
                    obj_corners[2] = Point(object.cols, object.rows);
                    obj_corners[3] = Point(0, object.rows);

                    obj_corners2[0] = Point(0, 0);
                    obj_corners2[1] = Point(object2.cols, 0);
                    obj_corners2[2] = Point(object2.cols, object2.rows);
                    obj_corners2[3] = Point(0, object2.rows);

                    obj_corners3[0] = Point(0, 0);
                    obj_corners3[1] = Point(object3.cols, 0);
                    obj_corners3[2] = Point(object3.cols, object3.rows);
                    obj_corners3[3] = Point(0, object3.rows);


                    video >> frame;

                    if (framecount < 5)
                    {
                        framecount++;
                        continue;
                    }
                    //
                    cvtColor(frame, image, COLOR_RGB2GRAY);
                    resize(image, image, Size(200, 200));


                    descriptor->detect(image, kp_image);
                    descriptor->compute(image, kp_image, des_image);





                    matcher.knnMatch(des_object, des_image, matches, 2);
                    matcher2.knnMatch(des_object2, des_image, matches2, 2);
                    matcher3.knnMatch(des_object3, des_image, matches3, 2);


                    for (int i = 0; i < min(des_image.rows - 1, (int)matches.size()); i++)
                    {
                        if ((matches[i][0].distance < 0.8 * (matches[i][1].distance)) && ((int)matches[i].size() <= 2 && (int)matches[i].size() > 0))
                        {
                            matches_filtrados.push_back(matches[i][0]);

                        }
                    }

                    for (int i = 0; i < min(des_image.rows - 1, (int)matches2.size()); i++)
                    {

                        if ((matches2[i][0].distance < 0.8 * (matches2[i][1].distance)) && ((int)matches2[i].size() <= 2 && (int)matches2[i].size() > 0))
                        {
                            matches_filtrados2.push_back(matches2[i][0]);

                        }
                    }

                    for (int i = 0; i < min(des_image.rows - 1, (int)matches3.size()); i++)
                    {

                        if ((matches3[i][0].distance < 0.8 * (matches3[i][1].distance)) && ((int)matches3[i].size() <= 2 && (int)matches3[i].size() > 0))
                        {
                            matches_filtrados3.push_back(matches3[i][0]);

                        }
                    }





                    drawMatches(object, kp_object, image, kp_image, matches_filtrados, img_matches);
                    drawMatches(object2, kp_object2, image, kp_image, matches_filtrados2, img_matches2);
                    drawMatches(object3, kp_object3, image, kp_image, matches_filtrados3, img_matches3);


                    if (matches_filtrados.size() >= 3)
                    {

                        for (int i = 0; i < matches_filtrados.size(); i++)
                        {

                            obj.push_back(kp_object[matches_filtrados[i].queryIdx].pt);
                            scene.push_back(kp_image[matches_filtrados[i].trainIdx].pt);
                        }
                        try
                        {
                            H = findHomography(obj, scene, RANSAC);
                            perspectiveTransform(obj_corners, scene_corners, H);
                        }
                        catch (Exception e) {}




                        line(img_matches, scene_corners[0] + Point2f(object.cols, 0), scene_corners[1] + Point2f(object.cols, 0), Scalar(255, 0, 0), 4);
                        line(img_matches, scene_corners[1] + Point2f(object.cols, 0), scene_corners[2] + Point2f(object.cols, 0), Scalar(255, 0, 0), 4);
                        line(img_matches, scene_corners[2] + Point2f(object.cols, 0), scene_corners[3] + Point2f(object.cols, 0), Scalar(255, 0, 0), 4);
                        line(img_matches, scene_corners[3] + Point2f(object.cols, 0), scene_corners[0] + Point2f(object.cols, 0), Scalar(255, 0, 0), 4);
                    }

                    if (matches_filtrados2.size() >= 3)
                    {

                        for (int i = 0; i < matches_filtrados2.size(); i++)
                        {

                            obj2.push_back(kp_object2[matches_filtrados2[i].queryIdx].pt);
                            scene2.push_back(kp_image[matches_filtrados2[i].trainIdx].pt);
                        }
                        try
                        {
                            H2 = findHomography(obj2, scene2, RANSAC);
                            perspectiveTransform(obj_corners2, scene_corners2, H2);
                        }
                        catch (Exception e) {}

                        line(img_matches2, scene_corners2[0] + Point2f(object2.cols, 0), scene_corners2[1] + Point2f(object2.cols, 0), Scalar(0, 0, 255), 4);
                        line(img_matches2, scene_corners2[1] + Point2f(object2.cols, 0), scene_corners2[2] + Point2f(object2.cols, 0), Scalar(0, 0, 255), 4);
                        line(img_matches2, scene_corners2[2] + Point2f(object2.cols, 0), scene_corners2[3] + Point2f(object2.cols, 0), Scalar(0, 0, 255), 4);
                        line(img_matches2, scene_corners2[3] + Point2f(object2.cols, 0), scene_corners2[0] + Point2f(object2.cols, 0), Scalar(0, 0, 255), 4);
                    }

                    if (matches_filtrados3.size() >= 3)
                    {

                        for (int i = 0; i < matches_filtrados3.size(); i++)
                        {

                            obj3.push_back(kp_object3[matches_filtrados3[i].queryIdx].pt);
                            scene3.push_back(kp_image[matches_filtrados3[i].trainIdx].pt);
                        }
                        try
                        {
                            H3 = findHomography(obj3, scene3, RANSAC);
                            perspectiveTransform(obj_corners3, scene_corners3, H3);
                        }
                        catch (Exception e) {}

                        line(img_matches3, scene_corners3[0] + Point2f(object3.cols, 0), scene_corners3[1] + Point2f(object3.cols, 0), Scalar(0, 255, 0), 4);
                        line(img_matches3, scene_corners3[1] + Point2f(object3.cols, 0), scene_corners3[2] + Point2f(object3.cols, 0), Scalar(0, 255, 0), 4);
                        line(img_matches3, scene_corners3[2] + Point2f(object3.cols, 0), scene_corners3[3] + Point2f(object3.cols, 0), Scalar(0, 255, 0), 4);
                        line(img_matches3, scene_corners3[3] + Point2f(object3.cols, 0), scene_corners3[0] + Point2f(object3.cols, 0), Scalar(0, 255, 0), 4);
                    }
					
					
					destroyWindow("hsv");
					destroyWindow("threshold");
					imshow("video", frame);
                    imshow("Good Matches Primer Objeto", img_matches);
                    imshow("Good Matches segundo objeto", img_matches2);
                    imshow("Good Matches Tercer objeto", img_matches3);
					
				

                    matches_filtrados.clear();
                    matches_filtrados2.clear();
                    matches_filtrados3.clear();

                 
                

			
			}


			if (waitKey(23) == 27)
				break;
		}
	}
}
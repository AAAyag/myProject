// Include OpenCV's C++ Interface
#include "opencv2/opencv.hpp"

// Include the rest of our code!
#include "detectObject.h"       // Easily detect faces or eyes (using LBP or Haar Cascades).
#include "preprocessFace.h"     // Easily preprocess face images, for face recognition.
#include "recognition.h"     // Train the face recognition system and recognize a person from an image.

#include <io.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <string>

using namespace std;
using namespace cv;
using std::vector;
void getTrainingSet(string datapath, map<string, int> labelmap);
void initDetectors(CascadeClassifier &faceCascade, CascadeClassifier &eyeCascade1, CascadeClassifier &eyeCascade2);

// The Face Recognition algorithm can be one of these and perhaps more, depending on your version of OpenCV, which must be atleast v2.4.1:
//    "FaceRecognizer.Eigenfaces":  Eigenfaces, also referred to as PCA (Turk and Pentland, 1991).
//    "FaceRecognizer.Fisherfaces": Fisherfaces, also referred to as LDA (Belhumeur et al, 1997).
//    "FaceRecognizer.LBPH":        Local Binary Pattern Histograms (Ahonen et al, 2006).
const char *facerecAlgorithm = "FaceRecognizer.Fisherfaces";
//const char *facerecAlgorithm = "FaceRecognizer.Eigenfaces";


// Sets how confident the Face Verification algorithm should be to decide if it is an unknown person or a known person.
// A value roughly around 0.5 seems OK for Eigenfaces or 0.7 for Fisherfaces, but you may want to adjust it for your
// conditions, and if you use a different Face Recognition algorithm.
// Note that a higher threshold value means accepting more faces as known people,
// whereas lower values mean more faces will be classified as "unknown".
const float UNKNOWN_PERSON_THRESHOLD = 0.7f;


// Cascade Classifier file, used for Face Detection.
//const char *faceCascadeFilename = "lbpcascade_frontalface.xml";     // LBP face detector.
const char *faceCascadeFilename = "haarcascade_frontalface_alt_tree.xml";  // Haar face detector.
//const char *eyeCascadeFilename1 = "haarcascade_lefteye_2splits.xml";   // Best eye detector for open-or-closed eyes.
//const char *eyeCascadeFilename2 = "haarcascade_righteye_2splits.xml";   // Best eye detector for open-or-closed eyes.
const char *eyeCascadeFilename1 = "haarcascade_mcs_lefteye.xml";       // Good eye detector for open-or-closed eyes.
const char *eyeCascadeFilename2 = "haarcascade_mcs_righteye.xml";       // Good eye detector for open-or-closed eyes.
//const char *eyeCascadeFilename1 = "haarcascade_eye.xml";               // Basic eye detector for open eyes only.
//const char *eyeCascadeFilename2 = "haarcascade_eye_tree_eyeglasses.xml"; // Basic eye detector for open eyes if they might wear glasses.
const bool preprocessLeftAndRightSeparately = true;   // Preprocess left & right sides of the face separately, in case there is stronger light on one side.

// Set the desired face dimensions. Note that "getPreprocessedFace()" will return a square face.
const int faceWidth = 70;
const int faceHeight = faceWidth;

vector<Mat> images;
vector<int> labels;
string LFWDataPath="C:\\Users\\Eric\\Desktop\\FaceDataBase\\���ݼ�\\LFW\\�ü�\\*.jpg";		// ע��˴�����ָ��*.jpg
string LFWRootPath="C:\\Users\\Eric\\Desktop\\FaceDataBase\\���ݼ�\\LFW\\�ü�";		

void main()
{
	CascadeClassifier faceCascade;
    CascadeClassifier eyeCascade1;
    CascadeClassifier eyeCascade2;
	initDetectors(faceCascade, eyeCascade1, eyeCascade2);
	//labeledLFW �������ļ���+���  eg: Aaron_Eckhart_0001.jpg 1 
	string labelSetFile = "labeledLFW.txt";
	map<string, int> labelSet;			//�ļ����ͱ��һ һӳ��
	ifstream fLFW;
	fLFW.open(labelSetFile.c_str());
	string name;
	int label;
	while (fLFW>>name>>label)
	{
		labelSet[name] = label;
	}	
	//cout <<"the label sample"<<labelSet["Adriana_Lima_0001.jpg"] <<endl;
	cout <<"The LFW datebase name+label has been stored in the map:labelSet"<<endl;
	getTrainingSet(LFWDataPath, labelSet);

	
	/*Mat faceimg=images[2];
	imshow("faceimg",faceimg);
	cv::waitKey(10);*/
    int  countnum=0;
	for (unsigned i = 0; i< images.size(); i++)
	{
	   
		 Mat faceimg = images[i];
		 Rect faceRect;  // Position of detected face.
		 Rect searchedLeftEye, searchedRightEye; // top-left and top-right regions of the face, where eyes were searched.
		 Point leftEye, rightEye;    // Position of the detected eyes.
		 Mat preprocessedFace = getPreprocessedFace(faceimg, faceWidth, faceCascade, eyeCascade1, eyeCascade2, preprocessLeftAndRightSeparately, &faceRect, &leftEye, &rightEye, &searchedLeftEye,		&searchedRightEye);
		 
		 if (preprocessedFace.data)
		 {
			countnum++;
			cout<<"the num is "<<countnum<<endl;
		 }	 
	}
		

 	 

	system("pause");
}


/****************************************************************************************/
/*���ܣ���ȡѵ�������õ� Vector<Mat>images �� Vector<int>labels
  ���룺���ݿ��ַ����ǩmap
  ����ֵ����                                                                              */
/****************************************************************************************/
void getTrainingSet(string datapath, map<string, int>labelmap)
{

	struct _finddata_t FileInfo;
	long Handle = _findfirst(datapath.c_str(),&FileInfo);

	if (Handle == -1L)    
	{        
		cerr << "can not match the folder path" << endl;        
		exit(-1);    
	}    
	do
	{   
		string name=FileInfo.name;			//��ȡ�ļ���
		images.push_back(imread(LFWRootPath+"\\"+name));		
		labels.push_back(labelmap[name]);
		cout<< "already pushed "<< name <<" label: " <<labelmap[name]<<endl;		
	}while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);  
	cout<< "the imgas size is "<<images.size() <<"\n the labels size is "<< labels.size()<<endl;
}
/****************************************************************************************/
/*���ܣ���ʼ������������ط�����
  ���룺CascadeClassifier &faceCascade, CascadeClassifier &eyeCascade1, CascadeClassifier &eyeCascade2
  ����ֵ����                                                                              */
/****************************************************************************************/
// Load the face and 1 or 2 eye detection XML classifiers.
void initDetectors(CascadeClassifier &faceCascade, CascadeClassifier &eyeCascade1, CascadeClassifier &eyeCascade2)
{
    // Load the Face Detection cascade classifier xml file.
    try {   // Surround the OpenCV call by a try/catch block so we can give a useful error message!
        faceCascade.load(faceCascadeFilename);
    } catch (cv::Exception &e) {}
    if ( faceCascade.empty() ) {
        cerr << "ERROR: Could not load Face Detection cascade classifier [" << faceCascadeFilename << "]!" << endl;
        cerr << "Copy the file from your OpenCV data folder (eg: 'C:\\OpenCV\\data\\lbpcascades') into this WebcamFaceRec folder." << endl;
        exit(1);
    }
    cout << "Loaded the Face Detection cascade classifier [" << faceCascadeFilename << "]." << endl;

    // Load the Eye Detection cascade classifier xml file.
    try {   // Surround the OpenCV call by a try/catch block so we can give a useful error message!
        eyeCascade1.load(eyeCascadeFilename1);
    } catch (cv::Exception &e) {}
    if ( eyeCascade1.empty() ) {
        cerr << "ERROR: Could not load 1st Eye Detection cascade classifier [" << eyeCascadeFilename1 << "]!" << endl;
        cerr << "Copy the file from your OpenCV data folder (eg: 'C:\\OpenCV\\data\\haarcascades') into this WebcamFaceRec folder." << endl;
        exit(1);
    }
    cout << "Loaded the 1st Eye Detection cascade classifier [" << eyeCascadeFilename1 << "]." << endl;

    // Load the Eye Detection cascade classifier xml file.
    try {   // Surround the OpenCV call by a try/catch block so we can give a useful error message!
        eyeCascade2.load(eyeCascadeFilename2);
    } catch (cv::Exception &e) {}
    if ( eyeCascade2.empty() ) {
        cerr << "Could not load 2nd Eye Detection cascade classifier [" << eyeCascadeFilename2 << "]." << endl;
        // Dont exit if the 2nd eye detector did not load, because we have the 1st eye detector at least.
        //exit(1);
    }
    else
        cout << "Loaded the 2nd Eye Detection cascade classifier [" << eyeCascadeFilename2 << "]." << endl;
}


#pragma once

#include "ofxKinectCommonBridge.h"
#include "ofMain.h"

#define HFOV 1.01447
#define HFOVTANHALF tan(HFOV/2)

struct Position{
	float x;
	float y;
};

struct ImageSource{
	Position position;
	float width;
	float height;
};

struct Image{
	float width;
	float height;
	ImageSource source;
};

struct Bodypart{
	Position position;
	float scale;
	float angle;
	Image img;
};

struct Body{
	Bodypart spine;
	Bodypart shoulderLeft;
	Bodypart elbowLeft;
	Bodypart wristLeft;
	Bodypart handLeft;
	Bodypart shoulderCenter;
	Bodypart head;
	Bodypart shoulderRight;
	Bodypart elbowRight;
	Bodypart wristRight;
	Bodypart handRight;
	Bodypart hipLeft;
	Bodypart kneeLeft;
	Bodypart ankleLeft;
	Bodypart footLeft;
	Bodypart hipRight;
	Bodypart kneeRight;
	Bodypart ankleRight;
	Bodypart footRight;
};

struct Screen{
	float width;
	float height;
	float borderLeft;
	float borderTop;
	float borderRight;
	float borderBottom;
};

struct Frame{
	float width;
	float height;
	Position position;
};

struct ArduinoButton{
	bool enabled;
	bool pressed;
};

struct Record{
	Body frames[30 * 10];
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void initKinect();
		void initWebcam();
		void initArduino();
		void initXML();
		void calibrateFrame();

		void updateKinect();
		void updateWebcam();
		void updateArduino();
		void updateXML();

		void drawIdleState();
		void drawScanState();
		void drawDanceState();

		void setupArduino(const int & version);
		void digitalPinChanged(const int & pinNum);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void loadXML();
		
		ofxKinectCommonBridge kinect;
		ofPixels outputPixels;
		ofTexture outputTexture;

		ofImage backgroundImage;
		ofImage foregroundImage;

		ofTexture inputImage;
		ofVideoGrabber vidGrabber;
		unsigned char * videoInverted;
		ofTexture videoTexture;
		int camWidth;
		int camHeight;

		unsigned char * colorAlphaPixels;
		ofArduino ard;
		bool bSetupArduino;

		int	buttonb;
		int ledb;
		int buttonr;
		int ledr;

		ofTrueTypeFont displayFont;
		ofTrueTypeFont textFont;

		double _angle( int current_x , int current_y , int tar_x , int tar_y ){
			return atan2(tar_y - current_y, tar_x - current_x);
		}

		Screen screen;
		Frame frame;
		Body body;
		float factor;

		string currentState;
		ArduinoButton buttonBlue;
		ArduinoButton buttonRed;

		ofImage templateImage;

		bool recording;
		//Record records[1];
		bool recorded;
		bool captured;

		int recordTime;

		ofImage videoFrames[5];
		ofImage videoFrame;

		ofImage imageIdle;
		ofImage imageScan;
		ofImage imageDance;

		int selectedBackground;

		ofSoundPlayer blob;
		ofSoundPlayer fart;
		ofSoundPlayer soundtrack;
		bool soundtrackPlaying;
		int soundIndex;

		float startTime; // store when we start time timer
		float endTime; // when do want to stop the timer
		
		bool  bTimerReached; // used as a trigger when we hit the timer

		int numFrames;
		
		ofXml recordedXML;

		Body recordedBodies[100];
};
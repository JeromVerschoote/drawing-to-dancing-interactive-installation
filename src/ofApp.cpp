#include "ofApp.h"

//----------------------------------------------------------------------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(30);
	//
	templateImage.loadImage("img/templateImage.png");
	imageIdle.loadImage("img/stateIdle.png");
	imageScan.loadImage("img/stateScan.png");
	imageDance.loadImage("img/stateDance.png");
	blob.loadSound("sounds/blob.mp3");
	//
	blob.setVolume(2);
	fart.setVolume(2);
	//
	initKinect();
	initWebcam();
	initArduino();
	//initXML();
	//
	factor = 1.3;
	selectedBackground = 1;
	soundIndex = 1;
	recording = false;
	recorded = false;
	recordTime = 10 * 30; // 10 seconden
	Body recordedBodies[100];
	soundtrackPlaying = false;
	//
	currentState = "idle";
	ofLog() << "entered idle state";
	//
	bTimerReached = false;
    endTime = 5000; // in milliseconds
}

void ofApp::initKinect(){
	kinect.initSensor();
	kinect.initSkeletonStream(false);
	kinect.start();
	ofSetWindowShape(1920 * 2, 1080 * 2 * 2);
	ofDisableAlphaBlending(); //Kinect alpha channel is default 0;
}

void ofApp::initWebcam(){
	camWidth = 1280;
	camHeight = 720;
	vector<ofVideoDevice> devices = vidGrabber.listDevices();
    for(int i = 0; i < devices.size(); i++){
		cout << devices[i].id << ": " << devices[i].deviceName; 
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl; 
        }
	}
	vidGrabber.setDeviceID(0);
	vidGrabber.setDesiredFrameRate(60);
	vidGrabber.initGrabber(camWidth,camHeight);
	ofSetVerticalSync(true);
	calibrateFrame();
}

void ofApp::initArduino(){
	buttonb = 12;
	ledb = 13;
	buttonr = 9;
	ledr = 10;
	//
	ard.connect("COM3", 57600);
	ard.sendFirmwareVersionRequest();
	ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
	bSetupArduino = true;
}

void ofApp::setupArduino(const int & version) {

	ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino = true;
    
    ofLog() << ard.getFirmwareName(); 
    ofLog() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    // Webcam Button
    ard.sendDigitalPinMode(buttonb, ARD_INPUT);
	ard.sendDigitalPinMode(ledb, ARD_OUTPUT);
    
    // Record Button
    ard.sendDigitalPinMode(buttonr, ARD_INPUT);
	ard.sendDigitalPinMode(ledr, ARD_OUTPUT);

    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
}

void ofApp::calibrateFrame(){
	screen.width = 1280; // 1700
	screen.height = 720; // 2338
	screen.borderLeft = 430; // 23
	screen.borderTop = 25; // 137
	screen.borderRight = 400; // 97
	screen.borderBottom = 64; // 255
	//
	frame.width = screen.width - screen.borderLeft - screen.borderRight; // 1580
	frame.height = screen.height - screen.borderTop - screen.borderBottom; // 1976
	frame.position.x = screen.borderLeft;
	frame.position.y = screen.borderTop;
}

void ofApp::initXML(){
	/*if(XML.load("record1.xml")){
		ofLog() << "record loaded";
		//message = "skeletonRecord1.xml loaded!";
	}else{
        XML.addChild("SKELETON");
        XML.setTo("SKELETON");
		//
		xmlFrames.addChild("FRAMES");
		xmlFrames.setTo("FRAMES");
		//
		//XML.addXml(xmlFrames);
		//
		//message = "unable to load skeletonRecord1.xml check data/ folder";
	}

	xmlPosition.addChild("POSITION");
	xmlPosition.setTo("POSITION");
	xmlPosition.addValue("X", 0);
	xmlPosition.addValue("Y", 0);
	xmlPosition.addValue("Z", 0);*/
}

//----------------------------------------------------------------------------------------------------------------------------
void ofApp::update(){
	updateKinect();
	updateWebcam();
	updateArduino();
	//updateXML();
	ofSoundUpdate();
}

void ofApp::updateKinect(){
	kinect.update();
	if (kinect.isFrameNew()) {
		/*
		ofPixels colorPixelsRef = kinect.getColorPixelsRef();
		ofColor alpha = ofColor(0, 0, 0, 0);
		NUI_DEPTH_IMAGE_PIXEL* nuiDepthPixels = kinect.getNuiDepthPixelsRef();
		ofShortPixels rawDepthPixels = kinect.getRawDepthPixelsRef();

		INuiSensor * sensor = & kinect.getNuiSensor();
		NUI_IMAGE_RESOLUTION _sourceDepthResolution = NUI_IMAGE_RESOLUTION_640x480;
		NUI_IMAGE_RESOLUTION _sourceRGBResolution = NUI_IMAGE_RESOLUTION_640x480;
		USHORT packedDepth;
		long depthX, depthY, rgbX, rgbY;
		for (int y = 0; y < 480; y++) {
			for (int x = 0; x < 640; x++) {
				int index = y * 640 + x;
				NUI_DEPTH_IMAGE_PIXEL nuiDepthPixel = nuiDepthPixels[index];

				packedDepth = rawDepthPixels.getPixels()[index] << 4;

				if (nuiDepthPixel.playerIndex > 0) {
					sensor->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(_sourceRGBResolution,
						_sourceDepthResolution,
						NULL,
						x, 
						y, 
						packedDepth,
						&rgbX,
						&rgbY
					);
					if (rgbX < 640 && rgbY < 480 && rgbX >= 0 && rgbY >= 0) {
						outputPixels.setColor(x, y, colorPixelsRef.getColor(rgbX, rgbY));
					} else {
						outputPixels.setColor(x, y, alpha);
					}					
				} else {
					outputPixels.setColor(x, y, alpha);
				}
			}
		}

		outputTexture.loadData(outputPixels, GL_RGBA);
		*/
		// --- body update

		// head
		body.head.img.source.position.x = frame.position.x + (frame.width / 2.62);
		body.head.img.source.position.y = frame.position.y + 0;
		body.head.img.source.width = frame.width / 4.213;
		body.head.img.source.height =  frame.height / 5.474;

		// left hand
		body.handLeft.img.source.position.x = frame.position.x + 0;
		body.handLeft.img.source.position.y = frame.position.y + (frame.height / 5.161);
		body.handLeft.img.source.width = frame.width / 8.729;
		body.handLeft.img.source.height = frame.height / 7.936;

		// left wrist
		body.wristLeft.img.source.position.x = frame.position.x + (frame.width / 8.426); //  9.1907
		body.wristLeft.img.source.position.y =  frame.position.y + (frame.height / 5.161);
		body.wristLeft.img.source.width = frame.width / 22.253;
		body.wristLeft.img.source.height = frame.height / 7.936;

		// left elbow
		body.elbowLeft.img.source.position.x = frame.position.x + (frame.width / 5.908);  //  6.587
		body.elbowLeft.img.source.position.y = frame.position.y + (frame.height / 5.161);
		body.elbowLeft.img.source.width = frame.width / 9.133;
		body.elbowLeft.img.source.height = frame.height / 7.936;

		// left shoulder
		body.shoulderLeft.img.source.position.x = frame.position.x + (frame.width / 3.473);
		body.shoulderLeft.img.source.position.y =  frame.position.y + (frame.height / 5.161);
		body.shoulderLeft.img.source.width = frame.width / 7.560;
		body.shoulderLeft.img.source.height = frame.height / 7.936;

		// center shoulder
		body.shoulderCenter.img.source.position.x = frame.position.x + (frame.width / 2.336);
		body.shoulderCenter.img.source.position.y =  frame.position.y + (frame.height / 5.161);
		body.shoulderCenter.img.source.width = frame.width / 7.418;
		body.shoulderCenter.img.source.height = frame.height / 7.936;

		// right shoulder
		body.shoulderRight.img.source.position.x = frame.position.x + (frame.width / 1.742);
		body.shoulderRight.img.source.position.y =  frame.position.y + (frame.height / 5.161);
		body.shoulderRight.img.source.width = frame.width / 7.488;
		body.shoulderRight.img.source.height = frame.height / 7.936;

		// right elbow
		body.elbowRight.img.source.position.x = frame.position.x + (frame.width / 1.393);
		body.elbowRight.img.source.position.y =  frame.position.y + (frame.height / 5.161);
		body.elbowRight.img.source.width = frame.width / 9.133;
		body.elbowRight.img.source.height = frame.height / 7.936;

		// right wrist
		body.wristRight.img.source.position.x = frame.position.x + (frame.width / 1.193);
		body.wristRight.img.source.position.y = frame.position.y + (frame.height / 5.161);
		body.wristRight.img.source.width = frame.width /  22.253;
		body.wristRight.img.source.height = frame.height / 7.936;

		// right hand
		body.handRight.img.source.position.x = frame.position.x + (frame.width / 1.119);
		body.handRight.img.source.position.y = frame.position.y + (frame.height / 5.161);
		body.handRight.img.source.width = frame.width /  8.729;
		body.handRight.img.source.height = frame.height / 7.936;

		// spine
		body.spine.img.source.position.x = frame.position.x + (frame.width / 3.153);
		body.spine.img.source.position.y = frame.position.y + (frame.height / 3.107);
		body.spine.img.source.width = frame.width /  2.816;
		body.spine.img.source.height = frame.height / 4.563;

		// left hip
		body.hipLeft.img.source.position.x = frame.position.x + (frame.width / 3.153);
		body.hipLeft.img.source.position.y = frame.position.y + (frame.height / 1.823);
		body.hipLeft.img.source.width = frame.width /  5.704 ; // 9.106
		body.hipLeft.img.source.height = frame.height / 9.191; // 9.277

		// right hip
		body.hipRight.img.source.position.x = frame.position.x + (frame.width / 1.985);
		body.hipRight.img.source.position.y = frame.position.y + (frame.height / 1.823);
		body.hipRight.img.source.width = frame.width /  5.704;
		body.hipRight.img.source.height =  frame.height / 9.191;

		// left knee
		body.kneeLeft.img.source.position.x = frame.position.x + (frame.width / 3.153);
		body.kneeLeft.img.source.position.y = frame.position.y + (frame.height / 1.506);
		body.kneeLeft.img.source.width = frame.width /  5.704;
		body.kneeLeft.img.source.height =  frame.height / 4.268;;

		// right knee
		body.kneeRight.img.source.position.x = frame.position.x + (frame.width / 1.985);
		body.kneeRight.img.source.position.y = frame.position.y + (frame.height / 1.506);
		body.kneeRight.img.source.width = frame.width /  5.704;
		body.kneeRight.img.source.height = frame.height / 4.268;

		// left ankle
		body.ankleLeft.img.source.position.x = frame.position.x + (frame.width / 3.153);
		body.ankleLeft.img.source.position.y = frame.position.y + (frame.height / 1.103);
		body.ankleLeft.img.source.width =frame.width /  5.704;
		body.ankleLeft.img.source.height = frame.height / 34.667;

		// right ankle
		body.ankleRight.img.source.position.x = frame.position.x + (frame.width / 1.985);
		body.ankleRight.img.source.position.y = frame.position.y + (frame.height / 1.103);
		body.ankleRight.img.source.width = frame.width /  5.704;
		body.ankleRight.img.source.height = frame.height / 34.667;

		// left foot
		body.footLeft.img.source.position.x = frame.position.x + (frame.width / 3.153);
		body.footLeft.img.source.position.y = frame.position.y + (frame.height / 1.063);
		body.footLeft.img.source.width = frame.width /  5.704;
		body.footLeft.img.source.height = frame.height / 15.318;

		// rigth foot
		body.footRight.img.source.position.x = frame.position.x + (frame.width / 1.985);
		body.footRight.img.source.position.y = frame.position.y + (frame.height / 1.063);
		body.footRight.img.source.width = frame.width /  5.704;
		body.footRight.img.source.height = frame.height / 15.318;
	}
}

void ofApp::updateWebcam(){
	vidGrabber.update();
	//
	 if (vidGrabber.isFrameNew()){
	 	int totalPixels = camWidth*camHeight*3;
	 	unsigned char * pixels = vidGrabber.getPixels();
	 }
}

void ofApp::updateArduino(){
    ard.update();
}

//----------------------------------------------------------------------------------------------------------------------------
void ofApp::draw(){
	buttonBlue.enabled = true;
	buttonBlue.pressed = false;
	buttonRed.enabled = true;
	buttonRed.pressed = false;

	if(currentState == "idle"){
		drawIdleState();
	}

	if(currentState == "scan"){
		drawScanState();
	}

	if(currentState == "dance"){
		drawDanceState();
	}
}

void ofApp::drawIdleState(){

	ofPushMatrix();
	ofScale(1, 1);
	imageIdle.draw(0,0);
	ofPopMatrix();

	if(currentState == "idle" && recorded){
		ofLog() << "loading safed rig";
		ofLog() << numFrames;
		ofPushMatrix();
		ofScale(1, 1);

		for(int i = 0; i < 100; i++){
			ofLog() << "test log xpos" << recordedBodies[i].spine.position.x;

			inputImage.drawSubsection(
				recordedBodies[i].spine.position.x - (recordedBodies[i].spine.img.width / 2),
				recordedBodies[i].spine.position.y - (recordedBodies[i].spine.img.height / 2),
				recordedBodies[i].spine.img.width,
				recordedBodies[i].spine.img.height,
				recordedBodies[i].spine.img.source.position.x,
				recordedBodies[i].spine.img.source.position.y,
				recordedBodies[i].spine.img.source.width,
				recordedBodies[i].spine.img.source.height
			);
		}
		ofPopMatrix();
		

		

		//ofSetHexColor(0xFFFFFF);
		//videoFrame.draw(0, 0);
		/*for(int i = 0; i < 5; i++){
			ofPushMatrix();
			ofScale(0.3, 0.3);
			videoFrames[i].draw(0,0);
			ofPopMatrix();
		}*/
		/*for(int j = 0; j < recordTime; j++){
			ofLog() << records[0].frames[j].spine.position.x << records[0].frames[j].spine.position.y;
			inputImage.drawSubsection(
				records[0].frames[j].spine.position.x - (records[0].frames[j].spine.img.width / 2),
				records[0].frames[j].spine.position.y - (records[0].frames[j].spine.img.height / 2),
				records[0].frames[j].spine.img.width,
				records[0].frames[j].spine.img.height,
				records[0].frames[j].spine.img.source.position.x,
				records[0].frames[j].spine.img.source.position.y,
				records[0].frames[j].spine.img.source.width,
				records[0].frames[j].spine.img.source.height
			);
			}
		/*for (int i = 0; i < 1; i++){

		}*/
	}
}

void ofApp::drawScanState(){
	//
	ofPushMatrix();
	ofTranslate(-650, 0);
	ofScale(1, 1);
	ofPushMatrix();
	ofScale(1.5,1.5);
	ofRotateZ(1);	
	vidGrabber.draw(0, 0);
	ofPopMatrix();

	/*if(captured){
		ofPushMatrix();
		ofTranslate(0, 0);
		ofScale(1, 1);
		ofRotateZ(1);
		inputImage.draw(0, 0);
		ofPopMatrix();
	}*/

	ofPushMatrix();
	ofScale(0.48, 0.48);
	ofEnableAlphaBlending();
	templateImage.draw(1220, 80);
	ofPopMatrix();
	ofPopMatrix();

	ofPushMatrix();
	ofScale(1, 1);
	imageScan.draw(0,0);
	ofPopMatrix();
}

void ofApp::drawDanceState(){
	
	if(recording){
		/*float timer = ofGetElapsedTimeMillis() - startTime;
		ofLog() << "timer started";
		ofLog() << timer;

		ofXml xmlFrame;
		xmlFrame.addChild("FRAME");
		xmlFrame.setTo("FRAME");
		//
		xmlHead.addChild("HEAD");
		xmlHead.setTo("HEAD");
		xmlPosition.setValue("X", ofToString(body.head.position.x));
		xmlPosition.setValue("Y", ofToString(body.head.position.y));
		xmlHead.addXml(xmlPosition);
		xmlFrame.addXml(xmlHead);

		xmlHandLeft.addChild("HAND_LEFT");
		xmlHandLeft.setTo("HAND_LEFT");
		xmlPosition.setValue("X", ofToString(body.handLeft.position.x));
		xmlPosition.setValue("Y", ofToString(body.handLeft.position.y));
		xmlHandLeft.addXml(xmlPosition);
		xmlFrame.addXml(xmlHandLeft);

		xmlWristLeft.addChild("WRIST_LEFT");
		xmlWristLeft.setTo("WRIST_LEFT");
		xmlPosition.setValue("X", ofToString(body.wristLeft.position.x));
		xmlPosition.setValue("Y", ofToString(body.wristLeft.position.y));
		xmlWristLeft.addXml(xmlPosition);
		xmlFrame.addXml(xmlWristLeft);

		xmlElbowLeft.addChild("ELBOW_LEFT");
		xmlElbowLeft.setTo("ELBOW_LEFT");
		xmlPosition.setValue("X", ofToString(body.elbowLeft.position.x));
		xmlPosition.setValue("Y", ofToString(body.elbowLeft.position.y));
		xmlElbowLeft.addXml(xmlPosition);
		xmlFrame.addXml(xmlElbowLeft);

		xmlShoudlerLeft.addChild("SHOULDER_LEFT");
		xmlShoudlerLeft.setTo("SHOULDER_LEFT");
		xmlPosition.setValue("X", ofToString(body.shoulderLeft.position.x));
		xmlPosition.setValue("Y", ofToString(body.shoulderLeft.position.y));
		xmlShoudlerLeft.addXml(xmlPosition);
		xmlFrame.addXml(xmlShoudlerLeft);

		xmlShoulderCenter.addChild("SHOULDER_CENTER");
		xmlShoulderCenter.setTo("SHOULDER_CENTER");
		xmlPosition.setValue("X", ofToString(body.shoulderCenter.position.x));
		xmlPosition.setValue("Y", ofToString(body.shoulderCenter.position.y));
		xmlShoulderCenter.addXml(xmlPosition);
		xmlFrame.addXml(xmlShoulderCenter);

		xmlShoulderRight.addChild("SHOULDER_RIGHT");
		xmlShoulderRight.setTo("SHOULDER_RIGHT");
		xmlPosition.setValue("X", ofToString(body.shoulderRight.position.x));
		xmlPosition.setValue("Y", ofToString(body.shoulderRight.position.y));
		xmlShoulderRight.addXml(xmlPosition);
		xmlFrame.addXml(xmlShoulderRight);

		xmlSpine.addChild("SPINE");
		xmlSpine.setTo("SPINE");
		xmlPosition.setValue("X", ofToString(body.spine.position.x));
		xmlPosition.setValue("Y", ofToString(body.spine.position.y));
		xmlSpine.addXml(xmlPosition);
		xmlFrame.addXml(xmlSpine);

		xmlHipLeft.addChild("HIP_LEFT");
		xmlHipLeft.setTo("HIP_LEFT");
		xmlPosition.setValue("X", ofToString(body.hipLeft.position.x));
		xmlPosition.setValue("Y", ofToString(body.hipLeft.position.y));
		xmlHipLeft.addXml(xmlPosition);
		xmlFrame.addXml(xmlHipLeft);

		xmlHipRight.addChild("HIP_RIGHT");
		xmlHipRight.setTo("HIP_RIGHT");
		xmlPosition.setValue("X", ofToString(body.hipRight.position.x));
		xmlPosition.setValue("Y", ofToString(body.hipRight.position.y));
		xmlHipRight.addXml(xmlPosition);
		xmlFrame.addXml(xmlHipRight);

		xmlKneeLeft.addChild("KNEE_LEFT");
		xmlKneeLeft.setTo("KNEE_LEFT");
		xmlPosition.setValue("X", ofToString(body.kneeLeft.position.x));
		xmlPosition.setValue("Y", ofToString(body.kneeLeft.position.y));
		xmlKneeLeft.addXml(xmlPosition);
		xmlFrame.addXml(xmlKneeLeft);

		xmlKneeRight.addChild("KNEE_RIGHT");
		xmlKneeRight.setTo("KNEE_RIGHT");
		xmlPosition.setValue("X", ofToString(body.kneeRight.position.x));
		xmlPosition.setValue("Y", ofToString(body.kneeRight.position.y));
		xmlKneeRight.addXml(xmlPosition);
		xmlFrame.addXml(xmlKneeRight);

		xmlAnkleLeft.addChild("ANKLE_LEFT");
		xmlAnkleLeft.setTo("ANKLE_LEFT");
		xmlPosition.setValue("X", ofToString(body.ankleLeft.position.x));
		xmlPosition.setValue("Y", ofToString(body.ankleLeft.position.y));
		xmlAnkleLeft.addXml(xmlPosition);
		xmlFrame.addXml(xmlAnkleLeft);

		xmlAnkleRight.addChild("ANKLE_RIGHT");
		xmlAnkleRight.setTo("ANKLE_RIGHT");
		xmlPosition.setValue("X", ofToString(body.ankleRight.position.x));
		xmlPosition.setValue("Y", ofToString(body.ankleRight.position.y));
		xmlAnkleRight.addXml(xmlPosition);
		xmlFrame.addXml(xmlAnkleRight);

		xmlFootLeft.addChild("FOOT_LEFT");
		xmlFootLeft.setTo("FOOT_LEFT");
		xmlPosition.setValue("X", ofToString(body.footLeft.position.x));
		xmlPosition.setValue("Y", ofToString(body.footLeft.position.y));
		xmlFootLeft.addXml(xmlPosition);
		xmlFrame.addXml(xmlFootLeft);

		xmlFootRight.addChild("FOOT_RIGHT");
		xmlFootRight.setTo("FOOT_RIGHT");
		xmlPosition.setValue("X", ofToString(body.footRight.position.x));
		xmlPosition.setValue("Y", ofToString(body.footRight.position.y));
		xmlFootRight.addXml(xmlPosition);
		xmlFrame.addXml(xmlFootRight);

		xmlFrames.addXml(xmlFrame);

		if(timer >= endTime && !bTimerReached) {
			ofLog() << "timer ended";
			bTimerReached = true;
			XML.addXml(xmlFrames);
			XML.save("record1.xml");
			recording = false;
		}
		//for(int i = 0; i < recordTime; i++){
		//	ofLog() << "recording frame " << i;
		//	records[0].frames[i] = body;
		//}
		//videoFrame.grabScreen(0, 0, 1920, 1080);
		//string fileName = "snapshot_"+ofToString(10000)+".png";
		//videoFrame.saveImage(fileName);

		/*for(int i = 0; i < 5; i++){
			//ofSetColor(255, 255, 255);
			videoFrames[i].allocate(1920, 1080, GL_RGB);
			videoFrames[i].loadScreenData(0, 0, 1920, 1080);
			//string name = "frames/frame" + ofToString(i) + ".png";
			//videoFrames[i].saveImage(name);
			//
			//videoFrames[i].setImageType(OF_IMAGE_COLOR);
			//ofSaveImage(videoFrames[i].getPixelsRef(), name, OF_IMAGE_QUALITY_BEST); 
			//videoFrames[i].saveImage(name);
		}*/
	}
	//
	ofPushMatrix();
		ofScale(1, 1);
		ofTranslate(0, 0);
		backgroundImage.draw(0, 0);
	ofPopMatrix();

	ofPushMatrix();
		ofPushStyle();
		ofScale(2, 2);
		ofTranslate(200, 30);
		ofSetColor(255, 150, 0);

		auto skeletons = kinect.getSkeletons();
		for(auto & skeleton : skeletons) {
		ofSetColor(255);
		ofEnableAlphaBlending();
			// GET SKELETON LOCATIONS
			
			if(skeleton.find(NUI_SKELETON_POSITION_HEAD) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HEAD)->second;
				body.head.position.x = bone.getScreenPosition().x;
				body.head.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.head.scale = pixelWidth / 1024.0;
				body.head.img.width = body.head.img.source.width * (body.head.scale * factor);
				body.head.img.height =  body.head.img.source.height * (body.head.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_HAND_LEFT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
				body.handLeft.position.x = bone.getScreenPosition().x;
				body.handLeft.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.handLeft.scale = pixelWidth / 1024.0;
				body.handLeft.img.width = body.handLeft.img.source.width * (body.handLeft.scale * factor);
				body.handLeft.img.height =  body.handLeft.img.source.height * (body.handLeft.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_WRIST_LEFT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
				body.wristLeft.position.x = bone.getScreenPosition().x;
				body.wristLeft.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.wristLeft.scale = pixelWidth / 1024.0;
				body.wristLeft.img.width = body.wristLeft.img.source.width * (body.wristLeft.scale * factor);
				body.wristLeft.img.height =  body.wristLeft.img.source.height * (body.wristLeft.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_ELBOW_LEFT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second;
				body.elbowLeft.position.x = bone.getScreenPosition().x - 30;
				body.elbowLeft.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.elbowLeft.scale = pixelWidth / 1024.0;
				body.elbowLeft.img.width = body.elbowLeft.img.source.width * (body.elbowLeft.scale * factor);
				body.elbowLeft.img.height =  body.elbowLeft.img.source.height * (body.elbowLeft.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_LEFT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_SHOULDER_LEFT)->second;
				body.shoulderLeft.position.x = bone.getScreenPosition().x - 10;
				body.shoulderLeft.position.y = bone.getScreenPosition().y; // +30
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.shoulderLeft.scale = pixelWidth / 1024.0;
				body.shoulderLeft.img.width = body.shoulderLeft.img.source.width * (body.shoulderLeft.scale * factor);
				body.shoulderLeft.img.height =  body.shoulderLeft.img.source.height * (body.shoulderLeft.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_CENTER) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_SHOULDER_CENTER)->second;
				body.shoulderCenter.position.x = bone.getScreenPosition().x;
				body.shoulderCenter.position.y = bone.getScreenPosition().y + 30;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.shoulderCenter.scale = pixelWidth / 1024.0;
				body.shoulderCenter.img.width = body.shoulderCenter.img.source.width * (body.shoulderCenter.scale * factor);
				body.shoulderCenter.img.height =  body.shoulderCenter.img.source.height * (body.shoulderCenter.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_RIGHT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_SHOULDER_RIGHT)->second;
				body.shoulderRight.position.x = bone.getScreenPosition().x +10;
				body.shoulderRight.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.shoulderRight.scale = pixelWidth / 1024.0;
				body.shoulderRight.img.width = body.shoulderRight.img.source.width * (body.shoulderRight.scale * factor);
				body.shoulderRight.img.height =  body.shoulderRight.img.source.height * (body.shoulderRight.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_ELBOW_RIGHT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second;
				body.elbowRight.position.x = bone.getScreenPosition().x ;
				body.elbowRight.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.elbowRight.scale = pixelWidth / 1024.0;
				body.elbowRight.img.width = body.elbowRight.img.source.width * (body.elbowRight.scale * factor);
				body.elbowRight.img.height =  body.elbowRight.img.source.height * (body.elbowRight.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_WRIST_RIGHT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
				body.wristRight.position.x = bone.getScreenPosition().x;
				body.wristRight.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.wristRight.scale = pixelWidth / 1024.0;
				body.wristRight.img.width = body.wristRight.img.source.width * (body.wristRight.scale * factor);
				body.wristRight.img.height =  body.wristRight.img.source.height * (body.wristRight.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_HAND_RIGHT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
				body.handRight.position.x = bone.getScreenPosition().x;
				body.handRight.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.handRight.scale = pixelWidth / 1024.0;
				body.handRight.img.width = body.handRight.img.source.width * (body.handRight.scale * factor);
				body.handRight.img.height =  body.handRight.img.source.height * (body.handRight.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_SPINE) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_SPINE)->second;
				body.spine.position.x = bone.getScreenPosition().x;
				body.spine.position.y = bone.getScreenPosition().y + 25;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.spine.scale = pixelWidth / 1024.0;
				body.spine.img.width = body.spine.img.source.width * (body.spine.scale * factor);
				body.spine.img.height =  body.spine.img.source.height * (body.spine.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_HIP_LEFT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HIP_LEFT)->second;
				body.hipLeft.position.x = bone.getScreenPosition().x - 15;
				body.hipLeft.position.y = bone.getScreenPosition().y + 60;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.hipLeft.scale = pixelWidth / 1024.0;
				body.hipLeft.img.width = body.hipLeft.img.source.width * (body.hipLeft.scale * factor);
				body.hipLeft.img.height =  body.hipLeft.img.source.height * (body.hipLeft.scale * factor);		
			};

			if(skeleton.find(NUI_SKELETON_POSITION_HIP_RIGHT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HIP_RIGHT)->second;
				body.hipRight.position.x = bone.getScreenPosition().x  + 15;
				body.hipRight.position.y = bone.getScreenPosition().y + 60;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.hipRight.scale = pixelWidth / 1024.0;
				body.hipRight.img.width = body.hipRight.img.source.width * (body.hipRight.scale * factor);
				body.hipRight.img.height =  body.hipRight.img.source.height * (body.hipRight.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_KNEE_LEFT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_KNEE_LEFT)->second;
				body.kneeLeft.position.x = bone.getScreenPosition().x;
				body.kneeLeft.position.y = bone.getScreenPosition().y + 30;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.kneeLeft.scale = pixelWidth / 1024.0;
				body.kneeLeft.img.width = body.kneeLeft.img.source.width * (body.kneeLeft.scale * factor);
				body.kneeLeft.img.height =  body.kneeLeft.img.source.height * (body.kneeLeft.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_KNEE_RIGHT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_KNEE_RIGHT)->second;
				body.kneeRight.position.x = bone.getScreenPosition().x;
				body.kneeRight.position.y = bone.getScreenPosition().y + 30;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.kneeRight.scale = pixelWidth / 1024.0;
				body.kneeRight.img.width = body.kneeRight.img.source.width * (body.kneeRight.scale * factor);
				body.kneeRight.img.height =  body.kneeRight.img.source.height * (body.kneeRight.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_ANKLE_LEFT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_ANKLE_LEFT)->second;
				body.ankleLeft.position.x = bone.getScreenPosition().x;
				body.ankleLeft.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.ankleLeft.scale = pixelWidth / 1024.0;
				body.ankleLeft.img.width = body.ankleLeft.img.source.width * (body.ankleLeft.scale * factor);
				body.ankleLeft.img.height =  body.ankleLeft.img.source.height * (body.ankleLeft.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_ANKLE_RIGHT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_ANKLE_RIGHT)->second;
				body.ankleRight.position.x = bone.getScreenPosition().x;
				body.ankleRight.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.ankleRight.scale = pixelWidth / 1024.0;
				body.ankleRight.img.width = body.ankleRight.img.source.width * (body.ankleRight.scale * factor);
				body.ankleRight.img.height =  body.ankleRight.img.source.height * (body.ankleRight.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_FOOT_LEFT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_FOOT_LEFT)->second;
				body.footLeft.position.x = bone.getScreenPosition().x;
				body.footLeft.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.footLeft.scale = pixelWidth / 1024.0;
				body.footLeft.img.width = body.footLeft.img.source.width * (body.footLeft.scale * factor);
				body.footLeft.img.height =  body.footLeft.img.source.height * (body.footLeft.scale * factor);
			};

			if(skeleton.find(NUI_SKELETON_POSITION_FOOT_RIGHT) != skeleton.end()) {
				SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_FOOT_RIGHT)->second;
				body.footRight.position.x = bone.getScreenPosition().x;
				body.footRight.position.y = bone.getScreenPosition().y;
				//
				float pixelWidth = 1024.0 / (HFOVTANHALF * bone.getStartPosition().z);
				body.footRight.scale = pixelWidth / 1024.0;
				body.footRight.img.width = body.footRight.img.source.width * (body.footRight.scale * factor);
				body.footRight.img.height =  body.footRight.img.source.height * (body.footRight.scale * factor);
			};


			// DRAW IMAGES

			if(currentState == "dance"){
			if(skeleton.find(NUI_SKELETON_POSITION_SPINE) != skeleton.end()) {
				inputImage.drawSubsection(
					body.spine.position.x - (body.spine.img.width / 2),
					body.spine.position.y - (body.spine.img.height / 2),
					body.spine.img.width,
					body.spine.img.height,
					body.spine.img.source.position.x,
					body.spine.img.source.position.y,
					body.spine.img.source.width,
					body.spine.img.source.height
				);
			} 

			if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_CENTER) != skeleton.end()) {
				inputImage.drawSubsection(
					body.shoulderCenter.position.x - (body.shoulderCenter.img.width / 2),
					body.shoulderCenter.position.y - (body.shoulderCenter.img.height / 2),
					body.shoulderCenter.img.width,
					body.shoulderCenter.img.height,
					body.shoulderCenter.img.source.position.x,
					body.shoulderCenter.img.source.position.y,
					body.shoulderCenter.img.source.width,
					body.shoulderCenter.img.source.height
				);
			}

			if(skeleton.find(NUI_SKELETON_POSITION_ELBOW_LEFT) != skeleton.end()) {
				body.elbowLeft.angle = _angle(body.elbowLeft.position.x, body.elbowLeft.position.y, body.shoulderLeft.position.x, body.shoulderLeft.position.y) * 70;
				ofPushMatrix();
				ofTranslate(
					body.elbowLeft.position.x,
					body.elbowLeft.position.y
				);
				ofRotateZ(body.elbowLeft.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.elbowLeft.img.width / 2),
						- (body.elbowLeft.img.height / 2),
						body.elbowLeft.img.width + 25,
						body.elbowLeft.img.height,
						body.elbowLeft.img.source.position.x,
						body.elbowLeft.img.source.position.y,
						body.elbowLeft.img.source.width,
						body.elbowLeft.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_ELBOW_RIGHT) != skeleton.end()) {
				body.elbowRight.angle = _angle(body.shoulderRight.position.x, body.shoulderRight.position.y, body.elbowRight.position.x, body.elbowRight.position.y) * 70; 
				ofPushMatrix();
				ofTranslate(
					body.elbowRight.position.x,
					body.elbowRight.position.y
				);
				ofRotateZ(body.elbowRight.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.elbowRight.img.width / 2),
						- (body.elbowRight.img.height / 2),
						body.elbowRight.img.width +25,
						body.elbowRight.img.height,
						body.elbowRight.img.source.position.x,
						body.elbowRight.img.source.position.y - 5,
						body.elbowRight.img.source.width,
						body.elbowRight.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_KNEE_LEFT) != skeleton.end()) {
				body.kneeLeft.angle =_angle(body.kneeLeft.position.x, body.kneeLeft.position.y, body.hipLeft.position.x, body.hipLeft.position.y) * 110;
				ofPushMatrix();
				ofTranslate(
					body.kneeLeft.position.x ,
					body.kneeLeft.position.y
				);
					ofRotateZ(body.kneeLeft.angle);
						ofPushMatrix();
						inputImage.drawSubsection(
							- (body.kneeLeft.img.width / 2),
							- (body.kneeLeft.img.height / 2),
							body.kneeLeft.img.width,
							body.kneeLeft.img.height,
							body.kneeLeft.img.source.position.x,
							body.kneeLeft.img.source.position.y,
							body.kneeLeft.img.source.width,
							body.kneeLeft.img.source.height
						);
						ofPopMatrix();
					ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_KNEE_RIGHT) != skeleton.end()) {
				body.kneeRight.angle =_angle(body.hipRight.position.x, body.hipRight.position.y, body.kneeRight.position.x, body.kneeRight.position.y) * 110;
				ofPushMatrix();
				ofTranslate(
					body.kneeRight.position.x ,
					body.kneeRight.position.y
				);
				ofRotateZ(body.kneeRight.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.kneeRight.img.width / 2),
						- (body.kneeRight.img.height / 2),
						body.kneeRight.img.width,
						body.kneeRight.img.height,
						body.kneeRight.img.source.position.x,
						body.kneeRight.img.source.position.y,
						body.kneeRight.img.source.width,
						body.kneeRight.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_HEAD) != skeleton.end()) {
				body.head.angle = _angle(body.head.position.x, body.head.position.y, body.shoulderCenter.position.x, body.shoulderCenter.position.y);
				ofPushMatrix();
				ofTranslate(
					body.head.position.x ,
					body.head.position.y
				);
				ofRotateZ(body.head.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.head.img.width / 2),
						- (body.head.img.height / 2),
						body.head.img.width,
						body.head.img.height,
						body.head.img.source.position.x,
						body.head.img.source.position.y + 10,
						body.head.img.source.width,
						body.head.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_WRIST_LEFT) != skeleton.end()) {
				body.wristLeft.angle = _angle(body.wristLeft.position.x, body.wristLeft.position.y, body.shoulderLeft.position.x, body.shoulderLeft.position.y) * 70;
				//
				ofPushMatrix();
				ofTranslate(
					body.wristLeft.position.x ,
					body.wristLeft.position.y
				);
				ofRotateZ(body.wristLeft.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.wristLeft.img.width / 2),
						- (body.wristLeft.img.height / 2),
						body.wristLeft.img.width,
						body.wristLeft.img.height,
						body.wristLeft.img.source.position.x,
						body.wristLeft.img.source.position.y,
						body.wristLeft.img.source.width,
						body.wristLeft.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_WRIST_RIGHT) != skeleton.end()) {
				body.wristRight.angle = _angle(body.shoulderRight.position.x, body.shoulderRight.position.y, body.wristRight.position.x, body.wristRight.position.y) * 70;
				ofPushMatrix();
				ofTranslate(
					body.wristRight.position.x ,
					body.wristRight.position.y
				);
				ofRotateZ(body.wristRight.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.wristRight.img.width / 2),
						- (body.wristRight.img.height / 2),
						body.wristRight.img.width,
						body.wristRight.img.height,
						body.wristRight.img.source.position.x,
						body.wristRight.img.source.position.y - 5,
						body.wristRight.img.source.width,
						body.wristRight.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_ANKLE_LEFT) != skeleton.end()) {
				body.ankleLeft.angle =_angle(body.ankleLeft.position.x, body.ankleLeft.position.y, body.hipLeft.position.x, body.hipLeft.position.y) * 110;
				ofPushMatrix();
				ofTranslate(
					body.ankleLeft.position.x ,
					body.ankleLeft.position.y
				);
				ofRotateZ(body.ankleLeft.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.ankleLeft.img.width / 2),
						- (body.ankleLeft.img.height / 2),
						body.ankleLeft.img.width,
						body.ankleLeft.img.height,
						body.ankleLeft.img.source.position.x,
						body.ankleLeft.img.source.position.y,
						body.ankleLeft.img.source.width,
						body.ankleLeft.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_ANKLE_RIGHT) != skeleton.end()) {
				body.ankleRight.angle =_angle(body.hipRight.position.x, body.hipRight.position.y, body.ankleRight.position.x, body.ankleRight.position.y) * 110;
				ofPushMatrix();
				ofTranslate(
					body.ankleRight.position.x ,
					body.ankleRight.position.y
				);
				ofRotateZ(body.ankleRight.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.ankleRight.img.width / 2),
						- (body.ankleRight.img.height / 2),
						body.ankleRight.img.width,
						body.ankleRight.img.height,
						body.ankleRight.img.source.position.x,
						body.ankleRight.img.source.position.y,
						body.ankleRight.img.source.width,
						body.ankleRight.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_HAND_LEFT) != skeleton.end()) {
				body.handLeft.angle = _angle(body.handLeft.position.x, body.handLeft.position.y, body.shoulderLeft.position.x, body.shoulderLeft.position.y) * 70;
				ofPushMatrix();
				ofTranslate(
					body.handLeft.position.x ,
					body.handLeft.position.y
				);
				ofRotateZ(body.handLeft.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.handLeft.img.width / 2),
						- (body.handLeft.img.height / 2),
						body.handLeft.img.width,
						body.handLeft.img.height,
						body.handLeft.img.source.position.x,
						body.handLeft.img.source.position.y,
						body.handLeft.img.source.width,
						body.handLeft.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_LEFT) != skeleton.end()) {
				inputImage.drawSubsection(
					body.shoulderLeft.position.x - (body.shoulderLeft.img.width / 2),
					body.shoulderLeft.position.y - (body.shoulderLeft.img.height / 2),
					body.shoulderLeft.img.width,
					body.shoulderLeft.img.height,
					body.shoulderLeft.img.source.position.x,
					body.shoulderLeft.img.source.position.y,
					body.shoulderLeft.img.source.width,
					body.shoulderLeft.img.source.height
				);
			}

			if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_RIGHT) != skeleton.end()) {
				inputImage.drawSubsection(
					body.shoulderRight.position.x - (body.shoulderRight.img.width / 2),
					body.shoulderRight.position.y - (body.shoulderRight.img.height / 2),
					body.shoulderRight.img.width,
					body.shoulderRight.img.height,
					body.shoulderRight.img.source.position.x,
					body.shoulderRight.img.source.position.y,
					body.shoulderRight.img.source.width,
					body.shoulderRight.img.source.height
				);
			}

			if(skeleton.find(NUI_SKELETON_POSITION_HAND_RIGHT) != skeleton.end()) {
				body.handRight.angle = _angle(body.shoulderLeft.position.x, body.shoulderLeft.position.y, body.handRight.position.x, body.handRight.position.y) * 70;
				ofPushMatrix();
				ofTranslate(
					body.handRight.position.x ,
					body.handRight.position.y
				);
				ofRotateZ(body.handRight.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.handRight.img.width / 2),
						- (body.handRight.img.height / 2),
						body.handRight.img.width,
						body.handRight.img.height,
						body.handRight.img.source.position.x,
						body.handRight.img.source.position.y - 5,
						body.handRight.img.source.width,
						body.handRight.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_HIP_LEFT) != skeleton.end()) {
				inputImage.drawSubsection(
					body.hipLeft.position.x - (body.hipLeft.img.width / 2),
					body.hipLeft.position.y - (body.hipLeft.img.height / 2),
					body.hipLeft.img.width,
					body.hipLeft.img.height,
					body.hipLeft.img.source.position.x,
					body.hipLeft.img.source.position.y,
					body.hipLeft.img.source.width,
					body.hipLeft.img.source.height
				);
			}

			if(skeleton.find(NUI_SKELETON_POSITION_HIP_RIGHT) != skeleton.end()) {
				inputImage.drawSubsection(
					body.hipRight.position.x - (body.hipRight.img.width / 2),
					body.hipRight.position.y - (body.hipRight.img.height / 2),
					body.hipRight.img.width,
					body.hipRight.img.height,
					body.hipRight.img.source.position.x,
					body.hipRight.img.source.position.y,
					body.hipRight.img.source.width,
					body.hipRight.img.source.height
				);
			}

			if(skeleton.find(NUI_SKELETON_POSITION_FOOT_LEFT) != skeleton.end()) {
				body.footLeft.angle =_angle(body.footLeft.position.x, body.footLeft.position.y, body.hipLeft.position.x, body.hipLeft.position.y) * 110;
				ofPushMatrix();
				ofTranslate(
					body.footLeft.position.x ,
					body.footLeft.position.y
				);
				ofRotateZ(body.footLeft.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.footLeft.img.width / 2),
						- (body.footLeft.img.height / 2),
						body.footLeft.img.width,
						body.footLeft.img.height,
						body.footLeft.img.source.position.x,
						body.footLeft.img.source.position.y - 50,
						body.footLeft.img.source.width,
						body.footLeft.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}

			if(skeleton.find(NUI_SKELETON_POSITION_FOOT_RIGHT) != skeleton.end()) {
				body.ankleRight.angle =_angle(body.hipRight.position.x, body.hipRight.position.y, body.ankleRight.position.x, body.ankleRight.position.y) * 110;
				ofPushMatrix();
				ofTranslate(
					body.ankleRight.position.x ,
					body.ankleRight.position.y
				);
				ofRotateZ(body.ankleRight.angle);
					ofPushMatrix();
					inputImage.drawSubsection(
						- (body.footRight.img.width / 2),
						- (body.footRight.img.height / 2),
						body.footRight.img.width,
						body.footRight.img.height,
						body.footRight.img.source.position.x,
						body.footRight.img.source.position.y - 50,
						body.footRight.img.source.width,
						body.footRight.img.source.height
					);
					ofPopMatrix();
				ofPopMatrix();
			}
			}
			
			//ofDisableAlphaBlending();

			/*for(auto & bone : skeleton) {
				switch(bone.second.getTrackingState()) {
				case SkeletonBone::Inferred:
					ofSetColor(0, 0, 255);
					break;

				case SkeletonBone::Tracked:
					ofSetColor(0, 255, 0);
					break;

				case SkeletonBone::NotTracked:
					ofSetColor(255, 150, 0);
					break;
				}

				//auto index = bone.second.getStartJoint();
				//auto connectedTo = skeleton.find((_NUI_SKELETON_POSITION_INDEX) index);

				//if (connectedTo != skeleton.end()) {
				//	ofLine(connectedTo->second.getScreenPosition(), bone.second.getScreenPosition());
				//}

				//ofCircle(bone.second.getScreenPosition(), 10.0f);
			}*/

			// debug
			/*
				ofVec2f point(body.elbowLeft.position.x, body.elbowLeft.position.y);
				ofVec2f pointParent(body.shoulderLeft.position.x, body.shoulderLeft.position.y);
				//float angle = point.angle(pointParent);
				//float angle = atan2(body.elbowLeft.position.x - body.elbowLeft.position.y, body.shoulderLeft.position.x - body.shoulderLeft.position.y);
				//float angle = cal_angle(body.elbowLeft.position.x, body.elbowLeft.position.y, body.shoulderLeft.position.x, body.shoulderLeft.position.y);
				ofPushStyle();
				ofSetColor(255, 0, 0);
				ofCircle(point, 10);
					ofPushMatrix();
					ofRotateZ(body.elbowLeft.angle);
					ofTranslate(body.elbowLeft.position.x + 10,  body.elbowLeft.position.y);
					ofPushStyle();
					ofSetColor(0, 0, 255);
					ofCircle(0, 0, 5);
					ofPopStyle();
					ofPopMatrix();
				ofPopStyle();
				ofPushStyle();
				ofSetColor(0, 255, 0);
				ofCircle(pointParent, 10);
				ofPopStyle();
				//
				//body.elbowLeft.angle = point.angle(pointParent); 
				ofPushStyle();
				ofSetColor(255, 0, 0);
				ofLine(point, pointParent);
				ofPopStyle();
				*/
		}
		
	ofPopStyle();
	ofPopMatrix();

	ofPushMatrix();
	ofScale(1, 1);
		foregroundImage.draw(0, 0);
		imageDance.draw(0,0);
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::digitalPinChanged(const int & pinNum) {

	if(!buttonBlue.pressed){
		
		if(ard.getDigital(buttonb)){
        	ard.sendDigital(ledb, ARD_HIGH);
		}else{
			ard.sendDigital(ledb, ARD_LOW);

			if(soundIndex > 3){
				soundIndex = 1;
			}

			if(currentState == "idle" && buttonBlue.enabled){
				buttonBlue.enabled = false;
				ard.sendDigital(ledb, ARD_LOW);
				fart.loadSound("sounds/fart" + ofToString(soundIndex) + ".mp3");
				fart.play();
				soundIndex += 1;
			}

			if(currentState == "scan" && buttonBlue.enabled){
				buttonBlue.enabled = false;
				ard.sendDigital(ledb, ARD_LOW);
				fart.loadSound("sounds/fart" + ofToString(soundIndex) + ".mp3");
				fart.play();
				soundIndex += 1;
			}

			if(currentState == "dance" && buttonBlue.enabled){
				currentState = "scan";
				ofLog() << "entered scan state";
				buttonBlue.enabled = false;
				blob.play();
			}
		}
	buttonBlue.pressed = true;
	}

	if(!buttonRed.pressed){
		if(ard.getDigital(buttonr)){
			ard.sendDigital(ledr, ARD_HIGH);
		}else{
			ard.sendDigital(ledr, ARD_LOW);
			blob.play();

				if(currentState == "idle" && buttonRed.enabled){
					currentState = "scan";
					ofLog() << "entered scan state";
					ofLog() << "scan state: drawing scanned";
					buttonRed.enabled = false;
					//
					captured = true;
				}

				if(currentState == "scan" && buttonRed.enabled){
					currentState = "dance";
					ofLog() << "entered dance state";
					buttonRed.enabled = false;
					//
					inputImage.loadData(vidGrabber.getPixels(), camWidth, camHeight, GL_RGB);
					//
					backgroundImage.loadImage("img/scene" + ofToString(selectedBackground) + "_background.jpg");
					foregroundImage.loadImage("img/scene" + ofToString(selectedBackground) + "_foreground.png");
					soundtrack.loadSound("sounds/soundtrack" + ofToString(selectedBackground) + ".wav");

					if(!soundtrackPlaying){
						soundtrack.play();
						soundtrackPlaying = true;
					}
					
					if(selectedBackground > 4){
						selectedBackground = 0;
					}
				}

				if(currentState == "dance" && buttonRed.enabled){
					currentState = "idle";
					ofLog() << "entered idle state";
					buttonRed.enabled = false;
					//
					selectedBackground += 1;
					soundtrack.stop();
					soundtrackPlaying = false;
					//recordedXML.load("record1.xml");
				}

			/*
			if(currentState == "scan" && buttonRed.enabled){
				ofLog() << "scan state: drawing scanned";
				buttonRed.enabled = false;
				//
				ofPushMatrix();
				ofRotateZ(1);
				inputImage.loadData(vidGrabber.getPixels(), camWidth, camHeight, GL_RGB);
				ofPopMatrix();
				captured = true;
			}

			if(currentState == "dance" && buttonRed.enabled){
				ofLog() << "dance state: started recording";
				buttonRed.enabled = false;
				//
				startTime = ofGetElapsedTimeMillis();  // get the start time
				recording = true;
				//
				loadXML();
				//
				recorded = true;
			}
			*/
		}
	buttonRed.pressed = true;
	}
}

void ofApp::loadXML(){
		recordedXML.setTo("SKELETON");

		if(recordedXML.getName() == "SKELETON" && recordedXML.setTo("FRAMES")){
			int numbFrames = recordedXML.getNumChildren();
			numFrames = numbFrames;
			ofXml xmlCurrentFrame;
			ofXml xmlCurrentBodypart;
			ofXml xmlCurrentProperty;
		

		for(int i = 0; i < numbFrames; i++){
		xmlCurrentFrame.setTo("FRAME[" + ofToString(i) + "]");

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("HEAD")){
				if(xmlCurrentBodypart.getName() == "HEAD" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].head.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].head.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("HAND_LEFT")){
				if(xmlCurrentBodypart.getName() == "HAND_LEFT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].handLeft.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].handLeft.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("WRIST_LEFT")){
				if(xmlCurrentBodypart.getName() == "WRIST_LEFT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].wristLeft.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].wristLeft.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("ELBOW_LEFT")){
				if(xmlCurrentBodypart.getName() == "ELBOW_LEFT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].elbowLeft.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].elbowLeft.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("SHOULDER_LEFT")){
				if(xmlCurrentBodypart.getName() == "SHOULDER_LEFT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].shoulderLeft.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].shoulderLeft.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("SHOULDER_CENTER")){
				if(xmlCurrentBodypart.getName() == "SHOULDER_CENTER" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].shoulderCenter.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].shoulderCenter.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("SHOULDER_RIGHT")){
				if(xmlCurrentBodypart.getName() == "SHOULDER_RIGHT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].shoulderRight.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].shoulderRight.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("ELBOW_RIGHT")){
				if(xmlCurrentBodypart.getName() == "ELBOW_RIGHT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].elbowRight.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].elbowRight.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("WRIST_RIGHT")){
				if(xmlCurrentBodypart.getName() == "WRIST_RIGHT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].wristRight.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].wristRight.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("HAND_RIGHT")){
				if(xmlCurrentBodypart.getName() == "HAND_RIGHT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].handRight.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].handRight.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("SPINE")){
				if(xmlCurrentBodypart.getName() == "SPINE" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].spine.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].spine.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("HIP_LEFT")){
				if(xmlCurrentBodypart.getName() == "HIP_LEFT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].hipLeft.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].hipLeft.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("HIP_RIGHT")){
				if(xmlCurrentBodypart.getName() == "HIP_RIGHT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].hipRight.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].hipRight.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("KNEE_LEFT")){
				if(xmlCurrentBodypart.getName() == "KNEE_LEFT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].kneeLeft.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].kneeLeft.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("KNEE_RIGHT")){
				if(xmlCurrentBodypart.getName() == "KNEE_RIGHT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].kneeRight.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].kneeRight.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("ANKLE_LEFT")){
				if(xmlCurrentBodypart.getName() == "ANKLE_LEFT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].ankleLeft.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].ankleLeft.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("ANKLE_RIGHT")){
				if(xmlCurrentBodypart.getName() == "ANKLE_RIGHT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].ankleRight.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].ankleRight.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("FOOT_LEFT")){
				if(xmlCurrentBodypart.getName() == "FOOT_LEFT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].footLeft.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].footLeft.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}

			if(xmlCurrentFrame.getName() == "FRAME" && xmlCurrentBodypart.setTo("FOOT_RIGHT")){
				if(xmlCurrentBodypart.getName() == "FOOT_RIGHT" && xmlCurrentProperty.setTo("POSITION")){
					recordedBodies[i].footRight.position.x = ofToFloat(xmlCurrentProperty.getValue("X"));
					recordedBodies[i].footRight.position.y = ofToFloat(xmlCurrentProperty.getValue("Y"));
				}
			}
		}
	}
}

/*
void AllFramesReady(object sender, AllFramesReadyEventArgs e){
    frames++;

    using (SkeletonFrame sFrame = e.OpenSkeletonFrameData()){

        if (sFrame == null){
            return;
		}
        skeletonFrame.CopySkeletonDataTo(skeletons);

        Skeleton skeleton = (from s in skeletons where s.TrackingState == SkeletonTrackingState.Tracked select s);
        if (skeleton == null){
			return;
		}

        if (skeleton.TrackingState == SkeletonTrackingState.Tracked){
            writer.Write("{0} {1}@", frames, timestamp);//I dont know how you want to do this
            foreach (Joint joint in skeleton.Joints){
                writer.Write(joint.Position.X + "," + joint.Position.Y + "," joint.Position.Z + ",");
            }
            writer.Write(Environment.NewLine);
        }
    }
}
*/

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

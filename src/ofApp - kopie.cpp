#include "ofApp.h"

struct Position{
	float x;
	float y;
};

struct Bodypart{
	float height;
	float width;
	Position position;
};

struct Skel{
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

//--------------------------------------------------------------
void ofApp::setup(){
	inputImage.loadImage("img/robot.png");

	outputPixels.allocate(640, 480, 4);
	outputTexture.allocate(640, 480, GL_RGBA);

	kinect.initSensor();
	kinect.initColorStream(640, 480, true);
	kinect.initDepthStream(640, 480, true);
	kinect.initSkeletonStream(false);

	kinect.start();
	ofSetWindowShape(1280 * 2, 480  * 2 * 2);
	ofDisableAlphaBlending(); //Kinect alpha channel is default 0;
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.update();
	if (kinect.isFrameNew()) {
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
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	// ------

	ofPushMatrix();
	ofScale(2, 2);
	//
	kinect.draw(0,0);
	kinect.drawDepth(640, 0);
	//outputTexture.draw(0, 480, 640, 480);
	//
	ofPushStyle();
	ofSetColor(255, 150, 0);
	//ofSetLineWidth(3.0f);
	auto skeletons = kinect.getSkeletons();

	for(auto & skeleton : skeletons) {

		ofSetColor(255);
		ofEnableAlphaBlending();

		float scaleFactor = 0.2; // 1 = 100%

		Bodypart head;
		head.height = 363;
		head.width = 375;
		head.position.x = 627;
		head.position.y = 134;

		Bodypart arm;
		arm.height = 249;
		arm.width = 1583;
		arm.position.x = 25;
		arm.position.y = 510;

		Bodypart handLeft;
		handLeft.height = arm.height;
		handLeft.width = arm.width / 8.94;
		handLeft.position.x = arm.position.x;
		handLeft.position.y = arm.position.y;

		Bodypart wristLeft;
		wristLeft.height = arm.height;
		wristLeft.width = arm.width / 22.3;
		wristLeft.position.x = handLeft.position.x + handLeft.width;
		wristLeft.position.y = arm.position.y;

		Bodypart elbowLeft;
		elbowLeft.height = arm.height;
		elbowLeft.width = arm.width / 9.2;
		elbowLeft.position.x = wristLeft.position.x + wristLeft.width;
		elbowLeft.position.y = arm.position.y;

		Bodypart shoulderLeft;
		shoulderLeft.height = arm.height;
		shoulderLeft.width = arm.width / 7.61;
		shoulderLeft.position.x = elbowLeft.position.x + elbowLeft.width;
		shoulderLeft.position.y = arm.position.y;

		Bodypart shoulderCenter;
		shoulderCenter.height = arm.height;
		shoulderCenter.width = arm.width - (2 * handLeft.width) - (2 * wristLeft.width) - (2 * elbowLeft.width) - (2 * shoulderLeft.width);
		shoulderCenter.position.x = shoulderLeft.position.x + shoulderLeft.width;
		shoulderCenter.position.y = arm.position.y;

		Bodypart shoulderRight;
		shoulderRight.height = arm.height;
		shoulderRight.width = shoulderLeft.width;
		shoulderRight.position.x = shoulderCenter.position.x + shoulderCenter.width;
		shoulderCenter.position.y = arm.position.y;

		Bodypart elbowRight;
		elbowRight.height = arm.height;
		elbowRight.width = elbowLeft.width;
		elbowRight.position.x = shoulderRight.position.x + shoulderRight.width;
		elbowRight.position.y = arm.position.y;

		Bodypart wristRight;
		wristRight.height = arm.height;
		wristRight.width = wristLeft.width;
		wristRight.position.x = elbowRight.position.x + elbowRight.width;
		wristRight.position.y = arm.position.y;

		Bodypart handRight;
		handRight.height = arm.height;
		handRight.width = handLeft.width;
		handRight.position.x = wristRight.position.x + wristRight.width;
		handRight.position.y = arm.position.y;

		Bodypart body;
		body.height = 1340;
		body.width = 560;
		body.position.x = 530;
		body.position.y = 770;

		Bodypart spine;
		spine.height = body.height / 3.1;
		spine.width = body.width;
		spine.position.x = body.position.x;
		spine.position.y = body.position.y;

		Bodypart hipLeft;
		hipLeft.height = body.height / 6.22;
		hipLeft.width = body.width / 2;
		hipLeft.position.x = body.position.x;
		hipLeft.position.y = body.position.y + spine.height;

		Bodypart hipRight;
		hipRight.height = hipLeft.height;
		hipRight.width = hipLeft.width;
		hipRight.position.x = hipLeft.position.x + hipLeft.width;
		hipRight.position.y = hipLeft.position.y;

		Bodypart kneeLeft;
		kneeLeft.height = body.height / 2.89;
		kneeLeft.width = body.width / 2;
		kneeLeft.position.x = body.position.x;
		kneeLeft.position.y = hipLeft.position.y + hipLeft.height;

		Bodypart kneeRight;
		kneeRight.height = kneeLeft.height;
		kneeRight.width = kneeLeft.width;
		kneeRight.position.x = kneeLeft.position.x + kneeLeft.width;
		kneeRight.position.y = kneeLeft.position.y;

		Bodypart ankleLeft;
		ankleLeft.height = body.height / 24.87;
		ankleLeft.width = body.width / 2;
		ankleLeft.position.x = body.position.x;
		ankleLeft.position.y = kneeLeft.position.y + kneeLeft.height;

		Bodypart ankleRight;
		ankleRight.height = ankleLeft.height;
		ankleRight.width = ankleLeft.width;
		ankleRight.position.x = ankleLeft.position.x + ankleLeft.width;
		ankleRight.position.y = ankleLeft.position.y;

		Bodypart footLeft;
		footLeft.height = body.height - spine.height - hipLeft.height - kneeLeft.height - ankleLeft.height;
		footLeft.width = body.width / 2;
		footLeft.position.x = body.position.x;
		footLeft.position.y = ankleLeft.position.y + ankleLeft.height;

		Bodypart footRight;
		footRight.height = footLeft.height;
		footRight.width = footLeft.width;
		footRight.position.x = footLeft.position.x + footLeft.width;
		footRight.position.y = footLeft.position.y;

		//-------

		Skel skel;

		if(skeleton.find(NUI_SKELETON_POSITION_HEAD) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HEAD)->second;
			skel.head.position.x = bone.getScreenPosition().x;
			skel.head.position.y = bone.getScreenPosition().y;
			//inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, head.width  * scaleFactor, head.height  * scaleFactor, head.position.x, head.position.y, head.width, head.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_HAND_LEFT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HAND_LEFT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, handLeft.width * scaleFactor, handLeft.height * scaleFactor, handLeft.position.x, handLeft.position.y, handLeft.width, handLeft.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_WRIST_LEFT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_WRIST_LEFT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, wristLeft.width * scaleFactor, wristLeft.height * scaleFactor, wristLeft.position.x, wristLeft.position.y, wristLeft.width, wristLeft.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_ELBOW_LEFT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_ELBOW_LEFT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, elbowLeft.width * scaleFactor, elbowLeft.height * scaleFactor, elbowLeft.position.x, elbowLeft.position.y, elbowLeft.width, elbowLeft.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_LEFT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_SHOULDER_LEFT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, shoulderLeft.width * scaleFactor, shoulderLeft.height * scaleFactor, shoulderLeft.position.x, shoulderLeft.position.y, shoulderLeft.width, shoulderLeft.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_CENTER) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_SHOULDER_CENTER)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, shoulderCenter.width * scaleFactor, shoulderCenter.height * scaleFactor, shoulderCenter.position.x, shoulderCenter.position.y, shoulderCenter.width, shoulderCenter.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_SHOULDER_RIGHT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_SHOULDER_RIGHT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, shoulderRight.width * scaleFactor, shoulderRight.height * scaleFactor, shoulderRight.position.x, shoulderCenter.position.y, shoulderCenter.width, shoulderCenter.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_ELBOW_RIGHT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_ELBOW_RIGHT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, elbowRight.width * scaleFactor, elbowRight.height * scaleFactor, elbowRight.position.x, elbowRight.position.y, elbowRight.width, elbowRight.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_WRIST_RIGHT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_WRIST_RIGHT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, wristRight.width * scaleFactor, wristRight.height * scaleFactor, wristRight.position.x, wristRight.position.y, wristRight.width, wristRight.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_HAND_RIGHT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HAND_RIGHT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, handRight.width * scaleFactor, handRight.height * scaleFactor, handRight.position.x, handRight.position.y, handRight.width, handRight.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_HIP_LEFT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HIP_LEFT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, hipLeft.width * scaleFactor, hipLeft.height * scaleFactor, hipLeft.position.x, hipLeft.position.y, hipLeft.width, hipLeft.height);
		}
		
		if(skeleton.find(NUI_SKELETON_POSITION_HIP_RIGHT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HIP_RIGHT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, hipRight.width * scaleFactor, hipRight.height * scaleFactor, hipRight.position.x, hipRight.position.y, hipRight.width, hipRight.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_KNEE_LEFT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_KNEE_LEFT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, kneeLeft.width * scaleFactor, kneeLeft.height * scaleFactor, kneeLeft.position.x, kneeLeft.position.y, kneeLeft.width, kneeLeft.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_KNEE_RIGHT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_KNEE_RIGHT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, kneeRight.width * scaleFactor, kneeRight.height * scaleFactor, kneeRight.position.x, kneeRight.position.y, kneeRight.width, kneeRight.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_ANKLE_LEFT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_ANKLE_LEFT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, ankleLeft.width * scaleFactor, ankleLeft.height * scaleFactor, ankleLeft.position.x, ankleLeft.position.y, ankleLeft.width, ankleLeft.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_ANKLE_RIGHT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_ANKLE_RIGHT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, ankleRight.width * scaleFactor, ankleRight.height * scaleFactor, ankleRight.position.x, ankleRight.position.y, ankleRight.width, ankleRight.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_FOOT_LEFT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_FOOT_LEFT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, footLeft.width * scaleFactor, footLeft.height * scaleFactor, footLeft.position.x, footLeft.position.y, footLeft.width, footLeft.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_FOOT_RIGHT) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_FOOT_RIGHT)->second;
			inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, footRight.width * scaleFactor, footRight.height * scaleFactor, footRight.position.x, footRight.position.y, footRight.width, footRight.height);
		}

		if(skeleton.find(NUI_SKELETON_POSITION_HEAD) != skeleton.end()) {
			SkeletonBone bone = skeleton.find(NUI_SKELETON_POSITION_HEAD)->second;
			inputImage.drawSubsection(skel.head.position.x, skel.head.position.y, head.width  * scaleFactor, head.height  * scaleFactor, head.position.x, head.position.y, head.width, head.height);
			//inputImage.drawSubsection(bone.getScreenPosition().x, bone.getScreenPosition().y, head.width  * scaleFactor, head.height  * scaleFactor, head.position.x, head.position.y, head.width, head.height);
		}
		
		ofDisableAlphaBlending();

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
	}

	ofPopStyle();
	ofPopMatrix();
}

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

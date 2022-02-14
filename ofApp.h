/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "sj_common.h"
#include "PixEncoder.h"
#include "AddEdge.h"

/**************************************************
**************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	/********************
	********************/
	ofVideoGrabber WebCam;
	
	const int Cam_id;
	const int CamWidth;
	const int CamHeight;
	const int CamFps;
	
	/********************
	********************/
	ofFbo fbo_src;
	ofFbo fbo_out;
	
	ofShader shader_Gray;
	PIX_ENCODER PixEncoder;
	
	ADD_EDGE AddEdge;
	
	/********************
	********************/
	enum STATE{
		STATE__CAM,
		STATE__VIDEO,
		STATE__PREP_VIDEO,
	};
	STATE State = STATE__CAM;
	int t_from = 0;
	
	ofVideoPlayer VideoPlayer;
	
	/********************
	********************/
	ofImage img_Screen;
	
	/****************************************
	****************************************/
	void setup_Gui();
	void setup_camera();
	void update_WebCam();
	void update_Video();
	void updateFbo_SrcToOut();
	
public:
	/****************************************
	****************************************/
	ofApp(int _Cam_id, int _CamWidth, int _CamHeight, int _CamFps);
	~ofApp();

	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};

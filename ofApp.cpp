/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _Cam_id, int _CamWidth, int _CamHeight, int _CamFps)
: Cam_id(_Cam_id)
, CamWidth(_CamWidth)
, CamHeight(_CamHeight)
, CamFps(_CamFps){
}

/******************************
******************************/
ofApp::~ofApp(){
	delete Gui_Global;
	
	if(fp_Log)	fclose(fp_Log);
}

/******************************
******************************/
void ofApp::exit(){
	printf("> Good-bye\n");
	fflush(stdout);
}


/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("PixelEncoder");
	
	ofSetWindowShape( WINDOW_WIDTH, WINDOW_HEIGHT );
	ofSetVerticalSync(true);	// trueとどっちがいいんだろう？
	ofSetFrameRate(30);
	
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_ADD
	ofEnableSmoothing();
	
	/********************
	********************/
	fp_Log = fopen("../../../data/Log.csv", "w");
	setup_Gui();
	
	/********************
	********************/
	setup_camera();
	
	fbo_src.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	ClearFbo(fbo_src);
	
	fbo_out.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	ClearFbo(fbo_out);
	
	shader_Gray.load( "shader_sj/Gray.vert", "shader_sj/Gray.frag" );
	
	/********************
	********************/
	PixEncoder.setup(fbo_src);
	AddEdge.setup();
	
	/********************
	********************/
	VideoPlayer.load("mov/mov.mp4");
	VideoPlayer.setLoopState(OF_LOOP_NORMAL);
	VideoPlayer.setSpeed(1);
	VideoPlayer.setVolume(1.0);
	// VideoPlayer.setVolume(0.0);
	VideoPlayer.setPaused(false);
	VideoPlayer.stop();
	VideoPlayer.setPosition(0); // *.movは、"OF_LOOP_NONE"の時、明示的にsetPosition(0) しないと、戻らないようだ. : seekをplay の後に移動(2020.10.08)
}

/******************************
******************************/
void ofApp::setup_camera()
{
	/********************
	********************/
	printf("> setup camera\n");
	fflush(stdout);
	
	/********************
	細かく情報出してもらう
	********************/
	ofSetLogLevel(OF_LOG_VERBOSE);
    WebCam.setVerbose(true);
	
	vector< ofVideoDevice > Devices = WebCam.listDevices();// 上 2行がないと、List表示されない.
	
	/********************
	********************/
	if(Cam_id == -1){
		ofExit();
		return;
	}else{
		if(Devices.size() <= Cam_id) { ERROR_MSG(); ofExit(); return; }
		
		WebCam.setDeviceID(Cam_id);
		printf("\n> WebCam set device id = %d\n", Cam_id);
		printf("> DeviceID     = %d\n", Devices[Cam_id].id);
		printf("> CamName      = %s\n", Devices[Cam_id].deviceName.c_str());
		printf("> HardwareName = %s\n", Devices[Cam_id].hardwareName.c_str());
		printf("> serialID     = %s\n", Devices[Cam_id].serialID.c_str());
		
		printf( "\n> Cam size asked  = (%d, %d)\n", int(CamWidth), int(CamHeight) );
		/*
			https://openframeworks.cc/documentation/video/ofVideoGrabber/#show_setDesiredFrameRate
				Set's the desired frame rate of the grabber. This should be called before initGrabber(), which will try to initialize at the desired frame rate. 
				Not all frame rates will be supported, but this at least gives you some abilitity to try grab at different rates.
		*/
		WebCam.setDesiredFrameRate(CamFps);
		// WebCam.initGrabber(CAM_WIDTH, CAM_HEIGHT);
		WebCam.setup(CamWidth, CamHeight, true); // bool ofVideoGrabber::initGrabber(int w, int h, bool setUseTexture){
		printf( "> Cam size actual = (%d, %d)\n\n", int(WebCam.getWidth()), int(WebCam.getHeight()) );
		fflush(stdout);
	}
}

/******************************
description
	memoryを確保は、app start後にしないと、
	segmentation faultになってしまった。
******************************/
void ofApp::setup_Gui()
{
	/********************
	********************/
	Gui_Global = new GUI_GLOBAL;
	Gui_Global->setup("param", "gui.xml", 10, 10);
}

/******************************
******************************/
void ofApp::update(){
	/********************
	********************/
	int now = ofGetElapsedTimeMillis();
	ofSoundUpdate();
	
	/********************
	********************/
	if(Gui_Global->Video_Button_ToZero){
		Gui_Global->Video_Button_ToZero = false;
		
		State = STATE__PREP_VIDEO;
		t_from = now;
		
		if(!VideoPlayer.isPlaying()) VideoPlayer.play();
		VideoPlayer.setPaused(true);
		VideoPlayer.setPosition(0);
	}
	
	/********************
	********************/
	bool Video_Button_StartStop = false;
	if(Gui_Global->Video_Button_StartStop){
		Gui_Global->Video_Button_StartStop = false;
		Video_Button_StartStop = true;
	}
	
	switch(State){
		case STATE__CAM:
			if(Video_Button_StartStop){
				if(VideoPlayer.isLoaded()){
					State = STATE__VIDEO;
					if(!VideoPlayer.isPlaying()) VideoPlayer.play();
					VideoPlayer.setPaused(false);
					// VideoPlayer.setPosition(0);
				}
			}
			break;
			
		case STATE__VIDEO:
			if(!VideoPlayer.isLoaded()){
				State = STATE__CAM;
			}else if(Video_Button_StartStop){
				State = STATE__CAM;
				VideoPlayer.setPaused(true);
				// VideoPlayer.stop();
			}
			break;
			
		case STATE__PREP_VIDEO:
			if(300 < now - t_from){
				State = STATE__VIDEO;
				VideoPlayer.setPaused(false);
			}
			break;
	}
	
	/********************
	********************/
	switch(State){
		case STATE__CAM:
			update_WebCam();
			break;
			
		case STATE__VIDEO:
			update_Video();
			break;
			
		case STATE__PREP_VIDEO:
			ClearFbo(fbo_src);
			break;
	}
	
	/********************
	********************/
	ClearFbo(fbo_out);
	
	if(Gui_Global->PixelEncoder__Enable)	PixEncoder.update(fbo_src, fbo_out, int(Gui_Global->PixelEncoder__xN));
	else									updateFbo_SrcToOut();
	
	if(Gui_Global->AddEdge__Enable)	AddEdge.update(fbo_src, fbo_out);
}

/******************************
******************************/
void ofApp::updateFbo_SrcToOut(){
	fbo_out.begin();
		ofDisableAlphaBlending();
		// ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_ADD
		ofDisableSmoothing();
		
		ofClear(0, 0, 0, 0);
		fbo_src.draw(0, 0, fbo_out.getWidth(), fbo_out.getHeight());
	fbo_out.end();
}

/******************************
******************************/
void ofApp::update_WebCam(){
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableSmoothing();
	
	WebCam.update();
	if(WebCam.isFrameNew()) {
		fbo_src.begin();
		ofPushMatrix();
			ofTranslate(fbo_src.getWidth(), 0);
			ofScale(-1, 1, 1);
			
			if(Gui_Global->b_Gray) shader_Gray.begin();
				ofBackground(0);
				
				ofSetColor(255);
				WebCam.draw(0, 0, fbo_src.getWidth(), fbo_src.getHeight());
			if(Gui_Global->b_Gray) shader_Gray.end();
		ofPopMatrix();
		fbo_src.end();
	}
}

/******************************
******************************/
void ofApp::update_Video(){
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableSmoothing();
	
	VideoPlayer.update();
	if(VideoPlayer.isFrameNew()) {
		fbo_src.begin();
		ofPushMatrix();
			if(Gui_Global->b_Gray) shader_Gray.begin();
				ofBackground(0);
				
				ofSetColor(255);
				VideoPlayer.draw(0, 0, fbo_src.getWidth(), fbo_src.getHeight());
			if(Gui_Global->b_Gray) shader_Gray.end();
		ofPopMatrix();
		fbo_src.end();
	}
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	ofBackground(0);
	
	/********************
	********************/
	ofDisableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_ADD
	ofDisableSmoothing();
	
	/********************
	********************/
	ofSetColor(255);
	
	fbo_out.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	/********************
	********************/
	if(Gui_Global->b_Disp){
		/********************
		********************/
		Gui_Global->gui.draw();
		
		/********************
		********************/
		{
			ofSetColor(255);
			string info;
			info += "FPS = " + ofToString(ofGetFrameRate(), 2) + "\n";
			ofDrawBitmapString(info, 30, 30);
		}
	}

}

/******************************
******************************/
void ofApp::keyPressed(int key){
	/********************
	********************/
	switch(key){
		case 'd':
			Gui_Global->b_Disp = !Gui_Global->b_Disp;
			break;
			
		case 'm':
			Gui_Global->gui.minimizeAll();
			break;
			
		case 'j':
			img_Screen.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			img_Screen.save("screenshot.jpg");
			
			printf("> saved image\n");
			fflush(stdout);
			break;
			
		case 'p':
			img_Screen.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			img_Screen.save("screenshot.png");
			
			printf("> saved image\n");
			fflush(stdout);
			break;
	}
}

/******************************
******************************/
void ofApp::keyReleased(int key){
}

/******************************
******************************/
void ofApp::mouseMoved(int x, int y ){

}

/******************************
******************************/
void ofApp::mouseDragged(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mousePressed(int x, int y, int button){
}

/******************************
******************************/
void ofApp::mouseReleased(int x, int y, int button){
}

/******************************
******************************/
void ofApp::mouseEntered(int x, int y){

}

/******************************
******************************/
void ofApp::mouseExited(int x, int y){

}

/******************************
******************************/
void ofApp::windowResized(int w, int h){

}

/******************************
******************************/
void ofApp::gotMessage(ofMessage msg){

}

/******************************
******************************/
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

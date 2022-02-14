/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
// #include "ofxCv.h"

#include "sj_common.h"

/**************************************************
**************************************************/

class PIX_ENCODER{
private:
	/****************************************
	****************************************/
	const int NumDots = 7;
	int xN = 1;
	ofFbo fbo_Low;
	
	ofShader shader_PixEncoder;
	
	/****************************************
	****************************************/
	void update_fboLow(const ofFbo& fbo_src);
	void update_fboOut(const ofFbo& fbo_src, ofFbo& fbo_out);
	void alloc_fboLow(const ofFbo& fbo_src);
	void copy_fboLow_to_fboOut(ofFbo& fbo_out);

public:
	/****************************************
	****************************************/
	PIX_ENCODER();
	~PIX_ENCODER();
	
	void setup(const ofFbo& fbo_src);
	void update(const ofFbo& fbo_src, ofFbo& fbo_out, int _xN);
};



/************************************************************
************************************************************/
#include "AddEdge.h"

/************************************************************
************************************************************/

/******************************
******************************/
ADD_EDGE::ADD_EDGE()
{
}

/******************************
******************************/
ADD_EDGE::~ADD_EDGE()
{
}

/******************************
******************************/
void ADD_EDGE::setup()
{
	/********************
	********************/
	printf("> setup : ADD_EDGE\n");
	fflush(stdout);
	
	/********************
	********************/
	b_High = Gui_Global->AddEdge__b_High;
	allocate_fbo_img();
}

/******************************
******************************/
void ADD_EDGE::allocate_fbo_img()
{
	/********************
	********************/
	if(fbo_Low.isAllocated())	fbo_Low.clear();
	if(img_Low.isAllocated())	img_Low.clear();
	if(img_Edge.isAllocated())	img_Edge.clear();
	
	/********************
	********************/
	if(b_High){
		fbo_Low.allocate(IMGPROCESS__WIDTH__HIGH, IMGPROCESS__HEIGHT__HIGH, GL_RGBA);
		ClearFbo(fbo_Low);
		
		img_Low.allocate(IMGPROCESS__WIDTH__HIGH, IMGPROCESS__HEIGHT__HIGH, OF_IMAGE_COLOR_ALPHA);
		img_Edge.allocate(IMGPROCESS__WIDTH__HIGH, IMGPROCESS__HEIGHT__HIGH, OF_IMAGE_COLOR_ALPHA);
	}else{
		fbo_Low.allocate(IMGPROCESS__WIDTH__LOW, IMGPROCESS__HEIGHT__LOW, GL_RGBA);
		ClearFbo(fbo_Low);
		
		img_Low.allocate(IMGPROCESS__WIDTH__LOW, IMGPROCESS__HEIGHT__LOW, OF_IMAGE_COLOR_ALPHA);
		img_Edge.allocate(IMGPROCESS__WIDTH__LOW, IMGPROCESS__HEIGHT__LOW, OF_IMAGE_COLOR_ALPHA);
	}
}

/******************************
******************************/
void ADD_EDGE::update(const ofFbo& fbo_src, ofFbo& fbo_out)
{
	/********************
	********************/
	if(Gui_Global->AddEdge__b_High != b_High){
		b_High = Gui_Global->AddEdge__b_High;
		allocate_fbo_img();
	}
	
	/********************
	********************/
	if(Gui_Global->AddEdge__AlignColorTo_PixEncoder){
		Gui_Global->AddEdge__AlignColorTo_PixEncoder = false;
		Gui_Global->AddEdge__color = ofColor(Gui_Global->PixelEncoder__color);
	}
	
	/********************
	********************/
	update_fboLow(fbo_src);
	
	/********************
	********************/
	fbo_Low.readToPixels(img_Low.getPixels());

	/********************
	********************/
	float th1 = min(float(Gui_Global->AddEdge__Canny_th1), float(Gui_Global->AddEdge__Canny_th2));
	float th2 = max(float(Gui_Global->AddEdge__Canny_th1), float(Gui_Global->AddEdge__Canny_th2));
	
	ofxCv::Canny(img_Low, img_Edge, th1, th2);
	img_Edge.update();
	
	/********************
	********************/
	fbo_out.begin();
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ADD); // OF_BLENDMODE_ALPHA
		ofDisableSmoothing();
		
		ofSetColor(Gui_Global->AddEdge__color);
		
		img_Edge.draw(0, 0, fbo_out.getWidth(), fbo_out.getHeight());
	fbo_out.end();
}

/******************************
******************************/
void ADD_EDGE::update_fboLow(const ofFbo& fbo_src)
{
	fbo_Low.begin();
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_ADD
		ofEnableSmoothing();
		
		ofClear(0, 0, 0, 0);
		ofSetColor(255);
		fbo_src.draw(0, 0, fbo_Low.getWidth(), fbo_Low.getHeight());
	fbo_Low.end();
}





/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include <stdio.h>

#include <ofMain.h>
#include <ofxGui.h>

/************************************************************
************************************************************/
#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);

/************************************************************
************************************************************/

enum{
	/*
	WINDOW_WIDTH	= 1280,
	WINDOW_HEIGHT	= 720,
	*/
	WINDOW_WIDTH	= 1920,
	WINDOW_HEIGHT	= 1080,
};




/************************************************************
************************************************************/

/**************************************************
**************************************************/
class GUI_GLOBAL{
private:
	/****************************************
	****************************************/
	
public:
	/****************************************
	****************************************/
	void setup(string GuiName, string FileName = "gui.xml", float x = 10, float y = 10);
	
	ofxGuiGroup Group_Video;
		ofxToggle Video_Button_StartStop;
		ofxToggle Video_Button_ToZero;
		
	ofxGuiGroup Group_misc;
		ofxToggle b_Gray;
		
	ofxGuiGroup Group_PixelEncoder;
		ofxToggle PixelEncoder__Enable;
		ofxToggle PixelEncoder__Disp_LowFbo;
		ofxFloatSlider PixelEncoder__PixelType;
		ofxFloatSlider PixelEncoder__xN;
		ofxColorSlider PixelEncoder__color;
		ofxToggle PixelEncoder__b_Boundary;
		ofxFloatSlider PixelEncoder__col_Boundary;
	
	ofxGuiGroup Group_AddEdge;
		ofxToggle AddEdge__Enable;
		ofxToggle AddEdge__b_High;
		ofxToggle AddEdge__AlignColorTo_PixEncoder;
		ofxColorSlider AddEdge__color;
		ofxFloatSlider AddEdge__Canny_th1;
		ofxFloatSlider AddEdge__Canny_th2;
		
	ofxPanel gui;
	
	bool b_Disp = false;
};

/************************************************************
************************************************************/
double LPF(double LastVal, double CurrentVal, double Alpha_dt, double dt);
double LPF(double LastVal, double CurrentVal, double Alpha);
double sj_max(double a, double b);

bool checkIf_ContentsExist(char* ret, char* buf);
void Align_StringOfData(string& s);
void print_separatoin();
void ClearFbo(ofFbo& fbo);

/************************************************************
************************************************************/
extern GUI_GLOBAL* Gui_Global;

extern FILE* fp_Log;

extern int GPIO_0;
extern int GPIO_1;


/************************************************************
************************************************************/


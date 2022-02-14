/************************************************************
************************************************************/
#include "sj_common.h"

/************************************************************
************************************************************/
/********************
********************/
int GPIO_0 = 0;
int GPIO_1 = 0;

/********************
********************/
GUI_GLOBAL* Gui_Global = NULL;

FILE* fp_Log = nullptr;


/************************************************************
func
************************************************************/
/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha_dt, double dt)
{
	double Alpha;
	if((Alpha_dt <= 0) || (Alpha_dt < dt))	Alpha = 1;
	else									Alpha = 1/Alpha_dt * dt;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha)
{
	if(Alpha < 0)		Alpha = 0;
	else if(1 < Alpha)	Alpha = 1;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double sj_max(double a, double b)
{
	if(a < b)	return b;
	else		return a;
}

/******************************
******************************/
bool checkIf_ContentsExist(char* ret, char* buf)
{
	if( (ret == NULL) || (buf == NULL)) return false;
	
	string str_Line = buf;
	Align_StringOfData(str_Line);
	vector<string> str_vals = ofSplitString(str_Line, ",");
	if( (str_vals.size() == 0) || (str_vals[0] == "") ){ // no_data or exist text but it's",,,,,,,".
		return false;
	}else{
		return true;
	}
}

/******************************
******************************/
void Align_StringOfData(string& s)
{
	size_t pos;
	while((pos = s.find_first_of(" 　\t\n\r")) != string::npos){ // 半角・全角space, \t 改行 削除
		s.erase(pos, 1);
	}
}

/******************************
******************************/
void print_separatoin()
{
	printf("---------------------------------\n");
}

/******************************
******************************/
void ClearFbo(ofFbo& fbo)
{
	fbo.begin();
		ofClear(0, 0, 0, 0);
	fbo.end();
}


/************************************************************
class
************************************************************/

/******************************
******************************/
void GUI_GLOBAL::setup(string GuiName, string FileName, float x, float y)
{
	/********************
	********************/
	gui.setup(GuiName.c_str(), FileName.c_str(), x, y);
	
	/********************
	********************/
	Group_Video.setup("Video");
		Group_Video.add(Video_Button_StartStop.setup("StartStop", false));
		Group_Video.add(Video_Button_ToZero.setup("ToZero", false));
	gui.add(&Group_Video);
	
	Group_misc.setup("misc");
		Group_misc.add(b_Gray.setup("b_Gray", true));
	gui.add(&Group_misc);
	
	Group_PixelEncoder.setup("PixelEncoder");
		Group_PixelEncoder.add(PixelEncoder__Enable.setup("Enable", true));
		Group_PixelEncoder.add(PixelEncoder__Disp_LowFbo.setup("Disp_LowFbo", false));
		Group_PixelEncoder.add(PixelEncoder__PixelType.setup("PixelType", 0.0, 0.0, 2.0));
		Group_PixelEncoder.add(PixelEncoder__xN.setup("xN", 1, 1.0, 5.0));
		{
			ofColor initColor = ofColor(255, 255, 255, 255);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_PixelEncoder.add(PixelEncoder__color.setup("color", initColor, minColor, maxColor));
		}
		Group_PixelEncoder.add(PixelEncoder__b_Boundary.setup("b_Boundary", true));
		Group_PixelEncoder.add(PixelEncoder__col_Boundary.setup("col_Boundary", 0.3, 0.0, 1.0));
	gui.add(&Group_PixelEncoder);
	
	Group_AddEdge.setup("AddEdge");
		Group_AddEdge.add(AddEdge__Enable.setup("Enable", false));
		Group_AddEdge.add(AddEdge__b_High.setup("b_High", false));
		Group_AddEdge.add(AddEdge__AlignColorTo_PixEncoder.setup("AlignColor", false));
		{
			ofColor initColor = ofColor(255, 255, 255, 255);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_AddEdge.add(AddEdge__color.setup("color", initColor, minColor, maxColor));
		}
		Group_AddEdge.add(AddEdge__Canny_th1.setup("Canny_1", 50, 1, 500));
		Group_AddEdge.add(AddEdge__Canny_th2.setup("Canny_2", 150, 1, 500));
	gui.add(&Group_AddEdge);
	
	/********************
	********************/
	gui.minimizeAll();
}


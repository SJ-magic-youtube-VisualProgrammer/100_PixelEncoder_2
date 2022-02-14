/************************************************************
************************************************************/
#include "PixEncoder.h"

/************************************************************
************************************************************/

/******************************
******************************/
PIX_ENCODER::PIX_ENCODER()
{
}

/******************************
******************************/
PIX_ENCODER::~PIX_ENCODER()
{
}

/******************************
******************************/
void PIX_ENCODER::setup(const ofFbo& fbo_src)
{
	/********************
	********************/
	printf("> setup : PIX_ENCODER\n");
	
	/********************
	********************/
	alloc_fboLow(fbo_src);
	printf("fbo_Low = (%f, %f)\n", fbo_Low.getWidth(), fbo_Low.getHeight());
	
	/********************
	********************/
	shader_PixEncoder.load( "shader_sj/PixEncoder.vert", "shader_sj/PixEncoder.frag" );
	
	/********************
	********************/
	fflush(stdout);
}

/******************************
******************************/
void PIX_ENCODER::alloc_fboLow(const ofFbo& fbo_src)
{
	if(fbo_Low.isAllocated()){
		fbo_Low.clear();
	}
	
	int w = ceil(fbo_src.getWidth() / (NumDots * xN));
	int h = ceil(fbo_src.getHeight() / (NumDots * xN));
	fbo_Low.allocate(w, h, GL_RGBA);
	ClearFbo(fbo_Low);
}

/******************************
******************************/
void PIX_ENCODER::update(const ofFbo& fbo_src, ofFbo& fbo_out, int _xN)
{
	if(_xN != xN){
		xN = _xN;
		alloc_fboLow(fbo_src);
	}
	
	update_fboLow(fbo_src);
	if(Gui_Global->PixelEncoder__Disp_LowFbo)	copy_fboLow_to_fboOut(fbo_out);
	else										update_fboOut(fbo_src, fbo_out);
}

/******************************
******************************/
void PIX_ENCODER::update_fboLow(const ofFbo& fbo_src)
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

/******************************
******************************/
void PIX_ENCODER::copy_fboLow_to_fboOut(ofFbo& fbo_out)
{
	fbo_out.begin();
		ofDisableAlphaBlending();
		// ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_ADD
		ofDisableSmoothing();
		
		ofClear(0, 0, 0, 0);
		fbo_Low.draw(0, 0, fbo_out.getWidth(), fbo_out.getHeight());
	fbo_out.end();
}

/******************************
******************************/
void PIX_ENCODER::update_fboOut(const ofFbo& fbo_src, ofFbo& fbo_out)
{
	fbo_out.begin();
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA); // OF_BLENDMODE_ADD
		ofEnableSmoothing();
		
		ofClear(0, 0, 0, 0);
		
		shader_PixEncoder.begin();
			shader_PixEncoder.setUniformTexture( "texture_1", fbo_Low.getTextureReference(), 1 ); 
			shader_PixEncoder.setUniform1i( "xN", xN );
			shader_PixEncoder.setUniform1i( "PixelType", int(Gui_Global->PixelEncoder__PixelType) );
			
			if(Gui_Global->PixelEncoder__b_Boundary)	shader_PixEncoder.setUniform1i( "b_BoundaryNoCheck", 0 );
			else										shader_PixEncoder.setUniform1i( "b_BoundaryNoCheck", 2 );
			
			shader_PixEncoder.setUniform1f( "Gray", Gui_Global->PixelEncoder__col_Boundary );
			
			ofSetColor(Gui_Global->PixelEncoder__color);
			fbo_src.draw(0, 0, fbo_out.getWidth(), fbo_out.getHeight());
		shader_PixEncoder.end();
	fbo_out.end();
}





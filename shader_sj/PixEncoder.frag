/************************************************************
■Pixel Encoder - Trailer
	https://www.youtube.com/watch?v=07iyUnv6aAs

■【GLSL(シェーダー)】でよく使う関数とユーザー関数のまとめ【計算編】※随時更新
	https://coinbaby8.com/glsl-useful-functions.html

■my note
	https://note.com/nobuhirosaijo/n/n606a3f5d8e89
	
	■条件分岐のためにstep関数を使う時の考え方をまとめてみた
		https://qiita.com/yuichiroharai/items/6e378cd128279ac9a2f0
	
	■プログラマブルシェーダとGPUについて
		http://hiyoko1986.wiki.fc2.com/wiki/%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%9E%E3%83%96%E3%83%AB%E3%82%B7%E3%82%A7%E3%83%BC%E3%83%80%E3%81%A8GPU%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6
		
	■GLSLについてのメモ
		https://qiita.com/edo_m18/items/71f6064f3355be7e4f45
	
■ビルトイン関数(一部)
	http://qiita.com/edo_m18/items/71f6064f3355be7e4f45
	
■GLSLで宣言時に配列を初期化する
	https://qiita.com/aa_debdeb/items/49f8706143b4e9e231c1
************************************************************/
#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/************************************************************
************************************************************/
bool is_equal(float val_0, float val_1);
int sj_division(float x, float y);
int sj_mod(float x, float y);

/************************************************************
************************************************************/
uniform sampler2DRect texture_0;
uniform sampler2DRect texture_1;	// Low Resolution

uniform int xN = 1;
uniform int PixelType = 0;

uniform int b_BoundaryNoCheck = 1;
uniform float Gray = 0.0;

int[147] array = int[](
	24,	48,	23,	46,	20,	42,	15,
	47,	21,	44,	18,	40,	13,	36,
	22,	43,	16,	38,	11,	34,	8,
	45,	17,	37,	9,	32,	6,	30,
	19,	39,	10,	31,	4,	28,	3,
	41,	12,	33,	5,	27,	1,	26,
	14,	35,	7,	29,	2,	25,	0,
	
	24,	47,	21,	43,	16,	37,	9,
	48,	22,	44,	17,	38,	10,	31,
	23,	45,	18,	39,	11,	32,	4,
	46,	19,	40,	12,	33,	5,	27,
	20,	41,	13,	34,	6,	28,	1,
	42,	14,	35,	7,	29,	2,	25,
	15,	36,	8,	30,	3,	26,	0,
	
	23,	46,	17,	37,	18,	47,	24,
	45,	16,	36,	6,	38,	19,	48,
	15,	35,	5,	27,	7,	39,	20,
	34,	4,	26,	0, 28,	8,	40,
	14,	33,	3,	25,	1,	29,	9,
	44,	13,	32,	2,	30,	10,	41,
	22,	43,	12,	31,	11,	42,	21
);

/************************************************************
************************************************************/

/******************************
******************************/
void main(){
	/********************
	********************/
	const int NumDots = 7;
	
	/********************
	********************/
	vec2 Coord_org = gl_TexCoord[0].xy;
	vec2 Coord_Low = Coord_org / (7 * xN);
	
	/********************
	********************/
	vec4 col_ofThisBlock = texture2DRect( texture_1, Coord_Low );
	
	float val_ofThisBlock = 0.299 * col_ofThisBlock.r + 0.587 * col_ofThisBlock.g + 0.114 * col_ofThisBlock.b;
	
	/********************
	********************/
	int NumWhites = int( floor(val_ofThisBlock * (NumDots * NumDots)) );
	
	int _y = int(sj_division(sj_mod(Coord_org.y, NumDots * xN), xN));
	int _x = int(sj_division(sj_mod(Coord_org.x, NumDots * xN), xN));
	
	int _id = _y * NumDots + _x;
	
#if 0
	vec4 color;
	if(_id == 48){
		color = vec4(1.0, 0.0, 0.0, 1.0);
	}else{
		color = vec4(0.0, 0.0, 0.0, 1.0);
	}
#else
	int _PixelType = min(PixelType, 2); // for sefety.
	int Order_OfThisPix = array[(NumDots * NumDots * _PixelType) + _id];
	
	/********************
	float val_ofThisPix;
	if(id_ofThisPix < NumWhites)	val_ofThisPix = 1.0;
	else							val_ofThisPix = 0.0;
	********************/
	float val_ofThisPix = 1.0 - step(NumWhites, Order_OfThisPix);
	
	/********************
	********************/
	int check_boundary = int( step(sj_mod(floor(Coord_org.x), NumDots * xN), 0) + step(sj_mod(floor(Coord_org.y), NumDots * xN), 0) - b_BoundaryNoCheck );
	val_ofThisPix = (1 <= check_boundary) ? Gray : val_ofThisPix;
	
	/********************
	********************/
	vec4 color = vec4(val_ofThisPix, val_ofThisPix, val_ofThisPix, 1.0);
#endif

	/********************
	********************/
	gl_FragColor = color * gl_Color;
	// gl_FragColor = color * gl_FrontColor;
}


/******************************
******************************/
bool is_equal(float val_0, float val_1){
	return (abs(val_0 - val_1) < 1e-3) ? true : false;
	// return (abs(val_0 - val_1) < 1e-8) ? true : false;
}

/******************************
******************************/
int sj_division(float x, float y){
	float delta = 1e-3;
	// float delta = 1e-7;
	
	return int (x/y + delta);
	// return int ((x + delta) / y);
}

/******************************
******************************/
int sj_mod(float x, float y){
	float delta = 1e-3;
	// float delta = 1e-7;
	
	return int(x - y * floor(x/y + delta));
	// return int( mod(x + delta, y) );
}


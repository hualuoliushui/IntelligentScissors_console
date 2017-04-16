#pragma once
#include <cv.h>
#include<opencv2/opencv.hpp>

#include "Vector12_selft.h"
#include "Log.h"

using namespace cv;

enum INTELLIGENT_SCISSORS_MODE{
	_mode0 = 0,
	_mode1 = 1
};

class IntelligentScissors{
private:
	Log log;

	INTELLIGENT_SCISSORS_MODE mode;

	Mat& img;
	Mat target;

	uchar* fZ;// Laplacian Zero-Crossing
	float* fG;// Gradient Magnitude

	Vector12_self<short>* D;// D`(p)

	float* Dlink;//D(link)

	int* paths;//记录路径
	float* ILC;

	//-------------------
	void init();
	void init_fZ();
	void init_fG_D();
	void init_ILC();

	float computefD(int px, int py, int qx, int qy);
	float computeILC(int px, int py, int qx, int qy);

	//helper
	uchar get_target_pixel(int x, int y){
		if (x < 0) x = 0;
		if (x >= target.cols) x = target.cols - 1;
		if (y < 0) y = 0;
		if (y >= target.rows) y = target.rows - 1;

		return target.data[y*target.cols + x];
	}

public:
	IntelligentScissors(Mat& image, INTELLIGENT_SCISSORS_MODE _mode=INTELLIGENT_SCISSORS_MODE::_mode1);

	~IntelligentScissors();

	//寻路函数
	void DP(int sx, int sy);
	//鼠标回调事件
	void MouseCallbackFunc(int m_event, int x, int y,int flags, void *param);
};

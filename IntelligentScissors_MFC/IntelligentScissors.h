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

typedef struct{
	float* pDP_percent;
}DPPercent;

class IntelligentScissors{
private:
	Log log;

	INTELLIGENT_SCISSORS_MODE mode;

	Mat& img;
	Mat target;
	Mat temp;

	uchar* fZ;// Laplacian Zero-Crossing
	float* fG;// Gradient Magnitude

	Vector12_self<short>* D;// D`(p)

	float* Dlink;//D(link)

	vector<int> coldPaths;//最终路径
	int* paths;//记录路径
	float* ILC;//记录局部代价

	float DP_percent;//记录DP函数的进度
	bool isFirst;//记录鼠标回调函数调用DP
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

	int* getPaths(){
		return paths;
	}

	Mat getTemp(){
		return temp;
	}

	Mat getResult(){
		temp = img.clone();

		int cols = temp.cols;
		for (int i = 0; i < coldPaths.size(); i++){
			int tempx = coldPaths[i] % cols;
			int tempy = coldPaths[i] / cols;
			temp.data[tempy*cols * 3 + tempx * 3] = 0;
			temp.data[tempy*cols * 3 + tempx * 3 + 1] = 0;
			temp.data[tempy*cols * 3 + tempx * 3 + 2] = 255;
		}
		return temp;
	}
	//寻路函数
	void DP(int sx, int sy);
	//鼠标回调事件
	void MouseCallbackFunc(int m_event, int x, int y,int flags, void *param);
};

#pragma once
#include "IntelligentScissors.h"
#include "MinHeap_self.h"

#include <iostream>
#include <iomanip>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define DEBUG_SELF

#define PI 3.1415946535898f
#define PI_1 (1.0f/PI)
#define sqrt2 1.414213538f

#define WZ 0.43f
#define WD 0.43f
#define WG 0.14f

class Compare_PixelCost{
public:
	static float* g;
	static bool prior(int left, int right){
		return g[left] < g[right];
	}
};
float* Compare_PixelCost::g = nullptr;

IntelligentScissors::IntelligentScissors(Mat& image, INTELLIGENT_SCISSORS_MODE _mode)
	:img(image),
	fZ(nullptr), fG(nullptr), D(nullptr),
	Dlink(nullptr),
	paths(nullptr), ILC(nullptr){
	mode = _mode;
	init();
}

IntelligentScissors::~IntelligentScissors(){
	if (fZ != nullptr) delete[] fZ;
	if (fG != nullptr) delete[] fG;
	if (D != nullptr) delete[] D;

	if (Dlink != nullptr) delete[]Dlink;

	if (paths != nullptr) delete[] paths;
	if (ILC != nullptr) delete[] ILC;
}

void IntelligentScissors::DP(int sx, int sy){
	int cols = target.cols;
	int index = sy*cols + sx;

	float* g = new float[target.rows*target.cols]();
	bool* e = new bool[target.rows*target.cols]();

	Compare_PixelCost::g = g;

	int Noffset[16];
	Noffset[0] = -1;//y
	Noffset[1] = -1;//x
	Noffset[2] = -1;
	Noffset[3] = 0;
	Noffset[4] = -1;
	Noffset[5] = 1;
	Noffset[6] = 0;
	Noffset[7] = -1;
	Noffset[8] = 0;//y
	Noffset[9] = 1;//x
	Noffset[10] = 1;
	Noffset[11] = -1;
	Noffset[12] = 1;
	Noffset[13] = 0;
	Noffset[14] = 1;
	Noffset[15] = 1;

	MinHeap_self<int, Compare_PixelCost> heap;

	log.init_time();
	log.cout << "DP 计算:\n";

	g[index] = 0.0f;
	heap.insert(index);

	int num = 1;
	int size = target.rows*target.cols;

	while (!heap.empty()){		
		int q = heap.removefirst();
		e[q] = true;

		num++;
		if (num%target.cols == 0){
			cout << "\r" << setfill(' ') << setw(10) << setiosflags(ios::right) << setprecision(4) << 
				(1.0*num / size*100.0) << "%";
		}

		int x = q%cols, y = q / cols;
		for (int i = 0; i < 8; i++){
			int ry = y + Noffset[2 * i];
			int rx = x + Noffset[2 * i + 1];
			int r = ry*cols + rx;
			if (rx < 0 || ry < 0 || rx >= target.cols || ry >= target.rows
				|| e[r]){
				continue;
			}

			float gtemp = g[q] + ILC[y*cols * 8 + x * 8 + i];
			if (heap.isContains(r) && gtemp < g[r]){
				heap.remove_value(r);
			}
			if (!heap.isContains(r)){
				g[r] = gtemp;
				paths[r] = q;
				heap.insert(r);
			}			
		}
	}

	log.end_time();

	delete[]g;
	delete[]e;
}

void IntelligentScissors::MouseCallbackFunc(int m_event, int x, int y, int flags,void* param){
	const char* name = "working";

	static bool isFirst = true;
	static Mat temp = img.clone();
	static int px = 0, py = 0;
	static vector<int> coldPaths;

	switch (m_event){
	case CV_EVENT_MOUSEMOVE:
		if (!isFirst){
			cout << "\r(" << setfill(' ') << setw(5) << setiosflags(ios::right) <<
				x 
				<< "," << setfill(' ') << setw(5) << setiosflags(ios::right) << 
				y 
				<< ")";

			temp = img.clone();

			int cols = temp.cols;
			for (int i = 0; i < coldPaths.size(); i++){
				int tempx = coldPaths[i] % cols;
				int tempy = coldPaths[i] / cols;
				temp.data[tempy*cols * 3 + tempx * 3] = 0;
				temp.data[tempy*cols * 3 + tempx * 3 + 1] = 0;
				temp.data[tempy*cols * 3 + tempx * 3 + 2] = 255;
			}

			int free = y*temp.cols + x, seed = py*temp.cols + px;
			if (free != seed && free>=0 && free < temp.rows*temp.cols){
				int tempx, tempy;
				while (paths[free] != seed){
					tempx = free%cols, tempy = free / cols;
					temp.data[tempy*cols * 3 + tempx * 3] = 0;
					temp.data[tempy*cols * 3 + tempx * 3 + 1] = 0;
					temp.data[tempy*cols * 3 + tempx * 3 + 2] = 255;
					free = paths[free];
				}
			}
		}
		break;
	case CV_EVENT_LBUTTONDOWN:

		temp.data[y*temp.cols * 3 + x * 3] = 0;
		temp.data[y*temp.cols * 3 + x * 3 + 1] = 0;
		temp.data[y*temp.cols * 3 + x * 3 + 2] = 255;

		cout << "选定点 " << x << " " << y << endl;
		if (isFirst){
			cv::imshow(name, img);
			temp = img.clone();
			coldPaths.clear();
			px = x;
			py = y;
			cout << "正在预处理，请稍后..." << endl;
			DP(px, py);
			isFirst = false;//首次计算所有的路径
			cout << "预处理结束！" << endl;
		}
		else{
			if (px == x&&py == y) return;
			int cols = temp.cols;
			int free = y*cols + x;
			int seed = py*cols + px;
			int tempx, tempy;

			while (paths[free] != seed){
				tempx = free%cols, tempy = free / cols;
				temp.data[tempy*cols * 3 + tempx * 3] = 0;
				temp.data[tempy*cols * 3 + tempx * 3 + 1] = 0;
				temp.data[tempy*cols * 3 + tempx * 3 + 2] = 255;
				coldPaths.push_back(free);
				free = paths[free];
			}

			px = x, py = y;

			cout << "正在重新定位点，请稍后...\n";
			DP(x, y);//重新计算种子点
			cout << "结束，请继续选择下一个种子点\n";
		}
		break;
	case CV_EVENT_RBUTTONDOWN:
		
		break;
	default:
		break;
	}
	cv::imshow(name, temp);
}

void IntelligentScissors::init(){
	//灰度图
	if (img.channels() != 1){
		cvtColor(img, target, CV_BGR2GRAY);
		cv::imshow("灰度图", target);
	}
	else{
		target = img;
	}

	log.cout << "图片规格: " << img.rows << "x" << img.cols << "\n";
	
	//使用灰度图
	switch (mode)
	{
	case _mode0:
		log.init_time();
		log.cout << "原始论文，初始化\n";

		if (fZ != nullptr) delete[] fZ;
		fZ = new uchar[target.rows*target.cols]();

		if (fG != nullptr) delete[] fG;
		fG = new float[target.rows*target.cols]();

		if (D != nullptr) delete[] D;
		D = new Vector12_self<short>[target.rows*target.cols]();

		init_fZ();
		init_fG_D();

		break;
	case _mode1:
		log.init_time();
		log.cout << "改进代价函数，初始化\n";

		if (Dlink != nullptr) delete[] Dlink;
		Dlink = new float[target.rows*target.cols * 8]();
		break;
	default:
		break;
	}

	if (paths != nullptr) delete[] paths;
	paths = new int[target.rows*target.cols]();

	if (ILC != nullptr) delete[] ILC;
	ILC = new float[target.rows*target.cols * 8]();
	init_ILC();
	log.end_time();
}

void IntelligentScissors::init_fZ(){
	Mat temp(target.rows, target.cols, CV_8UC1);
	for (int i = 0; i < target.rows; i++){
		for (int j = 0; j < target.cols; j++){
			int index = i*target.cols + j;
			int pixel = (int)get_target_pixel(j + 1, i) +
				(int)get_target_pixel(j, i - 1) +
				(int)get_target_pixel(j - 1, i) +
				(int)get_target_pixel(j, i + 1) -
				(int)get_target_pixel(j, i) * 4;
			if (pixel < 0) pixel = 0;
			if (pixel > 255) pixel = 255;
			if (pixel <= 0){
				fZ[index] = 0;
				temp.data[i*target.cols + j] = 0;
			}
			else{
				fZ[index] = 1;
				temp.data[i*target.cols + j] = pixel;
			}
		}
	}

	cv::imshow("temp", temp);
}

void IntelligentScissors::init_fG_D(){
	float max_val = 0.0f;

	//cout << "D:\n";
	for (int i = 0; i < target.rows; i++){
		for (int j = 0; j < target.cols; j++){
			int index = i*target.cols + j;
			D[index].x(get_target_pixel(j, i + 1) - get_target_pixel(j, i));//Iy
			D[index].y(get_target_pixel(j, i) - get_target_pixel(j + 1, i));//-Ix
			fG[index] = D[index].getLength();

			//归一化 D
			D[index] /= fG[index];
			//cout << "(" << D[index].x() << "," << D[index].y() << ") ";

			if (fG[index] > max_val) max_val = fG[index];
		}
		//cout << "\n";
	}
	//cout << "\n";

	//cout << "fG:\n";
	for (int i = 0; i < target.rows; i++){
		for (int j = 0; j < target.cols; j++){
			int index = i*target.cols + j;
			fG[index] = 1 - fG[index] / max_val;
			//cout << fG[index] << " ";
		}
		//cout << "\n";
	}
}

void IntelligentScissors::init_ILC(){
	int cols = target.cols;
	int Noffset[16] = {
		-1, -1, // dy,dx
		-1, 0,
		-1, 1,
		0, -1,
		0, 1,
		1, -1,
		1, 0,
		1, 1
	};
	switch (mode)
	{
	case _mode0:
	{
		for (int i = 0; i < target.rows; i++){
			for (int j = 0; j < target.cols; j++){	
				for (int k = 0; k < 8; k++){
					int index = i*cols * 8 + j * 8 + k;
					int py = i + Noffset[2 * k];
					int px = j + Noffset[2 * k + 1];
					if (py < 0 || px<0 || py >= target.rows || px >= target.cols)
						continue;
					ILC[index] = computeILC(j, i, px, py);
				}
			}
		}
	}
		break;
	case _mode1:
	{
		float max_value = 0.0f;
		for (int y = 0; y < target.rows; y++){
			for (int x = 0; x < target.cols; x++){
				for (int k = 0; k < 8; k++){
					int index = y*cols * 8 + x * 8 + k;
					int py = y + Noffset[2 * k];
					int px = x + Noffset[2 * k + 1];
					
					if (y != py && x != px){
						Dlink[index] = abs(get_target_pixel(x,py) - get_target_pixel(px,y)) / sqrt2;
					}
					else if (y==py){
						Dlink[index] = abs(
							(get_target_pixel(x, y - 1) + get_target_pixel(px, y - 1)) / 2
							-
							(get_target_pixel(x, y + 1) + get_target_pixel(px, y + 1)) / 2
							) / 2;
					}
					else if (x==px){
						Dlink[index] = abs(
							(get_target_pixel(x - 1, py) + get_target_pixel(x - 1, y)) / 2
							-
							(get_target_pixel(x + 1, py) + get_target_pixel(x + 1, y)) / 2
							) / 2;
					}
					if (Dlink[index] > max_value) max_value = Dlink[index];
				}
			}
		}
		for (int y = 0; y < target.rows; y++){
			for (int x = 0; x < target.cols; x++){
				for (int k = 0; k < 8; k++){
					int index = y*cols * 8 + x * 8 + k;
					int py = y + Noffset[2 * k];
					int px = x + Noffset[2 * k + 1];
					if (py < 0 || px<0 || py >= target.rows || px >= target.cols)
						continue;
					if (py != y && px!=x){
						ILC[index] = (max_value - Dlink[index])*sqrt2;
					}
					else{
						ILC[index] = (max_value - Dlink[index]) * 1;
					}
				}
			}
		}
	}
		break;
	default:
		break;
	}
}

float IntelligentScissors::computefD(int px, int py, int qx, int qy){
	int pIndex = py*target.cols + px;
	int qIndex = qy*target.cols + qx;
	//q-p
	Vector12_self<int> qp(qx - px, qy - py);
	//L
	Vector12_self<float> L;
	if (qp.dot(D[pIndex]) >= 0){
		L.x(qp.x());
		L.y(qp.y());
	}
	else{
		L.x(-qp.x());
		L.y(-qp.y());
	}

	float dp = L.dot(D[pIndex]);
	float dq = L.dot(D[qIndex]);

	return PI_1*(acos(dp) + acos(dq));
}

float IntelligentScissors::computeILC(int px, int py, int qx, int qy){
	int pIndex = py*target.cols + px;
	int qIndex = qy*target.cols + qx;

	float fg = fG[qIndex];
	if (qx == px || qy == py) fg /= sqrt2;
	
	return (WZ*fZ[qIndex]);// +WG*fg + WD*computefD(px, py, qx, qy));
}
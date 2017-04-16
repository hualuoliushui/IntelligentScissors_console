#include <highgui.h>
#include <cv.h>
#include <iostream>
#include <vector>
#include "IntelligentScissors.h"

using namespace std;
using namespace cv;

#define Const(a,b) (int)(a##e##b)

void MouseCallbackFunc(int m_event, int x, int y, int flags, void *param){
	IntelligentScissors* inte_p = (IntelligentScissors*)param;
	inte_p->MouseCallbackFunc(m_event, x, y, flags, NULL);
}

int main(){
	Mat src = cvLoadImage("D:\\opencv_workspace\\picture\\羊.jpg");
	if (!src.data){
		cout << "图片载入出错\n请检查路径是否正确\n";
		return -1;
	}
	IntelligentScissors inte(src,INTELLIGENT_SCISSORS_MODE::_mode1);

	const char* name = "working";
	namedWindow(name, WINDOW_AUTOSIZE);
	setMouseCallback(name, MouseCallbackFunc, &inte);
	imshow(name, src);
	waitKey();

	cvDestroyAllWindows();

	return 0;
}
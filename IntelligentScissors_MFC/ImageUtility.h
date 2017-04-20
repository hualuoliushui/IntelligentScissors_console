#pragma once
#ifndef IMAGEUTILTIY
#define IMAGEUTILTIY
#include <cv.h>
#include "stdafx.h"

using namespace cv;

class  ImageUtility
{
public:
	//Mat转化成CImage
	static void MatToCImage(Mat *mat, ATL::CImage **cImage){
		int width = mat->cols;
		int height = mat->rows;
		int channels = mat->channels();

		if (*cImage){
			(*cImage)->Destroy();
			*cImage = NULL;
		}
		*cImage = new ATL::CImage();
		(*cImage)->Create(width,
			height,
			8 * channels);

		uchar* ps;
		uchar* pimg = (uchar*)(*cImage)->GetBits();
		int step = (*cImage)->GetPitch();

		for (int i = 0; i < height; ++i)
		{
			ps = (mat->ptr<uchar>(i));
			for (int j = 0; j < width; ++j)
			{
				if (channels == 1) //gray  
				{
					*(pimg + i*step + j) = ps[j];
				}
				else if (channels == 3) //color  
				{
					for (int k = 0; k < 3; ++k)
					{
						*(pimg + i*step + j * 3 + k) = ps[j * 3 + k];
					}
				}
			}
		}
	};

	//将Mat图像缩放到适合的大小
	static cv::Mat* Mat_Resize(cv::Mat* mat_srcP, int nWidth, int nHeight){

		if (!mat_srcP){
			return NULL;
		}

		int cols = mat_srcP->cols;
		int rows = mat_srcP->rows;

		int fixed_width = min(cols, nWidth);
		int fixed_height = min(rows, nHeight);

		double ratio_w = fixed_width / (double)cols;
		double ratio_h = fixed_height / (double)rows;
		double ratio = min(ratio_w, ratio_h);

		int show_width = (int)(ratio * cols);
		int show_height = (int)(ratio * rows);
		int offsetx = (nWidth - show_width) / 2;
		int offsety = (nHeight - show_height) / 2;

		Size dsize = Size(ratio*cols, ratio*rows);

		Mat* mat_drtP = new Mat(dsize, CV_32S);
		resize(*mat_srcP, *mat_drtP, dsize);
		return mat_drtP;
	};
};

#endif

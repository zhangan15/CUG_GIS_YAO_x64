#include <QtCore>
#include <iostream>
#include "gdal_priv.h"
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include "GDALRead.h"

using namespace std;
using namespace dlib;

//Fitter for simulation
using net_type = loss_mean_squared<
	fc<1,
	dropout<relu<fc<128,
	dropout<relu<con<32, 3, 3, 1, 1,
	max_pool<2, 2, 2, 2, relu<con<32, 3, 3, 1, 1,
	max_pool<2, 2, 2, 2, relu<con<16, 3, 3, 1, 1,
	input<matrix<rgb_pixel>>
	>>>>>>>>>>>>>>;


struct training_sample
{
	matrix<rgb_pixel> input_image;
	double dHousingPrice; // The ground-truth label of each pixel.
};

// obtain image block from HSR RS image
bool obtainImageFromHSR(CGDALRead* pInImg, double dLon, double dLat, int nWidth, int nHeight, matrix<rgb_pixel>& input_image)
{
	if (pInImg == NULL)
		return false;

	//读入数据


	//处理
	input_image.set_size(nWidth, nHeight);
	

	return true;
}

int main(int argc, char *argv[])
{
	//load and check GDAL
	if (GDALGetDriverCount() == 0)
	{
		GDALAllRegister();
		OGRRegisterAll();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//支持中文路径;
		CPLSetConfigOption("SHAPE_ENCODING", "");			//支持属性表中的中文字符;
		cout << "[Init]  environment initialization success!\n";
	}


	//读入房价数据

	//读入遥感影像

	//生成训练数据

	//构建训练数据集

	//构建逐步训练


	//输出训练文件


	//输出模拟房价文件



	return 0;
}

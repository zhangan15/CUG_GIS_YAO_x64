#include <QtCore>
#include <iostream>
#include "gdal_priv.h"
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include "GDALRead.h"

using namespace std;
using namespace dlib;

#define INPUT_DATATYPE GDT_Byte
#define MAX_RECT_SIZE 100
#define RAND_RECT_SIZE 50

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
	if (pInImg == NULL || pInImg->datatype() != INPUT_DATATYPE || pInImg->bandnum() != 3)
	{
		cout << "Input image format error." << endl;
		return false;
	}

	double curCol, curRow;
	pInImg->world2Pixel(dLat, dLon, &curCol, &curRow);

	//��������
	int minCol, minRow, maxCol, maxRow;
	minCol = int(curCol - nHeight/2 + 0.5);
	maxCol = int(curCol + nHeight / 2 + 0.5);
	minRow = int(curRow - nWidth / 2 + 0.5);
	maxRow = int(curRow + nWidth / 2 + 0.5);

	if (minCol < 0 || maxCol > pInImg->cols() || minRow < 0 || maxRow > pInImg->rows())
		return false;
	
	//����
	input_image.set_size(nWidth, nHeight);

	int nCol, nRow;
	for (nRow = minRow; nRow < maxRow; nRow++)
	{
		for (nCol = minCol; nCol < maxCol; nCol++)
		{
			input_image(nRow-minRow, nCol-minCol).red = *(unsigned char*) pInImg->read(nRow, nCol, 0);
			input_image(nRow - minRow, nCol - minCol).green = *(unsigned char*) pInImg->read(nRow, nCol, 1);
			input_image(nRow - minRow, nCol - minCol).blue = *(unsigned char*) pInImg->read(nRow, nCol, 2);
		}
	}
	

	return true;
}

int main(int argc, char *argv[])
{
	//load and check GDAL
	if (GDALGetDriverCount() == 0)
	{
		GDALAllRegister();
		OGRRegisterAll();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//֧������·��;
		CPLSetConfigOption("SHAPE_ENCODING", "");			//֧�����Ա��е������ַ�;
		cout << "[Init]  environment initialization success!\n";
	}


	//���뷿������

	//����ң��Ӱ��

	//����ѵ������
	std::vector<training_sample> training_samples;

	//����ѵ�����ݼ�

	//������ѵ��


	//���ѵ���ļ�


	//���ģ�ⷿ���ļ�



	return 0;
}

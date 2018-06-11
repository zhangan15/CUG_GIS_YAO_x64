#ifndef UMCNN_PUBLIC_FUNCTIONS_H_FILE
#define UMCNN_PUBLIC_FUNCTIONS_H_FILE


#include "GDALRead.h"
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <iostream>

using namespace std;
using namespace dlib;

#define INPUT_DATATYPE GDT_Byte
#define OUTPUT_DATATYPE GDT_Float32
#define MAX_RECT_SIZE 120
#define RAND_RECT_SIZE 60

struct hp_sample
{
	matrix<rgb_pixel> input_image;
	double dPrice; // The ground-truth label of each pixel.
};

struct hp_data
{
	double dlat, dlon;
	double dPrice;
};

bool ObtainRectImageFromHsrByLatLon(CGDALRead* pInImg, double dLon, double dLat, int nWidth, int nHeight, matrix<rgb_pixel>& input_image);
bool ObtainRectImageFromHSR(CGDALRead* pInImg, int CurRow, int CurCol, int nWidth, int nHeight, matrix<rgb_pixel>& input_image);
bool LoadHpDataFromCSV(char* fn, std::vector<hp_data>& vHpData, double dMinVal = 1000, double dMaxVal = 50000);
bool ChangeHpData2CnnSamples(CGDALRead* pInImg, std::vector<hp_data> vHpData, int nWidth, int nHeight, std::vector<hp_sample>& vSamples);
void RandomlyCropImage(const matrix<rgb_pixel>& input_image, matrix<rgb_pixel>& output_image, dlib::rand& rnd, int nCropWidth, int nCropHeight);
void CenterCropImage(const matrix<rgb_pixel>& input_image, matrix<rgb_pixel>& output_image, int nCropWidth, int nCropHeight);
void CenterRandomlyCropImage(const matrix<rgb_pixel>& input_image, matrix<rgb_pixel>& output_image, dlib::rand& rnd, int nCropWidth, int nCropHeight);

#endif

#ifndef GEO2VEC_COMMON_FUNCTIONS_H_FILE
#define GEO2VEC_COMMON_FUNCTIONS_H_FILE

//#include "GDALRead.h"
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <iostream>
#include <QtCore>

using namespace dlib;
using namespace std;

#define LAYER_NUM 8

// 该函数用于读取每个位置只生成一张图片的文件夹，旋转和位移在读入后完成
// 文件命名格式为 GeoHash_LayerID.jpg
// 从文件夹中读取文件名列表，存储在哈希表中
bool load_objects_list(const char* dir, QHash<QString, std::vector<std::string>>& hash_objects);

// 当图片较多时每次遍历一次文件夹需要时间太长，因此可以事先将所有的文件名写入到 object_list 文件中
bool load_objects_list(const char* dir, const char* sObjectListFile, QHash<QString, std::vector<std::string>>& hash_objects);


// 该函数用于读取每个位置随机生成了多张图片的文件夹，旋转和位移在生成图片的时候已经生成完毕，即Loc2Vec中的做法
// 文件命名格式为 GeoHash_ImageID_LayerID.jpg
// 从文件夹中读取文件名列表，存储在哈希表中
bool load_objects_list(const char* dir, QHash<QString, std::vector<std::vector<std::string>>>& hash_objects);

// 当图片较多时每次遍历一次文件夹需要时间太长，因此可以事先将所有的文件名写入到 object_list 文件中
bool load_objects_list(const char* dir, const char* sObjectListFile, QHash<QString, std::vector<std::vector<std::string>>>& hash_objects);


// 根据传入的文件名读取图片，用于预测
bool load_predict_images(
	std::vector<std::vector<std::string>> svvImagesPath, 
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	int nCropHeight, int nCropWidth
);


// 用于读取标签，该函数在每个位置只生成一张图片时有效，文件格式为第一行是文件头，然后每一行是 GeoHash,Label 
bool load_lables(const char* sLabelFile, QHash<QString, std::vector<std::string>> hash_objects, std::vector<std::vector<QString>>& svvLables);

// 用于读取标签，该函数在每个位置生成多张图片时有效(loc2vec)，文件格式为第一行是文件头，然后每一行是 GeoHash,Label 
bool load_lables(const char* sLabelFile, QHash<QString, std::vector<std::vector<std::string>>> hash_objects, std::vector<std::vector<QString>>& svvLables);

// 数据载入函数，用于多线程读入图像到 qimages 里，在每个位置只生成一张图片时有效
void data_loader_one(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd, bool bIsOneLabelMultiLoc,
	dlib::pipe<std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>>& qimages,
	dlib::pipe<std::vector<unsigned long>>& qlabels,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables);

// 数据载入函数，用于多线程读入图像到 qimages 里，在每个位置生成多张图片时有效(loc2vec)
void data_loader_loc2vec(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd, bool bIsOneLabelMultiLoc,
	dlib::pipe<std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>>& qimages,
	dlib::pipe<std::vector<unsigned long>>& qlabels,
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects,
	std::vector<std::vector<QString>> svvLables);


// 该函数在每个位置生成多张图片有效，即loc2vec中的方式
// 每一次在一类label中只选一个位置的图片
bool load_mini_batch_one_label_one_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels);

// 该函数在每个位置生成多张图片有效，即loc2vec中的方式
// 每一次在一类label中随机选择属于该label的位置的图片
bool load_mini_batch_one_label_multi_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels);


// 该函数在每个位置只生成一张图片有效
// 每一次在一类label中只选一个位置，然后进行随机裁剪和旋转
bool load_mini_batch_one_label_one_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels);

// 该函数在每个位置只生成一张图片有效
// 每一次在一类label中随机选择属于该label的位置，进行随机裁剪和旋转
bool load_mini_batch_one_label_multi_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels);


// 只进行位移，不旋转，输入为多层的矩阵
void RandomlyCropImage(
	const std::array<matrix<unsigned char>, LAYER_NUM>& input_image, 
	std::array<matrix<unsigned char>, LAYER_NUM>& output_image, 
	dlib::rand& rnd, int nCropWidth, int nCropHeight);


// 只进行位移，不旋转，输入为RGB的矩阵
void RandomlyCropImage(
	const matrix<rgb_pixel>& input_image, 
	matrix<rgb_pixel>& output_image, 
	dlib::rand& rnd, int nCropWidth, int nCropHeight);


// 图像中心位置不变，只进行旋转，不进行位移，输入为多层的矩阵
void CenterRandomlyCropImage(
	const std::array<matrix<unsigned char>, LAYER_NUM>& input_image, 
	std::array<matrix<unsigned char>, LAYER_NUM>& output_image, 
	dlib::rand& rnd, int nCropWidth, int nCropHeight);


// 图像中心位置不变，只进行旋转，不进行位移，输入为RGB的矩阵
void CenterRandomlyCropImage(
	const matrix<rgb_pixel>& input_image, 
	matrix<rgb_pixel>& output_image, 
	dlib::rand& rnd, int nCropWidth, int nCropHeight);


// 对图像周边填充0
void PaddingZero(
	const matrix<unsigned char>& input_image,
	matrix<unsigned char>& output_image,
	int nCropWidth, int nCropHeight
);

// 裁剪到特定大小的图像
void ClipImage(
	const matrix<unsigned char>& input_image,
	matrix<unsigned char>& output_image,
	int nCropWidth, int nCropHeight
);

// 将图像插值到特定大小
void InterpolateImage(
	const matrix<unsigned char>& input_image,
	matrix<unsigned char>& output_image,
	int nCropWidth, int nCropHeight
);

void InterpolateImage(
	const matrix<rgb_pixel>& input_image,
	matrix<rgb_pixel>& output_image,
	int nCropWidth, int nCropHeight
);

#endif

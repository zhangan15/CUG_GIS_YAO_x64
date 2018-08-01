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

// �ú������ڶ�ȡÿ��λ��ֻ����һ��ͼƬ���ļ��У���ת��λ���ڶ�������
// �ļ�������ʽΪ GeoHash_LayerID.jpg
// ���ļ����ж�ȡ�ļ����б��洢�ڹ�ϣ����
bool load_objects_list(const char* dir, QHash<QString, std::vector<std::string>>& hash_objects);

// ��ͼƬ�϶�ʱÿ�α���һ���ļ�����Ҫʱ��̫������˿������Ƚ����е��ļ���д�뵽 object_list �ļ���
bool load_objects_list(const char* dir, const char* sObjectListFile, QHash<QString, std::vector<std::string>>& hash_objects);


// �ú������ڶ�ȡÿ��λ����������˶���ͼƬ���ļ��У���ת��λ��������ͼƬ��ʱ���Ѿ�������ϣ���Loc2Vec�е�����
// �ļ�������ʽΪ GeoHash_ImageID_LayerID.jpg
// ���ļ����ж�ȡ�ļ����б��洢�ڹ�ϣ����
bool load_objects_list(const char* dir, QHash<QString, std::vector<std::vector<std::string>>>& hash_objects);

// ��ͼƬ�϶�ʱÿ�α���һ���ļ�����Ҫʱ��̫������˿������Ƚ����е��ļ���д�뵽 object_list �ļ���
bool load_objects_list(const char* dir, const char* sObjectListFile, QHash<QString, std::vector<std::vector<std::string>>>& hash_objects);


// ���ݴ�����ļ�����ȡͼƬ������Ԥ��
bool load_predict_images(
	std::vector<std::vector<std::string>> svvImagesPath, 
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	int nCropHeight, int nCropWidth
);


// ���ڶ�ȡ��ǩ���ú�����ÿ��λ��ֻ����һ��ͼƬʱ��Ч���ļ���ʽΪ��һ�����ļ�ͷ��Ȼ��ÿһ���� GeoHash,Label 
bool load_lables(const char* sLabelFile, QHash<QString, std::vector<std::string>> hash_objects, std::vector<std::vector<QString>>& svvLables);

// ���ڶ�ȡ��ǩ���ú�����ÿ��λ�����ɶ���ͼƬʱ��Ч(loc2vec)���ļ���ʽΪ��һ�����ļ�ͷ��Ȼ��ÿһ���� GeoHash,Label 
bool load_lables(const char* sLabelFile, QHash<QString, std::vector<std::vector<std::string>>> hash_objects, std::vector<std::vector<QString>>& svvLables);

// �������뺯�������ڶ��̶߳���ͼ�� qimages ���ÿ��λ��ֻ����һ��ͼƬʱ��Ч
void data_loader_one(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd, bool bIsOneLabelMultiLoc,
	dlib::pipe<std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>>& qimages,
	dlib::pipe<std::vector<unsigned long>>& qlabels,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables);

// �������뺯�������ڶ��̶߳���ͼ�� qimages ���ÿ��λ�����ɶ���ͼƬʱ��Ч(loc2vec)
void data_loader_loc2vec(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd, bool bIsOneLabelMultiLoc,
	dlib::pipe<std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>>& qimages,
	dlib::pipe<std::vector<unsigned long>>& qlabels,
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects,
	std::vector<std::vector<QString>> svvLables);


// �ú�����ÿ��λ�����ɶ���ͼƬ��Ч����loc2vec�еķ�ʽ
// ÿһ����һ��label��ֻѡһ��λ�õ�ͼƬ
bool load_mini_batch_one_label_one_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels);

// �ú�����ÿ��λ�����ɶ���ͼƬ��Ч����loc2vec�еķ�ʽ
// ÿһ����һ��label�����ѡ�����ڸ�label��λ�õ�ͼƬ
bool load_mini_batch_one_label_multi_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels);


// �ú�����ÿ��λ��ֻ����һ��ͼƬ��Ч
// ÿһ����һ��label��ֻѡһ��λ�ã�Ȼ���������ü�����ת
bool load_mini_batch_one_label_one_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels);

// �ú�����ÿ��λ��ֻ����һ��ͼƬ��Ч
// ÿһ����һ��label�����ѡ�����ڸ�label��λ�ã���������ü�����ת
bool load_mini_batch_one_label_multi_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels);


// ֻ����λ�ƣ�����ת������Ϊ���ľ���
void RandomlyCropImage(
	const std::array<matrix<unsigned char>, LAYER_NUM>& input_image, 
	std::array<matrix<unsigned char>, LAYER_NUM>& output_image, 
	dlib::rand& rnd, int nCropWidth, int nCropHeight);


// ֻ����λ�ƣ�����ת������ΪRGB�ľ���
void RandomlyCropImage(
	const matrix<rgb_pixel>& input_image, 
	matrix<rgb_pixel>& output_image, 
	dlib::rand& rnd, int nCropWidth, int nCropHeight);


// ͼ������λ�ò��䣬ֻ������ת��������λ�ƣ�����Ϊ���ľ���
void CenterRandomlyCropImage(
	const std::array<matrix<unsigned char>, LAYER_NUM>& input_image, 
	std::array<matrix<unsigned char>, LAYER_NUM>& output_image, 
	dlib::rand& rnd, int nCropWidth, int nCropHeight);


// ͼ������λ�ò��䣬ֻ������ת��������λ�ƣ�����ΪRGB�ľ���
void CenterRandomlyCropImage(
	const matrix<rgb_pixel>& input_image, 
	matrix<rgb_pixel>& output_image, 
	dlib::rand& rnd, int nCropWidth, int nCropHeight);


// ��ͼ���ܱ����0
void PaddingZero(
	const matrix<unsigned char>& input_image,
	matrix<unsigned char>& output_image,
	int nCropWidth, int nCropHeight
);

// �ü����ض���С��ͼ��
void ClipImage(
	const matrix<unsigned char>& input_image,
	matrix<unsigned char>& output_image,
	int nCropWidth, int nCropHeight
);

// ��ͼ���ֵ���ض���С
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

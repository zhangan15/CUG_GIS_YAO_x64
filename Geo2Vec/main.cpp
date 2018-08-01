#include <QtCore>
#include <dlib/dnn.h>
#include <iostream>
#include <dlib/data_io.h>

#include "CommonFunctions.h"

using namespace std;
using namespace dlib;


#define LAYER_NUM 8


// The next page of code defines a ResNet network.  It's basically copied
// and pasted from the dnn_imagenet_ex.cpp example, except we replaced the loss
// layer with loss_metric and make the network somewhat smaller.

template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N, BN, 1, tag1<SUBNET>>>;

template <template <int, template<typename>class, int, typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2, 2, 2, 2, skip1<tag2<block<N, BN, 2, tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block = BN<con<N, 3, 3, 1, 1, relu<BN<con<N, 3, 3, stride, stride, SUBNET>>>>>;


template <int N, typename SUBNET> using res = relu<residual<block, N, bn_con, SUBNET>>;
template <int N, typename SUBNET> using ares = relu<residual<block, N, affine, SUBNET>>;
template <int N, typename SUBNET> using res_down = relu<residual_down<block, N, bn_con, SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block, N, affine, SUBNET>>;

// ----------------------------------------------------------------------------------------

template <typename SUBNET> using level0 = res_down<256, SUBNET>;
template <typename SUBNET> using level1 = res<256, res<256, res_down<256, SUBNET>>>;
template <typename SUBNET> using level2 = res<128, res<128, res_down<128, SUBNET>>>;
template <typename SUBNET> using level3 = res<64, res<64, res<64, res_down<64, SUBNET>>>>;
template <typename SUBNET> using level4 = res<32, res<32, res<32, SUBNET>>>;

template <typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template <typename SUBNET> using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;


// training network type
using net_type = loss_metric<fc_no_bias<128, avg_pool_everything<
	level0<
	level1<
	level2<
	level3<
	level4<
	max_pool<3, 3, 2, 2, relu<bn_con<con<32, 7, 7, 2, 2,
	//input_rgb_image
	input<std::array<matrix<unsigned char>, LAYER_NUM>>
	>>>>>>>>>>>>;

// testing network type (replaced batch normalization with fixed affine transforms)
using anet_type = loss_metric<fc_no_bias<128, avg_pool_everything<
	alevel0<
	alevel1<
	alevel2<
	alevel3<
	alevel4<
	max_pool<3, 3, 2, 2, relu<affine<con<32, 7, 7, 2, 2,
	//input_rgb_image
	input<std::array<matrix<unsigned char>, LAYER_NUM>>
	>>>>>>>>>>>>;

// using net_type = loss_metric<
// 	fc<128,
// 	dropout<relu<fc<256,
// 	relu<fc<512,
// 	max_pool<2, 2, 2, 2, relu<con<64, 3, 3, 1, 1,
// 	max_pool<2, 2, 2, 2, relu<con<32, 3, 3, 1, 1,
// 	max_pool<2, 2, 2, 2, relu<con<16, 3, 3, 1, 1,
// 	//input<matrix<unsigned char>>
// 	//input_rgb_image
// 	input<std::array<matrix<unsigned char>, LAYER_NUM>>
// 	>>>>>>>>>>>>>>>>;


// ѵ�� Geo2Vec
bool TrainGeo2Vec(
	const char* sImageDir,      //����OSM��Ƭ���ݵ��ļ���
	const char* sObjectListFile,//��������ͼƬ���ļ���
	const char* sLabelFile,     //��OSM��Ƭ���ݶ�Ӧ��Label�ļ�
	const char* sOutModelFile,  //����ѵ����ɵ�ģ��
	const char* sOutTempFile,   //����ѵ�������е���ʱģ��
	int nBatchLoc,              //ÿ�� mini-batch ������λ������
	int nSamplesPerLoc,         //ÿ��λ��ȡ������ͼƬ
	bool bIsLoc2Vec,            //�Ƿ���� Loc2Vec �Ĵ���ʽ���ǵĻ�����ͼƬ����ʱһ��λ�������ɺö������Ӱ�񣬷����ڱ������������ת��λ�ƽ���ѡȡ
	bool bIsOneLabelMultiLoc,   //�ڽ���ÿ��batch������ʱ���Ƿ���label��ͬ������λ�����ȡ����Ϊ��ÿ��batchֻ����ͬһ��lable�е�ĳһ��λ�õ�ͼƬ
	int nCropSize,              //��ͼƬ�ü���Ĵ�С��������� Loc2Vec �еĴ���ʽ��˲�����Ч
	int nThreadsNum             //����ͼƬ��ȡ���߳���
)
{
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>> images;
	std::vector<unsigned long> labels;

	dlib::pipe<std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>> qimages(10);
	dlib::pipe<std::vector<unsigned long>> qlabels(10);

	dlib::rand rnd(time(0));
	std::vector<std::vector<QString>> svvLables;
	std::thread * p_data_loader = new std::thread[nThreadsNum];
	
	net_type net;
	dnn_trainer<net_type> trainer(net, sgd(0.00001, 0.9));
	trainer.set_learning_rate(0.1);
	trainer.be_verbose();
	trainer.set_synchronization_file(sOutTempFile, std::chrono::minutes(5));
	trainer.set_iterations_without_progress_threshold(10000);


	if (bIsLoc2Vec)
	{
		QHash<QString, std::vector<std::vector<std::string>>> hash_objects;
		
		load_objects_list(sImageDir, sObjectListFile, hash_objects);
		load_lables(sLabelFile, hash_objects, svvLables);

		std::cout << "Number of locations: " << hash_objects.size() << std::endl;
		std::cout << "Number of types of labels: " << svvLables.size() << std::endl;

		for (int i = 0; i < nThreadsNum; i++)
		{
			p_data_loader[i] = std::thread(data_loader_loc2vec, nBatchLoc, nSamplesPerLoc, nCropSize, nCropSize, std::ref(rnd), bIsOneLabelMultiLoc, std::ref(qimages), std::ref(qlabels), hash_objects, svvLables);
		}

		while (trainer.get_learning_rate() >= 1e-5)
		{
			qimages.dequeue(images);
			qlabels.dequeue(labels);
			trainer.train_one_step(images, labels);
		}
	}
	else 
	{
		QHash<QString, std::vector<std::string>> hash_objects;
		
		load_objects_list(sImageDir, sObjectListFile, hash_objects);
		load_lables(sLabelFile, hash_objects, svvLables);

		std::cout << "Number of locations: " << hash_objects.size() << std::endl;
		std::cout << "Number of types of labels: " << svvLables.size() << std::endl;

		for (int i = 0; i < nThreadsNum; i++)
		{
			p_data_loader[i] = std::thread(data_loader_one, nBatchLoc, nSamplesPerLoc, nCropSize, nCropSize, std::ref(rnd), bIsOneLabelMultiLoc, std::ref(qimages), std::ref(qlabels), hash_objects, svvLables);
		}

		while (trainer.get_learning_rate() >= 1e-5)
		{
			qimages.dequeue(images);
			qlabels.dequeue(labels);
			trainer.train_one_step(images, labels);
		}
	}
	
	// Wait for training threads to stop
	trainer.get_net();
	std::cout << "Training Done." << std::endl;

	//Save the network to disk
	net.clean();
	dlib::serialize(sOutModelFile) << net;

	qimages.disable();
	qlabels.disable();
	for (int i = 0; i < nThreadsNum; i++)
	{
		p_data_loader[i].join();
	}

	return true;
}

// �����ȡһ��ѵ�������е� mini-batch �鿴����
double getTrainMiniBatchArruracy(
	const char* sImageDir, const char* sLabelFile, const char* sObjectListFile,
	int nCropSize, const char* sOutModelFile)
{
	int num_loc = 10;
	int samples_per_loc = 20;

	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>> images;
	std::vector<unsigned long> labels;

	dlib::rand rnd(time(0));

	QHash<QString, std::vector<std::vector<std::string>>> hash_objects;
	std::vector<std::vector<QString>> svvLables;

	load_objects_list(sImageDir, sObjectListFile, hash_objects);
	load_lables(sLabelFile, hash_objects, svvLables);
	load_mini_batch_one_label_multi_loc(num_loc, samples_per_loc, nCropSize, nCropSize, rnd, hash_objects, svvLables, images, labels);

	anet_type net;
	deserialize(sOutModelFile) >> net;

	std::vector<matrix<float, 0, 1>> embedded = net(images);

	int num_right = 0;
	int num_wrong = 0;
	for (size_t i = 0; i < embedded.size(); ++i)
	{
		for (size_t j = i + 1; j < embedded.size(); ++j)
		{
			if (labels[i] == labels[j])
			{
				// The loss_metric layer will cause images with the same label to be less
				// than net.loss_details().get_distance_threshold() distance from each
				// other.  So we can use that distance value as our testing threshold.
				if (length(embedded[i] - embedded[j]) < net.loss_details().get_distance_threshold())
					++num_right;
				else
					++num_wrong;
			}
			else
			{
				if (length(embedded[i] - embedded[j]) >= net.loss_details().get_distance_threshold())
					++num_right;
				else
					++num_wrong;
			}
		}
	}

	cout << "num_right: " << num_right << endl;
	cout << "num_wrong: " << num_wrong << endl;

	double dAccuracy = (double)num_right / (num_right + num_wrong);
	std::cout << "accuracy: " << dAccuracy << std::endl;

	return dAccuracy;
}

// ���뱣��ͼƬ���ļ��У���ÿ��ͼƬ���������б���
bool PredictGeo2Vec(const char* sModelFile,const char* sImageDir, const char* sObjectListFile, const char* sGeoVecFile, int nCropSize)
{
	int nPredictBatchSize = 100;

	QHash<QString, std::vector<std::string>> hash_objects;
	load_objects_list(sImageDir, sObjectListFile, hash_objects);

	std::vector<std::vector<std::string>> svvImagesPath;
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>> images;
	std::vector<QString> svGeoHash;
	
	QHash<QString, std::vector<std::string>>::iterator iter_hash;
	for (iter_hash = hash_objects.begin(); iter_hash != hash_objects.end(); iter_hash++)
	{
		QString sHash = iter_hash.key();
		svGeoHash.push_back(sHash);
	}
	
	anet_type net;
	deserialize(sModelFile) >> net;
	int nVecDim =  layer<1>(net).layer_details().get_num_outputs();

	QFile _outfile(sGeoVecFile);
	if (!_outfile.open(QIODevice::WriteOnly))
	{
		std::cout << "Create GeoVecFile error!" << std::endl;
		return false;
	}
	QTextStream _out(&_outfile);
	_out << "GeoHash";

	for (int i = 0; i < nVecDim; i++)
	{
		_out << ",Vec_" << i;
	}
	_out << "\r\n";

	int nPredictNums = hash_objects.size() / nPredictBatchSize;
	int i = 0, j = 0;
	for (i = 0; i < nPredictNums; i++)
	{
		for (j = 0; j < nPredictBatchSize; j++)
		{
			QString sHash = svGeoHash[i * nPredictBatchSize + j];
			std::vector<std::string> svPath = hash_objects[sHash];
			svvImagesPath.push_back(svPath);
		}
		load_predict_images(svvImagesPath, images, nCropSize, nCropSize);
		std::vector<matrix<float, 0, 1>> embedded = net(images);
		
		for (int m = 0; m < svvImagesPath.size(); m++)
		{
			_out << svGeoHash[m];
			for (int n = 0; n < nVecDim; n++)
			{
				QString strOut = QString(",%1").arg(embedded[m](n, 0), 0, 'f', 12);
				_out << strOut;
			}
			_out << "\r\n";
		}

		svvImagesPath.clear();
		images.clear();
	}
	
	for (j = i * nPredictBatchSize; j < hash_objects.size(); j++)
	{
		QString sHash = svGeoHash[j];
		std::vector<std::string> svPath = hash_objects[sHash];
		svvImagesPath.push_back(svPath);
	}

	load_predict_images(svvImagesPath, images, nCropSize, nCropSize);
	std::vector<matrix<float, 0, 1>> embedded = net(images);
	
	for (int m = 0; m < svvImagesPath.size(); m++)
	{
		_out << svGeoHash[m];
		for (int n = 0; n < nVecDim; n++)
		{
			QString strOut = QString(",%1").arg(embedded[m](n, 0), 0, 'f', 12);
			_out << strOut;
		}
		_out << "\r\n";
	}
	
	_outfile.close();
	return true;
}

int main(int argc, char *argv[]) try
{
	const char* sImageDir = "./data/osm_per_loc_multi_image";
	const char* sLabelFile = "./data/lonlat_label";
	const char* sOutModelFile = "./data/Geo2Vec_Model.dat";
	const char* sOutTempFile = "./data/Geo2Vec_Model_tmp";
	const char* sObjectListFile = "./data/osm_per_loc_multi_image_object_list.txt";   //��������ͼƬ�ļ������ļ���ͼƬ�����϶�ʱ����һ�λ�ȡ�����ļ�����Ҫʱ�����

	int nBatchLoc = 5;
	int nSamplesPerLoc = 10;
	bool bIsLoc2Vec = true;
	bool bIsOneLabelMultiLoc = false; //�ڽ���ÿ��batch������ʱ���Ƿ���label��ͬ������λ�����ȡ����Ϊ��ÿ��batchֻ����ͬһ��lable�е�ĳһ��λ�õ�ͼƬ
	int nCropSize = 256;
	int nThreadsNum = 5;
	
	// =================== Train and Test ===============
	TrainGeo2Vec(sImageDir, sObjectListFile, sLabelFile, sOutModelFile, sOutTempFile, nBatchLoc, nSamplesPerLoc, bIsLoc2Vec, bIsOneLabelMultiLoc, nCropSize, nThreadsNum);
	//getTrainMiniBatchArruracy(sImageDir, sLabelFile, nCropSize, sOutModelFile);


	// =================== Predict ======================
	const char* sPredictImageDir = "./data/osm_per_loc_one_image";
	const char* sPredictObejectListFile = "./data/osm_per_loc_one_image_object_list.txt";
	const char* sGeoVecFile = "./data/geo_vec.csv";
	//PredictGeo2Vec(sOutModelFile, sPredictImageDir, sPredictObejectListFile, sGeoVecFile, nCropSize);
}
catch (std::exception& e)
{
	std::cout << e.what() << std::endl;
}
#include <QtCore>
#include <iostream>
#include "gdal_priv.h"
#include "PublicFunctions.h"



//Fitter for simulation
using net_type = loss_mean_squared<
	fc<1,
	dropout<relu<fc<128,
	dropout<relu<con<32, 3, 3, 1, 1,
	max_pool<2, 2, 2, 2, relu<con<32, 3, 3, 1, 1,
	max_pool<2, 2, 2, 2, relu<con<16, 3, 3, 1, 1,
	input<matrix<rgb_pixel>>
	>>>>>>>>>>>>>>;


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

	//输入数据参数
	char* sHpCsvFn = "./data/wuhan_price.csv";
	double dMinVal = 1000;
	double dMaxVal = 60000;
	int nCropCount = 5;
	char* sImgFn = "E:/Data/wuhan_google_earth/Level15/wuhan_ge.tif";

	//网络训练参数
	double dInitLearningRate = 0.1;
	double dMinLearningRate = 0.00001;
	int nMinBatchSize = 128;
	char* sNetFn = "./wuhan_umcnn.dnn";	//输出

	//variables
	std::vector<hp_data> vHpData;
	std::vector<hp_sample> vSamples;
	CGDALRead input_img;

	//读入房价数据
	cout << "Loading Housing Price ..." << endl;
	vHpData.clear();
	if (!LoadHpDataFromCSV(sHpCsvFn, vHpData, dMinVal, dMaxVal))
	{
		cout << "Load Housing Price Data Fail. Exit." << endl;
		return -1;
	}


	//读入遥感影像
	cout << "Loading HSR Remote Sensing Image ..." << endl;
	if (!input_img.loadFrom(sImgFn))
	{
		cout << "Load HSR RS Data Fail. Exit." << endl;
		return -2;
	}
	cout << "Load RS Image Success. Image Size = " << \
		input_img.rows() << " * " << input_img.cols() \
		<< " * " << input_img.bandnum() << endl;


	//生成训练数据
	cout << "Initializing Training Samples ..." << endl;
	if (!ChangeHpData2CnnSamples(&input_img, vHpData, MAX_RECT_SIZE, MAX_RECT_SIZE, vSamples))
	{
		cout << "Init Training Samples Fail. Exit." << endl;
		return -3;
	}
	
	input_img.close();

	//构建训练数据集
	cout << "Generating Training and Testing Samples ..." << endl;
	std::vector<matrix<rgb_pixel>> training_images;
	std::vector<float> training_labels;
	std::vector<matrix<rgb_pixel>> testing_images;
	std::vector<float>	testing_labels;

	//根据输入数据随机裁剪
	dlib::rand rnd(time(0));
	int nCount = 0;
	foreach (hp_sample sap, vSamples)
	{
		nCount++;
		if (nCount % 500 == 0)
			cout << "Processing No. " << nCount << " / " << vSamples.size() << "..." << endl;

		matrix<rgb_pixel> output_img;
		for (int i = 0; i < nCropCount; i++)
		{
			RandomlyCropImage(sap.input_image, output_img, rnd, RAND_RECT_SIZE, RAND_RECT_SIZE);

			// 20% data set in test_images
			if (rnd.get_random_double() <= 0.8)
			{
				training_images.push_back(output_img);
				training_labels.push_back(sap.dPrice);
			}
			else
			{
				testing_images.push_back(output_img);
				testing_labels.push_back(sap.dPrice);
			}
		}
	}
	cout << "Training Data Count = " << training_images.size() << endl;
	cout << "Training Label Count = " << training_labels.size() << endl;
	cout << "Testing Data Count = " << testing_images.size() << endl;
	cout << "Testing Label Count = " << testing_labels.size() << endl;

	//
	cout << "Start Training UMCNN ..." << endl;
	net_type net;
	dnn_trainer<net_type> trainer(net);
	trainer.set_learning_rate(dInitLearningRate);
	trainer.set_min_learning_rate(dMinLearningRate);
	trainer.set_mini_batch_size(nMinBatchSize);
	trainer.be_verbose();


	//训练文件
	trainer.set_synchronization_file("./umcnn_sync.dat", std::chrono::seconds(20));
	trainer.train(training_images, training_labels);

	//保存训练文件
	net.clean();
	serialize(sNetFn) << net;

	//精度评价
	std::vector<float> predicted_labels = net(testing_images);
	predicted_labels = net(testing_images);

	double sum_val = 0;
	for (size_t i = 0; i < testing_images.size(); ++i)
	{
		sum_val += (predicted_labels[i]-testing_labels[i])*(predicted_labels[i] - testing_labels[i]);
	}
	sum_val = sqrt(sum_val/(double)testing_images.size());
	cout << "Testing RMSE = " << sum_val << endl;
	
	return 0;
}

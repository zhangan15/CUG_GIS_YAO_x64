#include <QtCore>
#include <iostream>
#include "gdal_priv.h"
#include "GDALWrite.h"
#include "PublicFunctions.h"
#include <omp.h>

// #define UMCNN_TRAIN_PROCESS
#define UMCNN_PREDICT_PROCESS

//Fitter for simulation
using net_type = loss_mean_squared<
	fc<1,
	dropout<relu<fc<32,
	dropout<relu<fc<128,
	dropout<relu<con<32, 3, 3, 1, 1,
	max_pool<2, 2, 2, 2, relu<con<32, 3, 3, 1, 1,
	max_pool<2, 2, 2, 2, relu<con<16, 3, 3, 1, 1,
	input<matrix<rgb_pixel>>
	>>>>>>>>>>>>>>>>>;

int UMCNN_Training_Process(
	char* sInHousingPriceCsvFn,		//�������� lng, lat, housing_price(yuan/m2)
	char* sInHsrRsImgFn,			//�߷�ң��Ӱ�����ݣ�����3�����Σ���ֵ����unsigned char
	char* sOutDnnFn,				//���DNN�ļ�
	char* sOutTempFn = "./temp_dnn.dat",	//ѵ��DNN��������ʱ�ļ�λ��
	double dNormalVal = 30000,		//������ݽ������ֹ��ʧ������ը
	double dMinVal = 1000,			//���˵�����С
	double dMaxVal = 100000,		//���˳������
	int nRandCropCount = 10,		//ÿ�����ݵ����ȡͼ����Ŀ
	double dInitLearningRate = 0.01,	//����ѧϰ��
	double dMinLearningRate = 0.00001,	//����ѧϰ��
	int nMinBatchSize = 128				//batch size
)
{
	//variables
	std::vector<hp_data> vHpData;
	std::vector<hp_sample> vSamples;
	CGDALRead input_img;

	//���뷿������
	cout << "Loading Housing Price ..." << endl;
	vHpData.clear();
	if (!LoadHpDataFromCSV(sInHousingPriceCsvFn, vHpData, dMinVal, dMaxVal))
	{
		cout << "Load Housing Price Data Fail. Exit." << endl;
		return -1;
	}


	//����ң��Ӱ��
	cout << "Loading HSR Remote Sensing Image ..." << endl;
	if (!input_img.loadFrom(sInHsrRsImgFn))
	{
		cout << "Load HSR RS Data Fail. Exit." << endl;
		return -2;
	}
	cout << "Load RS Image Success. Image Size = " << \
		input_img.rows() << " * " << input_img.cols() \
		<< " * " << input_img.bandnum() << endl;


	//����ѵ������
	cout << "Initializing Training Samples ..." << endl;
	if (!ChangeHpData2CnnSamples(&input_img, vHpData, MAX_RECT_SIZE, MAX_RECT_SIZE, vSamples))
	{
		cout << "Init Training Samples Fail. Exit." << endl;
		return -3;
	}

	input_img.close();

	//����ѵ�����ݼ�
	cout << "Generating Training and Testing Samples ..." << endl;
	std::vector<matrix<rgb_pixel>> training_images;
	std::vector<float> training_labels;
	std::vector<matrix<rgb_pixel>> testing_images;
	std::vector<float>	testing_labels;

	//����������������ü�
	dlib::rand rnd(time(0));
	int nCount = 0;
	foreach(hp_sample sap, vSamples)
	{
		nCount++;
		if (nCount % 500 == 0)
			cout << "Processing No. " << nCount << " / " << vSamples.size() << "..." << endl;

		matrix<rgb_pixel> output_img;
		for (int i = 0; i < nRandCropCount; i++)
		{
			RandomlyCropImage(sap.input_image, output_img, rnd, RAND_RECT_SIZE, RAND_RECT_SIZE);

			// 20% data set in test_images
			if (rnd.get_random_double() <= 0.8)
			{
				training_images.push_back(output_img);
				training_labels.push_back(sap.dPrice / dNormalVal);
			}
			else
			{
				testing_images.push_back(output_img);
				testing_labels.push_back(sap.dPrice / dNormalVal);
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


	//ѵ���ļ�
	trainer.set_synchronization_file(sOutTempFn, std::chrono::seconds(20));
	trainer.train(training_images, training_labels);

	//����ѵ���ļ�
	net.clean();
	serialize(sOutDnnFn) << net;

	//��������
	std::vector<float> predicted_labels = net(testing_images);
	predicted_labels = net(testing_images);

	double sum_val = 0;
	for (size_t i = 0; i < testing_images.size(); ++i)
	{
		sum_val += (predicted_labels[i] - testing_labels[i])*(predicted_labels[i] - testing_labels[i])*dNormalVal*dNormalVal;
	}
	sum_val = sqrt(sum_val / (double)testing_images.size());
	cout << "Testing RMSE = " << sum_val << endl;

	return 0;
}

int UMCNN_Predicting_Process(
	char* sInHsrRsImgFn,			//�߷�ң��Ӱ�����ݣ�����3�����Σ���ֵ����unsigned char
	char* sInDnnFn,					//����DNN�ļ�
	char* sOutHpTifFn,				//���float���͵ķ�������
	double dNormalVal = 30000		//������ݽ������ֹ��ʧ������ը
)
{
	//��������
	cout << "Loading UMCNN Net ... " << endl;
	net_type net;
	try
	{		
		deserialize(sInDnnFn) >> net;
		cout << "Load UMCNN Net Success." << endl;
	}
	catch (std::exception& e)
	{
		cout << "Load Net Fail. Reason: " << e.what() << endl;
		return -1;
	}	

	//����ң��Ӱ��
	CGDALRead input_img;
	cout << "Loading HSR Remote Sensing Image ..." << endl;
	if (!input_img.loadFrom(sInHsrRsImgFn))
	{
		cout << "Load HSR RS Data Fail. Exit." << endl;
		return -2;
	}
	cout << "Load RS Image Success. Image Size = " << \
		input_img.rows() << " * " << input_img.cols() \
		<< " * " << input_img.bandnum() << endl;

	//ɾ�����е�ң��Ӱ��
	if (QFile::exists(sOutHpTifFn))
		QFile::remove(sOutHpTifFn);

	//�������ң��Ӱ��
	CGDALWrite output_img;
	output_img.init(sOutHpTifFn, &input_img, 1, OUTPUT_DATATYPE, 0.0f);

	int nRow = 0, nCol = 0, k = 0;
	int _tmpSum = 0;
	float val = 0;

	//�ӿ�Ч��
//#pragma omp parallel for private (nCol, k, _tmpSum, val�� net) schedule(static)
	for (nRow = 0; nRow < output_img.rows(); nRow++)
	{
		//if ((nRow + 1) % 50 == 0)
		cout << "Processing Line No. " << (nRow + 1) << " / " << output_img.rows() << " ..." << endl;

		for (nCol = 0; nCol < output_img.cols(); nCol++)
		{			
			_tmpSum = 0;
			for (k = 0; k<input_img.bandnum(); k++)
				_tmpSum += *(unsigned char*)input_img.read(nRow, nCol, k);				
					
			if (_tmpSum == 0)
				val = 0.0f;
			else
			{
				matrix<rgb_pixel> clipImg;
				if (!ObtainRectImageFromHSR(&input_img, nRow, nCol, RAND_RECT_SIZE, RAND_RECT_SIZE, clipImg))
					val = 0.0f;
				else
				{
					val = net(clipImg);
					val *= dNormalVal;
				}
			}

			output_img.write(nRow, nCol, 0, &val);			
			
		}
	}


	output_img.close();
	input_img.close();
	cout << "Mapping Success." << endl;
	return 0;

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
		cout << "[Init] Environment Initialization Success!\n";
	}

	//omp_set_num_threads(4);
	

	//�������ݲ���
	char* sHpCsvFn = "./data/wuhan_price.csv";		//�������� lng, lat, housing_price(yuan/m2)
	double dMinVal = 1000;		//���˵�����С
	double dMaxVal = 60000;		//���˳������
	int nCropCount = 10;		//ÿ�����ݵ����ȡͼ����Ŀ
	char* sImgFn = "./data/wuhan_ge_clip_center.tif";	//�߷�ң��Ӱ�����ݣ�����3�����Σ���ֵ����unsigned char
	

	//����ѵ������
	double dNormalVal = 30000;	//������ݽ������ֹ��ʧ������ը
	double dInitLearningRate = 0.01;	//����ѧϰ��
	double dMinLearningRate = 0.00001;	//����ѧϰ��
	int nMinBatchSize = 128;			//batch size
	char* sTempNetFn = "./data/temp_dnn.dat";	//ѵ��DNN��ʱ�ļ�
	char* sNetFn = "./data/wuhan_umcnn.dnn";	//���DNN�ļ�

	//Training
#ifdef UMCNN_TRAIN_PROCESS
	UMCNN_Training_Process(sHpCsvFn, sImgFn, sNetFn, sTempNetFn, \
		dNormalVal, dMinVal, dMaxVal, nCropCount, \
		dInitLearningRate, dMinLearningRate, nMinBatchSize);
#endif
	

	//Predicting	
#ifdef UMCNN_PREDICT_PROCESS
	char* sPreFn = "./data/guanggu_4m.tif";
	char* sOutFn = "./data/guanggu_housing_price.tif";
	UMCNN_Predicting_Process(sPreFn, sNetFn, sOutFn, dNormalVal);
#endif
	

	
	return 0;
}

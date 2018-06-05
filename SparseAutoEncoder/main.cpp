
#include "preprocess.h"
#include "sae_activiation.h"
#include "sae.h"
#include "matrix.h"

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <string>
#include <assert.h>
using namespace std;

//#define SAE_TRAINING
#define ENCODER


int main(int argc, char* argv[])
{
	std::string data_dir = "./data/";
	srand(unsigned(time(0)));
	//Matrix X = generateRandomMat(64, 1000, 0, 1);
#ifdef SAE_TRAINING
	// read data, filename, nFeatureNum, nSampleNum
	Matrix X = csv2mat(data_dir + "mnist.csv", 784, 60000);	//csv2mat输入的是nFeats*nSamples文件 （提供的mnist.csv属于这种）
	// -- Matrix X = normal_csv2mat(data_dir + "mnist.csv", 60000, 784 ); //normal_csv2mat输入的是nSamples*nFeats文件

	// normalize data
	cout << "normalizing...";
	normalizeData(X);
	cout << "success!\n";
	
	// training model
	SparseAutoEncoder sae;
	// X, hidden_layer_size, learning_rate, epochs, batch_size, lambda, rho, sparsity_weight
	sae.trainSparseAutoEncoder(X, 100, 0.1, 200, 0, 0.0001, 0.1, 2);
	
	// save sae weights (W1, W2, b1, b2)
  	sae.saveNetWeight(data_dir + "training_weight.csv");
	// save W1, each line(1*input_size) could be reshaped to sqrt(input_size)*sqrt(input_size) to visualize
 	mat2csv(sae.getW1(), data_dir + "W1.csv");

#endif

#ifdef ENCODER
	// before loading weights, net structure should be defined first
	SparseAutoEncoder sae(784, 100);	//nFeats, hidden_layer_size
	sae.loadNetWeight(data_dir + "training_weight.csv");	//csv2mat输入的是nFeats*nSamples文件 （提供的mnist.csv属于这种）, normal_csv2mat输入的是nSamples*nFeats文件


	// 随机挑选5个例子进行对比;
	Matrix X = csv2mat(data_dir + "mnist.csv", 784, 60000);
	ofstream fout(data_dir + "comparison.csv");
	if (!fout.good())
	{
		cerr << "open file fail!\n";
		return -1;
	}

	Matrix testMat(X.RowNo(), 1);	//nFeats, 1
	for (int j = 0; j < 5; j++)
	{
		int ind = rand() % X.ColNo();
		for (int i = 0; i < X.RowNo(); i++)
			//testMat(i, 0) = rand() / (double)RAND_MAX;
			testMat(i, 0) = X(i, ind);

		// 编码再解码;
		Matrix encodevec = sigmoid(sae.getW1()*testMat + sae.getb1());

// 		for (int i = 0; i < encodevec.RowNo(); i++)
// 			encodevec(i, 0) = rand() / (double)RAND_MAX;


		Matrix decodevec = sigmoid(sae.getW2()*encodevec + sae.getb2());

		// 计算余弦值;
		cout << "index = " << ind << "   cosin = " << cosin(testMat, decodevec) << endl;

		// 对比结果写入文件;
		for (int i = 0; i < testMat.RowNo() - 1; i++)
			fout << testMat(i, 0) << ",";
		fout << testMat(testMat.RowNo() - 1, 0) << "\n";

		// 编码值，即隐含层的激活值;
		for (int i = 0; i < encodevec.RowNo() - 1; i++)
			fout << encodevec(i, 0) << ",";
		fout << encodevec(encodevec.RowNo() - 1, 0) << "\n";

		// 解码后的值，即输出层的输出值;
		for (int i = 0; i < decodevec.RowNo() - 1; i++)
			fout << decodevec(i, 0) << ",";
		fout << decodevec(decodevec.RowNo() - 1, 0) << "\n";
		fout.flush();
	}
	fout.close();

	
#endif
	return 0;
}
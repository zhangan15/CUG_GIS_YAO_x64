#ifndef SAE_H
#define SAE_H

/***********************************************

文件名: sae.h
作者： 刘鹏华
版本：v1.0
描述：实现了稀疏自编码 Sparse Autoencoder(SAE)
主要实现的方法包括:
SAE类的构造和析构，权重初始化，损失函数的计算，模型训练（主要的API），梯度检验，网络权重的保存和读取，编码和解码等方法。

***********************************************/



#include "preprocess.h"
#include "sae_activiation.h"
// #include "sae_layer.h"
// #include "sae_cost.h"
// #include "sae_optimizer.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;


struct TrainingParams
{
	double lr = 1.0;			// 学习率;
	double lambda = 0.0001;		// 正则项权重;
	double sparseParam = 0.1;	// 稀疏系数;
	double beta = 3;			// 稀疏项权重;
	int nMaxIter = 400;			// 训练迭代次数;

};


class SparseAutoEncoder
{
public:
	SparseAutoEncoder(){}
	SparseAutoEncoder(int nInputSize, int nHiddenSize) {
		mnInputFeatNum = nInputSize > 0 ? nInputSize : 5;
		mnHiddenNum = nHiddenSize > 0 ? nHiddenSize : 5;
		initNetWeights();
	}
	~SparseAutoEncoder(){
		mTheta.clear();
	}


	// 根据输入层和隐含层的节点数目初始化权重参数;
	inline void initNetWeights()
	{
		if (mnInputFeatNum <= 0)
		{
			cerr << "please set data first!\n";
			return;
		}
		if (mnHiddenNum <= 0)
		{
			cerr << "please set hidden layer size first!\n";
			return;
		}

		double r = sqrt(6.0) / sqrt(mnHiddenNum + mnInputFeatNum + 1);

		mTheta.clear();
		mTheta.push_back(generateRandomMat(mnHiddenNum, mnInputFeatNum, -r, r));
		mTheta.push_back(generateRandomMat(mnInputFeatNum, mnHiddenNum, -r, r));
		mTheta.push_back(generateRandomMat(mnHiddenNum, 1, -r, r));
		mTheta.push_back(generateRandomMat(mnInputFeatNum, 1, -r, r));

		cout << "sae_net struct: " << mnInputFeatNum << " - " << mnHiddenNum << " - " << mnInputFeatNum << endl << endl;
	}

	// 训练SAE模型;
	bool trainSparseAutoEncoder(Matrix X, int nHiddenSize = 25, double lr = 1.0, int nTrainingIter = 400, int nBatchSize = 0, double lambda = 0.0001, double sparseParam = 0.1, double beta = 3)
	{
		mMatData = X;
		mnSampleNum = X.ColNo();
		mnInputFeatNum = X.RowNo();
		mnHiddenNum = nHiddenSize;
		initNetWeights();
		if (mnSampleNum <= 0 || mnHiddenNum <= 0 || mnHiddenNum <= 0)
		{
			cerr << "shape of input data is illegal or hidden layer size <= 0\n";
			return false;
		}

		// 确定 batch size
		if (nBatchSize == 0) nBatchSize = mMatData.ColNo() / 50;
		if (nBatchSize == 0) nBatchSize = 1;
		if (nBatchSize > mMatData.ColNo()) nBatchSize = mMatData.ColNo();


		cout << "training batch size = " << nBatchSize << endl;
		mnHiddenNum = nHiddenSize;
		mParams.lr = lr;
		mParams.nMaxIter = nTrainingIter;
		mParams.lambda = lambda;
		mParams.sparseParam = sparseParam;
		mParams.beta = beta;

		double dCost = 0;
		//Adagrad optimizer(mParams.lr);
		//RMSProp optimizer(mParams.lr);
		//SGD optimizer(mParams.lr);

		//训练参数;
		double epsilont;
		double pt;
		int T = 200;
		double epsilon0 = 80.0;
		double f = 0.999;
		double pi = 0.5;
		double pf = 0.99;

		std::vector<Matrix> v_vW;
		v_vW.push_back(Matrix(mTheta[0].RowNo(), mTheta[0].ColNo()));
		v_vW.push_back(Matrix(mTheta[1].RowNo(), mTheta[1].ColNo()));
		v_vW.push_back(Matrix(mTheta[2].RowNo(), mTheta[2].ColNo()));
		v_vW.push_back(Matrix(mTheta[3].RowNo(), mTheta[3].ColNo()));

		cout << "start training sparse auto-encoder ...\n";
		for (int i_iter = 0; i_iter < mParams.nMaxIter; i_iter++)
		{
			//此处的参数是为了训练时采用不同的相对学习效率;
			if (i_iter > T) pt = pf;
			else pt = (double)i_iter / T * pi + (1 - (double)i_iter / T) * pf;
			epsilont = epsilon0 * pow(f, i_iter);

			// 随机选取小批量进行训练;
			int nULC = 0;
			if (nBatchSize != mMatData.ColNo())
				nULC = ((long)rand() + (long)rand()) % (mMatData.ColNo() - nBatchSize);
			Matrix data = rectMat(mMatData, 0, nULC, mMatData.RowNo(), nBatchSize);
			

			cout << "\r" << nULC <<" # iter " << setw(4) << i_iter + 1 << " # \tCOST: ";
			std::vector<Matrix> vGrad = calCost(data, dCost);
			cout << " = " << setw(6) << setprecision(3) << dCost << "\t";
			
// 			double _lr = mParams.lr;
// 			if (optimizer.getName() != "SGD")
// 			{
// 				// 计算所有权重的平方和;
// 				double g2 = 0;
// 				for (int j = 0; j < 4; j++)
// 					g2 += sumMat(multiplyMat(mTheta[j], mTheta[j]));
// 				
// 				// 更新学习率;
// 				//double _lr = ada.getLR(g2);
// 				_lr = optimizer.getLR(g2);
// 			}
// 			else
// 				_lr = optimizer.getLR(0);
// 			
// 			cout << "\t lr = " << _lr ;
// 
// 			for (int j = 0; j < mTheta.size(); j++)
// 				mTheta[j] += (- _lr*vGrad[j]);
			for (int j = 0; j < v_vW.size(); j++)
			{
				v_vW[j] = pt*v_vW[j] - (1.0 - pt)*epsilont*(mParams.lr*vGrad[j]);
				mTheta[j] += v_vW[j];
			}

			vGrad.clear();
			
		}
		v_vW.clear();
		cout << endl;
		return true;
	}

	
	//在check 梯度之前，必须先训练一次，否则各类参数均未知;
	bool checkGrad()
	{
		double dcost = 0;
		std::vector<Matrix> grad = calCost(mMatData, dcost);
		int outputcount = mTheta[3].RowNo() > 5 ? 5 : mTheta[3].RowNo();
		cout << "output gradient : ";
		for(int i = 0; i < outputcount; i ++)
			cout << grad[3](i, 0) << "\t";
		cout << endl;


		double de = 1e-6;
		cout << "numerical method: ";
		for (int i = 0; i < outputcount; i++)
		{
			mTheta[3](i, 0) += de;
			double dcost1 = 0, dcost2 = 0;
			calCost(mMatData, dcost1);
			mTheta[3](i, 0) -= (2 * de);
			calCost(mMatData, dcost2);
			cout << (dcost1 - dcost2) / (2.0*de) << "\t";
			
		}
		cout << endl;
		return  true;
	}

private:
	// 计算损失函数，返回目标函数对各参数的偏导数;
	std::vector<Matrix> calCost(Matrix &data, double &dCost)
	{
		if (mTheta.size() <= 0)
			initNetWeights();

		dCost = 0;
		int nSample = data.ColNo();

		Matrix W1 = mTheta[0], W2 = mTheta[1], B1 = mTheta[2], B2 = mTheta[3];

		// 在原始数据中加入噪声;
		Matrix noise_data = multiplyMat(data, getBernoulliMatrix(data.RowNo(), data.ColNo(), 0.15));
		Matrix active_value2 = sigmoid(W1*noise_data + repmat(B1, 1, nSample));
		Matrix active_value3 = sigmoid(W2*active_value2 + repmat(B2, 1, nSample));

		// computing cost function
		// (1)计算预测值与真实值的差的平方;
		// 对应元素相减，平方之后，按列求和得到每个样本的误差，再整体求和计算均值;
		double ave_square = sumMat(powMat(data - active_value3, 2.0)) / (nSample*2.0);


		// (2)计算 weight decay;
		double weight_decay = mParams.lambda / 2.0 * (sumMat(powMat(W1, 2)) + sumMat(powMat(W2, 2)));	// ||W1||2  ||W1||2， 所以是矩阵内部全部元素求和

		// (3)计算稀疏项;
		// 隐含层节点的平均激活值;
		Matrix pj = meanMat(active_value2, 0);	// 按行求均值，得到 hiddensize * 1

		
		// 计算sparsity;
		Matrix p_rou = repmat(mParams.sparseParam, mnHiddenNum, 1);	//ρ;
		double s1 = sumMat(multiplyMat(p_rou, logMat(devisionMat(p_rou, pj))));

		Matrix _p_rou = minusMat(1.0, p_rou), _pj = minusMat(1.0, pj);	//1-ρ, 1-ρj
		double s2 = sumMat(multiplyMat(_p_rou, logMat(devisionMat(_p_rou, _pj))));
		double sparsity = mParams.beta*(s1 + s2);

		// 总体损失函数值;
		dCost = ave_square + weight_decay + sparsity;
		cout << "[ " << setprecision(3) << ave_square << "\t" << setw(6) <<weight_decay << "\t" << setw(6) <<sparsity << " ]";


		// 求偏导，计算梯度;
		Matrix delta3 = multiplyMat(multiplyMat(active_value3 - data, active_value3), minusMat(1.0, active_value3));	//(a3-x)*[a3*(1-a3)],后者为sigmoid求导;
		Matrix sparsity_penalty = mParams.beta*(repmat(-devisionMat(p_rou, pj) + devisionMat(_p_rou, _pj), mnHiddenNum, nSample));
		Matrix delta2 = multiplyMat((~W2)*delta3 + sparsity_penalty, multiplyMat(active_value2, minusMat(1.0, active_value2)));

		
		Matrix W2grad = delta3*(~active_value2) / (double)nSample + mParams.lambda*W2;
		Matrix W1grad = delta2*(~data) / (double)nSample + mParams.lambda*W1;
		Matrix B2grad = meanMat(delta3, 0);
		Matrix B1grad = meanMat(delta2, 0);


		std::vector<Matrix> vGrad;
		vGrad.push_back(W1grad);
		vGrad.push_back(W2grad);
		vGrad.push_back(B1grad);
		vGrad.push_back(B2grad);

		return vGrad;
	}

public:
		// 编码过程，在模型训练好的情况下，输入一个向量，输出其编码后的向量;
		inline Matrix encode(Matrix mat)
		{
			if (mat.RowNo() != mnInputFeatNum || mat.ColNo() != 1)
			{
				cerr << "input size is not legal!\n";
				return mat;
			}
			return sigmoid(mTheta[0] * mat + mTheta[2]);
		}

		// 与编码相反，解码可以应用于GAN;
		inline Matrix decode(Matrix mat)
		{
			if (mat.RowNo() != mnHiddenNum || mat.ColNo() != 1)
			{
				cerr << "input size is not legal!\n";
				return mat;
			}
			return sigmoid(mTheta[1] * mat + mTheta[3]);
		}

		inline Matrix getW1()
		{
			return mTheta[0];
		}
		inline Matrix getW2()
		{
			return mTheta[1];
		}
		inline Matrix getb1()
		{
			return mTheta[2];
		}
		inline Matrix getb2()
		{
			return mTheta[3];
		}

		// 从文件中读取网络权重;
		bool loadNetWeight(std::string fn)
		{
			if (mTheta.size() <= 0)
			{
				cerr << "net has not been initialized yet!\n";
				return false;
			}
			ifstream fin(fn.c_str());
			if (!fin.good())
			{
				cerr << "open weight file fail!\n";
				return false;
			}
			std::string sLine;
			std::vector<std::string> slist;

			cout << "\nload net weight...";
			// 读取 W1
			for (int i = 0; i < mnHiddenNum; i++)
			{
				getline(fin, sLine);
				slist.clear();
				slist = split(sLine, ",");
				for (int j = 0; j < mnInputFeatNum; j++)
					mTheta[0](i, j) = atof(slist[j].c_str());
			}
			// 读取 W2
			for (int i = 0; i < mnInputFeatNum; i++)
			{
				getline(fin, sLine);
				slist.clear();
				slist = split(sLine, ",");
				for (int j = 0; j < mnHiddenNum; j++)
					mTheta[1](i, j) = atof(slist[j].c_str());
			}
			// 读取 b1
			for (int i = 0; i < mnHiddenNum; i++)
			{
				getline(fin, sLine);
				slist.clear();
				slist = split(sLine, ",");
				for (int j = 0; j < 1; j++)
					mTheta[2](i, j) = atof(slist[j].c_str());
			}
			// 读取 b2
			for (int i = 0; i < mnInputFeatNum; i++)
			{
				getline(fin, sLine);
				slist.clear();
				slist = split(sLine, ",");
				for (int j = 0; j < 1; j++)
					mTheta[3](i, j) = atof(slist[j].c_str());
			}
			fin.close();

			cout << "success!\n";
			return true;
		}

		// 写入网络权重;
		bool saveNetWeight(std::string fn)
		{
			if (mTheta.size() <= 0)
			{
				cerr << "net has not been initialized yet!\n";
				return false;
			}
			ofstream fout(fn.c_str());
			if (!fout.good())
			{
				cerr << "open weight file fail!\n";
				return false;
			}
			cout << "\nsave net weight...";
			for (int k = 0; k < 4; k++)
			{
				for (int i = 0; i < mTheta[k].RowNo(); i++)
				{
					for (int j = 0; j < mTheta[k].ColNo() - 1; j++)
						fout << mTheta[k](i, j) << ",";
					fout << mTheta[k](i, mTheta[k].ColNo() - 1) << "\n";
				}
			}
			fout.close();

			cout << "success!\n";
			return true;
		}

private:
	Matrix mMatData;		// 输入的特征，mnInputFeatNum*mnSampleNum维
	int mnSampleNum;		// 样本数目
	int mnInputFeatNum;		// 输入的特征维度
	int mnHiddenNum;		// 隐含层节点数目

	TrainingParams mParams;		// 训练参数
	std::vector<Matrix> mTheta;	// 网络权重;
};


#endif // !SAE_H


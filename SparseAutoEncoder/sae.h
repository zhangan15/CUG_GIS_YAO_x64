#ifndef SAE_H
#define SAE_H

/***********************************************

�ļ���: sae.h
���ߣ� ������
�汾��v1.0
������ʵ����ϡ���Ա��� Sparse Autoencoder(SAE)
��Ҫʵ�ֵķ�������:
SAE��Ĺ����������Ȩ�س�ʼ������ʧ�����ļ��㣬ģ��ѵ������Ҫ��API�����ݶȼ��飬����Ȩ�صı���Ͷ�ȡ������ͽ���ȷ�����

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
	double lr = 1.0;			// ѧϰ��;
	double lambda = 0.0001;		// ������Ȩ��;
	double sparseParam = 0.1;	// ϡ��ϵ��;
	double beta = 3;			// ϡ����Ȩ��;
	int nMaxIter = 400;			// ѵ����������;

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


	// ����������������Ľڵ���Ŀ��ʼ��Ȩ�ز���;
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

	// ѵ��SAEģ��;
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

		// ȷ�� batch size
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

		//ѵ������;
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
			//�˴��Ĳ�����Ϊ��ѵ��ʱ���ò�ͬ�����ѧϰЧ��;
			if (i_iter > T) pt = pf;
			else pt = (double)i_iter / T * pi + (1 - (double)i_iter / T) * pf;
			epsilont = epsilon0 * pow(f, i_iter);

			// ���ѡȡС��������ѵ��;
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
// 				// ��������Ȩ�ص�ƽ����;
// 				double g2 = 0;
// 				for (int j = 0; j < 4; j++)
// 					g2 += sumMat(multiplyMat(mTheta[j], mTheta[j]));
// 				
// 				// ����ѧϰ��;
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

	
	//��check �ݶ�֮ǰ��������ѵ��һ�Σ�������������δ֪;
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
	// ������ʧ����������Ŀ�꺯���Ը�������ƫ����;
	std::vector<Matrix> calCost(Matrix &data, double &dCost)
	{
		if (mTheta.size() <= 0)
			initNetWeights();

		dCost = 0;
		int nSample = data.ColNo();

		Matrix W1 = mTheta[0], W2 = mTheta[1], B1 = mTheta[2], B2 = mTheta[3];

		// ��ԭʼ�����м�������;
		Matrix noise_data = multiplyMat(data, getBernoulliMatrix(data.RowNo(), data.ColNo(), 0.15));
		Matrix active_value2 = sigmoid(W1*noise_data + repmat(B1, 1, nSample));
		Matrix active_value3 = sigmoid(W2*active_value2 + repmat(B2, 1, nSample));

		// computing cost function
		// (1)����Ԥ��ֵ����ʵֵ�Ĳ��ƽ��;
		// ��ӦԪ�������ƽ��֮�󣬰�����͵õ�ÿ������������������ͼ����ֵ;
		double ave_square = sumMat(powMat(data - active_value3, 2.0)) / (nSample*2.0);


		// (2)���� weight decay;
		double weight_decay = mParams.lambda / 2.0 * (sumMat(powMat(W1, 2)) + sumMat(powMat(W2, 2)));	// ||W1||2  ||W1||2�� �����Ǿ����ڲ�ȫ��Ԫ�����

		// (3)����ϡ����;
		// ������ڵ��ƽ������ֵ;
		Matrix pj = meanMat(active_value2, 0);	// �������ֵ���õ� hiddensize * 1

		
		// ����sparsity;
		Matrix p_rou = repmat(mParams.sparseParam, mnHiddenNum, 1);	//��;
		double s1 = sumMat(multiplyMat(p_rou, logMat(devisionMat(p_rou, pj))));

		Matrix _p_rou = minusMat(1.0, p_rou), _pj = minusMat(1.0, pj);	//1-��, 1-��j
		double s2 = sumMat(multiplyMat(_p_rou, logMat(devisionMat(_p_rou, _pj))));
		double sparsity = mParams.beta*(s1 + s2);

		// ������ʧ����ֵ;
		dCost = ave_square + weight_decay + sparsity;
		cout << "[ " << setprecision(3) << ave_square << "\t" << setw(6) <<weight_decay << "\t" << setw(6) <<sparsity << " ]";


		// ��ƫ���������ݶ�;
		Matrix delta3 = multiplyMat(multiplyMat(active_value3 - data, active_value3), minusMat(1.0, active_value3));	//(a3-x)*[a3*(1-a3)],����Ϊsigmoid��;
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
		// ������̣���ģ��ѵ���õ�����£�����һ����������������������;
		inline Matrix encode(Matrix mat)
		{
			if (mat.RowNo() != mnInputFeatNum || mat.ColNo() != 1)
			{
				cerr << "input size is not legal!\n";
				return mat;
			}
			return sigmoid(mTheta[0] * mat + mTheta[2]);
		}

		// ������෴���������Ӧ����GAN;
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

		// ���ļ��ж�ȡ����Ȩ��;
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
			// ��ȡ W1
			for (int i = 0; i < mnHiddenNum; i++)
			{
				getline(fin, sLine);
				slist.clear();
				slist = split(sLine, ",");
				for (int j = 0; j < mnInputFeatNum; j++)
					mTheta[0](i, j) = atof(slist[j].c_str());
			}
			// ��ȡ W2
			for (int i = 0; i < mnInputFeatNum; i++)
			{
				getline(fin, sLine);
				slist.clear();
				slist = split(sLine, ",");
				for (int j = 0; j < mnHiddenNum; j++)
					mTheta[1](i, j) = atof(slist[j].c_str());
			}
			// ��ȡ b1
			for (int i = 0; i < mnHiddenNum; i++)
			{
				getline(fin, sLine);
				slist.clear();
				slist = split(sLine, ",");
				for (int j = 0; j < 1; j++)
					mTheta[2](i, j) = atof(slist[j].c_str());
			}
			// ��ȡ b2
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

		// д������Ȩ��;
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
	Matrix mMatData;		// �����������mnInputFeatNum*mnSampleNumά
	int mnSampleNum;		// ������Ŀ
	int mnInputFeatNum;		// ���������ά��
	int mnHiddenNum;		// ������ڵ���Ŀ

	TrainingParams mParams;		// ѵ������
	std::vector<Matrix> mTheta;	// ����Ȩ��;
};


#endif // !SAE_H


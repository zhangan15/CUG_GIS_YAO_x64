#ifndef PREPROCESS_H
#define PREPROCESS_H

/***********************************************

�ļ���: preprocess.h
���ߣ� ������
�汾��v1.0
��������Ҫʵ����һЩ matrix.h ��ȱ�ٵľ������㣬�Լ�Ԥ��������
��Ҫʵ�ֵķ�������:
		������ѡȡ(rectMat),����Ԫ�����(sumMat),����Ԫ�����ֵ(meanMat),�����볣����Ӽ�(plusMat, minusMat), �����ӦԪ�����(multiplyMat), �����ӦԪ�����(devisionMat), ����Ԫ������(powMat)�� ����Ԫ����log(logMat)������Ԫ�ظ���(repmat)
		�Լ�Ԥ������������������̬�ֲ��������setMatGaussian�����ɲ�Ŭ���ֲ��������getBernoulliMatrix�����ɾ��ȷֲ��������generateRandomMat������Ԫ�ر�׼��normalizeData����csv��ȡ���ݽ������csv2mat����������д��csv�ļ�csv2mat��������Ԫ��д��ascii�ļ����ַ����ָ�split�� �������Ҿ������cosin

***********************************************/



#include "matrix.h"
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <assert.h>
#include <omp.h>

// ��ȡ�����еĲ���Ԫ����ɵ��Ӿ���ָ�����Ͻ������������Ŀ
Matrix rectMat(Matrix &mat, int ulR, int ulC, int nrow, int ncol)
{
	if (nrow == 0 || ncol == 0)
	{
		cerr << "length of row or col can not be 0!\n";
		return Matrix(0, 0);
	}
	if (ulR + nrow > mat.RowNo() || ulC + ncol > mat.ColNo())
	{
		cerr << "row or col size is out of matrix!\n";
		return Matrix(0, 0);
	}

	Matrix _mat(nrow, ncol);
	for (int i = ulR; i < ulR + nrow; i++)
	{
		for (int j = ulC; j < ulC + ncol; j++)
		{
			_mat(i - ulR, j - ulC) = mat(i, j);
		}
	}
	return _mat;

}

// ��������Ԫ�����
double sumMat(Matrix mat)
{
	double _sum = 0;
	//#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo(); j++)
			_sum += mat(i, j);
	}
	return _sum;
}

// axis = 0��ʾ������ͣ������״Ϊ r*1
// ����     ��ʾ������ͣ������״Ϊ 1*c
Matrix sumMat(Matrix mat, int axis)
{
	if (axis == 0)
	{
		Matrix _summat(mat.RowNo(), 1);
		//#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < mat.RowNo(); i++)
		{
			_summat(i, 0) = 0;
			for (int j = 0; j < mat.ColNo(); j++)
				_summat(i, 0) += mat(i, j);
		}
		return _summat;
	}
	else
	{
		Matrix _summat(1, mat.ColNo());
		//#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < mat.ColNo(); i++)
		{
			_summat(0, i) = 0;
			for (int j = 0; j < mat.RowNo(); j++)
				_summat(0, i) += mat(j, i);

		}
		return _summat;
	}
}

// ��������Ԫ�����ֵ;
double meanMat(Matrix mat)
{
	if (mat.RowNo() != 0 && mat.ColNo() != 0)
		return sumMat(mat) / (mat.RowNo()*mat.ColNo());
	else
		return 0;
}

// axis = 0��ʾ�������ֵ�������״Ϊ r*1
// ����     ��ʾ�������ֵ�������״Ϊ 1*c
Matrix meanMat(Matrix mat, int axis)
{
	Matrix _summat = sumMat(mat, axis);
	if (axis == 0)
	{
		for (int i = 0; i < _summat.RowNo(); i++)
			_summat(i, 0) /= mat.ColNo();
	}
	else
	{
		for (int i = 0; i < _summat.ColNo(); i++)
			_summat(0, i) /= mat.RowNo();
	}
	return _summat;
}

// �����볣�����;
Matrix plusMat(Matrix mat, double val)
{
	Matrix _mat(mat.RowNo(), mat.ColNo());
	//#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo(); j++)
			_mat(i, j) = mat(i, j) + val;
	}
	return _mat;
}

// �������֤���;
Matrix plusMat(double val, Matrix mat)
{
	return plusMat(mat, val);
}

// ������������;
Matrix minusMat(Matrix mat, double val)
{
	Matrix _mat(mat.RowNo(), mat.ColNo());
	//#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo(); j++)
			_mat(i, j) = mat(i, j) - val;
	}
	return _mat;
}

// �����볣�����;
Matrix minusMat(double val, Matrix mat)
{
	return -minusMat(mat, val);
}

// �����ӦԪ�����;
Matrix multiplyMat(Matrix mat1, Matrix mat2)
{
	if (mat1.RowNo() != mat2.RowNo() || mat1.ColNo() != mat2.ColNo())
	{
		cerr << "shape of mat1 and mat2 is not the same!\n";
		return mat1;
	}

	Matrix _mat(mat1.RowNo(), mat1.ColNo());
	//#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat1.RowNo(); i++)
	{
		for (int j = 0; j < mat1.ColNo(); j++)
			_mat(i, j) = mat1(i, j) * mat2(i, j);
	}
	return _mat;
}

// �����ӦԪ�����;
Matrix devisionMat(Matrix mat1, Matrix mat2)
{
	if (mat1.RowNo() != mat2.RowNo() || mat1.ColNo() != mat2.ColNo())
	{
		cerr << "shape of mat1 and mat2 is not the same!\n";
		return mat1;
	}

	Matrix _mat(mat1.RowNo(), mat1.ColNo());
	for (int i = 0; i < mat1.RowNo(); i++)
	{
		for (int j = 0; j < mat1.ColNo(); j++)
		{
			if (abs(mat2(i, j)) < 1e-6)
			{
				//cerr << "zero devision error!\n";
				// ���ʵ��������ֵ�������滻Ϊ��Ӧ���ŵļ�Сֵ;
				if (mat2(i, j) < 0)
					mat2(i, j) = -(1e-6);
				else
					mat2(i, j) = 1e-6;
			}
			_mat(i, j) = mat1(i, j) / mat2(i, j);
		}
	}
	return _mat;
}

// �����ӦԪ�ص���;
Matrix powMat(Matrix mat, double _pow)
{
	Matrix _mat(mat.RowNo(), mat.ColNo());
	//#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo(); j++)
			_mat(i, j) = pow(mat(i, j), _pow);
	}
	return _mat;
}

// �����ӦԪ���� ln;
Matrix logMat(Matrix mat)
{
	Matrix _mat(mat.RowNo(), mat.ColNo());
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo(); j++)
			_mat(i, j) = std::log(mat(i, j));
	}
	return _mat;
}

// ����һ��Ԫ��ֵȫ��Ϊval�ľ���;
Matrix repmat(double val, int r, int c)
{
	Matrix mat(r, c);
	//#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < r; i++)
	{
		for (int j = 0; j < c; j++)
			mat(i, j) = val;
	}
	return mat;
}

// һ�л�һ�и��� c �л� r ��;
Matrix repmat(Matrix mat, int r, int c)
{

	if (mat.RowNo() == 1)	// 1��c�У�����1�и��Ƶ�r��
	{
		Matrix _mat(r, mat.ColNo());
		//#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < r; i++)
		{
			for (int j = 0; j < mat.ColNo(); j++)
				_mat(i, j) = mat(0, j);
		}
		return _mat;
	}
	else if (mat.ColNo() == 1)	// 1��r��,����1�и��Ƶ�c��
	{
		Matrix _mat(mat.RowNo(), c);
		//#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < mat.RowNo(); i++)
		{
			for (int j = 0; j < c; j++)
				_mat(i, j) = mat(i, 0);
		}
		return _mat;
	}
	return mat;
}



// ������̬�ֲ������������;
void setMatGaussian(Matrix &mat)
{
	std::random_device rd;
	mt19937 gen(rd());
	normal_distribution<double> normal(0, 1);	// miu, sigma;
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo(); j++)
		{
			// ������̬�ֲ��������;
			mat(i, j) = normal(gen);
		}
	}
}

// ���ɲ�Ŭ���ֲ��Ķ�ֵ����;
Matrix getBernoulliMatrix(int height, int width, double prob)
{
	Matrix mat(height, width);
	// c++ 11 ��д��;
	//static std::default_random_engine e(time(0));  
	//static std::bernoulli_distribution b(prob); 
#pragma omp parallel for schedule (dynamic)
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++) {
			//bool tmp = b(e);
			//�Ըĵ�д�������ܲ��ܲ���Ԥ�ڵĸ��ʷֲ������ǲ�̫Ӱ��;
			bool tmp = rand() / ((double)RAND_MAX) >= prob;
			if (tmp) mat(i, j) = 1.0;
			else mat(i, j) = 0.0;
		}
	}
	return mat;
}

// ���ݱ�׼�������Ǿ�ֵ0��׼��1
bool normalizeData(Matrix &mat)
{	// ��ÿһ�У������ֵ��ÿ�м�ȥ���еľ�ֵ;
	// �������������ֵ�ı�׼��;

	double dMeanAll = 0;
	// ÿһ�м�ȥ���еľ�ֵ;
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat.ColNo(); i++)
	{
		// ����ÿһ�еľ�ֵ;
		double dmean = 0;	//��i�еľ�ֵ;
		for (int j = 0; j < mat.RowNo(); j++)
			dmean += mat(j, i);
		dmean /= (double)mat.RowNo();

		// ����ֵ��ȥ���еľ�ֵ;
		for (int j = 0; j < mat.RowNo(); j++)
		{
			mat(j, i) -= dmean;
			dMeanAll += mat(j, i);
		}
	}
	// ȫ��Ԫ�صľ�ֵ;
	dMeanAll /= ((double)(mat.RowNo()*mat.ColNo()));

	double dStd = 0;
	for (int i = 0; i < mat.RowNo(); i++)
		for (int j = 0; j < mat.ColNo(); j++)
			dStd += std::pow(mat(i, j) - dMeanAll, 2);

	// �����׼��;
	dStd = std::sqrt(dStd / (double)(mat.RowNo()*mat.ColNo()));
	if (abs(dStd) < 1e-6)
	{
		cerr << "1 error occurred while normalizing: std = 0\n";
		return false;
	}

	// ��׼��;
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo(); j++)
		{
			mat(i, j) = max(min(mat(i, j), dStd), -dStd) / dStd;
			//mat(i, j) = (mat(i, j) + 1) * 0.4 + 0.1;
		}
	}

	return true;
}



std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}


// �� csv �ж�ȡ���鵽matrix����״����;(Matrix�Ĵ�С��nFeats*nSamples)
Matrix csv2mat(std::string fn, int nFeats, int nSamples)
{
	cout << "load matrix...";

	ifstream fin(fn.c_str());
	assert(fin.good());
	std::string sLine;
	std::vector<std::string> slist;
	int i = 0;
	Matrix mat(nFeats, nSamples);
	while (getline(fin, sLine))
	{
		slist.clear();
		slist = split(sLine, ",");
		for (int j = 0; j < nSamples; j++)
			mat(i, j) = atof(slist[j].c_str());
		i++;
	}
	fin.close();
	cout << "success!\n";
	return mat;
}

// �� ����csv �ж�ȡ���鵽matrix����״����;(Matrix�Ĵ�С��nSamples*nFeats)
Matrix normal_csv2mat(std::string fn, int nSamples, int nFeats )
{
	cout << "load matrix...";

	ifstream fin(fn.c_str());
	assert(fin.good());
	std::string sLine;
	std::vector<std::string> slist;
	int i = 0;
	Matrix mat(nFeats, nSamples);
	while (getline(fin, sLine))
	{
		slist.clear();
		slist = split(sLine, ",");
		for (int j = 0; j < nFeats; j++)
			mat(j, i) = atof(slist[j].c_str());
		i++;
	}
	fin.close();
	cout << "success!\n";
	return mat;
}

// matrix д�� csv �ļ�
bool mat2csv(Matrix mat, std::string sOutFn)
{
	std::ofstream fout(sOutFn.c_str());
	if (!fout.good())
	{
		std::cerr << "save weights to file fail!\n";
		return false;
	}
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo() - 1; j++)
		{
			fout << mat(i, j) << ",";
		}
		fout << mat(i, mat.ColNo() - 1) << "\n";
	}
	fout.close();

	return true;
}

// matrix д�� ascii �ļ�
bool mat2asc(Matrix mat, std::string sOutFn)
{
	std::ofstream fout(sOutFn.c_str());
	if (!fout.good())
	{
		std::cerr << "save weights to file fail!\n";
		return false;
	}
	fout << "NCOLS " << mat.RowNo() << "\n" << "NROWS " << mat.ColNo() << "\n";
	fout << "XLLCORNER 0\nYLLCORNER 0\nCELLSIZE 1\nNODATA_VALUE - 1\n";

	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo() - 1; j++)
		{
			fout << mat(i, j) << " ";
		}
		fout << mat(i, mat.ColNo() - 1) << "\n";
	}
	fout.close();

	return true;
}

// �������������� cosin ֵ�� ֵԽ�����Խ����;
double cosin(Matrix v1, Matrix v2)
{
	if (v1.ColNo() != 1 || v2.ColNo() != 1)
	{
		cerr << "col of vector is not 1!\n";
		return -1;
	}
	double dv1v2 = sumMat(multiplyMat(v1, v2));

	double dv1 = sqrt(sumMat(powMat(v1, 2)));
	double dv2 = sqrt(sumMat(powMat(v2, 2)));

	return dv1v2 / (dv1*dv2);
}

// ������������ɵľ���;
Matrix generateRandomMat(int _nr = 1, int _nc = 1, double _min = 0, double _max = 256)
{
	Matrix mat(_nr, _nc);
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < _nr; i++)
	{
		for (int j = 0; j < _nc; j++)
		{
			mat(i, j) = (double)rand() / RAND_MAX * (_max - _min) + _min;
		}
	}
	return mat;
}


#endif

#ifndef PREPROCESS_H
#define PREPROCESS_H

/***********************************************

文件名: preprocess.h
作者： 刘鹏华
版本：v1.0
描述：主要实现了一些 matrix.h 中缺少的矩阵运算，以及预处理方法，
主要实现的方法包括:
		矩阵按列选取(rectMat),矩阵元素求和(sumMat),矩阵元素求均值(meanMat),矩阵与常数相加减(plusMat, minusMat), 矩阵对应元素相乘(multiplyMat), 矩阵对应元素相除(devisionMat), 矩阵元素求幂(powMat)， 矩阵元素求log(logMat)，矩阵元素复制(repmat)
		以及预处理方法，例如生成正态分布的随机数setMatGaussian，生成伯努利分布的随机数getBernoulliMatrix，生成均匀分布的随机数generateRandomMat，矩阵元素标准化normalizeData，从csv读取数据进入矩阵csv2mat，矩阵数据写入csv文件csv2mat，，矩阵元素写入ascii文件，字符串分割split， 向量余弦距离计算cosin

***********************************************/



#include "matrix.h"
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <assert.h>
#include <omp.h>

// 获取矩阵中的部分元素组成的子矩阵，指定左上角坐标和行列数目
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

// 矩阵所有元素求和
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

// axis = 0表示按行求和，结果形状为 r*1
// 否则     表示按列求和，结果形状为 1*c
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

// 矩阵所有元素求均值;
double meanMat(Matrix mat)
{
	if (mat.RowNo() != 0 && mat.ColNo() != 0)
		return sumMat(mat) / (mat.RowNo()*mat.ColNo());
	else
		return 0;
}

// axis = 0表示按行求均值，结果形状为 r*1
// 否则     表示按列求均值，结果形状为 1*c
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

// 矩阵与常数相加;
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

// 常数与举证相加;
Matrix plusMat(double val, Matrix mat)
{
	return plusMat(mat, val);
}

// 常数与矩阵相减;
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

// 矩阵与常数相减;
Matrix minusMat(double val, Matrix mat)
{
	return -minusMat(mat, val);
}

// 矩阵对应元素相乘;
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

// 矩阵对应元素相除;
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
				// 如果实在碰到零值，则将零替换为对应符号的极小值;
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

// 矩阵对应元素的幂;
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

// 矩阵对应元素求 ln;
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

// 生成一个元素值全部为val的矩阵;
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

// 一行或一列复制 c 列或 r 行;
Matrix repmat(Matrix mat, int r, int c)
{

	if (mat.RowNo() == 1)	// 1行c列，将这1行复制到r行
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
	else if (mat.ColNo() == 1)	// 1列r行,将这1列复制到c列
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



// 生成正态分布的随机数矩阵;
void setMatGaussian(Matrix &mat)
{
	std::random_device rd;
	mt19937 gen(rd());
	normal_distribution<double> normal(0, 1);	// miu, sigma;
	for (int i = 0; i < mat.RowNo(); i++)
	{
		for (int j = 0; j < mat.ColNo(); j++)
		{
			// 服从正态分布的随机数;
			mat(i, j) = normal(gen);
		}
	}
}

// 生成伯努利分布的二值矩阵;
Matrix getBernoulliMatrix(int height, int width, double prob)
{
	Matrix mat(height, width);
	// c++ 11 的写法;
	//static std::default_random_engine e(time(0));  
	//static std::bernoulli_distribution b(prob); 
#pragma omp parallel for schedule (dynamic)
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++) {
			//bool tmp = b(e);
			//自改的写法，可能不能产生预期的概率分布，但是不太影响;
			bool tmp = rand() / ((double)RAND_MAX) >= prob;
			if (tmp) mat(i, j) = 1.0;
			else mat(i, j) = 0.0;
		}
	}
	return mat;
}

// 数据标准化，并非均值0标准差1
bool normalizeData(Matrix &mat)
{	// 对每一列，计算均值，每列减去该列的均值;
	// 计算矩阵中所有值的标准差;

	double dMeanAll = 0;
	// 每一列减去该列的均值;
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < mat.ColNo(); i++)
	{
		// 计算每一列的均值;
		double dmean = 0;	//第i列的均值;
		for (int j = 0; j < mat.RowNo(); j++)
			dmean += mat(j, i);
		dmean /= (double)mat.RowNo();

		// 矩阵值减去该列的均值;
		for (int j = 0; j < mat.RowNo(); j++)
		{
			mat(j, i) -= dmean;
			dMeanAll += mat(j, i);
		}
	}
	// 全部元素的均值;
	dMeanAll /= ((double)(mat.RowNo()*mat.ColNo()));

	double dStd = 0;
	for (int i = 0; i < mat.RowNo(); i++)
		for (int j = 0; j < mat.ColNo(); j++)
			dStd += std::pow(mat(i, j) - dMeanAll, 2);

	// 计算标准差;
	dStd = std::sqrt(dStd / (double)(mat.RowNo()*mat.ColNo()));
	if (abs(dStd) < 1e-6)
	{
		cerr << "1 error occurred while normalizing: std = 0\n";
		return false;
	}

	// 标准化;
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


// 从 csv 中读取数组到matrix，形状不变;(Matrix的大小：nFeats*nSamples)
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

// 从 正序csv 中读取数组到matrix，形状不变;(Matrix的大小：nSamples*nFeats)
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

// matrix 写入 csv 文件
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

// matrix 写入 ascii 文件
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

// 计算两个向量的 cosin 值， 值越大表明越相似;
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

// 生成随机数构成的矩阵;
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

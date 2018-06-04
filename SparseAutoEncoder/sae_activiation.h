#ifndef SAE_ACTIVIATION_H
#define SAE_ACTIVIATION_H

/***********************************************
文件名: sae_activiation.h
作者： 刘鹏华
版本：v1.0
描述：实现了两类激活函数(sigmoid 和 tanh)的求值和求导;
主要实现的方法包括:
sigmoid, tanh, d_sigmoid, d_tanh

***********************************************/

#include "matrix.h"
#include <cmath>


	/*sigmoid激活函数*/
	double sigmoid(double _x)
	{
		return 1.0 / (1.0 + std::exp(-_x));
	}


	Matrix sigmoid(Matrix _X)
	{
		int nRow = (int)_X.RowNo(), nCol = (int)_X.ColNo();
		Matrix sig_val(nRow, nCol);
		for (int i = 0; i < nRow; i++)
			for (int j = 0; j < nCol; j++)
				sig_val(i, j) = sigmoid(_X(i, j));
		return sig_val;
	}


	double d_sigmoid(double _x)
	{
		return sigmoid(_x)*(1.0-sigmoid(_x));
	}

	Matrix d_sigmoid(Matrix _X)
	{
		int nRow = (int)_X.RowNo(), nCol = (int)_X.ColNo();
		Matrix d_sig_val(nRow, nCol);
		for (int i = 0; i < nRow; i++)
			for (int j = 0; j < nCol; j++)
				d_sig_val(i, j) = d_sigmoid(_X(i, j));
		return d_sig_val;
	}


	// tanh 定义域为 R, 值域为(-1, 1)，与sigmoid相似
	Matrix tanh(Matrix _X)
	{
		int nRow = (int)_X.RowNo(), nCol = (int)_X.ColNo();
		Matrix tanh_val(nRow, nCol);
		for (int i = 0; i < nRow; i++)
			for (int j = 0; j < nCol; j++)
				tanh_val(i, j) = tanh(_X(i, j));
		return tanh_val;
	}

	double d_tanh(double _x)
	{
		return 1 - std::pow(tanh(_x), 2);
	}

	Matrix d_tanh(Matrix _X)
	{
		int nRow = (int)_X.RowNo(), nCol = (int)_X.ColNo();
		Matrix d_tanh_val(nRow, nCol);
		for (int i = 0; i < nRow; i++)
			for (int j = 0; j < nCol; j++)
				d_tanh_val(i, j) = d_tanh(_X(i, j));
		return d_tanh_val;
	}

#endif // ACTIVIATION_H

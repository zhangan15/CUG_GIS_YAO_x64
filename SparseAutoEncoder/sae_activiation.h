#ifndef SAE_ACTIVIATION_H
#define SAE_ACTIVIATION_H

/***********************************************
�ļ���: sae_activiation.h
���ߣ� ������
�汾��v1.0
������ʵ�������༤���(sigmoid �� tanh)����ֵ����;
��Ҫʵ�ֵķ�������:
sigmoid, tanh, d_sigmoid, d_tanh

***********************************************/

#include "matrix.h"
#include <cmath>


	/*sigmoid�����*/
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


	// tanh ������Ϊ R, ֵ��Ϊ(-1, 1)����sigmoid����
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

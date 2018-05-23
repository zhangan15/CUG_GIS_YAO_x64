#include "RFWeights.h"
#include <time.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdio>
#include <QtCore>
using namespace std;
using namespace CRFWeights;
using namespace alglib;
bool CRFWeights::calRFWeights(real_2d_array dataset, decisionforest *pDF, real_1d_array& RFWeights,int rf_type) 
{
	//////////////////////////文件路径/////////////////////////////////////
	//QString filename = "../Weights.csv";
	RFWeights.setlength(dataset.cols()-1);
	real_1d_array _calRFWeights;
	real_2d_array testarr;
	testarr.setlength(dataset.rows(),dataset.cols());
	for (int i=0; i<testarr.rows(); i++)
	{
		for (int j=0; j<testarr.cols(); j++)
		{
			testarr[i][j]= dataset[i][j];
		}
	}

	///////////////////////////计算总误分///////////////////////////////////可以不需要/////
	decisionforest *_pDF;
	_pDF = pDF;
	//decisionforest DF = *_pDF;
	if (rf_type == 0)
	{

		real_1d_array dst;
		int j;  
		int ncount = 0;
		double daccurcy = 0;

		for (int i=0; i<testarr.rows(); i++)
		{
			real_1d_array prearr;
			prearr.setlength(testarr.cols()-1);

			for (j=0; j<testarr.cols()-1; j++)
			{
				prearr[j] = testarr[i][j];
			}

			dfprocess(*_pDF, prearr, dst);
			int np = 0;
			double _dmax = 0;
			int _nflag = 0;

			for (np=0;np<dst.length();np++)
			{	   
				if(_dmax<dst[np])
				{
					_dmax=dst[np];
					_nflag=np;
				}

			}
			if((_nflag)!= testarr[i][testarr.cols()-1])
				ncount++;        
		}


		///////////////////////////计算每一组误分///////////////////////////////////
		_pDF = pDF;
		ncount=0;
		int nk;
		real_1d_array ndst;
		double* dcount = new double[testarr.cols()];
		for (int _reg = 0;_reg<testarr.cols()-1;_reg++)
		{
			dcount [_reg]= 0;
			for (int i=0;i<testarr.rows();i++)
			{
				real_1d_array changearr;
				changearr.setlength(testarr.cols());
				for (nk=0; nk<=testarr.cols()-1; nk++)
				{
					changearr[nk] = testarr[i][nk];
					changearr[_reg]=0;
				}

				dfprocess(*_pDF, changearr, ndst);

				int np = 0;
				double _nmax = 0;
				int _nflag = 0;
				for (np=0;np<ndst.length();np++)
				{	   
					if(_nmax<ndst[np])
					{
						_nmax=ndst[np];
						_nflag=np;
					}

				}
				if((_nflag)!= testarr[i][testarr.cols()-1])//dataset[i][testarr.cols()+1])
					dcount[_reg]++; 
			}
		}


		///////////////////////////计算特征向量贡献度///////////////////////////////////
		int _nSum = 0;
		double stdgy = 0;
		int i = 0;

		for (i=0;i<testarr.cols();i++)
		{
			_nSum+=dcount[i];
		}

		/////////////////////////归一化//////////////////////////////////////////////////
		for(i=0;i<testarr.cols()-1;i++)
		{
			double _dcount = dcount[i];
			stdgy = _dcount/_nSum;
			_calRFWeights[i] = stdgy;
			//cout<<setprecision(3)<<stdgy<<"\t";
		}

		for (i=0;i<testarr.cols();i++)
		{
			RFWeights[i] = _calRFWeights[i];
		}

		//cout<<endl;

		delete [] dcount;
	}
	if (rf_type == 1)
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		real_1d_array dst;
		int i,j,k;  
		int ncount = 0;
		double daccurcy = 0;
		double dchangeerr = 0;
		//double dresulterr=0;
		for (i=0; i<testarr.rows(); i++)
		{
			real_1d_array prearr;
			prearr.setlength(testarr.cols()-1);

			for (j=0; j<testarr.cols()-1; j++)
			{
				prearr[j] = testarr[i][j];
			}

			dfprocess(*_pDF, prearr, dst);

			double val0 = dst[0];
			double val1 = dataset[i][dataset.cols()-1];

// 			if (i == 1603)
// 			{
// 				cout<<"wait"<<endl;
// 			}

			dchangeerr+=sqrt((val0-val1)*(val0-val1));

//			cout<<i<<"  =   "<<dchangeerr<<endl;
// 			if (i==testarr.rows()-1)
// 			{
// 				dresulterr = dchangeerr;
// 				cout<<dresulterr<<endl;
// 			}

		}
		//////////////////////////////////预测//////////////////////////////////////

		//double* dErrRst = new double [testarr.cols()];
		real_1d_array ndst;
		double dchange_errarr = 0;
		real_1d_array _change_errarr;
		double max = 0;
		double min = 0;
		_change_errarr.setlength(dataset.cols() - 1);

		real_1d_array max_var, min_var;
		max_var.setlength(dataset.cols() - 1);
		min_var.setlength(dataset.cols() - 1);

		qsrand(QTime::currentTime().msec());

		for (i=0; i<dataset.cols()-1; i++)
		{
			max_var[i] = dataset[0][i];
			min_var[i] = dataset[0][i];

			for (j=0; j<dataset.rows(); j++)
			{
				if (dataset[j][i] > max_var[i])
					max_var[i] = dataset[j][i];
				if (dataset[j][i] < min_var[i])
					min_var[i] = dataset[j][i];
			}
		}

		real_1d_array prearray;
		prearray.setlength(dataset.cols() - 1);

		for (k=0; k<dataset.cols() - 1; k++)
		{
			//cout<<"calculating No. "<<k+1<<" / "<<dataset.cols()-1<<endl;
			dchange_errarr = 0;
			for (i=0; i<dataset.rows(); i++)
			{
				for (j=0; j<dataset.cols()-1; j++)
				{
					prearray[j] = dataset[i][j];
					prearray[k] += (double)pow((double)-1.0f, (double)qrand())*(min_var[k]+(double)max_var[k]*qrand()/((double)RAND_MAX+1));
					//prearray[k] = 0;
				}

				dfprocess(*pDF, prearray, ndst);
				dchange_errarr+=sqrt((pow(abs(ndst[0]-dataset[i][dataset.cols()-1]),2)));
			}

			
			_change_errarr[k] = abs(dchange_errarr - dchangeerr);

		}
		

// 		for (int j = 0;j<testarr.cols()-1;j++)
// 		{
// 
// 			dchange_errarr =0;
// 			for (int i=0;i<testarr.rows();i++)
// 			{
// 
// 				real_1d_array changearr;
// 				changearr.setlength(testarr.cols()-1);
// 				for (int k=0; k<=testarr.cols()-1; k++)
// 				{
// 					max= changearr[0];
// 					changearr[k] = testarr[i][k];
// 					min = changearr[0];
// 					for (int _p=0;_p<testarr.rows();_p++)
// 					{	   
// 						if(max<testarr[_p][k])
// 						{
// 							max=testarr[_p][k];
// 						}
// 
// 						if(min>testarr[_p][k])
// 						{
// 							min=testarr[_p][k];
// 						}
// 
// 					}
// 
// 					changearr[k]= min+(double)max*rand()/((double)RAND_MAX+1);
// 				}
// 
// 				dfprocess(*pDF, changearr, ndst);
// 
// 				dchange_errarr+=sqrt((pow(abs(ndst[0]-dataset[i][dataset.cols()-1]),2)));
// 
// 				if (i==testarr.rows()-1)
// 				{   
// 					_change_errarr[j] = (dchange_errarr - dchangeerr)/50;
// 					//cout<<"*********"<<dchange_errarr<<endl;
// 				}
// 
// 
// 			}
// 
// 		}





		int _nSum = 0;
		double stdgy = 0;
		for (int i=0;i<testarr.cols()-1;i++)
		{
			_nSum+=_change_errarr[i];
		}

		//QString str;
		//str = QString("%1").arg("各特征变量贡献度分别为:");
		//txtoutput<<str<<"\n";
		for(int i=0;i<testarr.cols()-1;i++)
		{
			double _dcount = _change_errarr[i];
			stdgy = _dcount/_nSum;
			 //cout<<i<<" = "<<stdgy<<endl;

			RFWeights[i] = stdgy;
		}
		//cout<<endl;
	}
	return true;
}

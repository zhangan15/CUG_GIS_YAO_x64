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
			prearr.setlength(testarr.cols());

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
			cout<<setprecision(3)<<stdgy<<"\t";
		}

		for (i=0;i<testarr.cols();i++)
		{
			RFWeights[i] = _calRFWeights[i];
		}

		cout<<endl;

		delete [] dcount;
	}
	if (rf_type == 1)
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		real_1d_array dst;
		int j;  
		int ncount = 0;
		double daccurcy = 0;
		double dchangeerr = 0;
		double dresulterr=0;
		for (int i=0; i<testarr.rows(); i++)
		{
			real_1d_array prearr;
			prearr.setlength(testarr.cols());

			for (j=0; j<testarr.cols()-1; j++)
			{
				prearr[j] = testarr[i][j];
			}

			dfprocess(*_pDF, prearr, dst);
			dchangeerr+=sqrt(pow(dst[0]-testarr[i][24],2));
			if (i==testarr.rows()-1)
			{
				dresulterr = dchangeerr;
				cout<<dresulterr<<endl;
			}

		}
		//////////////////////////////////预测//////////////////////////////////////

		double* dErrRst = new double [testarr.cols()];
		real_1d_array ndst;
		double dchange_errarr = 0;
		real_1d_array _change_errarr;
		double max = 0;
		double min = 0;
		_change_errarr.setlength(testarr.cols());
		for (int j = 0;j<testarr.cols()-1;j++)
		{

			dchange_errarr =0;
			for (int i=0;i<testarr.rows();i++)
			{

				real_1d_array changearr;
				changearr.setlength(testarr.cols());
				for (int k=0; k<=testarr.cols()-1; k++)
				{
					max= 0;
					changearr[k] = testarr[i][k];
					min = changearr[0];
					for (int _p=0;_p<testarr.rows();_p++)
					{	   
						if(max<changearr[_p])
						{
							max=changearr[_p];
						}

					}
					for (int _p=0;_p<testarr.rows();_p++)
					{	   
						if(min>changearr[_p])
						{
							min=changearr[_p];
						}

					}
					changearr[j]=min+(double)max*rand()/((double)RAND_MAX+1);
				}

				dfprocess(*pDF, changearr, ndst);

				dchange_errarr+=sqrt((pow(abs(ndst[0]-testarr[i][24]),2)));

				if (i==testarr.rows()-1)
				{   
					_change_errarr[j] = (dchange_errarr - dchangeerr)/50;
					cout<<"*********"<<dchange_errarr<<endl;
				}


			}

		}
		for (int i=0;i<testarr.cols()-1;i++)
		{
			cout<<setprecision(8)<<_change_errarr[i]<<endl;
		}
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
			cout<<setprecision(3)<<stdgy<<"\t";
			// QString _str = QString("%1,%2").arg(stdgy).arg("\t");
			//txtoutput<<_str;
		}
		cout<<endl;
	}
	return true;
}

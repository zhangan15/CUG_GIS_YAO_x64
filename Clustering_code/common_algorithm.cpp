#include "common_algorithm.h"
#include <iostream>
#include <sstream>  
#include <fstream>  
#include <time.h>
#include <math.h>  
#include <stdlib.h>
#include <QtCore>
#include <omp.h>
using namespace std;

#ifndef Min(a,b)
#define Min(a,b) (a<b?a:b)
#endif

#define ID_TRAN_KMEANS	-1
#define ID_DTW_KMEANS	-2
#define ID_COS_KMEANS	-3

QList<int> vID;
float** ppDistance = NULL;
QList<float> vSilVals;	//��������

int num_threads = omp_get_max_threads() - 2;

double DTW_distance(double* pArr1, double* pArr2, int nCount)
{
	int i, j, k;

	double** distance = new double*[nCount + 1];
	for (i = 0; i < nCount + 1; i++)
	{
		distance[i] = new double[nCount + 1];
		memset(distance[i], 0, (nCount + 1) * sizeof(double));
	}


	double** output = new double*[nCount + 1];
	for (i = 0; i < nCount + 1; i++)
	{
		output[i] = new double[nCount + 1];
		memset(output[i], 0, (nCount + 1) * sizeof(double));
	}

	for (i = 1; i <= nCount; i++)
		for (j = 1; j <= nCount; j++)
			distance[i][j] = (pArr2[j - 1] - pArr1[i - 1])*(pArr2[j - 1] - pArr1[i - 1]);    //��������֮���ŷʽ����

																							 //����ע�͵�, �������ŷʽ����ľ���
																							 // 	for(i=1;i<=nCount;i++)
																							 // 	{
																							 // 		for(j=1;j<=nCount;j++)
																							 // 			cout<<distance[i][j]<<'\t';
																							 // 		cout<<endl;
																							 // 	} 
																							 // 	cout<<endl;

																							 //DP���̣�����DTW����
	for (i = 1; i <= nCount; i++)
		for (j = 1; j <= nCount; j++)
		{
			double _minval = Min(Min(output[i][j - 1], output[i - 1][j]), output[i - 1][j - 1]);

			if (fabs(_minval - output[i - 1][j - 1]) <= 1e-5 && \
				fabs(output[i][j - 1] - output[i - 1][j - 1]) >= 1e-5 && \
				fabs(output[i - 1][j] - output[i - 1][j - 1]) >= 1e-5)
				output[i][j] = _minval + 2 * distance[i][j];	//б���2
			else
				output[i][j] = _minval + distance[i][j];
			//			output[i][j]=Min(Min(distance[i-1][j-1],distance[i][j-1]),distance[i-1][j])+distance[i][j];
		}

	//�������DTW�����������output[NUM][NUM]Ϊ���յ�DTW�����
	// 	for(i=0;i<=nCount;i++)
	// 	{
	// 		for(j=0;j<=nCount;j++)
	// 			cout<<output[i][j]<<'\t';
	// 		cout<<endl;
	// 	} 
	// 	cout<<endl;

	//DTW����
	double dval = output[nCount][nCount];

	// 	cout<<"DTW distance = "<<dval<<endl;
	// 	cout<<endl;

	//����ڴ�
	for (i = 0; i < nCount + 1; i++)
		delete[]distance[i];
	delete[]distance;

	for (i = 0; i < nCount + 1; i++)
		delete[]output[i];
	delete[]output;

	//����DTW������
	return dval;
}

double DTW_distance(real_1d_array pArr1, real_1d_array pArr2)
{
	int i, j, k;

	if (pArr1.length() != pArr2.length())
		return 9999;

	int nCount = pArr1.length();

	real_2d_array distance;
	distance.setlength(nCount + 1, nCount + 1);

	real_2d_array output;
	output.setlength(nCount + 1, nCount + 1);

	for (i = 0; i < nCount + 1; i++)
	{
		for (j = 0; j < nCount + 1; j++)
		{
			distance[i][j] = 0;
			output[i][j] = 0;
		}
	}

	// 	double** output = new double*[nCount+1];
	// 	for (i=0; i<nCount+1; i++)
	// 	{
	// 		output[i] = new double[nCount+1];
	// 		memset(output[i], 0, (nCount+1)*sizeof(double));
	// 	}

	for (i = 1; i <= nCount; i++)
		for (j = 1; j <= nCount; j++)
			distance[i][j] = (pArr2[j - 1] - pArr1[i - 1])*(pArr2[j - 1] - pArr1[i - 1]);    //��������֮���ŷʽ����

																							 //����ע�͵�, �������ŷʽ����ľ���
																							 // 	for(i=1;i<=nCount;i++)
																							 // 	{
																							 // 		for(j=1;j<=nCount;j++)
																							 // 			cout<<distance[i][j]<<'\t';
																							 // 		cout<<endl;
																							 // 	} 
																							 // 	cout<<endl;

																							 //DP���̣�����DTW����
	for (i = 1; i <= nCount; i++)
		for (j = 1; j <= nCount; j++)
		{
			double _minval = Min(Min(output[i][j - 1], output[i - 1][j]), output[i - 1][j - 1]);

			if (fabs(_minval - output[i - 1][j - 1]) <= 1e-5 && \
				fabs(output[i][j - 1] - output[i - 1][j - 1]) >= 1e-5 && \
				fabs(output[i - 1][j] - output[i - 1][j - 1]) >= 1e-5)
				output[i][j] = _minval + 2 * distance[i][j];	//б���2
			else
				output[i][j] = _minval + distance[i][j];
			//output[i][j]=Min(Min(distance[i-1][j-1],distance[i][j-1]),distance[i-1][j])+distance[i][j];
		}

	//�������DTW�����������output[NUM][NUM]Ϊ���յ�DTW�����
	// 	for(i=0;i<=nCount;i++)
	// 	{
	// 		for(j=0;j<=nCount;j++)
	// 			cout<<output[i][j]<<'\t';
	// 		cout<<endl;
	// 	} 
	// 	cout<<endl;

	//DTW����
	double dval = output[nCount][nCount];

	// 	cout<<"DTW distance = "<<dval<<endl;
	// 	cout<<endl;
	
	//����ڴ�
	// 		for (i=0; i<nCount+1; i++)
	// 			delete []distance[i];
	// 		delete []distance;
	// 
	// 		for (i=0; i<nCount+1; i++)
	// 			delete []output[i];
	// 		delete []output;

	//����DTW������
	return dval;
}

// int dataNum;//���ݼ������ݼ�¼��Ŀ  
// int dimNum;//ÿ����¼��ά��  

//��������Ԫ����ŷ�������  
double getDistXY(const Tuple& t1, const Tuple& t2, int disttype)
{
	if (disttype == ID_TRAN_KMEANS)
	{
		double sum = 0;
		for (int i = 1; i < t1.size(); ++i)
		{
			sum += (t1[i] - t2[i]) * (t1[i] - t2[i]);
		}
		return sqrt(sum);
	}
	else if (disttype == ID_COS_KMEANS)
	{
		double sum = 0;
		double sum1_2 = 0;
		double sum2_2 = 0;
		for (int i = 1; i < t1.size(); ++i)
		{
			sum += t1[i] * t2[i];
			sum1_2 += t1[i] * t1[i];
			sum2_2 += t2[i] * t2[i];
		}
		
		return 1 - sum / (sqrt(sum1_2)*sqrt(sum2_2));
	}
	else if (disttype == ID_DTW_KMEANS)
	{
		int nCount = t1.size() - 1;
		double* parr1 = new double[nCount];
		double* parr2 = new double[nCount];

		for (int i = 0; i < nCount; i++)
		{
			parr1[i] = t1[i + 1];
			parr2[i] = t2[i + 1];
		}

		double dval = DTW_distance(parr1, parr2, nCount);

		delete[]parr1;
		delete[]parr2;

		return dval;

		// 		int nCount = t1.size() - 1;
		// 		//double* parr1 = new double[nCount];
		// 		//double* parr2 = new double[nCount];
		// 		real_1d_array parr1, parr2;
		// 		parr1.setlength(nCount);
		// 		parr2.setlength(nCount);
		// 
		// 		for (int i = 0; i<nCount; i++)
		// 		{
		// 			parr1[i] = t1[i+1];
		// 			parr2[i] = t2[i+1];
		// 		}
		// 
		// 		double dval = DTW_distance(parr1, parr2);
		// 
		// 		return dval;

	}
	else
	{
		cout << "unkown distance type." << endl;
		return -1;
	}
}

double getDistXY_fast(const Tuple& t1, const Tuple& t2)
{
	

	//��ȡt1λ��
	int i = vID.indexOf(t1[0]);
	int j = vID.indexOf(t2[0]);

	float val = i >= j ? ppDistance[i][j] : ppDistance[j][i];
	
	return val;
}

void computeAllDistances(vector<Tuple>& tuples, int ndisttype, const char* soutputFilename)
{
	//�õ�ѵ�����ݵ�λ��
	int i, j;
	int nDataCount = tuples.size();	//��������
	vID.clear();
	for (i = 0; i < nDataCount; i++)
	{
		vID.push_back(tuples[i][0]);
	}

	if (ppDistance != NULL)
	{
		//�ͷ��ڴ�
		for (i = 0; i < nDataCount; i++)
		{
			delete[]ppDistance[i];
			ppDistance[i] = NULL;
		}
		delete[]ppDistance;
	}

	//���ڼ������
	ppDistance = new float*[nDataCount];
	for (i = 0; i < nDataCount; i++)
	{
		ppDistance[i] = new float[i + 1];
		memset(ppDistance[i], 0, (i + 1) * sizeof(float));
	}

	//��ʼ����
	//int num_threads = omp_get_max_threads();

#pragma omp parallel for num_threads(num_threads<(nDataCount-1)?num_threads:(nDataCount-1)) schedule(dynamic) private(j)
	for (i = 0; i < nDataCount; i++)
	{
		if ((i + 1) % 100 == 0)
			cout << "computing distance - No. " << i + 1 << " / " << nDataCount << "..." << endl;

		for (j = 0; j <= i; j++)
		{
			double val = getDistXY(tuples[i], tuples[j], ndisttype);
			ppDistance[i][j] = val;
			//ppDistance[j][i] = val;
		}
	}

	cout << "computing all distance done." << endl;
	//д���ļ�
	cout << "write distance dataset into file..." << endl;
	QFile _file(soutputFilename);
	if (!_file.open(QIODevice::WriteOnly))
	{
		cout << "write distance dataset failed." << endl;
		return;
	}
	QTextStream _in(&_file);
	//��һ����Ŀ
	_in << tuples.size() << "\r\n";
	//�ڶ���disttype
	_in << ndisttype << "\r\n";
	//������ids
	for (i = 0; i < vID.size(); i++)
	{
		if (i != vID.size() - 1)
			_in << vID[i] << ",";
		else
			_in << vID[i] << "\r\n";
	}

	//������д��distance
	for (i = 0; i < nDataCount; i++)
	{
		if ((i + 1) % 100 == 0)
			cout << "writing distance - No. " << i + 1 << " / " << nDataCount << "..." << endl;

		for (j = 0; j <= i; j++)
		{
			if (j != i)
				_in << ppDistance[i][j] << ",";
			else
				_in << ppDistance[i][j];
		}
		_in << "\r\n";
	}


	_file.flush();
	_file.close();
	cout << "write distance dataset success." << endl;

}

bool computeAllDistances_fromfile(vector<Tuple>& tuples, int ndisttype, const char* sDistFilename)
{
	//�ļ���һ������Ŀ���ڶ�����disttype�������������е�id�������п�ʼ�Ǿ���
	QFile _file(sDistFilename);
	if (!_file.open(QIODevice::ReadOnly))
	{
		cout << "open distance file error." << endl;
		return false;
	}
	QTextStream _out(&_file);

	QString smsg = _out.readLine();
	long nLineCount = smsg.toInt();
	smsg = _out.readLine();
	long nInDistType = smsg.toInt();

	if (nLineCount != tuples.size() || nInDistType != ndisttype)
	{
		cout << "dataset size or distance type are not matched. error." << endl;
		_file.close();
		return false;
	}

	int i, j;
	///////////////////////////////
	//�ж�id�Ƿ�һ��
	smsg = _out.readLine();
	QStringList slist = smsg.split(",", QString::SkipEmptyParts);
	if (slist.size() != tuples.size())
	{
		cout << "list size error." << endl;
		_file.close();
		return false;
	}

	vID.clear();
	for (int i = 0; i < tuples.size(); i++)
	{
		if (slist[i].toInt() != int(tuples[i][0]))
		{
			cout << "list id error." << endl;
			_file.close();
			return false;
		}
		vID.push_back(tuples[i][0]);
	}

	//��ʼ��������
	if (ppDistance != NULL)
	{
		//�ͷ��ڴ�
		for (i = 0; i < nLineCount; i++)
		{
			delete[]ppDistance[i];
			ppDistance[i] = NULL;
		}
		delete[]ppDistance;
	}

	//���ڼ������
	ppDistance = new float*[nLineCount];
	for (i = 0; i < nLineCount; i++)
	{
		ppDistance[i] = new float[i + 1];
		memset(ppDistance[i], 0, (i + 1) * sizeof(float));
	}

	//��ʼ����
	//int num_threads = omp_get_max_threads();

	for (i = 0; i < nLineCount; i++)
	{
		if ((i + 1) % 100 == 0)
			cout << "loading distance - No. " << i + 1 << " / " << nLineCount << "..." << endl;

		smsg = _out.readLine();
		slist = smsg.split(",");

		if (slist.size() != i + 1)
		{
			cout << "dataset error." << endl;
			_file.close();
			return false;
		}

		for (j = 0; j <= i; j++)
		{
			ppDistance[i][j] = slist[j].toDouble();
			//ppDistance[j][i] = val;
		}
	}

	//д���ļ�

	cout << "loading all distance done." << endl;

	_file.close();
	return true;
}

//�������ģ�������ǰԪ�������ĸ���  
int clusterOfTuple(Tuple means[], const Tuple& tuple, int nClusterNum, int disttype)
{
	int k = nClusterNum;
	int label = 0;//��ʾ������һ����  


	if (disttype == ID_TRAN_KMEANS)
	{
		double dist = getDistXY(means[0], tuple, disttype);
		double tmp;
		for (int i = 1; i < k; i++) {
			tmp = getDistXY(means[i], tuple, disttype);
			if (tmp < dist) { dist = tmp; label = i; }
		}
	}
	else
	{
		double dist = getDistXY_fast(means[0], tuple);
		double tmp;
		for (int i = 1; i < k; i++) {
			tmp = getDistXY_fast(means[i], tuple);
			if (tmp < dist) { dist = tmp; label = i; }
		}
	}



	return label;
}

//��ø����ؼ���ƽ�����  
double getVar(vector<Tuple> clusters[], Tuple means[], int nClusterNum, int disttype)
{
	double var = 0;
	int k = nClusterNum;

	int i, j;

	if (disttype == ID_TRAN_KMEANS)
	{
		for (int i = 0; i < k; i++)
		{
			vector<Tuple> t = clusters[i];
			for (int j = 0; j < t.size(); j++)
			{
				var += getDistXY(t[j], means[i], disttype);
			}
		}
	}
	else
	{
		for (int i = 0; i < k; i++)
		{
			vector<Tuple> t = clusters[i];
			for (int j = 0; j < t.size(); j++)
			{
				var += getDistXY_fast(t[j], means[i]);
			}
		}
	}

	//cout<<"sum:"<<sum<<endl;  
	return var;

}

//��õ�ǰ�صľ�ֵ�����ģ�  
Tuple getMeans(const vector<Tuple>& cluster, int dimNum, int ndisttype, bool& bSuccess)
{
	int num = cluster.size();
	if (num == 0)
	{
		cout << "one cluster is null." << endl;
		bSuccess = false;
		Tuple t1(dimNum + 1, 0);
		return t1;
	}

	dimNum = cluster[0].size() - 1;
	Tuple t(dimNum + 1, 0);

	int i = 0, j = 0;

	if (ndisttype == ID_TRAN_KMEANS)
	{
		for (i = 0; i < num; i++)
		{
			for (j = 1; j <= dimNum; ++j)
			{
				t[j] += cluster[i][j];
			}
		}
		for (j = 1; j <= dimNum; ++j)
			t[j] /= num;
	}
	else
	{
		//Ѱ�ҵ�ÿ����������
		float* pfdistance = new float[num];
		memset(pfdistance, 0, num * sizeof(float));

		//����ÿ���㵽�������DTW����
		int nCount = cluster[0].size() - 1;
		// 		double* parr1 = new double[nCount];
		// 		double* parr2 = new double[nCount];

		//int num_threads = omp_get_max_threads();

#pragma omp parallel for num_threads(num_threads<num?num_threads:num) schedule(dynamic)
		for (i = 0; i < num; i++)
		{
			/////////////////////////////////	
			double total_val = 0;

			for (int m = 0; m < num; m++)
			{
				if (i == m)
					continue;

				// 				for (int n = 0; n<nCount; n++)
				// 				{
				// 					parr1[n] = cluster[i][n+1];
				// 					parr2[n] = cluster[m][n+1];
				// 				}

				double dval = getDistXY_fast(cluster[i], cluster[m]);//DTW_distance(parr1, parr2, nCount);

				total_val += dval / double(num - 1);
			}
			pfdistance[i] = total_val;
		}

		// 		delete []parr1;
		// 		delete []parr2;

		//��ȡ��������Сֵ
		double min_dist = pfdistance[0];
		int nMinIdx = 0;
		for (i = 0; i < num; i++)
		{
			if (pfdistance[i] < min_dist)
			{
				nMinIdx = i;
				min_dist = pfdistance[i];
			}
		}

		delete[]pfdistance;

		t = cluster[nMinIdx];

	}



	bSuccess = true;
	return t;
	//cout<<"sum:"<<sum<<endl;  
}

void print(const vector<Tuple> clusters[], int nClusterNum)
{
	int k = nClusterNum;
	int dimNum = clusters[0][0].size() - 1;
	for (int lable = 0; lable < k; lable++)
	{
		cout << "No. " << lable + 1 << "Iterations" << " ..." << endl;
		vector<Tuple> t = clusters[lable];
		for (int i = 0; i < t.size(); i++)
		{
			cout << i + 1 << ".(";
			for (int j = 0; j <= dimNum; ++j)
			{
				cout << t[i][j] << ", ";
			}
			cout << ")\n";
		}
	}
}

bool outputFile(vector<Tuple>* clusters, int nClusterNum, char* outputfilename)
{
	cout << "writing result file..." << endl;
	QFile _file(outputfilename);
	if (!_file.open(QIODevice::WriteOnly))
	{
		cout << "write file failed." << endl;
		return false;
	}

	QTextStream _out(&_file);

	_out << "nFID, Class" << "\r\n";

	int k = nClusterNum;
	int dimNum = clusters[0][0].size() - 1;
	for (int lable = 0; lable < k; lable++)
	{
		vector<Tuple> t = clusters[lable];
		for (int i = 0; i < t.size(); i++)
		{
			//for(int j=0; j<=dimNum; j++)  
			//{  
			_out << t[i][0] << ", ";
			//}  
			_out << lable;	//���һ��Ϊ�������
			_out << "\r\n";
		}

		_out.flush();
	}


	_out.flush();
	_file.close();

	//���silhouette������ֵ
	QFileInfo silfileinfo(outputfilename);
	QString silfilename = silfileinfo.absolutePath() + "/" + silfileinfo.completeBaseName() + "_iter_silhouette.txt";
	QFile silfile(silfilename);

	if (!silfile.open(QIODevice::WriteOnly))
	{
		cout << "write silhouette file failed." << endl;
		return false;
	}

	QTextStream _silin(&silfile);
	_silin << "nIteration\t\tsilhouette_value\r\n" << endl;

	for (int i = 0; i < vSilVals.size(); i++)
	{
		QString smsg = QString("%1\t\t%2").arg(i).arg(vSilVals[i], 0, 'f', 6);
		_silin << smsg << "\r\n";
	}

	_silin.flush();
	silfile.flush();
	silfile.close();

	cout << "write result file success." << endl;
	return true;
}

void DTW_KMeans(vector<Tuple>& tuples, int nClusterNum, int nMaxIteraions, double dMaxExpectation, int disttype, vector<Tuple>*& clusters, bool isRndInit, float max_gap_dist, int nClusterNumTimes, int nRandParts, const char* means_filename, float minInitContribution, const char *distanceFileName)
{
	//�������
	cout << "starting compute distance matrix..." << endl;
	//const char* distance_filename = distance_filename1.toStdString().data();
	//cout<<distance_filename<<endl;
	if (!computeAllDistances_fromfile(tuples, disttype, distanceFileName))
	{
		cout << "load distance file error. re-computing..." << endl;
		computeAllDistances(tuples, disttype, distanceFileName);
	}
	cout << "distance matrix computation success." << endl;

	//���������means_filename
	QFile _file(means_filename);
	if (!_file.open(QIODevice::WriteOnly))
	{
		cout << "create means file failed. error." << endl;
		return;
	}
	cout << "create means record file success." << endl;
	QTextStream _in(&_file);

	QString _bestCenterfileName = QString(means_filename).left(QString(means_filename).size() - 4);
	_bestCenterfileName.append("_bestCenter.csv");
	QFile _bestCenterfile(_bestCenterfileName);
	if (!_bestCenterfile.open(QIODevice::WriteOnly))
	{
		cout << "create result centers file failed. error" << endl;
		return;
	}
	QTextStream _in_resultCenter(&_bestCenterfile);

	int k = nClusterNum;
	int dimNum = tuples[0].size() - 1;

	clusters = new vector<Tuple>[k];//k����  
	Tuple* means = new Tuple[k];//k�����ĵ�  
	Tuple* means_best = new Tuple[k];//���silhouetteֵ��Ӧ��k����������
	float* pContributions = NULL;
	int i = 0, j = 0;
	//һ��ʼ���ѡȡk����¼��ֵ��Ϊk���ص����ģ���ֵ��  
	if (isRndInit)
	{
		_in << "Initialization Method = Random" << "\r\n";
		_in << "\r\n";
		srand((unsigned int)time(NULL));
		for (i = 0; i < k;)
		{
			int iToSelect = rand() % tuples.size();
			if (means[i].size() == 0)
			{
				for (int j = 0; j <= dimNum; ++j)
				{
					means[i].push_back(tuples[iToSelect][j]);
				}
				++i;
			}
		}
	}
	else
	{
		_in << "Initialization Method = AP Clustering" << "\r\n";
		_in << "user set the minimum all contribution value = " << minInitContribution << "\r\n";
		_in << "\r\n";

		int nInitIterCount = 0;

		double sum_contribution = 0;
		while (sum_contribution < minInitContribution)
		{
			nInitIterCount++;
			DTW_findInitialCenters(tuples, means, pContributions, k, max_gap_dist, nClusterNumTimes);
			sum_contribution = 0;
			cout << "Iter No. " << nInitIterCount << " initial clustering center:" << endl;
			_in << "Iter No. " << nInitIterCount << " initial clustering center:" << "\r\n";
			for (i = 0; i < k; i++)
			{
				cout << "\tCenter No. " << i + 1 << " = " << int(means[i][0]) << "\t\tContribution = " << pContributions[i] << endl;
				_in << "\tCenter No. " << i + 1 << " = " << int(means[i][0]) << "\t\tContribution = " << pContributions[i] << "\r\n";
				sum_contribution += pContributions[i];
			}

			cout << "Iter No. " << nInitIterCount << " All Contributions = " << sum_contribution << endl;
			_in << "Iter No. " << nInitIterCount << " All Contributions = " << sum_contribution << "\r\n";
			_in << "\r\n";
		}

	}
	_in << "\r\n";

	//��ʼ��¼��ʼ����means
	_in << "Initialization Means = " << "\r\n";
	_in << "{\r\n";
	for (i = 0; i < nClusterNum; i++)
	{
		_in << "\t\t";
		for (j = 0; j < means[i].size(); j++)
		{
			if (j == 0)
				_in << int(means[i][j]) << ", ";
			else if (j != means[i].size() - 1)
				_in << means[i][j] << ", ";
			else
				_in << means[i][j];
		}
		_in << "\r\n";
	}
	_in << "}\r\n";
	// 	_in<<"initialization silhouette evaluation = "<<newVar<<"\r\n";
	// 	_in<<"}\r\n\r\n";
	// 	_in.flush();

	int lable = 0;
	//����Ĭ�ϵ����ĸ��ظ�ֵ 
	int nTuplesCount = tuples.size();

	//////

	vSilVals.clear();

	//int num_threads = omp_get_max_threads();

	//ѡȡ����
	float bestvar;
	vector<Tuple>* best_clusters = new vector<Tuple>[k];

#pragma omp parallel for num_threads(num_threads) schedule(dynamic)
	for (i = 0; i < nTuplesCount; i++)
	{
		lable = clusterOfTuple(means, tuples[i], k, disttype);

#pragma omp critical
		clusters[lable].push_back(tuples[i]);
	}


	float oldVar = -1;
	float newVar; //���
	float* pSilVals = NULL;
	silhouette(tuples, clusters, nClusterNum, disttype, pSilVals, newVar, distanceFileName);
	cout << "Initial average estimated silhouette value is " << newVar << endl;
	_in << "initialization silhouette evaluation = " << newVar << "\r\n";
	_in << "\r\n";
	_in.flush();


	int t = 0;
	int nIterCount = 0;
	vSilVals.append(newVar);
	bestvar = newVar;
	int bestTime = 0;

	for (j = 0; j < k; j++) //���ÿ����  
	{
		best_clusters[j].clear();

		for (i = 0; i < clusters[j].size(); i++)
		{
			best_clusters[j].push_back(clusters[j][i]);
		}
	}

	for (j = 0; j < k; j++)
	{
		means_best[j] = means[j];
	}

	while (newVar <= dMaxExpectation/*abs(newVar - oldVar) >= dMinError*/) //���¾ɺ���ֵ����1��׼����ֵ���������Ա仯ʱ���㷨��ֹ  
	{
		nIterCount++;
		cout << "Starting No. " << ++t << " iteration ..." << endl;
		for (i = 0; i < k; i++) //����ÿ���ص����ĵ�  
		{
			bool bSuc;
			Tuple temp_mean = getMeans(clusters[i], dimNum, disttype, bSuc);
			//means[i] = getMeans(clusters[i], dimNum, bSuc);
			if (bSuc)
			{
				means[i] = temp_mean;
			}
		}

		//cout<<"debug place: 0 Iter:"<<nIterCount<<endl;

		oldVar = newVar;
		//newVar = getVar(clusters,means,k,disttype); //�����µ�׼����ֵ  

		for (i = 0; i < k; i++) //���ÿ����  
		{
			clusters[i].clear();
		}

		//cout<<"debug place: 1 Iter:"<<nIterCount<<endl;

		//�����µ����Ļ���µĴ�  
#pragma omp parallel for num_threads(num_threads<nTuplesCount?num_threads:nTuplesCount) schedule(dynamic)
		for (i = 0; i < nTuplesCount; i++)
		{
			//cout<<"debug place: 1.5 Iter: "<<nIterCount<<" Tuples Count = "<<i<<endl;

			lable = clusterOfTuple(means, tuples[i], k, disttype);

#pragma omp critical
			clusters[lable].push_back(tuples[i]);
		}

		silhouette(tuples, clusters, nClusterNum, disttype, pSilVals, newVar, distanceFileName);
		vSilVals.append(newVar);

		//��ʼÿ�ε�����means
		_in << "Iteration No. " << nIterCount << " Means = " << "\r\n";
		_in << "{\r\n";
		for (i = 0; i < nClusterNum; i++)
		{
			_in << "\t\t";
			for (j = 0; j < means[i].size(); j++)
			{
				if (j == 0)
					_in << int(means[i][j]) << ", ";
				else if (j != means[i].size() - 1)
					_in << means[i][j] << ", ";
				else
					_in << means[i][j];
			}
			_in << "\r\n";
		}
		_in << "}\r\n";
		_in << "Iter No. " << nIterCount << " silhouette evaluation = " << newVar << "\r\n";
		_in << "\r\n";
		_in.flush();

		//����õı����best_clusters
		if (newVar > bestvar)
		{
			bestTime = nIterCount;
			bestvar = newVar;
			for (j = 0; j < k; j++) //���ÿ����  
			{
				best_clusters[j].clear();

				for (i = 0; i < clusters[j].size(); i++)
				{
					best_clusters[j].push_back(clusters[j][i]);
				}
			}

			for (j = 0; j < k; j++)
			{
				means_best[j] = means[j];
			}

		}

		cout << "Current average estimated silhouette value = " << newVar << endl;



		//cout<<"debug place: 2 Iter:"<<nIterCount<<endl;

		if (nIterCount >= nMaxIteraions)
			break;
	}

	// 	cout<<"The result is:\n";  
	// 	print(clusters, k);

	//����õĽ�����
	cout << "best silhouette value = " << bestvar << endl;
	for (j = 0; j < k; j++) //���ÿ����  
	{
		clusters[j].clear();

		for (i = 0; i < best_clusters[j].size(); i++)
		{
			clusters[j].push_back(best_clusters[j][i]);
		}
	}

	_in << "Best Iterations Time = " << bestTime << "\r\n";
	_in << "Best Silhouette Value = " << bestvar << "\r\n";


	//�����ѵľ�������
	_in_resultCenter << "Center_ID";
	for (i = 0; i < means_best[0].size() - 1; i++)
	{
		_in_resultCenter << ",V" << i;
	}
	_in_resultCenter << "\r\n";
	for (i = 0; i < k; i++)
	{
		_in_resultCenter << i;	//center ID
		for (j = 1; j < means_best[0].size(); j++)
			_in_resultCenter << "," << QString::number(means_best[i][j], 'g', 9);
		_in_resultCenter << "\r\n";
	}

	//��ʼÿ�ε�����means
	// 	_in<<"Best Means = "<<"\r\n";
	// 	_in<<"{\r\n"<<endl;
	// 	for (i=0; i<nClusterNum; i++)
	// 	{
	// 		_in<<"\t"<<endl;
	// 		for (j=0; j<means[i].size(); j++)
	// 		{
	// 			if (j==0)
	// 				_in<<int(means[i][j])<<", ";
	// 			else if (j != means[i].size()-1)
	// 				_in<<means[i][j]<<", ";
	// 			else
	// 				_in<<means[i][j];
	// 		}
	// 		_in<<"\r\n";
	// 	}
	// 	_in<<"}\r\n";
	// 	_in.flush();


	//cout<<clusters[0].size()<<endl;
	if (pSilVals != NULL)
		delete[]pSilVals;
	pSilVals = NULL;

	delete[]means;
	delete[]best_clusters;
	delete[]means_best;

	_in.flush();
	_file.flush();
	_file.close();
	_bestCenterfile.close();

}

bool TRAN_KMeans(real_2d_array trainingdata, real_1d_array dataids, const char* outputfilename, int nClassCount /*= 5*/, int ndistType /*= 2*/)
{
	int nSampleNum = trainingdata.rows();
	int nFeatureNum = trainingdata.cols();

	if (nSampleNum == 0 || nFeatureNum == 0)
	{
		cout << "samples count or features count  == 0! error" << endl;
		return false;
	}

	if (trainingdata.rows() != dataids.length())
	{
		cout << "data id is not match traning data rows." << endl;
		return false;
	}

	// 	cout<<"dist type = "<<ndistType<<endl;
	// 	cout<<"cluster count = "<<nClassCount<<endl;

	clusterizerstate s;
	ahcreport rep;
	integer_1d_array cidx;
	integer_1d_array cz;

	cout << "clustering..." << endl;
	clusterizercreate(s);
	clusterizersetpoints(s, trainingdata, nSampleNum, nFeatureNum, ndistType);
	clusterizerrunahc(s, rep);
	clusterizergetkclusters(rep, nClassCount, cidx, cz);
	cout << "cluster finished." << endl;

	//output file
	cout << "outputing file..." << endl;
	QFile _file(outputfilename);
	if (!_file.open(QIODevice::WriteOnly))
	{
		cout << "write file create failed." << endl;
		return false;
	}

	QTextStream _in(&_file);

	_in << "nFID, Class" << "\r\n";

	int i = 0;

	for (i = 0; i < cidx.length(); i++)
	{
		QString smsg = QString("%1, %2").arg(dataids[i]).arg(cidx[i]);
		_in << smsg << "\r\n";
		_in.flush();
	}


	_file.flush();
	_file.close();

	cout << "output finished." << endl;

	return true;
}

bool silhouette_output(vector<Tuple>& tuples, vector<Tuple>* clusters, int nClusterNum, int ndisttype, float*& pSilValues, float& aveSilValue, const char* outputfilename, const char* distanceFileName)
{
	cout << "start k-means / k-medoids silhouette evaluation ..." << endl;

	//�õ�ѵ�����ݵ�λ��
	int i, j;
	int nDataCount = tuples.size();	//��������
									// 	QList<int> vID;
									// 	for (i=0; i<nDataCount; i++)
									// 	{
									// 		vID.push_back(tuples[i][0]);
									// 	}

	if (ppDistance == NULL)
	{
		if (!computeAllDistances_fromfile(tuples, ndisttype, distanceFileName))
		{
			cout << "load distance file error. re-computing..." << endl;
			computeAllDistances(tuples, ndisttype, distanceFileName);
		}
	}

	//��¼��Ŷ�Ӧ�����
	QList<int> vClusterID;
	QList<int> vClusterClass;
	for (i = 0; i < nClusterNum; i++)
	{
		for (j = 0; j < clusters[i].size(); j++)
		{
			vClusterID.push_back(int(clusters[i][j][0]));
			vClusterClass.push_back(i);
		}
	}

	cout << "data count = " << vID.size() << endl;

	//���ڼ������
	// 	float** ppDistance = new float*[nDataCount];
	// 	for (i=0; i<nDataCount; i++)
	// 	{
	// 		ppDistance[i] = new float[i+1];
	// 		memset(ppDistance[i], 0, (i+1)*sizeof(float));
	// 	}


	// 	//��ʼ����
	// 	int num_threads = omp_get_max_threads();
	// 
	// #pragma omp parallel for num_threads(num_threads<(nDataCount-1)?num_threads:(nDataCount-1)) schedule(dynamic) private(j)
	// 	for (i=0; i<nDataCount; i++)
	// 	{
	// 		if ((i+1)%100 == 0)
	// 			cout<<"computing distance - No. "<<i+1<<" / "<<nDataCount<<"..."<<endl;
	// 
	// 		for (j=0; j<=i; j++)
	// 		{
	// 			double val = getDistXY(tuples[i], tuples[j], ndisttype);
	// 			ppDistance[i][j] = val;
	// 			//ppDistance[j][i] = val;
	// 		}
	// 	}
	// 	
	// 	cout<<"computing all distance done."<<endl;

	//�������������
	//float* pDist = new float[nClusterNum];

	float** ppDist = new float*[nDataCount];
	for (i = 0; i < nDataCount; i++)
	{
		ppDist[i] = new float[nClusterNum];
		memset(ppDist[i], 0, nClusterNum * sizeof(float));
	}


	if (pSilValues != NULL)
		delete pSilValues;
	pSilValues = new float[nDataCount];

	//int num_threads = omp_get_max_threads();

#pragma omp parallel for num_threads(num_threads) schedule(dynamic) private(j)
	for (i = 0; i < nDataCount; i++)
	{
		// 		if ((i+1)%100 == 0)
		// 			cout<<"processing No. "<<i+1<<" / "<<nDataCount<<"..."<<endl;

		//memset(pDist, 0, nClusterNum * sizeof(float));

		//�Լ���class ID
		int nClusterID = vClusterID.indexOf(tuples[i][0]);
		int nClassID = vClusterClass[nClusterID];

		//���������cluster�ľ����ֵ
		for (j = 0; j < nDataCount; j++)
		{
			int nOtherClusterID = vClusterID.indexOf(tuples[j][0]);
			int nOtherClusterClassID = vClusterClass[nOtherClusterID];

			ppDist[i][nOtherClusterClassID] += i >= j ? ppDistance[i][j] : ppDistance[j][i];
		}

		for (j = 0; j < nClusterNum; j++)
		{
			int nClusterSize = clusters[j].size();
			if (nClusterSize == 0)
				ppDist[i][j] = 10e6;

			ppDist[i][j] /= nClusterSize;
		}

		//���Լ����ľ���
		double _a = ppDist[i][nClassID];
		double _b = 10e6;
		for (j = 0; j < nClusterNum; j++)
		{
			if (j == nClassID)
				continue;

			_b = ppDist[i][j] < _b ? ppDist[i][j] : _b;
		}

		double _m = _a > _b ? _a : _b;

		if (_m == 0)
			pSilValues[i] = 0;
		else
			pSilValues[i] = (_b - _a) / _m;
	}
	cout << "finished process." << endl;

	//������������ֵ
	aveSilValue = 0;
	for (i = 0; i < nDataCount; i++)
	{
		aveSilValue += pSilValues[i];
	}
	aveSilValue /= nDataCount;	//�����������ֵ

	cout << "ave silhouette val = " << aveSilValue << endl;

	//����ļ�
	cout << "starting output evaluation data..." << endl;
	QFile _file(outputfilename);
	if (!_file.open(QIODevice::WriteOnly))
	{
		cout << "output file create failed. error." << endl;
		//�ͷ��ڴ�
		for (i = 0; i < nDataCount; i++)
		{
			delete[]ppDistance[i];
			delete[]ppDist[i];
			ppDistance[i] = NULL;
			ppDist[i] = NULL;
		}
		delete[]ppDistance;
		ppDistance = NULL;

		delete[]ppDist;
		ppDist = NULL;

		return false;
	}

	QTextStream _in(&_file);

	QString smsg = QString("average Silhouette Value = %1").arg(aveSilValue, 0, 'f', 6);
	_in << smsg << "\r\n";

	_in << "\r\n";
	_in << "nFID\t\t\tsilhouette_val\r\n";
	//
	for (i = 0; i < nDataCount; i++)
	{
		smsg = QString("%1\t\t\t%2").arg(vID[i]).arg(pSilValues[i], 0, 'f', 6);
		_in << smsg << "\r\n";
		_in.flush();
	}

	_in.flush();
	_file.flush();
	_file.close();

	//��������ļ�
	// 	QFileInfo _infoo(outputfilename);
	// 	QString matrixfilename = _infoo.absolutePath()+"/"+_infoo.completeBaseName()+"_matrix.txt";
	// 	
	// 	QFile _file2(matrixfilename);
	// 	if (!_file2.open(QIODevice::WriteOnly))
	// 	{
	// 		cout<<"output file create failed. error."<<endl;
	// 		//�ͷ��ڴ�
	// 		for (i=0; i<nDataCount; i++)
	// 		{
	// 			delete []ppDistance[i];
	// 			ppDistance[i] = NULL;
	// 		}
	// 		delete []ppDistance;
	// 		ppDistance = NULL;
	// 
	// 		delete []pDist;
	// 		return false;
	// 	}
	// 
	// 	QTextStream _in2(&_file2);
	// 	_in2<<"distance matrix = \r\n";
	// 	//����������
	// 	_in2<<"nFID,";
	// 	for (i=0; i<nDataCount; i++)
	// 	{
	// 		if (i<nDataCount-1)
	// 			_in2<<vID[i]<<",";
	// 		else
	// 			_in2<<vID[i]<<"\r\n";
	// 	}
	// 	_in2.flush();
	// 
	// 	for (i=0; i<nDataCount; i++)
	// 	{
	// 		_in2<<vID[i]<<",";
	// 		for (j=0; j<=i; j++)
	// 		{
	// 			smsg = QString("%1").arg(ppDistance[i][j], 0, 'f', 4);
	// 			if (j<i)
	// 				_in2<<smsg<<",";
	// 			else
	// 				_in2<<smsg<<"\r\n";			
	// 		}
	// 		_in2.flush();
	// 	}
	// 
	// 	_in2<<"\r\n";
	// 	_in2.flush();
	// 	_file2.flush();
	// 	_file2.close();

	cout << "create output evaluation file success." << endl;

	//�ͷ��ڴ�
	for (i = 0; i < nDataCount; i++)
	{
		delete[]ppDistance[i];
		delete[]ppDist[i];
		ppDistance[i] = NULL;
		ppDist[i] = NULL;
	}
	delete[]ppDistance;
	ppDistance = NULL;

	delete[]ppDist;
	ppDist = NULL;

	cout << "finished k-means / k-medoids silhouette evaluation." << endl;
	return true;
}

bool silhouette(vector<Tuple>& tuples, vector<Tuple>* clusters, int nClusterNum, int ndisttype, float*& pSilValues, float& aveSilValue, const char* distanceFileName)
{
	if (ndisttype != ID_TRAN_KMEANS && ndisttype != ID_DTW_KMEANS && ndisttype != ID_COS_KMEANS)
	{
		cout << "unknown disttype, program can not run silhouette evaluation." << endl;
		return false;
	}

	//cout<<"start k-means / k-medoids silhouette evaluation ..."<<endl;

	//�õ�ѵ�����ݵ�λ��
	int i, j;
	int nDataCount = tuples.size();	//��������
									// 	QList<int> vID;
									// 	for (i=0; i<nDataCount; i++)
									// 	{
									// 		vID.push_back(tuples[i][0]);
									// 	}

	if (ppDistance == NULL)
	{
		if (!computeAllDistances_fromfile(tuples, ndisttype, distanceFileName))
		{
			cout << "load distance file error. re-computing..." << endl;
			computeAllDistances(tuples, ndisttype, distanceFileName);
		}
	}

	//��¼��Ŷ�Ӧ�����
	QList<int> vClusterID;
	QList<int> vClusterClass;
	for (i = 0; i < nClusterNum; i++)
	{
		for (j = 0; j < clusters[i].size(); j++)
		{
			vClusterID.push_back(int(clusters[i][j][0]));
			vClusterClass.push_back(i);
		}
	}

	//cout<<"data count = "<<vID.size()<<endl;

	//���ڼ������
	// 	float** ppDistance = new float*[nDataCount];
	// 	for (i=0; i<nDataCount; i++)
	// 	{
	// 		ppDistance[i] = new float[i+1];
	// 		memset(ppDistance[i], 0, (i+1)*sizeof(float));
	// 	}


	// 	//��ʼ����
	// 	int num_threads = omp_get_max_threads();
	// 
	// #pragma omp parallel for num_threads(num_threads<(nDataCount-1)?num_threads:(nDataCount-1)) schedule(dynamic) private(j)
	// 	for (i=0; i<nDataCount; i++)
	// 	{
	// 		if ((i+1)%100 == 0)
	// 			cout<<"computing distance - No. "<<i+1<<" / "<<nDataCount<<"..."<<endl;
	// 
	// 		for (j=0; j<=i; j++)
	// 		{
	// 			double val = getDistXY(tuples[i], tuples[j], ndisttype);
	// 			ppDistance[i][j] = val;
	// 			//ppDistance[j][i] = val;
	// 		}
	// 	}
	// 	
	// 	cout<<"computing all distance done."<<endl;

	//�������������
	//float* pDist = new float[nClusterNum];
	
	float** ppDist = new float*[nDataCount];
	for (i = 0; i < nDataCount; i++)
	{
		ppDist[i] = new float[nClusterNum];
		memset(ppDist[i], 0, nClusterNum * sizeof(float));
	}
	

	if (pSilValues != NULL)
		delete pSilValues;
	pSilValues = new float[nDataCount];

	//int num_threads = omp_get_max_threads();

#pragma omp parallel for num_threads(num_threads) schedule(dynamic) private(j)
	for (i = 0; i < nDataCount; i++)
	{
// 		if ((i+1)%1000 == 0)
// 			cout<<"processing No. "<<i+1<<" / "<<nDataCount<<"..."<<endl;
		//float* pDist = new float[nClusterNum];
		//memset(pDist, 0, nClusterNum * sizeof(float));

		//�Լ���class ID
		int nClusterID = vClusterID.indexOf(tuples[i][0]);
		int nClassID = vClusterClass[nClusterID];

		//���������cluster�ľ����ֵ
		for (j = 0; j < nDataCount; j++)
		{
			int nOtherClusterID = vClusterID.indexOf(tuples[j][0]);
			int nOtherClusterClassID = vClusterClass[nOtherClusterID];
			ppDist[i][nOtherClusterClassID] += i >= j ? ppDistance[i][j] : ppDistance[j][i];
		}

		for (j = 0; j < nClusterNum; j++)
		{
			int nClusterSize = clusters[j].size();
			if (nClusterSize == 0)
				nClusterSize = 10e6;

			ppDist[i][j] /= nClusterSize;
		}

		//���Լ����ľ���
		double _a = ppDist[i][nClassID];
		double _b = 10e6;
		for (j = 0; j < nClusterNum; j++)
		{
			if (j == nClassID)
				continue;

			_b = ppDist[i][j] < _b ? ppDist[i][j] : _b;
		}

		double _m = _a > _b ? _a : _b;

		if (_m == 0 || _isnan(_m) || _isnanf(_m))
			pSilValues[i] = 0;
		else
			pSilValues[i] = (_b - _a) / _m;

		//delete[]pDist;
	}
	//cout<<"finished process."<<endl;

	//������������ֵ
	aveSilValue = 0;
	for (i = 0; i < nDataCount; i++)
	{
		aveSilValue += pSilValues[i];
	}
	aveSilValue /= nDataCount;	//�����������ֵ

								//	cout<<"ave silhoutte value = "<<aveSilValue<<endl;

								//�ͷ��ڴ�
								// 	for (i=0; i<nDataCount; i++)
								// 	{
								// 		delete []ppDistance[i];
								// 		ppDistance[i] = NULL;
								// 	}
								// 	delete []ppDistance;
								// 	ppDistance = NULL;

	//delete[]pDist;
	for (int i = 0; i < nDataCount; i++)
	{
		delete[] ppDist[i];
		ppDist[i] = NULL;
	}
	delete[] ppDist;
	ppDist = NULL;
	//cout<<"finished k-means / k-medoids silhouette evaluation."<<endl;
	return true;

}

///////////////////////////////////////////////////////////////////

void DTW_getCenterTuples(vector<Tuple> tuples, Tuple& destTuple)
{
	if (tuples.size() == 0)
		return;

	float* pval = new float[tuples.size()];

	////////
	int i = 0, j = 0;
	//����ÿ���㵽������ľ���
	//int num_threads = omp_get_max_threads();
#pragma omp parallel for num_threads(num_threads) schedule(dynamic) private(j)
	for (i = 0; i < tuples.size(); i++)
	{
		double ppSum = 0;
		for (j = 0; j < tuples.size(); j++)
		{
			ppSum += getDistXY_fast(tuples[i], tuples[j]);
		}
		pval[i] = ppSum / (tuples.size() - 1);
	}

	//�õ���С��i
	int min_i = 0;
	float min_val = pval[0];
	for (i = 0; i < tuples.size(); i++)
	{
		if (pval[i] < min_val)
		{
			min_i = i;
			min_val = pval[i];
		}
	}

	delete[]pval;
	//return tuples[min_i];
	destTuple.clear();

	destTuple = tuples[min_i];

// 	for (i = 0; i < tuples[min_i].size(); i++)
// 		destTuple.push_back(tuples[min_i][i]);
}

void DTW_getFarestFromCenterTuple(Tuple centerTuple, vector<Tuple> tuples, Tuple& destTuple)
{
	float *pval = new float[tuples.size()];

	int i, j;
	for (i = 0; i < tuples.size(); i++)
		pval[i] = getDistXY_fast(centerTuple, tuples[i]);

	int max_i = 0;
	float max_val = pval[i];

	for (i = 0; i < tuples.size(); i++)
	{
		if (pval[i] > max_val)
		{
			max_i = i;
			max_val = pval[i];
		}
	}


	delete[]pval;
	//return tuples[max_i];

	destTuple.clear();
	destTuple = tuples[max_i];
// 	for (i = 0; i < tuples[max_i].size(); i++)
// 		destTuple.push_back(tuples[max_i][i]);
}

bool DTW_findInitialCenters(vector<Tuple> origin_tuples, Tuple*& means, float*& pContribution, int nRealClusterNum, float max_gap_dist, int nClusterNumTimes, int nRandParts)
{
	if (origin_tuples.size() == 0 || ppDistance == NULL)
		return false;

	//����һ��
	vector<Tuple> tuples = origin_tuples;

	int nClusterNum = nRealClusterNum*nClusterNumTimes;	//2����ʼȡ��ֵ

														//�������Ŀռ�
	if (means != NULL)
		delete[]means;
	means = new Tuple[nClusterNum];

	if (pContribution != NULL)
		delete[]pContribution;
	pContribution = new float[nClusterNum];

	int i, j, nMeansIdx;
	srand((unsigned)time(NULL));
	//�����������Ϊ nRandParts ��
	vector<Tuple>* pRandTuples = new vector<Tuple>[nRandParts];
	while (1)
	{
		for (i = 0; i < nRandParts; i++)
			pRandTuples[i].clear();

		for (i = 0; i < tuples.size(); i++)
		{
			j = nRandParts*(double)rand() / RAND_MAX;
			while (j >= nRandParts)
				j = nRandParts*(double)rand() / RAND_MAX;

			pRandTuples[j].push_back(tuples[i]);
		}

		int _nflag = 1;
		for (i = 0; i < nRandParts; i++)
		{
			if (pRandTuples[i].size() == 0)
			{
				_nflag = 0;
				break;
			}
		}

		if (_nflag == 1)
			break;
	}



	vector<Tuple> pCenterTuple;
	//����ÿһ�ݵ�����
	for (i = 0; i < nRandParts; i++)
	{
		Tuple tp_tuple;
		DTW_getCenterTuples(pRandTuples[i], tp_tuple);
		pCenterTuple.push_back(tp_tuple);
	}

	//����ʵ������
	Tuple centerTuple;
	DTW_getCenterTuples(pCenterTuple, centerTuple);
	delete[]pRandTuples;

	cout << "Centre Tuples ID = " << int(centerTuple[0]) << endl;

	//��¼�Ѿ�����¼�ı��
	QList<int> used_ids;
	used_ids.clear();

	for (nMeansIdx = 0; nMeansIdx < nClusterNum; nMeansIdx++)
	{
		//cout<<"nMeansIds = "<<nMeansIdx<<" / "<<nClusterNum<<" ... "<<endl;
		//1.2 ͨ��ʵ������Ѱ�ҵ���Զ�ĵ�
		Tuple farestTuple;
		DTW_getFarestFromCenterTuple(centerTuple, tuples, farestTuple);

		float distance_CT_FT = 1.0f;
		//1.3 �����µ�Si
		vector<Tuple> newClusters;
		newClusters.clear();
		//newClusters.push_back(farestTuple);
		while (distance_CT_FT > max_gap_dist)
		{
			for (i = 0; i < origin_tuples.size(); i++)
			{
				if (used_ids.contains(int(origin_tuples[i][0])))
					continue;

				float dist1 = getDistXY_fast(centerTuple, origin_tuples[i]);
				float dist2 = getDistXY_fast(farestTuple, origin_tuples[i]);
				if (dist2 <= dist1)
					newClusters.push_back(origin_tuples[i]);
			}

			//1.4.1 ��ȡSi�����ĺ�ʵ�����Ľ��жԱ�
			Tuple newClusterCenterTuple;
			DTW_getCenterTuples(newClusters, newClusterCenterTuple);
			distance_CT_FT = getDistXY_fast(newClusterCenterTuple, farestTuple);
			if (distance_CT_FT > max_gap_dist)
				farestTuple = newClusterCenterTuple;
		}

		//Ϊ���ĸ�ֵ
		means[nMeansIdx] = farestTuple;
		pContribution[nMeansIdx] = (double)newClusters.size() / (double)origin_tuples.size();

		//1.4.2 ��tuples��ɾ��newClusters����
		// 	vector<Tuple>::iterator _titer = tuples.begin();
		// 	vector<Tuple>::iterator _niter = newClusters.begin();

		//vector<Tuple>::iterator _titer = find(tuples.begin(), tuples.end(), newClusters[i]);

		vector<Tuple>::iterator _titer = tuples.begin();
		for (i = 0; i < newClusters.size(); i++)
		{
			used_ids.append(newClusters[i][0]);
			//�ж�ÿһ�����ڵ�λ��
			for (j = 0; j < tuples.size(); j++)
			{
				if (int(newClusters[i][0]) == int(tuples[j][0]))
				{
					tuples.erase(_titer + j);		//ɾ����Ӧ��Ԫ��
					_titer = tuples.begin();
					break;
				}
			}
		}

		if (tuples.size() == 0)
			break;
	}

	//����pContributions��ȡ��ʵ��ֵ
	Tuple* temp_means = new Tuple[nClusterNum];
	float* temp_pContributions = new float[nClusterNum];

	//����
	for (i = 0; i < nClusterNum; i++)
	{
		temp_means[i] = means[i];
		temp_pContributions[i] = pContribution[i];
	}

	//ð�ݷ�����
	for (i = 0; i < nClusterNum - 1; i++)
	{
		for (j = i + 1; j < nClusterNum; j++)
		{
			if (temp_pContributions[i] < temp_pContributions[j])
			{
				float tpval = temp_pContributions[i];
				temp_pContributions[i] = temp_pContributions[j];
				temp_pContributions[j] = tpval;

				Tuple tptuple = temp_means[i];
				temp_means[i] = temp_means[j];
				temp_means[j] = tptuple;
			}
		}
	}


	//����
	delete[]means;
	delete[]pContribution;
	means = new Tuple[nRealClusterNum];
	pContribution = new float[nRealClusterNum];

	for (i = 0; i < nRealClusterNum; i++)
	{
		means[i] = temp_means[i];
		pContribution[i] = temp_pContributions[i];
	}

	//�ͷ��ڴ�
	delete[]temp_means;
	delete[]temp_pContributions;
	return true;
}



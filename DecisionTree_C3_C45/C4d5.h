#pragma once
/*
* C4d5.h
*
*  Created on: Sep 19, 2015
*      Author: th
*/

#ifndef C4d5_H_
#define C4d5_H_

#include "ID3.h"

using std::multimap;

class C4d5 :public ID3 {
protected:
	//C4.5�������Ϣ����
	double ComputeSplitEntropy(const vector<map<string, string> >& remain_Samples, string attribute_name, int& number)const;
	//C4.5����Ϣ����
	double ComputeGainRatio(const vector<map<string, string> >& remain_Samples, string attribute_name)const;

	//����ݹ鹹��������
	DecisionTreeNode* BuildDiscreteDecisionTree(DecisionTreeNode* current_node, vector<map<string, string> > remain_Samples, vector<string> remain_Attribute_Names);
	//��¼����е�E��N��majority_forecasting_attribute_value
	void RecordEN(const vector<map<string, string> >& remain_Samples, DecisionTreeNode* current_node);
	//���㱯�۵�����ʣ���NΪ0����0
	double ErrorRate(const int E, const int N, const double z)const;
	//�����û����������滻����1
	bool SubtreeReplacement(DecisionTreeNode* current_node, const double z);
	//��֦���õ�ǰ�ڵ��µ�Ҷ�Ӵ��浱ǰ�ڵ�
	void Pruning(DecisionTreeNode* current_node, const double z);
	//���������Ѱ����Ҫ��֦�Ľڵ�
	bool PessimisticPruning(DecisionTreeNode* current_node, const double z);
	//�������Ŷ�c���׼��̬�ֲ�����zֵ
	double ComputeZ(const double p)const;

	//����ݹ鹹�����������Եľ�����
	DecisionTreeNode* BuildContinuousDecisionTree(DecisionTreeNode* current_node, vector<map<string, string> > remain_Samples, vector<string> remain_Attribute_Names, map<string, multimap<double, string> > remain_Numerical_Attribute_Samples);
	//����remain_Samples����remain_Numerical_Attribute_Samples
	void PreProcessorForContinuous(const vector<map<string, string> >& remain_Samples, map<string, multimap<double, string> >& remain_Numerical_Attribute_Samples)const;
	//����������ֵ�ͷָ��λ������Ϣ��
	double ComputeContinuousEntropy(const multimap<double, string>& remain_Numerical_Samples, const multimap<double, string>::const_iterator& index)const;
	//����������ֵ�ͷָ��λ������Ϣ������
	double ComputeContinuousGainRatio(const multimap<double, string>& remain_Numerical_Samples, const multimap<double, string>::const_iterator& index)const;
	//�ҵ����зָ�㣬������ѷָ�����Ϣ�����ʣ�indexΪ��ѷָ���λ��
	double FindDividingPoint(const multimap<double, string>& remain_Numerical_Samples, multimap<double, string>::const_iterator& index)const;
	//��ɢ��������<[index]��>=[index]�����ֵ�string
	vector<string> DivideContinuousIntoTwoParts(const multimap<double, string>::const_iterator& index)const;
	//�����ӽڵ��remain_Numercial_Attribute_Samples��remain_Samples����new_remain_Samples��sizeû�䷵��1��sizeΪ0����2����������0
	int UpdateContinuousSamples(const vector<map<string, string> >& remain_Samples, vector<map<string, string> >& new_remain_Samples, map<string, multimap<double, string> >& new_remain_Numerical_Attribute_Samples, string attribute_name, string attribute_value);
	//�ж�continuous_value�Ƿ����㲻��ʽinequality
	bool IsInequalitySatisfied(const string& inequality, const string& continuous_value)const;

	//����������ֵ�ȷ�Ϊsegments��
	void PreProcessor(int segments);
public:
	C4d5(string path, int minLeaf = 2) :ID3(path, minLeaf) {};
	~C4d5() {};
	//��֦����
	void PessimisticPruning(const double p = 0.25);
	//flag�����Ƿ�Ԥ����ɢ��
	void BuildDecisionTree(bool flag = 0, int segments = 6);
	void BuildDecisionTree(double percentage, bool flag = 0, int segments = 6);

	string Predict(map<string, string> example)const;
	double Predict()const;
	void test();
};
#endif

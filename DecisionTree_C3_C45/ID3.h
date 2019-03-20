#pragma once
/*
* ID3.h
*
*  Created on: Sep 19, 2015
*      Author: th
*/

#ifndef ID3_H_
#define ID3_H_

#include <string>
#include <map>
#include <vector>

using std::vector;
using std::map;
using std::string;

class ID3 {
private:
	bool has_real;
protected:
	int minLeaf; //segmentsΪʵ��Ԥ����ʱ��Ϊ���Σ�minLeafΪҶ����Сʵ����
	string relation;
	string forecasting_attribute_name; //��¼��ҪԤ���������
	int forecasting_attribute_value_length; //��¼��ҪԤ������Եķ�֧��
	map<string, vector<string> > Attribute_Name_Values; //��¼�������Ͷ�Ӧ������ֵ
	vector<string> Attribute_Names; //��¼������
	vector<map<string, string> > Samples; //����ѵ����ʵ��
	vector<map<string, string> > Test_Samples; //�������Ե�ʵ��
	struct DecisionTreeNode {
		string attribute_name;
		string attribute_pre_value;
		string majority_forecasting_attribute_value; //��ǰʣ��ʵ��������forecasting����ֵ
		int E; int N; //���ڱ��ۼ�֦E����ʵ������N��ǰ��ʵ����
		vector<DecisionTreeNode*> children;
		DecisionTreeNode() :attribute_name(""), attribute_pre_value(""), majority_forecasting_attribute_value(""), E(-1), N(-1) {};
	} *root;

	//����غ���
	//����info([1,2,3,...])��number��¼ʵ������
	double Information(const map<string, int>& amount, int number)const;
	//���������·�֧����Ϣֵ����ǰ���ǵ�ʵ��������ǰ���ǵ���������������������ֵ��֧
	double ComputeEntropy(const vector<map<string, string> >& remain_Samples, string attribute_name, string attribute_value, int& number)const;
	//��ǰʵ��������Ϣֵ��������E��N
	double ComputeEntropy(const vector<map<string, string> >& remain_Samples, int& number)const;
	//��������Ӧ����Ϣ����
	double ComputeGain(const vector<map<string, string> >& remain_Samples, string attribute_name)const;

	//���ɾ���������ֹ��������
	//�ж�remain��forecasting����ֵ�Ƿ���ͬ�����񷵻�"ImpurE"
	string IsPure(const vector<map<string, string> >& remain_Samples)const;
	//����remain������forecasting����ֵ
	string MajorityDecision(const vector<map<string, string> >& remain_Samples)const;
	//����ݹ鹹��������
	virtual DecisionTreeNode* BuildDiscreteDecisionTree(DecisionTreeNode* current_node, vector<map<string, string> > remain_Samples, vector<string> remain_Attribute_Names);
	//����ݹ��ӡ������
	void PrintDecisionTree(DecisionTreeNode* node, int depth)const;
	//����ݹ��ͷž�����
	void FreeDecisionTree(DecisionTreeNode* node);
	//����has_real
	bool Gethasreal()const;
	void Sethasreal(bool flag);
public:
	//����arff����ע��Ԥ���ĸ����ԣ�����Ĭ��Ϊ���һ������
	ID3(string path, int minLeaf = 2);
	void PrintDecisionTree()const;
	virtual void BuildDecisionTree();
	//���հٷֱȽ�Samples��Ϊѵ���Ͳ��Եļ���
	virtual void BuildDecisionTree(double percentage);
	//���ݸ����ĵ�����������Ԥ��
	virtual string Predict(map<string, string> example)const;
	//����percentage���ֵĲ��Լ����в��ԣ�����׼ȷ��
	double Predict()const;
	virtual void PessimisticPruning(const double z = 0.69) {};
	void test();
	virtual ~ID3();
};

#endif

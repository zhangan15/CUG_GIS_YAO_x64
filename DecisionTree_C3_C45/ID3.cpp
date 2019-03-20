/*
* ID3.cpp
*
*  Created on: Sep 19, 2015
*      Author: th
*/

#include "ID3.h"
#include <math.h>
#include <fstream>
#include <iostream>

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;
using std::getline;
using std::stringstream;

ID3::ID3(string path, int minLeaf) {
	ifstream in;
	string source;
	bool data = 0;
	const string label1 = "@relation ";
	const string label2 = "@attribute ";
	const string label3 = "@data";
	has_real = 0;
	this->root = new DecisionTreeNode();
	this->minLeaf = minLeaf;
	relation = "";
	in.open(path.c_str(), ios::in);
	if (!in.is_open()) cout << "·������";
	while (!in.eof()) {
		getline(in, source);
		if ("" == source) break;
		if (!label1.compare(source.substr(0, 10))) //@relation
			relation = source.substr(10, source.length() - 10);
		else if (!label2.compare(source.substr(0, 11))) { //@attribute
			int i = 11, j;
			while (source[i] != ' ') ++i;
			Attribute_Names.push_back(source.substr(11, i - 11));
			forecasting_attribute_name = source.substr(11, i - 11);
			if (source[i + 1] == '{') i += 2;
			else if (source.substr(i + 1, 4) == "real") { //ʵ��
				Attribute_Name_Values[Attribute_Names.back()].push_back("ReaL");
				has_real = 1;
				continue;
			}
			else {
				cout << "error";
				break;
			}
			for (j = i; source[j] != '}'; ++j) {
				if (source[j] == ',') {
					Attribute_Name_Values[Attribute_Names.back()].push_back(source.substr(i, j - i));
					i = j + 1;
				}
			}
			Attribute_Name_Values[Attribute_Names.back()].push_back(source.substr(i, j - i));
		}
		else if (!label3.compare(source.substr(0, 5))) { //@data
			data = 1;
		}
		else if (data) {
			map<string, string> item;
			int i, j, k;
			for (i = 0, j = 0, k = 0; source[j] != '\0'; ++j) {
				if (source[j] == ',') {
					item[Attribute_Names[k++]] = source.substr(i, j - i);
					i = j + 1;
				}
			}
			item[Attribute_Names[k]] = source.substr(i, j - i);
			Samples.push_back(item);
		}
	}
	forecasting_attribute_value_length = Attribute_Name_Values[forecasting_attribute_name].size();
}

double ID3::Information(const map<string, int>& amount, int number)const {
	double ans = 0;
	for (map<string, int>::const_iterator i = amount.begin(); i != amount.end(); ++i) {
		ans += (-i->second / (double)number*log(i->second / (double)number) / log(2.0));
	}
	return ans;
}

double ID3::ComputeEntropy(const vector<map<string, string> >& remain_Samples, string attribute_name, string attribute_value, int& number)const {
	map<string, int> amount;
	number = 0;
	for (vector<map<string, string> >::const_iterator i = remain_Samples.begin(); i != remain_Samples.end(); ++i) {
		if (!attribute_value.compare(i->at(attribute_name))) {
			++amount[i->at(forecasting_attribute_name)];
			++number;
		}
	}
	return Information(amount, number);
}

double ID3::ComputeEntropy(const vector<map<string, string> >& remain_Samples, int& number)const {
	map<string, int> amount;
	number = 0;
	for (vector<map<string, string> >::const_iterator i = remain_Samples.begin(); i != remain_Samples.end(); ++i) {
		++amount[i->at(forecasting_attribute_name)];
		++number;
	}
	return Information(amount, number);
}

double ID3::ComputeGain(const vector<map<string, string> >& remain_Samples, string attribute_name)const {
	int sum, amount; //sumΪʵ��������amountΪ����ֵ��ʵ������
	double parent_entropy = ComputeEntropy(remain_Samples, sum);
	double children_entropy = 0;
	for (unsigned int i = 0; i < Attribute_Name_Values.at(attribute_name).size(); ++i) {
		double temp = ComputeEntropy(remain_Samples, attribute_name, Attribute_Name_Values.at(attribute_name)[i], amount);
		children_entropy += (double)amount / sum*temp;
	}
	return parent_entropy - children_entropy;
}

string ID3::IsPure(const vector<map<string, string> >& remain_Samples)const {
	string forecasting_attribute_value = remain_Samples[0].at(forecasting_attribute_name);
	for (vector<map<string, string> >::const_iterator i = remain_Samples.begin() + 1; i != remain_Samples.end(); ++i) {
		if (forecasting_attribute_value.compare(i->at(forecasting_attribute_name)))
			return "ImpurE";
	}
	return forecasting_attribute_value;
}

string ID3::MajorityDecision(const vector<map<string, string> >& remain_Samples)const {
	if (remain_Samples.empty()) return "unknown";
	map<string, int> count;
	map<string, int>::iterator ans;
	int max = 0;
	for (vector<map<string, string> >::const_iterator i = remain_Samples.begin(); i != remain_Samples.end(); ++i) {
		++count[i->at(forecasting_attribute_name)];
	}
	for (map<string, int>::iterator i = count.begin(); i != count.end(); ++i) {
		if (i->second > max) {
			max = i->second;
			ans = i;
		}
	}
	return ans->first;
}

ID3::DecisionTreeNode* ID3::BuildDiscreteDecisionTree(DecisionTreeNode* current_node, vector<map<string, string> > remain_Samples, vector<string> remain_Attribute_Names) {
	//��ֹ����
	if (remain_Samples.size() <= (unsigned)minLeaf) { //ʣ��ʵ������С����ֵ
		current_node->attribute_name = MajorityDecision(remain_Samples);
		return current_node;
	}
	string pure = IsPure(remain_Samples);
	if (pure.compare("ImpurE")) { //��ϢֵΪ0
		current_node->attribute_name = pure;
		return current_node;
	}
	if (remain_Attribute_Names.empty()) { //û��ʣ���������
		current_node->attribute_name = MajorityDecision(remain_Samples);
		return current_node;
	}
	//�������Ϣ����
	double max_gain = -1, temp_gain;
	vector<string>::iterator chosen_attribute_name;
	vector<string>::iterator i;
	for (i = remain_Attribute_Names.begin(); i != remain_Attribute_Names.end(); ++i) {
		temp_gain = ComputeGain(remain_Samples, *i);
		if (temp_gain > max_gain) {
			max_gain = temp_gain;
			chosen_attribute_name = i;
		}
	}
	current_node->attribute_name = *chosen_attribute_name;
	//�����ӽڵ��remain_Attribute_Names
	vector<map<string, string> > new_remain_Samples;
	vector<string> new_remain_Attribute_Names;
	for (i = remain_Attribute_Names.begin(); i != remain_Attribute_Names.end(); ++i) {
		if ((*i).compare(*chosen_attribute_name))
			new_remain_Attribute_Names.push_back(*i);
	}
	//����ȷ����name������Ӧvalue���ӽڵ�
	for (i = Attribute_Name_Values[*chosen_attribute_name].begin(); i != Attribute_Name_Values[*chosen_attribute_name].end(); ++i) {
		DecisionTreeNode* child_node = new DecisionTreeNode();
		child_node->attribute_pre_value = *i;
		for (vector<map<string, string> >::iterator j = remain_Samples.begin(); j != remain_Samples.end(); ++j) { //�����ӽڵ��remain_Samples
			if (!(*i).compare(j->at(*chosen_attribute_name))) {
				new_remain_Samples.push_back(*j);
			}
		}
		//�ݹ������ӽڵ�
		BuildDiscreteDecisionTree(child_node, new_remain_Samples, new_remain_Attribute_Names);
		current_node->children.push_back(child_node);
		new_remain_Samples.clear();
	}
	return current_node;
}

void ID3::BuildDecisionTree() {
	if (has_real) {
		cout << "ID3��֧����ֵ������" << endl;
		return;
	}
	vector<string> current_Attribute_Names;
	//������forecasting��������
	current_Attribute_Names.insert(current_Attribute_Names.begin(), Attribute_Names.begin(), Attribute_Names.end() - 1);
	BuildDiscreteDecisionTree(root, Samples, current_Attribute_Names);
}

void ID3::BuildDecisionTree(double percentage) {
	if (has_real) {
		cout << "ID3��֧����ֵ������" << endl;
		return;
	}
	if (percentage < 0 || percentage>1) {
		cout << "BuildDecisionTree(double percentage)��������";
		return;
	}
	vector<string> current_Attribute_Names;
	current_Attribute_Names.insert(current_Attribute_Names.begin(), Attribute_Names.begin(), Attribute_Names.end() - 1);
	int training_size = (int)((1.0 - percentage)*Samples.size());
	int test_size = Samples.size() - training_size;
	Test_Samples.insert(Test_Samples.begin(), Samples.end() - test_size, Samples.end()); //��percentage������ʵ����Ϊ����
	Samples.erase(Samples.end() - test_size, Samples.end());
	BuildDiscreteDecisionTree(root, Samples, current_Attribute_Names);
}

void ID3::PrintDecisionTree(ID3::DecisionTreeNode* node, int depth)const {
	int i;
	for (i = 0; i < depth; ++i) cout << '\t';
	if (!node->attribute_pre_value.empty()) {
		cout << node->attribute_pre_value << " " << node->E << " " << node->N << endl;
		for (i = 0; i < depth + 1; ++i) cout << '\t';
	}
	else cout << '\t';
	cout << node->attribute_name << endl;
	for (unsigned i = 0; i < node->children.size(); ++i) {
		PrintDecisionTree(node->children[i], depth + 2);
	}
}

void ID3::PrintDecisionTree()const {
	PrintDecisionTree(root, 0);
}

void ID3::FreeDecisionTree(ID3::DecisionTreeNode* node) {
	if (node == NULL) return;
	for (vector<DecisionTreeNode*>::iterator i = node->children.begin(); i != node->children.end(); ++i) {
		FreeDecisionTree(*i);
	}
	delete node;
}

ID3::~ID3() {
	FreeDecisionTree(root);
}

string ID3::Predict(map<string, string> example)const {
	if (root == NULL) {
		cout << "�������ɾ�����" << endl;
		return "ERROR";
	}
	DecisionTreeNode* p = root;
	vector<ID3::DecisionTreeNode*>::const_iterator i = p->children.begin();
	while (i != p->children.end()) {
		if (!example[p->attribute_name].compare((*i)->attribute_pre_value)) {
			p = *i;
			i = p->children.begin();
			continue;
		}
		++i;
	}
	return p->attribute_name;
}

double ID3::Predict()const {
	if (Test_Samples.empty()) {
		cout << "���Լ�Ϊ�գ�����BuildDecisionTree(double percentage)����";
		return 0;
	}
	int correct = 0;
	for (vector<map<string, string> >::const_iterator i = Test_Samples.begin(); i != Test_Samples.end(); ++i) {
		if (!(i->at(forecasting_attribute_name)).compare(Predict(*i))) {
			++correct;
		}
	}
	return (double)correct / Test_Samples.size();
}

bool ID3::Gethasreal()const {
	return has_real;
}

void ID3::Sethasreal(bool flag) {
	has_real = flag;
}

void ID3::test() {
	vector<map<string, string> > remain_Samples(Samples);
	//	cout << ComputeGainRatio(remain_Samples, "windy");
}




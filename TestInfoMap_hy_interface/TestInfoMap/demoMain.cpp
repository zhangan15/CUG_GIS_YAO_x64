#include "Infomap.h"
#include "QtCore\qstring.h"
#include <string.h>


struct inputPara {
	//�����ļ�·��
	QString inputFile = "";
	//�����ʽ��'pajek', 'link-list', 'states', '3gram', 'multilayer' or 'bipartite'
	//Link_list��ʽ����ʼ��� ��ֹ��� ��Ȩ��
	QString inputFormat = "link-list";
	//����ļ��У���Ҫ��������
	QString outputDir = "";
	
	//ѭ������
	int tryTimes = 1;
	//�Ƿ�����ͼ
	bool ifDirected = false;
	//�ڵ��Ƿ������ڲ�ͬ��model
	bool ifOverlap = false;
	//�ڵ��Ƿ��1��ʼ���
	bool ifZero = false;
	//�Ƿ���� links with the same source and target node.
	bool ifSelfLink = false;
	//�Ƿ�ֻ�����绮��Ϊtwo-level
	bool if2 = false;
	//�Ƿ����ÿ���ڵ��top cluster index
	bool ifTopClu = false;
	//�Ƿ����.map��ʽ�ļ���������ʹ����վ���ӻ����ܣ�
	bool ifMap = false;
	//�Ƿ����.tree��ʽ�ļ���ֱ��ʹ�ü��±��򿪣�����ʽ�����ı��ļ�)
	bool ifTree = false;
	//�Ƿ����.bftree��ʽ�ļ��������ƴ洢��������ʹ����վ���ӻ����ܣ�
	bool ifBFTree = false;
};

std::ostringstream convertPara(inputPara);

int main(int argc, char* argv[])
{
	inputPara newPar;
	newPar.tryTimes = 10;
	newPar.inputFile="Link_list.txt";
	newPar.outputDir = "output/";
	newPar.tryTimes = 10;
	newPar.ifZero = true;
	newPar.ifDirected = true;
	newPar.ifTree = true;


	std::ostringstream args = convertPara(newPar);

	//���ĳ���
	int ret = run(args.str());


	system("pause");
	return ret;
}


std::ostringstream convertPara(inputPara myPara) 
{
	std::ostringstream args("");

	if (myPara.inputFile.isEmpty()) {
		std::cout << "no input file";
		return args;
	}
	else {
		args << myPara.inputFile.toStdString().c_str()<< " ";
	}

	if (myPara.outputDir.isEmpty()) {
		std::cout << "no output dir";
		return args;
	}
	else {
		args << myPara.outputDir.toStdString().c_str() << " ";
	}

	if (myPara.inputFormat != "link-list") {
		args << myPara.inputFormat.toStdString().c_str() << " ";
	}

	if (myPara.tryTimes != 1) {
		args << "-N " << myPara.tryTimes << " ";
	}

	if (myPara.ifDirected) {
		args << "--directed ";
	}

	if (myPara.ifOverlap) {
		args << "--overlapping ";
	}

	if (myPara.ifZero) {
		args << "-z ";
	}

	if (myPara.ifSelfLink) {
		args << "-k ";
	}

	if (myPara.if2) {
		args << "-2 ";
	}

	if (myPara.ifTopClu) {
		args << "--clu ";
	}

	if (myPara.ifMap) {
		args << "--map ";
	}

	if (myPara.ifTree) {
		args << "--tree ";
	}

	if (myPara.ifBFTree) {
		args << "--bftree ";
	}

	return args;
}
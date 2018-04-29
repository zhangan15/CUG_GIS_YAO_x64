#include "Infomap.h"
#include "QtCore\qstring.h"
#include <string.h>


struct inputPara {
	//输入文件路径
	QString inputFile = "";
	//输入格式，'pajek', 'link-list', 'states', '3gram', 'multilayer' or 'bipartite'
	//Link_list格式：起始结点 终止结点 边权重
	QString inputFormat = "link-list";
	//输出文件夹（需要被创建）
	QString outputDir = "";
	
	//循环次数
	int tryTimes = 1;
	//是否有向图
	bool ifDirected = false;
	//节点是否能属于不同的model
	bool ifOverlap = false;
	//节点是否从1开始编号
	bool ifZero = false;
	//是否包括 links with the same source and target node.
	bool ifSelfLink = false;
	//是否只将网络划分为two-level
	bool if2 = false;
	//是否输出每个节点的top cluster index
	bool ifTopClu = false;
	//是否输出.map格式文件（有助于使用网站可视化功能）
	bool ifMap = false;
	//是否输出.tree格式文件（直接使用记事本打开，处理方式等于文本文件)
	bool ifTree = false;
	//是否输出.bftree格式文件（二进制存储，有利于使用网站可视化功能）
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

	//核心程序
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
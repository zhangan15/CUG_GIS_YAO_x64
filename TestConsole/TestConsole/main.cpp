#pragma execution_character_set("utf-8") //支持中文步骤1

#include <QtCore>
#include <windows.h>
#include <tchar.h>

//lib: wlanapi.lib
#include <wlanapi.h>
#include <iostream>
using namespace std;

#include "D:/Code/SYSU_GIS_YAO_x64/thirdlib/dlib/include/dlib/data_io.h"
using namespace dlib;



QStringList getFromFile(QString sfilename)
{
	QFile _file(sfilename);
	if(!_file.open(QIODevice::ReadOnly)) return QStringList();
	QTextStream _in(&_file);
	QString str = _in.readLine();
	QStringList slist = str.split(" ", QString::SkipEmptyParts);
	_file.close();
	return slist;
}

bool writeCHNFile(QString sfilename, QStringList slist)
{
	QFile _file(sfilename);
	if (!_file.open(QIODevice::WriteOnly)) return false;
	QTextStream _out(&_file);
	foreach(QString str, slist)
		_out << str << " ";
	_file.close();
	return true;
}

int getWifiTest()
{
	DWORD dwError = ERROR_SUCCESS;
	DWORD dwNegotiatedVersion;
	HANDLE hClientHandle = NULL;

	dwError = WlanOpenHandle(1, NULL, &dwNegotiatedVersion, &hClientHandle);
	if (dwError != ERROR_SUCCESS)
	{
		WlanCloseHandle(hClientHandle, NULL);
		return -1;
	}

	PWLAN_INTERFACE_INFO_LIST pInterfaceList = NULL;
	dwError = WlanEnumInterfaces(hClientHandle, NULL, &pInterfaceList);
	if (dwError != ERROR_SUCCESS)
	{
		WlanFreeMemory(pInterfaceList);
		WlanCloseHandle(hClientHandle, NULL);
		return -2;
	}

	GUID &guid = pInterfaceList->InterfaceInfo[0].InterfaceGuid;
	PWLAN_AVAILABLE_NETWORK_LIST pWLAN_AVAILABLE_NETWORK_LIST = NULL;

	dwError = WlanGetAvailableNetworkList(hClientHandle, &guid,
		2, NULL, &pWLAN_AVAILABLE_NETWORK_LIST);
	if (dwError != ERROR_SUCCESS)
	{
		WlanFreeMemory(pInterfaceList);
		WlanFreeMemory(pWLAN_AVAILABLE_NETWORK_LIST);
		WlanCloseHandle(hClientHandle, NULL);
		return -3;
	}

	WLAN_AVAILABLE_NETWORK wlanAN;
	bool isConnected = false;
	int numberOfItems = pWLAN_AVAILABLE_NETWORK_LIST->dwNumberOfItems;
	cout << "number of wifi servers = " << numberOfItems << endl;
	if (numberOfItems > 0)
	{
		for (int i = 0; i < numberOfItems; i++)
		{
			wlanAN = pWLAN_AVAILABLE_NETWORK_LIST->Network[i];

			int wifiQuality = (int)wlanAN.wlanSignalQuality;

			//wcout << wstring(wlanAN.strProfileName) << endl;	
			QString wifi_ssid = QString((char*)wlanAN.dot11Ssid.ucSSID);
			QString wifiName = QString::fromWCharArray(wlanAN.strProfileName);
			int wifi_flag = wlanAN.dwFlags;

			cout << "**********" << endl;
			cout << "wifi device No. " << i + 1 << endl;
			cout << "wifi flag = " << wifi_flag << endl;
			wcout << "wifi ssid = " << wifi_ssid.toStdWString() << endl;
			wcout << "wifi name = " << wifiName.toStdWString() << endl;
			cout << "wifi quality = " << wifiQuality << endl;
			cout << "**********" << endl;


			if (wlanAN.dwFlags & 1)
			{
				isConnected = true;

				// 				int wifiQuality = (int)wlanAN.wlanSignalQuality;
				// 				QString wifiName = QString::fromWCharArray(wlanAN.strProfileName);
				// 
				// 				cout << "wifi name = " << wifiName.toStdString().data() << endl;
				// 				cout << "wifi quality = " << wifiQuality << endl;

			}
		}
	}
	if (!isConnected)
	{
		cout << "wifi is not connectted" << endl;
	}
	WlanFreeMemory(pInterfaceList);
	WlanFreeMemory(pWLAN_AVAILABLE_NETWORK_LIST);
	WlanCloseHandle(hClientHandle, NULL);

	return 0;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	//支持中文步骤2
	wcout.imbue(locale("chs"));

	/*
	QStringList slist = getFromFile("D:\\Code\\SYSU_GIS_YAO_x64\\TestConsole\\TestConsole\\输出.txt");
	foreach(QString str, slist)
		wcout << str.toStdWString() << endl;

	slist << "这是外部的" << "中文编码";
	writeCHNFile("D:\\Code\\SYSU_GIS_YAO_x64\\TestConsole\\TestConsole\\输出2.txt", slist);
	return 0;
	*/

// 	matrix<uint8_t> label_image;
// 	dlib::load_image(label_image, "E:\\Data\\ADEChallengeData2016\\annotations\\validation\\ADE_val_00000003.png");
// 	cout << label_image.nr() << " * " << label_image.nc() << endl;
// 
// 	dlib::save_png(label_image, ".\\test.png");
// 
// 	matrix<uint16_t> label_image1;
// 	dlib::load_image(label_image1, ".\\test.png");
// 
// 	long nr = label_image.nr();
// 	long nc = label_image.nc();
// 	
// 	double dsum = 0;
// 	for (long r = 0; r < nr; ++r)
// 	{
// 		for (long c = 0; c < nc; ++c)
// 		{
// 			dsum += (int)label_image(r, c)-label_image1(r, c);
// 		}
// 		//cout << endl;
// 	}
// 
// 	cout << dsum << endl;

// 	QDir dir("E:\\Data\\ADEChallengeData2016\\images\\validation");
// 	QFileInfoList filist = dir.entryInfoList(QStringList(QString("*.jpg")));
// 	cout << filist.size() << endl;
// 
// 	QFile file("E:\\Data\\ADEChallengeData2016\\validation.txt");
// 	file.open(QIODevice::WriteOnly);
// 	QTextStream _in(&file);
// 	foreach(QFileInfo fi, filist)
// 	{
// 		_in << fi.completeBaseName() << "\r\n";
// 	}
// 	_in.flush();
// 	file.close();
// 	cout << "finished." << endl;

	QFile _inpf("D:\\Code\\SYSU_GIS_YAO_x64\\BuildingTextureRDF\\data\\all_buildings_step10_max500.csv");
	QFile _outf("D:\\Code\\SYSU_GIS_YAO_x64\\BuildingTextureRDF\\data\\all_buildings_step10_max500_entropy.csv");
	_inpf.open(QIODevice::ReadOnly);
	_outf.open(QIODevice::WriteOnly);
	QTextStream _in(&_inpf), _out(&_outf);
	QString s = _in.readLine();
	QStringList slist = s.split(",");
	int nFeatureSize = slist.size() - 4;

	_out << "nFID, entropy" << "\r\n";

	/////////////////////////////////////////
	while (!_in.atEnd())
	{
		s = _in.readLine();
		slist = s.split(",");

		if (slist.size() <= nFeatureSize)
			continue;

		long nID = slist[0].trimmed().toLong();
		double dEntropy = 0;
		for (int i = 4; i < slist.size(); i++)
		{
			double dval = slist[i].trimmed().toDouble();
			if (dval < 10e-6)
				continue;
			dEntropy += dval*log(dval);
		}
		dEntropy = -dEntropy;

		_out << QString("%1, %2").arg(nID).arg(dEntropy, 0, 'f', 6) << "\r\n";

		_out.flush();

	}


	_inpf.close();
	_out.flush();
	_outf.flush();
	_outf.close();

	cout << "process success." << endl;

	return a.exec();
}

#pragma execution_character_set("utf-8") //支持中文步骤1
#include <QtCore>

#include "Wifi_Detection.h"

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	//支持中文步骤2
	wcout.imbue(locale("chs"));

	//获取wifi数目
	QList<Wifi_Signal_Attribute> wifi_collection = WifiDetection();
	foreach(Wifi_Signal_Attribute wsa, wifi_collection)
	{
		cout << "**********" << endl;
		cout << "wifi flag = " << wsa.n_flag << endl;
		wcout << "wifi ssid = " << wsa.s_ssid.toStdWString() << endl;
		wcout << "wifi name = " << wsa.s_name.toStdWString() << endl;
		cout << "wifi quality = " << wsa.d_quality << endl;
		cout << "**********" << endl;
	}

	//从文件获取wifi设备信息
	QList<Wifi_Device_Attribute> wifi_devices = LoadWifiDevicesFromFile("./wifi_devices_demo.csv");

	cout << "*****All Wifi Devices*****" << endl;
	foreach(Wifi_Device_Attribute _wd, wifi_devices)
	{
		wcout << _wd.s_ssid.toStdWString() << ", " \
			<< _wd.d_transmitting_frequency_GHZ << " GHZ, " \
			<< _wd.d_transmitting_power_mW << " mW, loc (" \
			<< _wd.loc.dx << ", " << _wd.loc.dy << ", " << _wd.loc.dz << ")" << endl;
	}
	cout << "**********" << endl;
	

	//测试wifi距离，采用测试数据
	Wifi_Signal_Attribute wsAttr;
	wsAttr.s_ssid = "0001";
	wsAttr.s_name = "";
	wsAttr.d_quality = 80;
	wsAttr.n_flag = 1;
	wsAttr.b_isConnectted = false;

	Wifi_Device_Attribute wdAttr;
	wdAttr.s_ssid = wsAttr.s_ssid;
	wdAttr.d_transmitting_frequency_GHZ = 2.4;
	wdAttr.d_transmitting_power_mW = 20;
	wdAttr.loc.dx = wdAttr.loc.dy = wdAttr.loc.dz = 0;

	cout << "**********" << endl;
	double dist = getDistanceBetweenDeviceAndWifi(wdAttr, wsAttr);
	cout << "distance = " << dist << " meters" << endl;
	cout << "**********" << endl;

	//匹配wifi设备和devices, 并计算两者距离
	QList<Wifi_Device_Attribute> valid_wifi_devices = MatchDevicesAndWifi(wifi_collection, wifi_devices);
	cout << "*****Valid Wifi Devices*****" << endl;
	foreach(Wifi_Device_Attribute _wd, valid_wifi_devices)
	{
		wcout << _wd.s_ssid.toStdWString() << ", " \
			<< _wd.d_transmitting_frequency_GHZ << " GHZ, " \
			<< _wd.d_transmitting_power_mW << " mW, loc (" \
			<< _wd.loc.dx << ", " << _wd.loc.dy << ", " << _wd.loc.dz << "), distance = " \
			<< _wd.d_distance_to_cur_device << " meters" << endl;
	}
	cout << "**********" << endl;

	/*
	//测试查找设别距离（csv文件是随机模拟数据，所以这段代码得到的误差极大，请用下面室内定位demo代码）
	Coord_Location loc = getDevicePosition_GuassNewton(valid_wifi_devices, 500, 2, false); //迭代5000次，最大方差2 meters, 输出迭代信息

	cout << "**********" << endl;
	cout << "Best Result:" << endl;
	cout << "x, y, z, ratio, avgerror = " << loc.dx << ", " << loc.dy << ", " << loc.dz << ", " << loc.dRatio << ", " << loc.dAvgError << endl;
	cout << "**********" << endl;
	*/

	
	//室内定位 demo，根据wifi和高斯牛顿法计算相对位置，模拟位置(大约在一个矩形曲面的中间)
	Wifi_Device_Attribute wdAttr_1;
	wdAttr_1.s_ssid = "RD";
	wdAttr_1.d_transmitting_frequency_GHZ = 2.4;
	wdAttr_1.d_transmitting_power_mW = 20;
	wdAttr_1.loc.dx = 10;
	wdAttr_1.loc.dy = 10;
	wdAttr_1.loc.dz = 10;
	wdAttr_1.d_distance_to_cur_device = 15;

	Wifi_Device_Attribute wdAttr_2;
	wdAttr_2.s_ssid = "RU";
	wdAttr_2.d_transmitting_frequency_GHZ = 2.4;
	wdAttr_2.d_transmitting_power_mW = 20;
	wdAttr_2.loc.dx = 10;
	wdAttr_2.loc.dy = 30;
	wdAttr_2.loc.dz = 20;
	wdAttr_2.d_distance_to_cur_device = 17;


	Wifi_Device_Attribute wdAttr_3;
	wdAttr_3.s_ssid = "LD";
	wdAttr_3.d_transmitting_frequency_GHZ = 2.4;
	wdAttr_3.d_transmitting_power_mW = 20;
	wdAttr_3.loc.dx = 30;
	wdAttr_3.loc.dy = 10;
	wdAttr_3.loc.dz = 15;
	wdAttr_3.d_distance_to_cur_device = 14.8;

	Wifi_Device_Attribute wdAttr_4;
	wdAttr_4.s_ssid = "LU";
	wdAttr_4.d_transmitting_frequency_GHZ = 2.4;
	wdAttr_4.d_transmitting_power_mW = 20;
	wdAttr_4.loc.dx = 30;
	wdAttr_4.loc.dy = 30;
	wdAttr_4.loc.dz = 20;
	wdAttr_4.d_distance_to_cur_device = 16.3;

	QList<Wifi_Device_Attribute> wdAttrs;
	wdAttrs << wdAttr_1 << wdAttr_2 << wdAttr_3 << wdAttr_4;

	Coord_Location loc = getDevicePosition_GaussNewton(wdAttrs, 500, 2, false); //迭代5000次，最大方差2 meters, 不输出迭代信息
	
	cout << "**********" << endl;
	cout << "Best Result:" << endl;
	cout << "x, y, z, ratio, avgerror = " << loc.dx << ", " << loc.dy << ", " << loc.dz << ", " << loc.dRatio << ", " << loc.dAvgError << endl;
	cout << "**********" << endl;
	

	return 0;
}

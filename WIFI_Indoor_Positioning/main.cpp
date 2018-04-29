#pragma execution_character_set("utf-8") //֧�����Ĳ���1
#include <QtCore>

#include "Wifi_Detection.h"

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	//֧�����Ĳ���2
	wcout.imbue(locale("chs"));

	//��ȡwifi��Ŀ
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

	//���ļ���ȡwifi�豸��Ϣ
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
	

	//����wifi���룬���ò�������
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

	//ƥ��wifi�豸��devices, ���������߾���
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
	//���Բ��������루csv�ļ������ģ�����ݣ�������δ���õ������������������ڶ�λdemo���룩
	Coord_Location loc = getDevicePosition_GuassNewton(valid_wifi_devices, 500, 2, false); //����5000�Σ���󷽲�2 meters, ���������Ϣ

	cout << "**********" << endl;
	cout << "Best Result:" << endl;
	cout << "x, y, z, ratio, avgerror = " << loc.dx << ", " << loc.dy << ", " << loc.dz << ", " << loc.dRatio << ", " << loc.dAvgError << endl;
	cout << "**********" << endl;
	*/

	
	//���ڶ�λ demo������wifi�͸�˹ţ�ٷ��������λ�ã�ģ��λ��(��Լ��һ������������м�)
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

	Coord_Location loc = getDevicePosition_GaussNewton(wdAttrs, 500, 2, false); //����5000�Σ���󷽲�2 meters, �����������Ϣ
	
	cout << "**********" << endl;
	cout << "Best Result:" << endl;
	cout << "x, y, z, ratio, avgerror = " << loc.dx << ", " << loc.dy << ", " << loc.dz << ", " << loc.dRatio << ", " << loc.dAvgError << endl;
	cout << "**********" << endl;
	

	return 0;
}

#include "Wifi_Detection.h"
#include <windows.h>
#include <tchar.h>
#include <cmath>

#include <QFile>
#include <QTextStream>

#include "matrix.h"

//lib: wlanapi.lib
#include <wlanapi.h>
#include <iostream>
using namespace std;
using namespace math;

#ifndef _NO_TEMPLATE
typedef matrix<double> Matrix;
#else
typedef matrix Matrix;
#endif

#ifndef _NO_EXCEPTION
#  define TRYBEGIN()	try {
#  define CATCHERROR()	} catch (const STD::exception& e) { \
						cerr << "Error: " << e.what() << endl; }
#else
#  define TRYBEGIN()
#  define CATCHERROR()
#endif



QList<Wifi_Signal_Attribute> WifiDetection()
{
	//支持中文步骤2
	wcout.imbue(locale("chs"));
	
	//返回值
	QList<Wifi_Signal_Attribute> wifi_collection;
	wifi_collection.clear();

	DWORD dwError = ERROR_SUCCESS;
	DWORD dwNegotiatedVersion;
	HANDLE hClientHandle = NULL;

	dwError = WlanOpenHandle(1, NULL, &dwNegotiatedVersion, &hClientHandle);
	if (dwError != ERROR_SUCCESS)
	{
		WlanCloseHandle(hClientHandle, NULL);
		return wifi_collection;
	}

	PWLAN_INTERFACE_INFO_LIST pInterfaceList = NULL;
	dwError = WlanEnumInterfaces(hClientHandle, NULL, &pInterfaceList);
	if (dwError != ERROR_SUCCESS)
	{
		WlanFreeMemory(pInterfaceList);
		WlanCloseHandle(hClientHandle, NULL);
		return wifi_collection;
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
		return wifi_collection;
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

			Wifi_Signal_Attribute _wifi_attr;
			_wifi_attr.s_ssid = QString((char*)wlanAN.dot11Ssid.ucSSID);
			_wifi_attr.s_name = QString::fromWCharArray(wlanAN.strProfileName);
			_wifi_attr.n_flag = wlanAN.dwFlags;
			_wifi_attr.d_quality = (long)wlanAN.wlanSignalQuality;

// 			cout << "**********" << endl;
// 			cout << "wifi device No. " << i + 1 << endl;
// 			cout << "wifi flag = " << _wifi_attr.n_flag << endl;
// 			wcout << "wifi ssid = " << _wifi_attr.s_ssid.toStdWString() << endl;
// 			wcout << "wifi name = " << _wifi_attr.s_name.toStdWString() << endl;
// 			cout << "wifi quality = " << _wifi_attr.d_quality << endl;
// 			cout << "**********" << endl;
			
			if (wlanAN.dwFlags & 1)
				_wifi_attr.b_isConnectted = true;
			else
				_wifi_attr.b_isConnectted = false;

			_wifi_attr.d_distance_to_cur_wifi = -1.0f;

			wifi_collection.append(_wifi_attr);
		}
	}

	WlanFreeMemory(pInterfaceList);
	WlanFreeMemory(pWLAN_AVAILABLE_NETWORK_LIST);
	WlanCloseHandle(hClientHandle, NULL);

	return wifi_collection;
}

QList<Wifi_Device_Attribute> LoadWifiDevicesFromFile(const char* sfilename)
{
	QList<Wifi_Device_Attribute> wifi_devices;
	wifi_devices.clear();

	QFile _file(sfilename);
	if (!_file.open(QIODevice::ReadOnly))
		return wifi_devices;

	QTextStream _in(&_file);
	QString sline = _in.readLine();	//读取第一行
	QStringList slist = sline.split(",", QString::KeepEmptyParts);

	//ssid, transmitting_frequency_GHZ, transmitting_power_mW, x, y, z
	if (slist.size() < 6)
	{
		_file.close();
		return wifi_devices;
	}

	//
	while (!_in.atEnd())
	{
		sline = _in.readLine();
		slist = sline.split(",", QString::KeepEmptyParts);

		if (slist.size() < 6)
			continue;

		Wifi_Device_Attribute _wd;
		_wd.s_ssid = slist[0].trimmed();
		_wd.d_transmitting_frequency_GHZ = slist[1].trimmed().toDouble();
		_wd.d_transmitting_power_mW = slist[2].trimmed().toDouble();
		_wd.loc.dx = slist[3].trimmed().toDouble();
		_wd.loc.dy = slist[4].trimmed().toDouble();
		_wd.loc.dz = slist[5].trimmed().toDouble();
		_wd.d_distance_to_cur_device = -1.0f;

		wifi_devices.append(_wd);
	}


	_file.close();

	return wifi_devices;
}

QList<Wifi_Device_Attribute> MatchDevicesAndWifi(QList<Wifi_Signal_Attribute>& wsAttrs, QList<Wifi_Device_Attribute>& wdAttrs)
{
	QList<Wifi_Device_Attribute> wifi_devices;
	wifi_devices.clear();

	if (wsAttrs.isEmpty() || wdAttrs.isEmpty())
		return wifi_devices;
	
	//开始匹配
	foreach(Wifi_Signal_Attribute _ws, wsAttrs)
	{
		foreach(Wifi_Device_Attribute _wd, wdAttrs)
		{
			if (_ws.s_ssid.trimmed().toLower() == _wd.s_ssid.trimmed().toLower())
			{
				double dist = getDistanceBetweenDeviceAndWifi(_wd, _ws);
				if (dist <= 0) break;	//如果设备没有初始化不记录
				
				wifi_devices.append(_wd);
				break;
			}
		}
	}


	return wifi_devices;
}

double getDistanceBetweenDeviceAndWifi(Wifi_Device_Attribute& wdAttr, Wifi_Signal_Attribute& wsAttr)
{
	if (wdAttr.s_ssid.trimmed().toLower() != wsAttr.s_ssid.trimmed().toLower())
		return -1;

	if (wdAttr.d_transmitting_power_mW == 0 || wdAttr.d_transmitting_frequency_GHZ == 0)
		return -1;
	
	double dist = 1000 * exp(((1 - (double)wsAttr.d_quality / 100.0f) \
		*wdAttr.d_transmitting_power_mW - 32.45 \
		- 20.0f * log(wdAttr.d_transmitting_frequency_GHZ)) / \
		wdAttr.d_transmitting_power_mW);

	wdAttr.d_distance_to_cur_device = dist;
	wsAttr.d_distance_to_cur_wifi = dist;
	return dist;
}

Coord_Location getDevicePosition_GaussNewton(QList<Wifi_Device_Attribute> wdAttrs, int nMaxIterNum /*= 10*/, double minAvgError, bool bPrintInfo)
{
	Coord_Location device_loc, device_best_loc;
	device_loc.dx = device_loc.dy = device_loc.dz = -1.0f;
	device_loc.dRatio = 1.0f;
	device_loc.dAvgError = 0.0f;
	device_best_loc = device_loc;

	if (wdAttrs.size() < 3)
		return device_loc;

	//Guass-Newton Iterations
	int nIterCount = 0;
	int nWifiCount = wdAttrs.size();
	int i = 0;
	

	//获取初始化误差矩阵
	Matrix error_S(nWifiCount, 1);
	device_loc.dAvgError = 0.0f;
	for (i = 0; i < nWifiCount; i++)
	{
		error_S(i, 0) = abs(sqrt((device_loc.dx-wdAttrs[i].loc.dx)*(device_loc.dx - wdAttrs[i].loc.dx)+ \
			(device_loc.dy - wdAttrs[i].loc.dy)*(device_loc.dy - wdAttrs[i].loc.dy)+ \
			(device_loc.dz - wdAttrs[i].loc.dz)*(device_loc.dz - wdAttrs[i].loc.dz)) -
			(device_loc.dRatio*wdAttrs[i].d_distance_to_cur_device)/**(device_loc.dRatio*wdAttrs[i].d_distance_to_cur_device)*/);

		device_loc.dAvgError += error_S(i, 0);
	}
	double small_avg_error = device_loc.dAvgError;
	
	
	Matrix para_J(nWifiCount, 4);	//参量
	Matrix trans_x(4, 1);	//待求解变量

	while (nIterCount < nMaxIterNum || device_loc.dAvgError <= minAvgError)
	{
		//获取偏导矩阵J
		for (i=0; i<nWifiCount; i++)
		{
			para_J(i, 0) = -2.0f * (device_loc.dx - wdAttrs[i].loc.dx);
			para_J(i, 1) = -2.0f * (device_loc.dy - wdAttrs[i].loc.dy);
			para_J(i, 2) = -2.0f * (device_loc.dz - wdAttrs[i].loc.dz);
			para_J(i, 3) = 2.0 * device_loc.dRatio * wdAttrs[i].d_distance_to_cur_device * wdAttrs[i].d_distance_to_cur_device;
		}

		//求解偏移量x
		trans_x = !(~para_J * para_J) * (~para_J * error_S);

		//更新device_loc坐标
		device_loc.dx += trans_x(0, 0);
		device_loc.dy += trans_x(1, 0);
		device_loc.dz += trans_x(2, 0);
		device_loc.dRatio += trans_x(3, 0);		

		//更新误差矩阵并计算平均误差AvgError
		device_loc.dAvgError = 0.0f;
		for (i = 0; i < nWifiCount; i++)
		{
			error_S(i, 0) = abs(sqrt((device_loc.dx - wdAttrs[i].loc.dx)*(device_loc.dx - wdAttrs[i].loc.dx) + \
				(device_loc.dy - wdAttrs[i].loc.dy)*(device_loc.dy - wdAttrs[i].loc.dy) + \
				(device_loc.dz - wdAttrs[i].loc.dz)*(device_loc.dz - wdAttrs[i].loc.dz)) -
				(device_loc.dRatio*wdAttrs[i].d_distance_to_cur_device)/**(device_loc.dRatio*wdAttrs[i].d_distance_to_cur_device)*/);

			device_loc.dAvgError += error_S(i, 0);
		}

		//输出迭代过程
		if (bPrintInfo)
		{
			cout << "****************" << endl;
			cout << "iter No. " << nIterCount + 1 << endl;
			cout << "x, y, z, ratio, avgerror = " << device_loc.dx << ", " << device_loc.dy << ", " << device_loc.dz << ", " << device_loc.dRatio << ", " << device_loc.dAvgError << endl;
			cout << "****************" << endl;
		}		

		//判断是否最优
		if (small_avg_error > device_loc.dAvgError)
		{
			device_best_loc = device_loc;
			small_avg_error = device_loc.dAvgError;
		}
			

		nIterCount++;
	}

	return device_best_loc;
}

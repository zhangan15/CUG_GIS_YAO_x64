#ifndef CLASS_WIFI_DETECTION_20171015
#define CLASS_WIFI_DETECTION_20171015
#pragma execution_character_set("utf-8")

#include <QString>
#include <QList>

struct Coord_Location
{
	double dx;
	double dy;
	double dz;
	double dRatio;
	double dAvgError;
};

struct Wifi_Signal_Attribute
{
	QString s_ssid;
	QString s_name;
	long d_quality;
	int n_flag; //0 non-connection; >=1 connection
	bool b_isConnectted;
	double d_distance_to_cur_wifi;	//unknown = -1, unit: meters
};

struct Wifi_Device_Attribute
{
	QString s_ssid;
	double d_transmitting_frequency_GHZ; //2.4 or 5.0
	double d_transmitting_power_mW;	//Generally about 10mW-20mW in China
	double d_distance_to_cur_device;	//unknown = -1, unit: meters
	Coord_Location loc;
};

//STEP 1. 获取wifi数据, 但由于wifi属性未知，故离设备距离为-1
QList<Wifi_Signal_Attribute> WifiDetection();

//STEP 2. 载入wifi设备信息(csv, 英文逗号分隔)：ssid, transmitting_frequency_GHZ, transmitting_power_mW, x, y, z
QList<Wifi_Device_Attribute> LoadWifiDevicesFromFile(const char* sfilename);

//STEP 3. 匹配wifi设备，计算设备间距离，返回匹配wifi设备信息(带有距离)
QList<Wifi_Device_Attribute> MatchDevicesAndWifi(QList<Wifi_Signal_Attribute>& wsAttrs, QList<Wifi_Device_Attribute>& wdAttrs);

//STEP 4. 采用高斯牛顿法，求解中心距离 
Coord_Location getDevicePosition_GaussNewton(QList<Wifi_Device_Attribute> wdAttrs, int nMaxIterNum = 10, double minAvgError = 0.5, bool bPrintInfo = false);


/*******************以下为辅助算法*******************/

//根据信号比反推距离，并写入Wifi_Device_Attribute和Wifi_Signal_Attribute属性
double getDistanceBetweenDeviceAndWifi(Wifi_Device_Attribute& wdAttr, Wifi_Signal_Attribute& wsAttr);


#endif


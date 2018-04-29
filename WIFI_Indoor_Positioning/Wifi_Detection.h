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

//STEP 1. ��ȡwifi����, ������wifi����δ֪�������豸����Ϊ-1
QList<Wifi_Signal_Attribute> WifiDetection();

//STEP 2. ����wifi�豸��Ϣ(csv, Ӣ�Ķ��ŷָ�)��ssid, transmitting_frequency_GHZ, transmitting_power_mW, x, y, z
QList<Wifi_Device_Attribute> LoadWifiDevicesFromFile(const char* sfilename);

//STEP 3. ƥ��wifi�豸�������豸����룬����ƥ��wifi�豸��Ϣ(���о���)
QList<Wifi_Device_Attribute> MatchDevicesAndWifi(QList<Wifi_Signal_Attribute>& wsAttrs, QList<Wifi_Device_Attribute>& wdAttrs);

//STEP 4. ���ø�˹ţ�ٷ���������ľ��� 
Coord_Location getDevicePosition_GaussNewton(QList<Wifi_Device_Attribute> wdAttrs, int nMaxIterNum = 10, double minAvgError = 0.5, bool bPrintInfo = false);


/*******************����Ϊ�����㷨*******************/

//�����źűȷ��ƾ��룬��д��Wifi_Device_Attribute��Wifi_Signal_Attribute����
double getDistanceBetweenDeviceAndWifi(Wifi_Device_Attribute& wdAttr, Wifi_Signal_Attribute& wsAttr);


#endif


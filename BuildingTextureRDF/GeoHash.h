#ifndef GEOHASH_WRITE_BY_YAO_VER_10
#define GEOHASH_WRITE_BY_YAO_VER_10

#include <string>
using namespace std;

/*******************************************************************
* ���������ص�P1(lng1, lat1)��P2(lng2, lat2)�Ľ��ƴ���߾���(��λ km)
* double lng1: ��һ���ص�ľ���(�Ƕ�)
* double lat1: ��һ���ص��γ��(�Ƕ�)
* double lng2: �ڶ����ص�ľ���(�Ƕ�)
* double lat2: �ڶ����ص��γ��(�Ƕ�)
* ����ֵ: double, ������֮��Ľ��ƴ���߾��룬��λ km
*******************************************************************/
double getPtsDist(double lat1, double lng1, double lat2, double lng2);

namespace GeoHash {
	enum Direction 
	{
		Top = 0,
		Right = 1,
		Bottom = 2,
		Left = 3,
		Limit = 4
	};
		
	void RefineInterval(double* interval, int cd, int mask);

	// reference: https://www.cnblogs.com/aiweixiao/p/6188081.html
	string CalculateAdjacent(string hash, Direction direction);
	void DecodeGeoHash(string geohash, double& dlat, double& dlon);
	string EncodeLatLon(double latitude, double longitude, int precision = 12);

	// return unit: KM
	double distanceBetweenGeoHashes(string hash1, string hash2);
};




#endif





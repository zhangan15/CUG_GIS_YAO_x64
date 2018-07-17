#include <GeoHash.h>

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{

	

	double lat = 30.000000;
	double lon = 114.000000;

	std::string str = GeoHash::EncodeLatLon(lat, lon, 10);
	cout << str << endl;

	double lat1, lon1;
	GeoHash::DecodeGeoHash(str, lat1, lon1);
	GeoHash::DecodeGeoHash("ws10kygf", lat1, lon1);
	cout << lon1 << ", " << lat1 << endl;

	return -1;

	string str_top = GeoHash::CalculateAdjacent(str, GeoHash::Top);
	string str_bottom = GeoHash::CalculateAdjacent(str, GeoHash::Bottom);
	string str_Right = GeoHash::CalculateAdjacent(str, GeoHash::Right);
	string str_Left = GeoHash::CalculateAdjacent(str, GeoHash::Left);
	
	GeoHash::DecodeGeoHash(str_top, lat1, lon1);
	cout << str_top << " TOP: " << lon1 << ", " << lat1 << endl;
	GeoHash::DecodeGeoHash(str_bottom, lat1, lon1);
	cout << str_bottom << " BOTTOM: " << lon1 << ", " << lat1 << endl;
	GeoHash::DecodeGeoHash(str_Left, lat1, lon1);
	cout << str_Left << " LEFT: " << lon1 << ", " << lat1 << endl;
	GeoHash::DecodeGeoHash(str_Right, lat1, lon1);
	cout << str_Right << " RIGHT: " << lon1 << ", " << lat1 << endl;

	cout << GeoHash::distanceBetweenGeoHashes(str_top, str)*1000.0f << " meters" << endl;


	return 0;
}

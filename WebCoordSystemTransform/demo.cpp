
#include <iostream>
#include "WebCoordSystemTransform.h"
using namespace std;

int main(int argc, char *argv[])
{
	double marLat = 27.21255;
	double marLon = 106.89519;
	double wgslat, wgslon;

	//火星坐标系和WGS84坐标系互相转换
	WEBCSTRANSFORM::gcj2wgs(marLat, marLon, wgslat, wgslon);
	cout<<wgslat<<", "<<wgslon<<endl;
	WEBCSTRANSFORM::wgs2gcj(wgslat, wgslon, marLat, marLon);
	cout<<marLat<<", "<<marLon<<endl;

	//百度坐标系和WGS84互相转换
	double bdlat = 22.82406, bdlon = 113.9523;
	WEBCSTRANSFORM::bd092wgs(bdlat, bdlon, wgslat, wgslon);
	cout<<wgslat<<", "<<wgslon<<endl;
	WEBCSTRANSFORM::wgs2bd09(wgslat, wgslon, bdlat, bdlon);
	cout<<bdlat<<", "<<bdlon<<endl;

	return 0;
}

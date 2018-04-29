
#include <iostream>
#include "WebCoordSystemTransform.h"
using namespace std;

int main(int argc, char *argv[])
{
	double marLat = 27.21255;
	double marLon = 106.89519;
	double wgslat, wgslon;

	//��������ϵ��WGS84����ϵ����ת��
	WEBCSTRANSFORM::gcj2wgs(marLat, marLon, wgslat, wgslon);
	cout<<wgslat<<", "<<wgslon<<endl;
	WEBCSTRANSFORM::wgs2gcj(wgslat, wgslon, marLat, marLon);
	cout<<marLat<<", "<<marLon<<endl;

	//�ٶ�����ϵ��WGS84����ת��
	double bdlat = 22.82406, bdlon = 113.9523;
	WEBCSTRANSFORM::bd092wgs(bdlat, bdlon, wgslat, wgslon);
	cout<<wgslat<<", "<<wgslon<<endl;
	WEBCSTRANSFORM::wgs2bd09(wgslat, wgslon, bdlat, bdlon);
	cout<<bdlat<<", "<<bdlon<<endl;

	return 0;
}

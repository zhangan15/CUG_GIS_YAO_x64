// SixS.cpp: implementation of the SixS class.
//
//////////////////////////////////////////////////////////////////////

#include "include_global.h"
#include "SixS_IMG.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SixS_IMG::SixS_IMG()
{
	xa = xb = xc = NULL;
	xe = xd = NULL;
	hDLL = NULL;   // Handle to DLL
}

BOOL SixS_IMG::LoadDLL(const char *name)
{
	hDLL = LoadLibrary(name);
//	hDLL = LoadLibrary("E:\\SpaceSImgSrc\\HAPIMG\\Debug\\6S_DLL.dll");
	if (hDLL != NULL)
	{
//		SSSSSS = (void (_stdcall*)(int *flag, float *asol,float *phi0,float *avis,float *phiv,
//		   float xabc[5], const char *FILEI, unsigned int len))GetProcAddress(hDLL,
//			(const char *)1);
		_GetCorrCoef = (void (_stdcall*)(float *asol,float *phi0,float *avis,float *phiv,float *xabc))GetProcAddress(hDLL,	"GETCORRCOEF");
		_SetDefualtPara = (void (_stdcall*)(const char *FILEI,unsigned int len))GetProcAddress(hDLL,	"SETDEFAULTPARA");
		_SetMonthDate = (void (_stdcall*)(int *mon, int *day))GetProcAddress(hDLL,	"SETMONTHDATE");
		_SetWaterVapor = (void (_stdcall*)(float *watervapor))GetProcAddress(hDLL,	"SETWATERVAPOR");
		_SetHorVis = (void (_stdcall*)(float *visibility))GetProcAddress(hDLL,	"SETHORVIS");
		_SetAeroDepth = (void (_stdcall*)(float *aeroso_depth))GetProcAddress(hDLL,	"SETAERODEPTH");
		_SetTargetAlt = (void (_stdcall*)(float *altitude))GetProcAddress(hDLL,	"SETTARGETALT");
		_SetSpectralBand = (void (_stdcall*)(int *band_id))GetProcAddress(hDLL,	"SETSPECTRALBAND");
		_SetSpecFilt = (void (_stdcall*)(float *wl_inf, float *wl_sup, float *fltervalue))GetProcAddress(hDLL,	"SETSPECFILT");
//		_SetATMOSCOR = (void (_stdcall*)(float* a, float* b, int* c, float* d, float* e, float* f, char* g, unsigned int h, int* i))GetProcAddress(hDLL,"ATMOSCOR");

		if (!_GetCorrCoef||!_SetDefualtPara||!_SetMonthDate||!_SetWaterVapor
			||!_SetHorVis||!_SetAeroDepth||!_SetTargetAlt||!_SetSpectralBand
			||!_SetSpectralBand)
		{
			// handle the error
			FreeLibrary(hDLL);
			return false;
		}

// 		if(!_SetATMOSCOR)
// 		{
// 			AfxMessageBox("_SetATMOSCOR函数没找到！");
// 			FreeLibrary(hDLL);
// 			return false;
// 		}
		return true;
	}
	else return false;
}

SixS_IMG::~SixS_IMG()
{
	if(xa) free(xa);
	if(xb) free(xb);
	if(xc) free(xc);
	if(xd) free(xd);
	if(xe) free(xe);
	if(hDLL) FreeLibrary(hDLL);	
}

BOOL SixS_IMG::PrepareCoefficients(float dsza, float dvza, float draa)
{
	int j, index;
	delta[0] = dvza;
	delta[1] = dsza;
	delta[2] = draa;
	if(delta[1] < 0) 
	{
		delta[1] = -delta[1];
		sza_i = 1;
	}
	else if(delta[1] < 0.1)
	{
		delta[1] = 0.1;
		delta[1] = 90.0 / floor(90. /delta[1]);
		sza_i = floor(90 / delta[1]) + 2;
	}
	else if(delta[1] > 45)
	{
		delta[1] = 45;
		delta[1] = 90.0 / floor(90. /delta[1]);
		sza_i = floor(90 / delta[1]) + 2;
	}
	else
	{
		delta[1] = 90.0 / floor(90. /delta[1]);
		sza_i = floor(90 / delta[1]) + 2;
	}
	if(delta[0] < 0.1) delta[0] = 0.1;
	else if(delta[0] > 45) delta[0] = 45;
	delta[0] = 90.0 / floor(90. /delta[0]);
	if(delta[2] < 0.1) delta[2] = 0.1;
	else if(delta[2] > 45) delta[2] = 45;
	delta[2] = 90.0 / floor(90. /delta[2]);
	vza_i = floor(90 / delta[0]) + 2;
	raa_i = floor(180 / delta[2]) + 2;
	index = sza_i * vza_i * raa_i;
	if(xa) free(xa);
	xa = (float *)malloc(index * sizeof(float));
	if(xa == NULL) return false;
	if(xb) free(xb);
	xb = (float *)malloc(index * sizeof(float));
	if(xb == NULL) return false;
	if(xc) free(xc);
	xc = (float *)malloc(index * sizeof(float));
	if(xc == NULL) return false;
	if(xd) free(xd);
	xd = (float *)malloc(index * sizeof(float));
	if(xd == NULL) return false;
	if(xe) free(xe);
	xe = (float *)malloc(index * sizeof(float));
	if(xe == NULL) return false;
	for(j = 0; j < index; j++)
		xb[j] = 0;
	return true;
}

void SixS_IMG::SubGetCoeff(int iv, int is, int ir, float *a, float *b, float *c, float *d, float *e)
{
	int index;
	int flag1 = -1;
	float xabc1[5], saa1 = 0;
	float sza1, vza1, vaa1;
	index = iv * sza_i * raa_i + is * raa_i + ir;
	if(xb[index] == 0)
	{
		vza1 = iv * delta[0];
		sza1 = is * delta[1];
		vaa1 = ir * delta[2];
		GetCorrectionCoefficients(sza1, saa1, vza1, vaa1, xabc1);
		*a = xa[index] = xabc1[0];
		*b = xb[index] = xabc1[1];
		*c = xc[index] = xabc1[2];
		*d = xd[index] = xabc1[3];
		*e = xe[index] = xabc1[4];
	}
	else
	{
		*a = xa[index];
		*b = xb[index];
		*c = xc[index];
		*d = xd[index];
		*e = xe[index];
	}
}

BOOL SixS_IMG::GetCoefficients(float sza, float vza, float raa)
{
	int is, iv, ir;
	float a[8], b[8], c[8], d[8], e[8];
	
	if(sza < 0 || sza > 90 || vza < 0 || vza > 90)
	{
		xabc[1] = 1;
		return false;
	}
	while(raa > 180) raa -= 360;
	while(raa < -180) raa += 360;
	raa = fabs(raa);
	
	
	if(sza_i == 1)
	{
		iv = floor(vza/delta[0]);
		is = 1;
		ir = floor(raa/delta[2]);
		SubGetCoeff(iv, is, ir, &a[0], &b[0], &c[0], &d[0], &e[0]);
		//	SubGetCoeff(iv, is+1, ir, &a[1], &b[1], &c[1], &d[1], &e[1]);
		//	a[0] = (a[0] * ((1+is)*delta[1] - sza) + a[1] * (sza - is*delta[1])) / delta[1];
		//	b[0] = (b[0] * ((1+is)*delta[1] - sza) + b[1] * (sza - is*delta[1])) / delta[1];
		//	c[0] = (c[0] * ((1+is)*delta[1] - sza) + c[1] * (sza - is*delta[1])) / delta[1];
		//	d[0] = (d[0] * ((1+is)*delta[1] - sza) + d[1] * (sza - is*delta[1])) / delta[1];
		//	e[0] = (e[0] * ((1+is)*delta[1] - sza) + e[1] * (sza - is*delta[1])) / delta[1];
		
		SubGetCoeff(iv+1, is, ir, &a[2], &b[2], &c[2], &d[2], &e[2]);
		//	SubGetCoeff(iv+1, is+1, ir, &a[3], &b[3], &c[3], &d[3], &e[3]);
		//	a[2] = (a[2] * ((1+is)*delta[1] - sza) + a[3] * (sza - is*delta[1])) / delta[1];
		//	b[2] = (b[2] * ((1+is)*delta[1] - sza) + b[3] * (sza - is*delta[1])) / delta[1];
		//	c[2] = (c[2] * ((1+is)*delta[1] - sza) + c[3] * (sza - is*delta[1])) / delta[1];
		//	d[2] = (d[2] * ((1+is)*delta[1] - sza) + d[3] * (sza - is*delta[1])) / delta[1];
		//	e[2] = (e[2] * ((1+is)*delta[1] - sza) + e[3] * (sza - is*delta[1])) / delta[1];
		
		a[0] = (a[0] * ((1+iv)*delta[0] - vza) + a[2] * (vza - iv*delta[0])) / delta[0];
		b[0] = (b[0] * ((1+iv)*delta[0] - vza) + b[2] * (vza - iv*delta[0])) / delta[0];
		c[0] = (c[0] * ((1+iv)*delta[0] - vza) + c[2] * (vza - iv*delta[0])) / delta[0];
		d[0] = (d[0] * ((1+iv)*delta[0] - vza) + d[2] * (vza - iv*delta[0])) / delta[0];
		e[0] = (e[0] * ((1+iv)*delta[0] - vza) + e[2] * (vza - iv*delta[0])) / delta[0];
		
		
		SubGetCoeff(iv, is, ir+1, &a[4], &b[4], &c[4], &d[4], &e[4]);
		//	SubGetCoeff(iv, is+1, ir+1, &a[5], &b[5], &c[5], &d[5], &e[5]);
		//	a[4] = (a[4] * ((1+is)*delta[1] - sza) + a[5] * (sza - is*delta[1])) / delta[1];
		//	b[4] = (b[4] * ((1+is)*delta[1] - sza) + b[5] * (sza - is*delta[1])) / delta[1];
		//	c[4] = (c[4] * ((1+is)*delta[1] - sza) + c[5] * (sza - is*delta[1])) / delta[1];
		//	d[4] = (d[4] * ((1+is)*delta[1] - sza) + d[5] * (sza - is*delta[1])) / delta[1];
		//	e[4] = (e[4] * ((1+is)*delta[1] - sza) + e[5] * (sza - is*delta[1])) / delta[1];
		
		SubGetCoeff(iv+1, is, ir+1, &a[6], &b[6], &c[6], &d[6], &e[6]);
		//	SubGetCoeff(iv+1, is+1, ir+1, &a[7], &b[7], &c[7], &d[7], &e[7]);
		//	a[6] = (a[6] * ((1+is)*delta[1] - sza) + a[7] * (sza - is*delta[1])) / delta[1];
		//	b[6] = (b[6] * ((1+is)*delta[1] - sza) + b[7] * (sza - is*delta[1])) / delta[1];
		//	c[6] = (c[6] * ((1+is)*delta[1] - sza) + c[7] * (sza - is*delta[1])) / delta[1];
		//	d[6] = (d[6] * ((1+is)*delta[1] - sza) + d[7] * (sza - is*delta[1])) / delta[1];
		//	e[6] = (e[6] * ((1+is)*delta[1] - sza) + e[7] * (sza - is*delta[1])) / delta[1];
	}
	else{
		iv = floor(vza/delta[0]);
		is = floor(sza/delta[1]);
		ir = floor(raa/delta[2]);
		SubGetCoeff(iv, is, ir, &a[0], &b[0], &c[0], &d[0], &e[0]);
		//	SubGetCoeff(iv, is+1, ir, &a[1], &b[1], &c[1], &d[1], &e[1]);
		//	a[0] = (a[0] * ((1+is)*delta[1] - sza) + a[1] * (sza - is*delta[1])) / delta[1];
		//	b[0] = (b[0] * ((1+is)*delta[1] - sza) + b[1] * (sza - is*delta[1])) / delta[1];
		//	c[0] = (c[0] * ((1+is)*delta[1] - sza) + c[1] * (sza - is*delta[1])) / delta[1];
		//	d[0] = (d[0] * ((1+is)*delta[1] - sza) + d[1] * (sza - is*delta[1])) / delta[1];
		//	e[0] = (e[0] * ((1+is)*delta[1] - sza) + e[1] * (sza - is*delta[1])) / delta[1];
		
		SubGetCoeff(iv+1, is, ir, &a[2], &b[2], &c[2], &d[2], &e[2]);
		//	SubGetCoeff(iv+1, is+1, ir, &a[3], &b[3], &c[3], &d[3], &e[3]);
		//	a[2] = (a[2] * ((1+is)*delta[1] - sza) + a[3] * (sza - is*delta[1])) / delta[1];
		//	b[2] = (b[2] * ((1+is)*delta[1] - sza) + b[3] * (sza - is*delta[1])) / delta[1];
		//	c[2] = (c[2] * ((1+is)*delta[1] - sza) + c[3] * (sza - is*delta[1])) / delta[1];
		//	d[2] = (d[2] * ((1+is)*delta[1] - sza) + d[3] * (sza - is*delta[1])) / delta[1];
		//	e[2] = (e[2] * ((1+is)*delta[1] - sza) + e[3] * (sza - is*delta[1])) / delta[1];
		
		a[0] = (a[0] * ((1+iv)*delta[0] - vza) + a[2] * (vza - iv*delta[0])) / delta[0];
		b[0] = (b[0] * ((1+iv)*delta[0] - vza) + b[2] * (vza - iv*delta[0])) / delta[0];
		c[0] = (c[0] * ((1+iv)*delta[0] - vza) + c[2] * (vza - iv*delta[0])) / delta[0];
		d[0] = (d[0] * ((1+iv)*delta[0] - vza) + d[2] * (vza - iv*delta[0])) / delta[0];
		e[0] = (e[0] * ((1+iv)*delta[0] - vza) + e[2] * (vza - iv*delta[0])) / delta[0];
		
		
		SubGetCoeff(iv, is, ir+1, &a[4], &b[4], &c[4], &d[4], &e[4]);
		//	SubGetCoeff(iv, is+1, ir+1, &a[5], &b[5], &c[5], &d[5], &e[5]);
		//	a[4] = (a[4] * ((1+is)*delta[1] - sza) + a[5] * (sza - is*delta[1])) / delta[1];
		//	b[4] = (b[4] * ((1+is)*delta[1] - sza) + b[5] * (sza - is*delta[1])) / delta[1];
		//	c[4] = (c[4] * ((1+is)*delta[1] - sza) + c[5] * (sza - is*delta[1])) / delta[1];
		//	d[4] = (d[4] * ((1+is)*delta[1] - sza) + d[5] * (sza - is*delta[1])) / delta[1];
		//	e[4] = (e[4] * ((1+is)*delta[1] - sza) + e[5] * (sza - is*delta[1])) / delta[1];
		
		SubGetCoeff(iv+1, is, ir+1, &a[6], &b[6], &c[6], &d[6], &e[6]);
		//	SubGetCoeff(iv+1, is+1, ir+1, &a[7], &b[7], &c[7], &d[7], &e[7]);
		//	a[6] = (a[6] * ((1+is)*delta[1] - sza) + a[7] * (sza - is*delta[1])) / delta[1];
		//	b[6] = (b[6] * ((1+is)*delta[1] - sza) + b[7] * (sza - is*delta[1])) / delta[1];
		//	c[6] = (c[6] * ((1+is)*delta[1] - sza) + c[7] * (sza - is*delta[1])) / delta[1];
		//	d[6] = (d[6] * ((1+is)*delta[1] - sza) + d[7] * (sza - is*delta[1])) / delta[1];
		//	e[6] = (e[6] * ((1+is)*delta[1] - sza) + e[7] * (sza - is*delta[1])) / delta[1];
	}
	a[4] = (a[4] * ((1+iv)*delta[0] - vza) + a[6] * (vza - iv*delta[0])) / delta[0];
	b[4] = (b[4] * ((1+iv)*delta[0] - vza) + b[6] * (vza - iv*delta[0])) / delta[0];
	c[4] = (c[4] * ((1+iv)*delta[0] - vza) + c[6] * (vza - iv*delta[0])) / delta[0];
	d[4] = (d[4] * ((1+iv)*delta[0] - vza) + d[6] * (vza - iv*delta[0])) / delta[0];
	e[4] = (e[4] * ((1+iv)*delta[0] - vza) + e[6] * (vza - iv*delta[0])) / delta[0];
	
	
	a[0] = (a[0] * ((1+ir)*delta[2] - raa) + a[4] * (raa - ir*delta[2])) / delta[2];
	b[0] = (b[0] * ((1+ir)*delta[2] - raa) + b[4] * (raa - ir*delta[2])) / delta[2];
	c[0] = (c[0] * ((1+ir)*delta[2] - raa) + c[4] * (raa - ir*delta[2])) / delta[2];
	d[0] = (d[0] * ((1+ir)*delta[2] - raa) + d[4] * (raa - ir*delta[2])) / delta[2];
	e[0] = (e[0] * ((1+ir)*delta[2] - raa) + e[4] * (raa - ir*delta[2])) / delta[2];
	
	xabc[0] = a[0];
	xabc[1] = b[0];
	xabc[2] = c[0];
	xabc[3] = d[0];
	xabc[4] = e[0];
	return true;
}

BOOL SixS_IMG::Init6S(const char *filei, float dsza, float dvza, float draa)
{
	float zero = 0;
	int flag = 1;
	strcpy(filename, filei);
	SetDefualtParameters(filei);
	if(PrepareCoefficients(dsza, dvza, draa))return true;
	else return false;
}

float SixS_IMG::BufferedAtmosphereCorrect(float sza, float saa, float vza, float vaa, float rapp, int datatype)
{
	GetCoefficients(sza, vza, vaa - saa);
	if(datatype == 2) // image is reflectance without sun angle correction
	{
		rapp = rapp/cos(sza*PI/180);
//		rapp = rapp;
		rapp = (rapp - xabc[0])/ xabc[1];
		rapp = rapp / (1 + rapp * xabc[2]);
//		rapp = rapp*100.;
	}
	else if(datatype == 3) // image is measured radiance
	{
		rapp = rapp/xabc[3];
		rapp = (rapp - xabc[0])/ xabc[1];
		rapp = rapp / (1 + rapp * xabc[2]);
//		rapp = rapp*100.;
	}
	else // assume image is reflectance which is sun angle corrected 
	{
		rapp = rapp;
//		rapp = rapp*cos(sza*PI/180);
		rapp = (rapp - xabc[0])/ xabc[1];
		rapp = rapp / (1 + rapp * xabc[2]);
//		rapp = rapp*100.;
	}
	return rapp;
}

float SixS_IMG::DirectAtmosphereCorrect(float sza, float saa, float vza, float vaa, float rapp, int datatype)
{
	GetCorrectionCoefficients(sza, saa, vza, vaa, xabc);

//	GetCoefficients(sza, vza, vaa - saa);
	if(datatype == 2) // image is reflectance without sun angle correction
	{
		rapp = rapp/cos(sza*PI/180);
		rapp = (rapp - xabc[0])/ xabc[1];
		rapp = rapp / (1 + rapp * xabc[2]);
//		rapp = rapp*100.;
	}
	else if(datatype == 3) // image is measured radiance
	{
		rapp = rapp/xabc[3];
		rapp = (rapp - xabc[0])/ xabc[1];
		rapp = rapp / (1 + rapp * xabc[2]);
//		rapp = rapp*100.;
	}
	else // assume image is reflectance which is sun angle corrected 表观反射率：datatype＝1
	{
		rapp = rapp;
		rapp = (rapp - xabc[0])/ xabc[1];
		rapp = rapp / (1 + rapp * xabc[2]);
//		rapp = rapp*100.;
	}
	return rapp;
}
//new function
float SixS_IMG::DoCorrectionWithCoefficients(float coef[5],float rapp, int datatype)
{
	if(datatype == 2) //image is reflectance without sun angle correction
	{
		float sza;
		sza = coef[4];
		rapp = rapp/cos(sza*PI/180);
		rapp = (rapp - coef[0])/ coef[1];
		rapp = rapp / (1 + rapp * coef[2]);
	}
	else if(datatype == 3) // image is measured radiance
	{
		rapp = rapp/coef[3];
		rapp = (rapp - coef[0])/ coef[1];
		rapp = rapp / (1 + rapp * coef[2]);
	}
	else if(datatype==1) //assume image is reflectance which is sun angle corrected 
	{
		rapp = rapp;
		rapp = (rapp - coef[0])/ coef[1];
		rapp = rapp / (1 + rapp * coef[2]);
	}
	else if(datatype==4)//assume image is reflectance of atmospherically corrected.
	{ 
		float ar;
		ar = rapp/(1-rapp*coef[2])*coef[1]+coef[0];
		return ar;
	}
	return rapp;
}

BOOL SixS_IMG::GetCorrectionCoefficients(float& sza, float& saa, float& vza, float& vaa, float coef[5])
{
	coef[3] = -99;
	_GetCorrCoef(&sza,&saa,&vza,&vaa,coef);
//	coef[4] = sza;
	if(coef[3]==-99)
		return false;
	else
		return true;
}

BOOL SixS_IMG::SetDefualtParameters(const char *filei)
//file name of text file of 6S standard input format
{
	int i;
	strcpy(filename,filei);
	for(i = strlen(filei); i < 80; i++)
		filename[i] = ' ';
	try{
		_SetDefualtPara(filename,80);
	}
	catch (.../*CException* e*/)
	{
		return false;
	}
	return true;
}

void SixS_IMG::SetMonthDate(int month, int date)
// month, day in the month
{
	_SetMonthDate(&month,&date);
}
void SixS_IMG::SetWaterVapor(float watervapor)
// h2o  (g/cm2)
{
	_SetWaterVapor(&watervapor);
}
void SixS_IMG::SetHorizontalVisibility(float visibility)
// visibility in km
{
	_SetHorVis(&visibility);
}
void SixS_IMG::SetAerosolConcentration(float aeroso_depth)
// optical thickness at 550nm 
{
	_SetAeroDepth(&aeroso_depth);
}
void SixS_IMG::SetTargetAltitude(float altitude)
// according to 6S, positive value means below see level, so we should enter negative value expressed in km, for example -0.05
{
	_SetTargetAlt(&altitude);
}
void SixS_IMG::SetSpectralBand(int predefined_filter_id)
// the same integers as 6s standard input
{
	_SetSpectralBand(&predefined_filter_id);
}
void SixS_IMG::SetSpectralFilterFun(float wlinf, float wlsup, float *fltervalue)
// min/max boundary of the filter function, and the filter function value a 0.025um interval
{
	_SetSpecFilt(&wlinf, &wlsup, fltervalue);
}

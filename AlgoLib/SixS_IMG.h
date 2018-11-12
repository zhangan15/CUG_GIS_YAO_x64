// SixS.h: interface for the SixS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIXS_H__55206F8D_0BF3_4685_8D6B_2D6312FC4D38__INCLUDED_)
#define AFX_SIXS_H__55206F8D_0BF3_4685_8D6B_2D6312FC4D38__INCLUDED_

#include "Comlib.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ALGOLIB_API SixS_IMG  
{
public:
	SixS_IMG();
	virtual ~SixS_IMG();
	HINSTANCE hDLL;
	float *xa, *xb, *xc, *xd, *xe;	
	float delta[3];
	float xabc[5];
	int sza_i, vza_i, raa_i;
	char filename[80];
	
public:   //interface functions
	BOOL LoadDLL(const char *name);
	BOOL SetDefualtParameters(const char *filei); //file name of text file of 6S standard input format


	//修改几何条件
	void SetMonthDate(int month, int date);   // month, day in the month
   //修改大气条件
	void SetWaterVapor(float watervapor);     // h2o  (g/cm2)
	//修改气溶胶条件
	void SetHorizontalVisibility(float visibility);  // visibility in km
	void SetAerosolConcentration(float aeroso_depth);  // optical thickness at 550nm 
	//修改目标高度 正代表海面下，负代表海面上，例如 -0.05 代表 海面上0.05公里高度
	void SetTargetAltitude(float altitude);  // according to 6S, positive value means below see level, so we should enter negative value expressed in km, for example -0.05

	//修改波段值： 只能按照6s波段设置修改
	void SetSpectralBand(int predefined_filter_id); // the same integer as 6s standard input
	//修改滤波函数
	void SetSpectralFilterFun(float wlinf, float wlsup, float *fltervalue);  // min/max boundary of the filter function, and the filter function value a 0.025um interval


	//得到每一个 点的 大气校正系数
	BOOL GetCorrectionCoefficients(float& sza, float& saa, float& vza, float& vaa, float coef[5]);//get xa,xb,xc
	                           //sun angle,             //view angl
	//对每一个点大气校正，
	float DoCorrectionWithCoefficients(float coef[5],  float rapp,      int datatype); 
    //coef大气校正系数
    // rapp:影像值value      
	// datatype: 校正类型 meaning： 1　输入的是大气顶反射率，经过太阳角度修正
	// datatype:                    2  输入的是大气顶 辐射值
	// datatype:                    3  输入的是大气顶反射率 没有经过太阳角度修正
	// datatype:                    4  输入的是地面反射率值，输出为大气顶反射率，经过太阳角度修正

	                                               
    //DoCorrectionWithCoefficients(buffer[i/100][j/100],rapp,1);// 1:toa reflectence 2:toa radience 3 :  no angle correct ref
	//old function

	BOOL Init6S(const char *filei, float dsza, float dvza, float draa); // old version function
	float DirectAtmosphereCorrect(float sza, float saa, float vza, float vaa, float rapp, int datatype); // old version function
	float BufferedAtmosphereCorrect(float sza, float saa, float vza, float vaa, float rapp, int datatype); // old version function

private:
	BOOL PrepareCoefficients(float dsza, float dvza, float draa); // old version function
	void SubGetCoeff(int iv, int is, int ir, float *a, float *b, float *c, float *d, float *e); // old version function
	BOOL GetCoefficients(float sza, float vza, float raa); // old version function
//	void  (_stdcall *SSSSSS)(int *flag, float *asol,float *phi0,float *avis,float *phiv,
//		   float xabc[5], const char *FILEI, unsigned int len);
	void  (_stdcall *_GetCorrCoef)(float *asol,float *phi0,float *avis,float *phiv,float *xabc);
	void  (_stdcall *_SetDefualtPara)(const char *FILEI,unsigned int len);
	void  (_stdcall *_SetMonthDate)(int *mon, int *day);
	void  (_stdcall *_SetWaterVapor)(float *watervapor);
	void  (_stdcall *_SetHorVis)(float *visibility);
	void  (_stdcall *_SetAeroDepth)(float *aeroso_depth);
	void  (_stdcall *_SetTargetAlt)(float *altitude);
	void  (_stdcall *_SetSpectralBand)(int *band_id);
	void  (_stdcall *_SetSpecFilt)(float *wl_inf, float *wl_sup, float *fltervalue);
	void  (_stdcall *_SetATMOSCOR)(float* a, float* b, int* c, float* d, float* e, float* f, char* g, unsigned int h, int* i);

};
#endif // !defined(AFX_SIXS_H__55206F8D_0BF3_4685_8D6B_2D6312FC4D38__INCLUDED_)

/*  This is just a example
void test(void)
{
	int i, j;
	double 	sza,saa,vza,vaa,rapp,reflectance;
	float buffer[10][10][5];  
	SixS_IMG ss;
	ss.LoadDLL("6S_DLL.dll");
	ss.SetDefualtParameters("6sDefaultParam.txt");
	ss.SetMonthDate(6,1);
	ss.SetSpectralBand(42);

	for(i=0;i<10;i++){//i,j 把图像分成100*100
		for(j=0;j<10;j++){
			sza = 20;
			saa = 0;
			vza = 20;
			vaa = 180;
			ss.SetWaterVapor(2.862); 
			ss.SetHorizontalVisibility(23);
//			ss.SetAerosolConcentration(0.2);
			ss.SetTargetAltitude(-0.05);
			ss.GetCorrectionCoefficients(sza,saa,vza,vaa,buffer[i][j]);
		}

	for(i=0;i<1000;i++){//i,j是图像行列

		for(j=0;j<1000;j++){
			rapp = 0.2; // read from image
			reflectance = ss.DoCorrectionWithCoefficients(buffer[i/100][j/100],rapp,1);
		}
	}
}
*/
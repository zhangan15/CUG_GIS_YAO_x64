#ifndef DLPS_H
#define DLPS_H

#include <iostream>
using namespace std;
#include <QtCore/QtCore>
#include <vector>
#include <string>
#include "gdal_priv.h"
#include "ogrsf_frmts.h"

struct DLPSParams{
	//确定一个地块是否分裂的规则：属性表中的split_flag字段为1且面积大于dMeanArea+3*dStd;
	double dMaxSize;
	double dMeanArea;
	double dStd;
	double dNTimes;
	//迭代次数,每次迭代都对每个地块判断和分裂;
	double nMaxIters;
	QString sInputFn;
	QString sOutputFn;
	QString sSplitField;
	bool bShowRunningInfo;
	bool bFeatureLimit;
};

struct DLPSPoint
{
	double x;
	double y;
};

struct Parcel{
	OGRGeometry* poGeometry;
	int nSplitFlag;
	double dArea;
	std::vector<std::string> vAttrs;
};

struct Extent{
	double dMinX;
	double dMaxX;
	double dMinY;
	double dMaxY;
};

struct AttributeInfo{
	std::string sName;
	OGRFieldType oType;
	int nPrecision;
	int nWidth;
};

class DLPS
{
public:
	DLPS();
	~DLPS();
	bool loadData(const char* fn);
	void setParams(QString fn1, QString fn2, QString field, double _maxSize, double dNTimes = 3, int _nIter = 5, bool showInfo = false, bool featureLimit = true);
	bool run();
	void printRight();
protected:
	/*  Summary: get area of a geometry, for OGRGeometry.getArea() is useless
 	Parameters:
      geom :OGRGeometry object
    Return : area of the geometry.  */
	double getAreaOfGeometry(OGRGeometry *geom);
	bool calAreaMeanStd();
protected:
/*  Summary: Rotate a point by an certain angle (clockwise rotation)
 	Parameters:
      x, y  :  initial coordinate of point
 	  x0,y0 :  coordinate of center point of rotation
 	  alpha :  rotate angle
 	  xo,yo :  coordinate of rotated point
    Return : true if rotation success.  */
	bool rotateAlphaAngle( double x, double y, double x0, double y0, double alpha, double &xo, double &yo );

	/*  Summary: parse wkt string and collect points from OGRGeometry
 	Parameters:
      wkt       :  wkt string exported from OGRGeometry
 	  vPoints   :  list of points
    Return : true if parse successfully.  */
	bool getPointsFromWKT(QString wkt, QList<DLPSPoint> &vPoints);
	
	/*  Summary: find the best rotation that get the minimum Boundary Rectangle
 	Parameters:
      vPoints       :  points
 	  nCenterNode   :  index of node that start as horizontal line
 	  _angle        :  rotating angle
	  XYExtent      :  min and max of X,Y
	  bOnX          :  if split on X-axis
    Return : true if success.  */
	bool findBestRotation(QList<DLPSPoint> vPoints, int &nCenterNode, double &_angle, Extent &XYExtent, bool &bOnX);
	
	/*  Summary: split the MABR into 2 smaller rectangles by drawing a vertical bisector of a certain edge
 	Parameters:
	  bOnX          :  if split on X-axis
	  _centerX      :  X of center point
	  _centerY      :  Y of center point
 	  _angle        :  rotating angle
	  XYExtent      :  extent of MABR
	  str1          :  wkt string of one of the smaller rectangle
	  str2          :  wkt string of another smaller rectangle
    Return : true if success.  */
	bool getHalfRectangle(bool bOnX, double _centerX, double _centerY, double _angle, Extent XYExtent, QString &str1, QString &str2);

	// check and transform multi-polygon
	bool checkMultiPolygons();

	/*  Summary: write shape-file
 	Parameters:
	  fn : output file name
    Return : true if success.  */
	bool writeResults(const char* fn);

	bool splitOnce();

	bool getSubPolygonStrs( OGRGeometry* geom, QString &str1, QString &str2);

private:
	//parameters input
	DLPSParams mParams;
	//spatial reference, useful in area calculating
	OGRSpatialReference* mSpatialRef;
	//parcels
	QList<Parcel> mvParcels;
	QList<AttributeInfo> mvAttributesInfo;
	QDateTime mt1, mt2;
};

#endif
#include "DLPS.h"
#include <cmath>
#include <ctime>
#include <cstdlib>

#define PI 3.141592653589793
#define MAXFEATURECOUNT 1000

DLPS::DLPS()
{
	mParams.dMeanArea = 0;
	mParams.dStd = 0;
}

DLPS::~DLPS()
{
	//delete 
	for (int i = 0; i < mvParcels.size(); i ++)
		OGRGeometryFactory::destroyGeometry(mvParcels[i].poGeometry);
	mvParcels.clear();
	mvAttributesInfo.clear();
}


bool DLPS::loadData( const char* fn )
{
	cout <<"\n================================================\n";
	//load and check GDAL
	if(GDALGetDriverCount() == 0)
	{
		GDALAllRegister();
		OGRRegisterAll();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//支持中文路径;
		CPLSetConfigOption("SHAPE_ENCODING", "");			//支持属性表中的中文字符;
		cout << "[Init]  environment initialization success!\n";
	}
	//open data source
	GDALDataset *poDS = (GDALDataset*)GDALOpenEx(fn, GDAL_OF_VECTOR, NULL, NULL, NULL);
	if (poDS == NULL)
	{
		cout << fn<<" open error!\n";
		return false;
	}

	//get layer
	OGRLayer* poLayer = poDS->GetLayer(0);
	int nParcelNum = poLayer->GetFeatureCount();

	//get spatial ref, used for output-file
	mSpatialRef = new OGRSpatialReference;
	mSpatialRef = poLayer->GetSpatialRef();

	QDateTime curTime = QDateTime::currentDateTime();
	cout << ""<<qPrintable(curTime.toString("yyyy-MM-dd hh:mm:ss"));
	cout <<" >>  loading data from "<<fn<<"..."<<endl;
	cout << "                [info]  feature count: "<<nParcelNum<<endl;
	if(nParcelNum == 0)
	{
		cerr << "[err]   "<< "feature size = 0!\n";
		exit(0);
	}

	//feature count limit
	if(mParams.bFeatureLimit)
	{
		if(nParcelNum >= MAXFEATURECOUNT)
		{
			cout << "\n[err]   "<< "feature count is limited (should be less than "<<MAXFEATURECOUNT<<")\n";
			exit(0);
		}
	}
	

	OGRFeatureDefn *poLayDefn = poLayer->GetLayerDefn();
	int nFieldCount = poLayDefn->GetFieldCount();
	cout << "                        field   count: "<<nFieldCount<<endl;

	int ind1 = poLayDefn->GetFieldIndex(mParams.sSplitField.toStdString().c_str());
	int ind2 = poLayDefn->GetFieldIndex("area");

	//get attributes name except split_flag and "area"
	for (int i = 0; i < nFieldCount; i ++)
	{
		if (i == ind1 || i == ind2)
			continue;
		AttributeInfo attrInfo;
		attrInfo.oType = poLayDefn->GetFieldDefn(i)->GetType();
		attrInfo.sName = poLayDefn->GetFieldDefn(i)->GetNameRef();
		attrInfo.nPrecision = poLayDefn->GetFieldDefn(i)->GetPrecision();
		attrInfo.nWidth = poLayDefn->GetFieldDefn(i)->GetWidth();
		mvAttributesInfo.append(attrInfo);
	}

	curTime = QDateTime::currentDateTime();
	cout << ""<<qPrintable(curTime.toString("yyyy-MM-dd hh:mm:ss"));
	cout <<" >>  loading features...";

	mvParcels.clear();
	OGRFeature *poFeature;
	std::string fieldVal;
	for (int i = 0; i < nParcelNum; i ++)
	{
		poFeature = poLayer->GetFeature(i);
		int split_flag = poFeature->GetFieldAsInteger(mParams.sSplitField.toStdString().c_str());
		std::vector<std::string> slist;
		//read a line as a list
		for (int j = 0; j < nFieldCount; j ++)
			if(j != ind1 && j != ind2)
			{
				fieldVal = poFeature->GetFieldAsString(j);
				slist.push_back(fieldVal);
			}

		Parcel myParcel;
		myParcel.poGeometry = poFeature->GetGeometryRef()->clone();
		myParcel.nSplitFlag = split_flag;
		myParcel.dArea = getAreaOfGeometry(myParcel.poGeometry->clone());
		myParcel.vAttrs = slist;
		mvParcels.append(myParcel);
	}
	cout << "Done!\n";
	cout <<"================================================\n\n";
	//destroy data source
	OGRFeature::DestroyFeature(poFeature);
	delete poLayDefn, poLayer;
	poLayDefn = NULL, poLayer = NULL;
	//OGRDataSource::DestroyDataSource( poDS );
	GDALClose(poDS);
	return true;
}

void DLPS::setParams(QString fn1, QString fn2, QString field, double _maxSize, double dNTimes/* = 3*/, int _nIter /*= 5*/, bool showInfo /*= false*/, bool featureLimit /*= true*/)
{
	mParams.sInputFn = fn1;
	mParams.sOutputFn = fn2;
	mParams.sSplitField =field;

	mParams.dMaxSize = _maxSize;
	mParams.dNTimes = dNTimes;
	mParams.nMaxIters = _nIter;

	mParams.bShowRunningInfo = showInfo;
	mParams.bFeatureLimit = featureLimit;
}

bool DLPS::run()
{
	srand(unsigned(time(0)));
 	mt1 = QDateTime::currentDateTime();
	loadData(mParams.sInputFn.toStdString().c_str());
	
	//loop
	for (int i = 0; i < mParams.nMaxIters; i ++)
	{
		cout << "========== iteration "<< i+1<<" ==========\n";
		calAreaMeanStd();
		splitOnce();
		cout << "[info]  feature size after split: "<< mvParcels.size()<<endl<<endl;
	}
	cout << "=================================================\n\n";
	if(!checkMultiPolygons())
	{
		cerr << "[err]   "<< "program abort:code -1\n";
		exit(1);
	}
	writeResults(mParams.sOutputFn.toStdString().c_str());
	mt2 = QDateTime::currentDateTime();
	cout << "\n[info]  run time: "<< qPrintable(QString::number(mt1.msecsTo(mt2)/1000.0)) << " seconds\n";
	return true;
}

bool DLPS::rotateAlphaAngle( double x, double y, double x0, double y0, double alpha, double &xo, double &yo )
{
	alpha = -1.0*alpha;
	xo = (x-x0)*cos(alpha) - (y-y0)*sin(alpha) + x0;
	yo = (y-y0)*cos(alpha) + (x-x0)*sin(alpha) + y0;
	return true;
}

bool DLPS::getPointsFromWKT( QString wkt, QList<DLPSPoint> &vPoints)
{
	//wkt string is just like "POLYGON((3 6,7 3,5 2),(6 3))"
	wkt = wkt.remove("POLYGON ((");
	int nend = wkt.indexOf("),");
	// '),' in wkt, which means polygon has at least 2 rings
	// otherwise, polygon has only 1 ring and has no inner rings
	if (nend < 0)
		// if so, remove the '))' flag and wkt string remains the coordinate of points
		wkt = wkt.remove("))");
	else
		//if so, the first ring is the outer ring
		wkt = wkt.mid(0, nend);

	//split coordinates string by ','
	QStringList slist = wkt.split(",");
	//for the last point is the first point, number of points is N-1
	int nPoint = slist.size()-1;

	QStringList sxy;
	vPoints.clear();

	for (int i = 0; i < nPoint; i ++)
	{
		sxy.clear();
		//x,y coordinates group is split by ' '
		sxy = slist[i].split(" ");

		//get x,y and add point to list
		DLPSPoint mypoint;
		//from string to float(only 14 decimals is accurate, maybe enough)
		mypoint.x = atof(sxy[0].toStdString().c_str());
		mypoint.y = atof(sxy[1].toStdString().c_str());
		vPoints.append(mypoint);
	}
	sxy.clear();
	slist.clear();
	//test_print 
// 	cout << "\nnumber of points: "<<nPoint<<endl;
// 	for (int i = 0; i < (nPoint>5?5:nPoint); i ++)
// 		cout << qPrintable(QString("%1").arg(vPoints[i].x, 0, 'f', 15))<<" "<<qPrintable(QString("%1").arg(vPoints[i].y, 0, 'f', 15))<<endl;

	return true;
}

bool DLPS::findBestRotation( QList<DLPSPoint> vPoints, int &nCenterNode, double &_angle, Extent &XYExtent, bool &bOnX )
{
	nCenterNode = 0;
	double _dmin_area = 10e10, _dtmp_area;
	double _dx, _dy, _dtheta, _minx, _miny, _maxx, _maxy;
	int j = 0;
	QList<double> vx, vy;

	for (int i = 0; i < vPoints.size(); i++)
	{
		// i is the start node, j is the end node
		if ((i+1) == vPoints.size())
			j = 0;
		else
			j = i+1;

		// calculate the angle that line from start node to end node should rotate to horizontal
		_dx = vPoints[j].x - vPoints[i].x;
		_dy = vPoints[j].y - vPoints[i].y;
		// if vertical, set _dx a little number so that atan() function will work normally
		if(abs(_dx) <= 0)
			_dx = 1e-6;
		_dtheta = atan(_dy/_dx);
 		if(_dx < 0)
 			_dtheta += PI;

		vx.clear();
		vy.clear();
		//after calculate the angle, try rotate 
		for (int k = 0; k < vPoints.size(); k ++)
		{
			double _xout, _yout;
			//rotate point-k around point-i by angle(_dtheta)
			rotateAlphaAngle(vPoints[k].x, vPoints[k].y, vPoints[i].x, vPoints[i].y, _dtheta, _xout, _yout);
			vx.append(_xout), vy.append(_yout);
		}
		//sort the xs and ys to calculate the min/max x/y
		qSort(vx.begin(), vx.end());
		qSort(vy.begin(), vy.end());
		_minx = vx.first(), _maxx = vx.last();
		_miny = vy.first(), _maxy = vy.last();
		_dtmp_area = (_maxx-_minx)*(_maxy-_miny);

		//if area is smaller, update the minimum area/center_node/angle/new_points
		if (_dtmp_area < _dmin_area)
		{
			_dmin_area = _dtmp_area;
			_angle = _dtheta;
			nCenterNode = i;
		}
	}

	//apply the best rotation
	vx.clear();
	vy.clear();
	for (int k = 0; k < vPoints.size(); k ++)
	{
		double _xout, _yout;
		//rotate point-k around point-i by angle(_dtheta)
		rotateAlphaAngle(vPoints[k].x, vPoints[k].y, vPoints[nCenterNode].x, vPoints[nCenterNode].y, _angle, _xout, _yout);
// 		if(k==0)
// 			cout << vPoints[k].x<<" "<<vPoints[k].y << " "<< vPoints[nCenterNode].x << " "<< vPoints[nCenterNode].y<<" "<< _dtheta <<" "<< _xout << " "<<_yout<<endl;
		vx.append(_xout), vy.append(_yout);
	}

	qSort(vx.begin(), vx.end());
	qSort(vy.begin(), vy.end());
	_minx = vx.first(), _maxx = vx.last();
	_miny = vy.first(), _maxy = vy.last();

	XYExtent.dMinX = _minx;
	XYExtent.dMaxX = _maxx;
	XYExtent.dMinY = _miny;
	XYExtent.dMaxY = _maxy;

	//compare X extent and Y extent to determine whether split on X-axis
	if((_maxx - _minx) > (_maxy - _miny))
		bOnX = true;
	else
		bOnX = false;

	//test print
	if(mParams.bShowRunningInfo)
	{
		QDateTime curTime = QDateTime::currentDateTime();
		cout << "\n"<<qPrintable(curTime.toString("yyyy-MM-dd hh:mm:ss"))<< " >>  center: P"<< nCenterNode<<"["<< qPrintable(QString("%1").arg(vPoints[nCenterNode].x, 0, 'f', 6))<<" "<<qPrintable(QString("%1").arg(vPoints[nCenterNode].y, 0, 'f', 6))<<"]"<<endl;
		cout << "                        rotate angle    : "<< qPrintable(QString("%1").arg(_angle*180/PI, 0, 'f', 3))<<"°"<<endl;
		if(bOnX)
			cout << "                        split on X-axis : TRUE"<<endl;
		else
			cout << "                        split on X-axis : FALSE"<<endl;
		//cout << "        new extent: X-["<<XYExtent.dMinX<<", "<<XYExtent.dMaxX<<"] Y-["<<XYExtent.dMinY<<", "<<XYExtent.dMaxY<<"]"<<endl;
		cout << "                        area of MABR    : "<< qPrintable(QString("%1").arg(_dmin_area, 0, 'f', 6))<<endl;
	}
 	

	return true;
}

bool DLPS::getHalfRectangle( bool bOnX, double _centerX, double _centerY, double _angle, Extent XYExtent, QString &str1, QString &str2 )
{
	double *x_ = new double[6];
	double *y_ = new double[6];
	_angle = -1.0*_angle;
	rotateAlphaAngle(XYExtent.dMinX, XYExtent.dMinY, _centerX, _centerY, _angle, x_[0], y_[0]);
	rotateAlphaAngle(XYExtent.dMaxX, XYExtent.dMinY, _centerX, _centerY, _angle, x_[1], y_[1]);
	rotateAlphaAngle(XYExtent.dMaxX, XYExtent.dMaxY, _centerX, _centerY, _angle, x_[2], y_[2]);
	rotateAlphaAngle(XYExtent.dMinX, XYExtent.dMaxY, _centerX, _centerY, _angle, x_[3], y_[3]);


	if (bOnX)
	{
/*		maxy 3 - - - 5 - - - 2
		     |       |       |
		     |       |       |
		     |       |       |
		     |       |       |
		miny 0 - - - 4 - - - 1
           minx            maxx
*/
		rotateAlphaAngle((XYExtent.dMinX + XYExtent.dMaxX)/2.0, XYExtent.dMinY, _centerX, _centerY, _angle, x_[4], y_[4]);
		rotateAlphaAngle((XYExtent.dMinX + XYExtent.dMaxX)/2.0, XYExtent.dMaxY, _centerX, _centerY, _angle, x_[5], y_[5]);

		// 0-4-5-3-0
		str1 = QString("POLYGON ((%1 %2,%3 %4,%5 %6,%7 %8,%1 %2))").
			arg(x_[0], 0, 'f', 14).arg(y_[0], 0, 'f', 14).
			arg(x_[4], 0, 'f', 14).arg(y_[4], 0, 'f', 14).
			arg(x_[5], 0, 'f', 14).arg(y_[5], 0, 'f', 14).
			arg(x_[3], 0, 'f', 14).arg(y_[3], 0, 'f', 14);
		// 4-1-2-5-4
		str2 = QString("POLYGON ((%1 %2,%3 %4,%5 %6,%7 %8,%1 %2))").
			arg(x_[4], 0, 'f', 14).arg(y_[4], 0, 'f', 14).
			arg(x_[1], 0, 'f', 14).arg(y_[1], 0, 'f', 14).
			arg(x_[2], 0, 'f', 14).arg(y_[2], 0, 'f', 14).
			arg(x_[5], 0, 'f', 14).arg(y_[5], 0, 'f', 14);
	}
	else
	{
/*		maxy 3 - - - - - 2
		     |           |
		     |           |
		     4 - - - - - 5
		     |           |
		     |           |
		miny 0 - - - - - 1
           minx         maxx
*/
		rotateAlphaAngle(XYExtent.dMinX, (XYExtent.dMinY + XYExtent.dMaxY)/2.0, _centerX, _centerY, _angle, x_[4], y_[4]);
		rotateAlphaAngle(XYExtent.dMaxX, (XYExtent.dMinY + XYExtent.dMaxY)/2.0, _centerX, _centerY, _angle, x_[5], y_[5]);

		// 0 - 1 - 5 - 4 - 0
		str1 = QString("POLYGON ((%1 %2,%3 %4,%5 %6,%7 %8,%1 %2))").
			arg(x_[0], 0, 'f', 14).arg(y_[0], 0, 'f', 14).
			arg(x_[1], 0, 'f', 14).arg(y_[1], 0, 'f', 14).
			arg(x_[5], 0, 'f', 14).arg(y_[5], 0, 'f', 14).
			arg(x_[4], 0, 'f', 14).arg(y_[4], 0, 'f', 14);
		// 4 - 5 - 2 - 3 - 4
		str2 = QString("POLYGON ((%1 %2,%3 %4,%5 %6,%7 %8,%1 %2))").
			arg(x_[4], 0, 'f', 14).arg(y_[4], 0, 'f', 14).               
			arg(x_[5], 0, 'f', 14).arg(y_[5], 0, 'f', 14).
			arg(x_[2], 0, 'f', 14).arg(y_[2], 0, 'f', 14).
			arg(x_[3], 0, 'f', 14).arg(y_[3], 0, 'f', 14);
	}

	delete x_;
	delete y_;

	return true;
}

bool DLPS::getSubPolygonStrs( OGRGeometry* geom, QString &str1, QString &str2 )
{
	//get points from wkt
	QList<DLPSPoint> vpoints;
	OGRPoint *oPoint = new OGRPoint();
	OGRLinearRing *oLineString = ((OGRPolygon *)geom)->getExteriorRing();
	for (int j = 0; j < oLineString->getNumPoints(); j ++)
	{
		DLPSPoint mypoint;
		oLineString->getPoint(j, oPoint);
		mypoint.x = oPoint->getX();
		mypoint.y = oPoint->getY();
		vpoints.append(mypoint);
	}
	delete oPoint, oLineString;
	oPoint = NULL, oLineString = NULL;

	//find best rotation params from points
	int nInd;
	double dAngle;
	bool onX;
	Extent xyextent;
	if(!findBestRotation(vpoints, nInd, dAngle, xyextent, onX))
	{
		cout << "[err]   " << "program terminated while in process:code 1";
		exit(1);
	}

	//choose the best rotation params and get wkt of  2 smaller rectangle
	if(!getHalfRectangle(onX, vpoints[nInd].x, vpoints[nInd].y, dAngle, xyextent, str1, str2))
	{
		cout << "[err]   " << "program terminated while in process:code 2";
		   exit(1);
	}
	vpoints.clear();
	return true;
}

bool DLPS::writeResults( const char* fn )
{
	//加载GDAL驱动;
	if(GDALGetDriverCount() == 0)
	{
		GDALAllRegister();
		OGRRegisterAll();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//支持中文路径;
		CPLSetConfigOption("SHAPE_ENCODING", "");			//支持属性表中的中文字符;
	}
	QDateTime t1 = QDateTime::currentDateTime();
	cout << qPrintable(t1.toString("yyyy-MM-dd hh:mm:ss")) << " >>  saving "<<fn<<"...";
	if(QFile::exists(fn))
	{
		cout << "\n[err]   output file name exist. please delete it first!\n";
		exit(0);
	}

	const char *pszDriverName = "ESRI Shapefile";
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	if (poDriver == NULL)
	{
		printf("%s driver not available.\n", pszDriverName);
		return false;
	}

	GDALDataset *poDS = poDriver->Create( fn, 0, 0, 0, GDT_Unknown, NULL);
	OGRLayer* poLayer= poDS->CreateLayer( "layer", mSpatialRef, wkbPolygon, NULL );

	//create fields
	for (int i = 0; i < mvAttributesInfo.size(); i ++)
	{
		OGRFieldDefn oField(mvAttributesInfo[i].sName.c_str(), mvAttributesInfo[i].oType);
		oField.SetPrecision(mvAttributesInfo[i].nPrecision);
		oField.SetWidth(mvAttributesInfo[i].nWidth);
		if( poLayer->CreateField( &oField ) != OGRERR_NONE )
		{
			cout << "[err]   creating field("<< mvAttributesInfo[i].sName << ") failed.\n" ;
			return false;
		}
	}

	OGRFieldDefn oField1(mParams.sSplitField.toStdString().c_str(), OFTInteger);
	if( poLayer->CreateField( &oField1 ) != OGRERR_NONE )
	{
		cout << "[err]   creating split_flag field failed.\n" ;
		return false;
	}
	OGRFieldDefn oField2("area", OFTReal);
	oField2.SetPrecision(6);
	if( poLayer->CreateField( &oField2 ) != OGRERR_NONE )
	{
		cout << "[err]   creating area field failed.\n" ;
		return false;
	}

	OGRFeature *poFeature;
	for (int i = 0; i < mvParcels.size(); i ++)
	{
		poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
		poFeature->SetField(mParams.sSplitField.toStdString().c_str(), (int)(mvParcels[i].nSplitFlag));
		poFeature->SetField("area", mvParcels[i].dArea);
		for (int j = 0; j < mvAttributesInfo.size(); j ++)
		{
			if (mvAttributesInfo[j].oType == OFTInteger)
				poFeature->SetField(mvAttributesInfo[j].sName.c_str(), atoi(mvParcels[i].vAttrs[j].c_str()));
			else if(mvAttributesInfo[j].oType == OFTReal)
				poFeature->SetField(mvAttributesInfo[j].sName.c_str(), atof(mvParcels[i].vAttrs[j].c_str()));
			else if(mvAttributesInfo[j].oType == OFTString)
				poFeature->SetField(mvAttributesInfo[j].sName.c_str(), mvParcels[i].vAttrs[j].c_str());
		}
		
		if(poFeature->SetGeometry(mvParcels[i].poGeometry)!=OGRERR_NONE)
		{
			cout<<"[err]   failed to create feature in shape file. error FID: "<< i<<endl;
			//return false;
			continue;//2018.06.08
		}

		if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
		{
			cout<<"[err]   failed to create feature in shape file. error FID: "<< i<<endl;
			//return false;
			continue;//2018.06.08
		}
	}
	cout <<"...Done!\n";

	OGRFeature::DestroyFeature(poFeature);
	GDALClose(poDS);
/*	delete  poLayer, poDriver;*/
	return true;
}

double DLPS::getAreaOfGeometry( OGRGeometry *geom )
{
	double area;
	switch (geom->getGeometryType()) {
	case wkbPolygon:
		area = ((OGRPolygon *)geom)->get_Area();
		break;

	case wkbMultiPolygon:
		area = ((OGRMultiPolygon *)geom)->get_Area();
		break;

	case wkbLinearRing:
		area = ((OGRLinearRing *)geom)->get_Area();
		break;

	case wkbGeometryCollection:
		area = ((OGRGeometryCollection *)geom)->get_Area();
		break;

	default:
		cerr << "[err]   unknown geometry type!\n";
		exit(1);
	}
	return area;
}

bool DLPS::checkMultiPolygons()
{
	int n = mvParcels.size();
	for (int i = 0; i < n; i ++)
	{
		if(mvParcels[i].nSplitFlag == 0)
			continue;
		//transform multi-polygon to polygons
		if(mvParcels[i].poGeometry->getGeometryType() == wkbMultiPolygon)
		{
			OGRMultiPolygon* poMultiPolygon = ((OGRMultiPolygon *)mvParcels[i].poGeometry->clone() );
			int num = poMultiPolygon->getNumGeometries();
			for (int j = 0; j < num; j ++)
			{
				Parcel parcel;
				parcel.poGeometry = poMultiPolygon->getGeometryRef(j);
				parcel.nSplitFlag = mvParcels[i].nSplitFlag;
				parcel.dArea = getAreaOfGeometry(parcel.poGeometry);
				parcel.vAttrs = mvParcels[i].vAttrs;
				mvParcels.append(parcel);
			}
			//delete poMultiPolygon;
			//poMultiPolygon = NULL;
			mvParcels.removeAt(i);
			i--;
			n--;
		}
		
	}
	cout << "[info]  feature size after check: " << mvParcels.size()<<endl;
	return true;
}

bool DLPS::calAreaMeanStd()
{
	// set 0
	mParams.dMeanArea = 0;
	mParams.dStd = 0;
	//calculate mean
	for (int i = 0; i < mvParcels.size(); i ++)
		mParams.dMeanArea += mvParcels[i].dArea;
	mParams.dMeanArea /= (double)mvParcels.size();

	//calculate std dev
	for (int i = 0; i < mvParcels.size(); i ++)
		mParams.dStd += pow(mvParcels[i].dArea - mParams.dMeanArea , 2);
	mParams.dStd = sqrt(mParams.dStd / (double)mvParcels.size());
	cout << "[info]  area_mean = "<< qPrintable(QString("%1").arg(mParams.dMeanArea, 0, 'f', 6)) << "  area_std = "<< qPrintable(QString("%1").arg(mParams.dStd, 0, 'f', 6))<<endl;
	return true;
}

bool DLPS::splitOnce()
{
	if(!checkMultiPolygons())
	{
		cerr << "[err]   "<< "program abort:code -1\n";
		exit(1);
	}
	
	char **wkt1 = new char*[1], **wkt2 = new char*[1];
	OGRGeometry *subPolygon1, *subPolygon2;
	OGRGeometry* bufferGeom;

	int nSize = mvParcels.size();
	for (int i = 0; i < nSize; i ++)
	{
		// judge if will split according to the split_flag and area
		if (mvParcels[i].nSplitFlag == 0)
		{
			cout << "\r[progress]  " << i+1 << "/" << nSize;
			continue;
		}
		else if (mParams.dMaxSize <= 0)
		{
			if (mvParcels[i].dArea < (((double)rand())/RAND_MAX * mParams.dNTimes * mParams.dStd ) + mParams.dMeanArea)
			{
				cout << "\r[progress]  " << i+1 << "/" << nSize;
				continue;
			}
		}
		else if(mParams.dMaxSize > 0)
		{
			if (mvParcels[i].dArea < mParams.dMaxSize)
			{
				cout << "\r[progress]  " << i+1 << "/" << nSize;
				continue;
			}
		}

		//split
		QString str1, str2;
		if(!getSubPolygonStrs(mvParcels[i].poGeometry, str1, str2))
		{
			cerr << "[err]   " << "program abort:code 3\n";
			exit(1);
		}


		//transform wkt to geometry
		wkt1[0] = new char[str1.size()+1];
		wkt2[0] = new char[str2.size()+1];
		strcpy(wkt1[0], str1.toStdString().c_str());
		strcpy(wkt2[0], str2.toStdString().c_str());
		str1.clear(), str2.clear();
		//create from wkt and intersection ,here subPolygon is the half MABR(rotated back)
		if(OGRGeometryFactory::createFromWkt(wkt1, mSpatialRef, &subPolygon1) != OGRERR_NONE)
		{
			cerr << "[err]   " << "program abort:code 4\n";
			exit(1);
		}

		if(OGRGeometryFactory::createFromWkt(wkt2, mSpatialRef, &subPolygon2) != OGRERR_NONE)
		{
			cerr << "[err]   " << "program abort:code 4\n"; 
			exit(1);
		}

		//half MABR intersects with the original geometry
		//buffer(0) can handle self intersection error
		bufferGeom = mvParcels[i].poGeometry->clone()->Buffer(0);
		subPolygon1 = (subPolygon1->Intersection(bufferGeom));
		subPolygon2 = (subPolygon2->Intersection(bufferGeom));

		//calculate area, for getArea() is not supporsed for OGRGeometry
		double area1 = getAreaOfGeometry(subPolygon1), area2 = getAreaOfGeometry(subPolygon2);

		//add data to structure list
		Parcel parcel2;
		mvParcels[i].poGeometry = subPolygon1->clone(); 
		parcel2.poGeometry = subPolygon2->clone();

		parcel2.nSplitFlag = 1;

		mvParcels[i].dArea = area1;
		parcel2.dArea = area2;

		parcel2.vAttrs = mvParcels[i].vAttrs;

		if(parcel2.dArea>0)
			mvParcels.append(parcel2);

		cout << "\r[progress]  " << i+1 << "/" << nSize;
	}
	cout <<endl;


	//delete pointer
	OGRGeometryFactory::destroyGeometry(subPolygon1);
	OGRGeometryFactory::destroyGeometry(subPolygon2);
	OGRGeometryFactory::destroyGeometry(bufferGeom);
	//delete []wkt[0], []wkt1[0], []wkt2[0];
	delete []wkt1;
	delete []wkt2;
	wkt1 = NULL, wkt2 = NULL;
	return true;
}

void DLPS::printRight()
{
	cout <<"\n*****************************************************************\n";
	cout <<"* Application for Realistic Land Parcel Subdivision (version 2.0)\n";
	cout <<"* All Right Reserved by Penghua Liu and Yao Yao @ SYSU\n";
	cout <<"* Any Problem PLZ contact WeChat public account: SYSU_BIGDATA\n";
	cout << "*****************************************************************\n\n";
}

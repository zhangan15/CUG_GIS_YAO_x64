#include "PublicFunctions.h"
#include <QtCore>

// obtain image block from HSR RS image
bool ObtainRectImageFromHsrByLatLon(CGDALRead* pInImg, double dLon, double dLat, int nWidth, int nHeight, matrix<rgb_pixel>& input_image)
{
	if (pInImg == NULL || pInImg->datatype() != INPUT_DATATYPE || pInImg->bandnum() < 3)
	{
		cout << "Input image format error." << endl;
		return false;
	}

	double curCol, curRow;
	pInImg->world2Pixel(dLat, dLon, &curCol, &curRow);

	//读入数据
	int minCol, minRow, maxCol, maxRow;
	minCol = int(curCol - nHeight / 2 + 0.5);
	maxCol = int(curCol + nHeight / 2 + 0.5);
	minRow = int(curRow - nWidth / 2 + 0.5);
	maxRow = int(curRow + nWidth / 2 + 0.5);

// 	if (minCol < 0 || maxCol > pInImg->cols() || minRow < 0 || maxRow > pInImg->rows())
// 		return false;

	//处理
	input_image.set_size(nWidth, nHeight);

	int nCol, nRow;
	for (nRow = minRow; nRow < maxRow; nRow++)
	{
		for (nCol = minCol; nCol < maxCol; nCol++)
		{
			int imRow = nRow - minRow;
			int imCol = nCol - minCol;
			imRow = (imRow > 0) ? imRow : 0;
			imRow = (imRow < nHeight) ? imRow : (nHeight - 1);
			imCol = (imCol > 0) ? imCol : 0;
			imCol = (imCol < nWidth) ? imCol : (nWidth - 1);

			input_image(imRow, imCol).red = *(unsigned char*)pInImg->readL(nRow, nCol, 0);
			input_image(imRow, imCol).green = *(unsigned char*)pInImg->readL(nRow, nCol, 1);
			input_image(imRow, imCol).blue = *(unsigned char*)pInImg->readL(nRow, nCol, 2);

			/*
			input_image(nRow - minRow, nCol - minCol).red = *(unsigned char*)pInImg->read(nRow, nCol, 0);
			input_image(nRow - minRow, nCol - minCol).green = *(unsigned char*)pInImg->read(nRow, nCol, 1);
			input_image(nRow - minRow, nCol - minCol).blue = *(unsigned char*)pInImg->read(nRow, nCol, 2);
			*/
		}
	}

	//dlib::save_png(input_image, "./test.png");

	return true;
}



bool ObtainRectImageFromHSR(CGDALRead* pInImg, int CurRow, int CurCol, int nWidth, int nHeight, matrix<rgb_pixel>& input_image)
{
	if (pInImg == NULL || pInImg->datatype() != INPUT_DATATYPE || pInImg->bandnum() < 3)
	{
		cout << "Input image format error." << endl;
		return false;
	}


	//读入数据
	int minCol, minRow, maxCol, maxRow;
	minCol = int(CurCol - nHeight / 2 + 0.5);
	maxCol = int(CurCol + nHeight / 2 + 0.5);
	minRow = int(CurRow - nWidth / 2 + 0.5);
	maxRow = int(CurRow + nWidth / 2 + 0.5);

// 	if (minCol < 0 || maxCol > pInImg->cols() || minRow < 0 || maxRow > pInImg->rows())
// 		return false;

	//处理
	input_image.set_size(nWidth, nHeight);


	int nCol, nRow;
	for (nRow = minRow; nRow < maxRow; nRow++)
	{
		for (nCol = minCol; nCol < maxCol; nCol++)
		{
			int imRow = nRow - minRow;
			int imCol = nCol - minCol;
			imRow = (imRow > 0) ? imRow : 0;
			imRow = (imRow < nHeight) ? imRow : (nHeight - 1);
			imCol = (imCol > 0) ? imCol : 0;
			imCol = (imCol < nWidth) ? imCol : (nWidth - 1);

			input_image(imRow, imCol).red = *(unsigned char*)pInImg->readL(nRow, nCol, 0);
			input_image(imRow, imCol).green = *(unsigned char*)pInImg->readL(nRow, nCol, 1);
			input_image(imRow, imCol).blue = *(unsigned char*)pInImg->readL(nRow, nCol, 2);
		}
	}

	//dlib::save_png(input_image, "./test.png");

	return true;
}

bool LoadHpDataFromCSV(char* fn, std::vector<hp_data>& vHpData, double dMinVal /*= 1000*/, double dMaxVal /*= 50000*/)
{
	QFile fi(fn);
	if (!fi.open(QIODevice::ReadOnly))
	{
		cout << "Open Housing Price File Error!" << endl;
		return false;
	}

	QTextStream out(&fi);
	QString str = out.readLine();
	QStringList slist = str.split(",");
	if (slist.size() < 3)
	{
		cout << "Unkown Format Housing Price File." << endl;
		fi.close();
		return false;
	}
	
	//
	//vHpData.clear();
	while (!out.atEnd())
	{
		str = out.readLine();
		slist = str.split(",");

		hp_data dt;
		dt.dlon = slist[0].trimmed().toDouble();
		dt.dlat = slist[1].trimmed().toDouble();
		if (dt.dlon < dt.dlat) std::swap(dt.dlon, dt.dlat);
		dt.dPrice = slist[2].trimmed().toDouble();

		if (dt.dPrice < dMinVal || dt.dPrice > dMaxVal)
			continue;

		vHpData.push_back(dt);
	}

	cout << "Load Housing Price File Success. Total Count = " << vHpData.size() << endl;
	
	fi.close();
	return true;
}

bool ChangeHpData2CnnSamples(CGDALRead* pInImg, std::vector<hp_data> vHpData, int nWidth, \
	int nHeight, std::vector<hp_sample>& vSamples)
{
	if (pInImg == NULL || vHpData.size() == 0)
	{
		cout << "Please Load RS Image and Housing Price First." << endl;
		return false;
	}

	int nCount = 0;
	foreach(hp_data dt, vHpData)
	{
		nCount++;
		if (nCount % 500 == 0)
			cout << "Processing No. " << nCount << " / " << vHpData.size() << "..." << endl;

		hp_sample sap;
		if (!ObtainRectImageFromHsrByLatLon(pInImg, dt.dlon, dt.dlat, nWidth, nHeight, sap.input_image))
			continue;
		sap.dPrice = dt.dPrice;
		vSamples.push_back(sap);
	}

	cout << "Load CNN samples Success. Total Count = " << vSamples.size() << endl;


	return true;
}

rectangle make_random_cropping_rect(
	const matrix<rgb_pixel>& img,
	dlib::rand& rnd
)
{
	// figure out what rectangle we want to crop from the image
	double mins = 0.25, maxs = 0.875;
	auto scale = mins + rnd.get_random_double()*(maxs - mins);
	auto size = scale*std::min(img.nr(), img.nc());
	rectangle rect(size, size);
	// randomly shift the box around
	point offset(rnd.get_random_32bit_number() % (img.nc() - rect.width()),
		rnd.get_random_32bit_number() % (img.nr() - rect.height()));
	return move_rect(rect, offset);
}

void RandomlyCropImage(
	const matrix<rgb_pixel>& input_image,
	matrix<rgb_pixel>& output_image,
	dlib::rand& rnd,
	int nCropWidth, 
	int nCropHeight
)
{
	const auto rect = make_random_cropping_rect(input_image, rnd);

	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth));

	// Crop the input image.
	extract_image_chip(input_image, chip_details, output_image, interpolate_bilinear());

	// Also randomly flip the input image and the labels.
	if (rnd.get_random_double() > 0.5)
		output_image = fliplr(output_image);	

	//dlib::save_png(output_image, "./data/test.png");

	// And then randomly adjust the colors.
	// apply_random_color_offset(output_image, rnd);
}

void CenterCropImage(const matrix<rgb_pixel>& input_image, \
	matrix<rgb_pixel>& output_image, int nCropWidth, int nCropHeight)
{
	dlib::rectangle rect(nCropWidth, nCropHeight);
	point offset((input_image.nc() - rect.width())/2, (input_image.nr() - rect.height())/2);
	rect = move_rect(rect, offset);

	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth));
	// Crop the input image.
	extract_image_chip(input_image, chip_details, output_image, interpolate_bilinear());
}

rectangle make_random_center_cropping_rect(
	const matrix<rgb_pixel>& img,
	dlib::rand& rnd
)
{
	// figure out what rectangle we want to crop from the image
	double mins = 0.25, maxs = 0.875;
	auto scale = mins + rnd.get_random_double()*(maxs - mins);
	auto size = scale*std::min(img.nr(), img.nc());
	rectangle rect(size, size);
	// randomly shift the box around
	point offset((img.nc() - rect.width()) / 2, (img.nr() - rect.height()) / 2);
	return move_rect(rect, offset);
}

void CenterRandomlyCropImage(const matrix<rgb_pixel>& input_image, matrix<rgb_pixel>& output_image, dlib::rand& rnd, int nCropWidth, int nCropHeight)
{
	const auto rect = make_random_center_cropping_rect(input_image, rnd);

	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth));

	// Crop the input image.
	extract_image_chip(input_image, chip_details, output_image, interpolate_bilinear());

	// Also randomly flip the input image and the labels.
	if (rnd.get_random_double() > 0.5)
		output_image = fliplr(output_image);
}


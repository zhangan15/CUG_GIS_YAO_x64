
#include "CommonFunctions.h"

bool load_objects_list(const char* dir, QHash<QString, std::vector<std::string>>& hash_objects)
{

	QDir _dir(dir);
	QFileInfoList finfolist = _dir.entryInfoList(QStringList(QString("*.jpg")), QDir::Files, QDir::Name);
	if (finfolist.isEmpty())
	{
		std::cout << "not find files in " << dir << std::endl;
		return false;
	}
	foreach(QFileInfo finfo, finfolist)
	{
		std::string sFilePath = finfo.filePath().toStdString();
		QString sBaseName = finfo.baseName();
		QStringList sBaseNameList = sBaseName.split("_");
		QString sGeoHash = sBaseNameList[0];

		if (hash_objects.contains(sGeoHash))
		{
			hash_objects[sGeoHash].push_back(sFilePath);
		}
		else
		{
			std::vector<std::string> svPerLocFilePath;
			svPerLocFilePath.push_back(sFilePath);
			hash_objects.insert(sGeoHash, svPerLocFilePath);
		}
	}

	QHash<QString, std::vector<std::string>>::iterator i = hash_objects.begin();
	while (i != hash_objects.end())
	{
		if (i.value().size() != LAYER_NUM)
		{
			std::cout << "Missing images in " << i.key().toStdString() << std::endl;
			i = hash_objects.erase(i);
		}
		i++;
	}

	return true;
}

bool load_objects_list(const char* dir, QHash<QString, std::vector<std::vector<std::string>>>& hash_objects)
{
	QDir _dir(dir);
	QFileInfoList finfolist = _dir.entryInfoList(QStringList(QString("*.jpg")), QDir::Files, QDir::Name);
	if (finfolist.isEmpty())
	{
		std::cout << "not find files in " << dir << std::endl;
		return false;
	}
	foreach(QFileInfo finfo, finfolist)
	{
		std::string sFilePath = finfo.filePath().toStdString();
		QString sBaseName = finfo.baseName();
		QStringList sBaseNameList = sBaseName.split("_");
		QString sGeoHash = sBaseNameList[0];
		int nImageID = sBaseNameList[1].trimmed().toInt();
		int nLayerID = sBaseNameList[2].trimmed().toInt();

		if (hash_objects.contains(sGeoHash))
		{
			while (hash_objects[sGeoHash].size() < (nImageID + 1))
			{
				std::vector<string> svTmpVec;
				hash_objects[sGeoHash].push_back(svTmpVec);
			}

			hash_objects[sGeoHash][nImageID].push_back(sFilePath);
		}
		else
		{
			std::vector<std::vector<std::string>> svvImagesPath(nImageID + 1);
			svvImagesPath[nImageID].push_back(sFilePath);
			hash_objects.insert(sGeoHash, svvImagesPath);
		}
	}

	QHash<QString, std::vector<std::vector<std::string>>>::iterator i = hash_objects.begin();
	while (i != hash_objects.end())
	{
		std::vector<std::vector<std::string>>::iterator j = i.value().begin();
		int _count = 0;
		while (j != i.value().end())
		{
			if ((*j).size() != LAYER_NUM)
			{
				j = i.value().erase(j);
				std::cout << "Missing images in " << i.key().toStdString() << "_" << _count << std::endl;
			}
			_count++;
			if(i.value().size() != 0)
				j++;
		}
		if (i.value().size() == 0)
		{
			std::cout << "Missing images in Location: " << i.key().toStdString() << std::endl;
			i = hash_objects.erase(i);
		}
		i++;
	}
	return true;
}

bool load_objects_list(const char* dir, const char* sObjectListFile, QHash<QString, std::vector<std::string>>& hash_objects)
{
	QFile _infile(sObjectListFile);
	if (!_infile.open(QIODevice::ReadOnly))
	{
		std::cout << "Open object list file error!" << std::endl;
		return false;
	}
	QTextStream _in(&_infile);

	QDir _dir(dir);
	if (!_dir.exists())
	{
		std::cout << "The save image directory did not exists." << std::endl;
		return false;
	}

	while (!_in.atEnd())
	{
		QString smsg = _in.readLine();
		QFileInfo finfo = QFileInfo(smsg);
		QString sFileAbsolutePath = QString("%1\\%2").arg(_dir.absolutePath()).arg(smsg);
		std::string sFilePath = sFileAbsolutePath.toStdString();

		QString sBaseName = finfo.baseName();
		QStringList sBaseNameList = sBaseName.split("_");
		QString sGeoHash = sBaseNameList[0];

		if (hash_objects.contains(sGeoHash))
		{
			hash_objects[sGeoHash].push_back(sFilePath);
		}
		else
		{
			std::vector<std::string> svPerLocFilePath;
			svPerLocFilePath.push_back(sFilePath);
			hash_objects.insert(sGeoHash, svPerLocFilePath);
		}
	}

	QHash<QString, std::vector<std::string>>::iterator i = hash_objects.begin();
	while (i != hash_objects.end())
	{
		if (i.value().size() != LAYER_NUM)
		{
			std::cout << "Missing images in " << i.key().toStdString() << std::endl;
			i = hash_objects.erase(i);
		}
		i++;
	}

	_infile.close();
	return true;
}

bool load_objects_list(const char* dir, const char* sObjectListFile, QHash<QString, std::vector<std::vector<std::string>>>& hash_objects)
{
	QFile _infile(sObjectListFile);
	if (!_infile.open(QIODevice::ReadOnly))
	{
		std::cout << "Open object list file error!" << std::endl;
		return false;
	}
	QTextStream _in(&_infile);

	QDir _dir(dir);
	if (!_dir.exists())
	{
		std::cout << "The save image directory did not exists." << std::endl;
		return false;
	}

	while (!_in.atEnd())
	{
		QString smsg = _in.readLine();
		QFileInfo finfo = QFileInfo(smsg);
		QString sFileAbsolutePath = QString("%1/%2").arg(_dir.absolutePath()).arg(smsg);
		std::string sFilePath = sFileAbsolutePath.toStdString();

		QString sBaseName = finfo.baseName();
		QStringList sBaseNameList = sBaseName.split("_");
		QString sGeoHash = sBaseNameList[0];
		int nImageID = sBaseNameList[1].trimmed().toInt();
		int nLayerID = sBaseNameList[2].trimmed().toInt();

		if (hash_objects.contains(sGeoHash))
		{
			while (hash_objects[sGeoHash].size() < (nImageID + 1))
			{
				std::vector<string> svTmpVec;
				hash_objects[sGeoHash].push_back(svTmpVec);
			}

			hash_objects[sGeoHash][nImageID].push_back(sFilePath);
		}
		else
		{
			std::vector<std::vector<std::string>> svvImagesPath(nImageID + 1);
			svvImagesPath[nImageID].push_back(sFilePath);
			hash_objects.insert(sGeoHash, svvImagesPath);
		}
		
	}

	QHash<QString, std::vector<std::vector<std::string>>>::iterator i = hash_objects.begin();
	while (i != hash_objects.end())
	{
		std::vector<std::vector<std::string>>::iterator j = i.value().begin();
		int _count = 0;
		while (j != i.value().end())
		{
			if ((*j).size() != LAYER_NUM)
			{
				j = i.value().erase(j);
				std::cout << "Missing images in " << i.key().toStdString() << "_" << _count << std::endl;
			}
			_count++;
			if (i.value().size() != 0)
				j++;
		}
		if (i.value().size() == 0)
		{
			std::cout << "Missing images in Location: " << i.key().toStdString() << std::endl;
			i = hash_objects.erase(i);
		}
		i++;
	}
	_infile.close();
	return false;
}

bool load_predict_images(std::vector<std::vector<std::string>> svvImagesPath, std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images, int nCropHeight, int nCropWidth)
{
	matrix<rgb_pixel> image_rgb;
	matrix<unsigned char> image_gray;
	std::array<matrix<unsigned char>, LAYER_NUM> imageLayers;
	std::array<matrix<unsigned char>, LAYER_NUM> output_image;

	for (int i = 0; i < svvImagesPath.size(); i++)
	{
		for (int j = 0; j < LAYER_NUM; j++)
		{
			load_image(image_rgb, svvImagesPath[i][j]);
			assign_image(image_gray, image_rgb);
			if (image_gray.nc() != nCropWidth && image_gray.nr() != nCropHeight)
			{
				matrix<unsigned char> image_interpolate;
				InterpolateImage(image_gray, image_interpolate, nCropWidth, nCropHeight);
				imageLayers[j] = image_interpolate;
			}
			else
				imageLayers[j] = image_gray;
		}
		images.push_back(imageLayers);
	}


	return true;
}

bool load_lables(const char* sLabelFile, QHash<QString, std::vector<std::string>> hash_objects, std::vector<std::vector<QString>>& svvLables)
{
	QHash<QString, unsigned long> hash_labels;

	QFile _file(sLabelFile);
	if (!_file.open(QIODevice::ReadOnly))
	{
		
		std::cout << "Read label file error!" << std::endl;
		return false;
	}
	QTextStream _in(&_file);
	_in.readLine();

	unsigned long nLabelNum = 0;
	while (!_in.atEnd())
	{
		QString smsg = _in.readLine();
		QStringList smsglist = smsg.split(",");

		QString sGeoHash = smsglist[0];
		unsigned long nLabel = smsglist[1].trimmed().toInt();

		if (hash_objects.contains(sGeoHash))
		{
			hash_labels.insert(sGeoHash, nLabel);
			if (nLabel > nLabelNum)
				nLabelNum = nLabel;
		}
	}
	nLabelNum++;
	svvLables.resize(nLabelNum);

	QHash<QString, unsigned long>::iterator _iter;
	for (_iter = hash_labels.begin(); _iter != hash_labels.end(); _iter++)
	{
		unsigned long nLabel = _iter.value();
		QString sGeoHash = _iter.key();

		svvLables[nLabel].push_back(sGeoHash);
	}

	_file.close();
	return true;
}

bool load_lables(const char* sLabelFile, QHash<QString, std::vector<std::vector<std::string>>> hash_objects, std::vector<std::vector<QString>>& svvLables)
{
	QHash<QString, unsigned long> hash_labels;

	QFile _file(sLabelFile);
	if (!_file.open(QIODevice::ReadOnly))
	{
		
		std::cout << "Read label file error!" << std::endl;
		return false;
	}
	QTextStream _in(&_file);
	_in.readLine();

	unsigned long nLabelNum = 0;
	while (!_in.atEnd())
	{
		QString smsg = _in.readLine();
		QStringList smsglist = smsg.split(",");

		QString sGeoHash = smsglist[0];
		unsigned long nLabel = smsglist[1].trimmed().toInt();

		if (hash_objects.contains(sGeoHash))
		{
			hash_labels.insert(sGeoHash, nLabel);
			if (nLabel > nLabelNum)
				nLabelNum = nLabel;
		}
	}
	nLabelNum++;
	svvLables.resize(nLabelNum);

	QHash<QString, unsigned long>::iterator _iter;
	for (_iter = hash_labels.begin(); _iter != hash_labels.end(); _iter++)
	{
		unsigned long nLabel = _iter.value();
		QString sGeoHash = _iter.key();

		svvLables[nLabel].push_back(sGeoHash);
	}

	_file.close();
	return true;
}

void data_loader_one(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd, bool bIsOneLabelMultiLoc,
	dlib::pipe<std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>>& qimages,
	dlib::pipe<std::vector<unsigned long>>& qlabels,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables
)
{
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>> images;
	std::vector<unsigned long> labels;
	while (qimages.is_enabled())
	{
		try
		{
			if(bIsOneLabelMultiLoc)
				load_mini_batch_one_label_multi_loc(num_loc, samples_per_loc, nCropWidth, nCropHeight, rnd, hash_objects, svvLables, images, labels);
			else
				load_mini_batch_one_label_one_loc(num_loc, samples_per_loc, nCropWidth, nCropHeight, rnd, hash_objects, svvLables, images, labels);
			qimages.enqueue(images);
			qlabels.enqueue(labels);
		}
		catch (std::exception& e)
		{
			std::cout << "EXCEPTION IN LOADING DATA" << std::endl;
			std::cout << e.what() << std::endl;
		}

	}
}


void data_loader_loc2vec(
	const size_t num_loc, const size_t samples_per_loc, 
	int nCropWidth, int nCropHeight, dlib::rand& rnd, bool bIsOneLabelMultiLoc,
	dlib::pipe<std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>>& qimages, 
	dlib::pipe<std::vector<unsigned long>>& qlabels, 
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects, 
	std::vector<std::vector<QString>> svvLables)
{
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>> images;
	std::vector<unsigned long> labels;
	while (qimages.is_enabled())
	{
		try
		{
			if(bIsOneLabelMultiLoc)
				load_mini_batch_one_label_multi_loc(num_loc, samples_per_loc, nCropWidth, nCropHeight, rnd, hash_objects, svvLables, images, labels);
			else
				load_mini_batch_one_label_one_loc(num_loc, samples_per_loc, nCropWidth, nCropHeight, rnd, hash_objects, svvLables, images, labels);
			qimages.enqueue(images);
			qlabels.enqueue(labels);
		}
		catch (std::exception& e)
		{
			std::cout << "EXCEPTION IN LOADING DATA" << std::endl;
			std::cout << e.what() << std::endl;
		}
	}
}

bool load_mini_batch_one_label_one_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels)
{
	images.clear();
	labels.clear();
	DLIB_CASSERT(num_loc <= svvLables.size(), "The dataset doesn't have that many types in it.");

	std::vector<bool> already_selected(svvLables.size(), false);
	matrix<rgb_pixel> image_rgb;
	matrix<unsigned char> image_gray;
	std::array<matrix<unsigned char>, LAYER_NUM> imageLayers;
	std::array<matrix<unsigned char>, LAYER_NUM> output_image;

	for (size_t i = 0; i < num_loc; i++)
	{
		size_t id = rnd.get_random_32bit_number() % svvLables.size();
		while (already_selected[id] || svvLables[id].size() == 0)
			id = rnd.get_random_32bit_number() % svvLables.size();
		already_selected[id] = true;

		size_t nRandIndex = rnd.get_random_32bit_number() % (svvLables[id].size());
		QString sGeoHash = svvLables[id][nRandIndex];
		std::vector<std::string> svImgLayers = hash_objects.value(sGeoHash);

		for (int _layer = 0; _layer < LAYER_NUM; _layer++)
		{
			load_image(image_rgb, svImgLayers[_layer]);
			assign_image(image_gray, image_rgb);
			if (image_gray.nc() != nCropWidth && image_gray.nr() != nCropHeight)
			{
				matrix<unsigned char> image_interpolate;
				InterpolateImage(image_gray, image_interpolate, nCropWidth, nCropHeight);
				imageLayers[_layer] = image_interpolate;
			}
			else
				imageLayers[_layer] = image_gray;
		}

		for (size_t j = 0; j < samples_per_loc; j++)
		{
			RandomlyCropImage(imageLayers, output_image, rnd, nCropWidth, nCropHeight);
			images.push_back(output_image);
			labels.push_back(id);
		}
	}

	return true;
}

bool load_mini_batch_one_label_one_loc(
	const size_t num_loc, const size_t samples_per_loc, 
	int nCropWidth, int nCropHeight, dlib::rand& rnd, 
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects, 
	std::vector<std::vector<QString>> svvLables, 
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images, 
	std::vector<unsigned long>& labels)
{
	images.clear();
	labels.clear();
	DLIB_CASSERT(num_loc <= svvLables.size(), "The dataset doesn't have that many types in it.");

	std::vector<bool> already_selected(svvLables.size(), false);
	matrix<rgb_pixel> image_rgb;
	matrix<unsigned char> image_gray;
	std::array<matrix<unsigned char>, LAYER_NUM> imageLayers;

	for (size_t i = 0; i < num_loc; i++)
	{
		size_t id = rnd.get_random_32bit_number() % svvLables.size();
		while (already_selected[id] || svvLables[id].size() == 0)
			id = rnd.get_random_32bit_number() % svvLables.size();
		already_selected[id] = true;

		size_t nRandImgInLabelID = rnd.get_random_32bit_number() % (svvLables[id].size());
		QString sGeoHash = svvLables[id][nRandImgInLabelID];
		std::vector<std::vector<std::string>> svImgs = hash_objects.value(sGeoHash);

		for (size_t j = 0; j < samples_per_loc; j++)
		{
			size_t nRandImgInLoc = rnd.get_random_32bit_number() % (svImgs.size());

			for (int _layer = 0; _layer < LAYER_NUM; _layer++)
			{
				load_image(image_rgb, svImgs[nRandImgInLoc][_layer]);
				assign_image(image_gray, image_rgb);
				if (image_gray.nc() != nCropWidth && image_gray.nr() != nCropHeight)
				{
					matrix<unsigned char> image_interpolate;
					InterpolateImage(image_gray, image_interpolate, nCropWidth, nCropHeight);
					imageLayers[_layer] = image_interpolate;
				}
				else
					imageLayers[_layer] = image_gray;
			}

			images.push_back(imageLayers);
			labels.push_back(id);
		}
	}

	return true;
}

bool load_mini_batch_one_label_multi_loc(
	const size_t num_loc, const size_t samples_per_loc,
	int nCropWidth, int nCropHeight, dlib::rand& rnd,
	QHash<QString, std::vector<std::string>> hash_objects,
	std::vector<std::vector<QString>> svvLables,
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images,
	std::vector<unsigned long>& labels)
{
	images.clear();
	labels.clear();
	DLIB_CASSERT(num_loc <= svvLables.size(), "The dataset doesn't have that many types in it.");

	std::vector<bool> already_selected(svvLables.size(), false);
	matrix<rgb_pixel> image_rgb;
	matrix<unsigned char> image_gray;
	std::array<matrix<unsigned char>, LAYER_NUM> imageLayers;
	std::array<matrix<unsigned char>, LAYER_NUM> output_image;

	for (size_t i = 0; i < num_loc; i++)
	{
		size_t id = rnd.get_random_32bit_number() % svvLables.size();
		while (already_selected[id] || svvLables[id].size() == 0)
			id = rnd.get_random_32bit_number() % svvLables.size();
		already_selected[id] = true;

		for (size_t j = 0; j < samples_per_loc; j++)
		{
			size_t nRandIndex = rnd.get_random_32bit_number() % (svvLables[id].size());
			QString sGeoHash = svvLables[id][nRandIndex];
			std::vector<std::string> svImgLayers = hash_objects.value(sGeoHash);

			for (int _layer = 0; _layer < LAYER_NUM; _layer++)
			{
				load_image(image_rgb, svImgLayers[_layer]);
				assign_image(image_gray, image_rgb);
				if (image_gray.nc() != nCropWidth && image_gray.nr() != nCropHeight)
				{
					matrix<unsigned char> image_interpolate;
					InterpolateImage(image_gray, image_interpolate, nCropWidth, nCropHeight);
					imageLayers[_layer] = image_interpolate;
				}
				else
					imageLayers[_layer] = image_gray;
			}

			RandomlyCropImage(imageLayers, output_image, rnd, nCropWidth, nCropHeight);
			images.push_back(output_image);
			labels.push_back(id);
		}
	}
	return true;
}

bool load_mini_batch_one_label_multi_loc(
	const size_t num_loc, const size_t samples_per_loc, 
	int nCropWidth, int nCropHeight, dlib::rand& rnd, 
	QHash<QString, std::vector<std::vector<std::string>>> hash_objects, 
	std::vector<std::vector<QString>> svvLables, 
	std::vector<std::array<matrix<unsigned char>, LAYER_NUM>>& images, 
	std::vector<unsigned long>& labels)
{
	images.clear();
	labels.clear();
	DLIB_CASSERT(num_loc <= svvLables.size(), "The dataset doesn't have that many types in it.");

	std::vector<bool> already_selected(svvLables.size(), false);
	matrix<rgb_pixel> image_rgb;
	matrix<unsigned char> image_gray;
	std::array<matrix<unsigned char>, LAYER_NUM> imageLayers;

	for (size_t i = 0; i < num_loc; i++)
	{
		size_t id = rnd.get_random_32bit_number() % svvLables.size();
		while (already_selected[id] || svvLables[id].size() == 0)
			id = rnd.get_random_32bit_number() % svvLables.size();
		already_selected[id] = true;

		for (size_t j = 0; j < samples_per_loc; j++)
		{
			size_t nRandImgInLabelID = rnd.get_random_32bit_number() % (svvLables[id].size());
			QString sGeoHash = svvLables[id][nRandImgInLabelID];
			std::vector<std::vector<std::string>> svImgs = hash_objects.value(sGeoHash);

			size_t nRandImgInLoc = rnd.get_random_32bit_number() % (svImgs.size());

			for (int _layer = 0; _layer < LAYER_NUM; _layer++)
			{
				load_image(image_rgb, svImgs[nRandImgInLoc][_layer]);
				assign_image(image_gray, image_rgb);
				if (image_gray.nc() != nCropWidth && image_gray.nr() != nCropHeight)
				{
					matrix<unsigned char> image_interpolate;
					InterpolateImage(image_gray, image_interpolate, nCropWidth, nCropHeight);
					imageLayers[_layer] = image_interpolate;
				}
				else
					imageLayers[_layer] = image_gray;
			}

			images.push_back(imageLayers);
			labels.push_back(id);
		}
	}

	return true;
}

void RandomlyCropImage(
	const std::array<matrix<unsigned char>, LAYER_NUM>& input_image,
	std::array<matrix<unsigned char>, LAYER_NUM>& output_image,
	dlib::rand& rnd,
	int nCropWidth,
	int nCropHeight)
{
	rectangle crop_rect(nCropWidth, nCropHeight);

	// randomly shift the box around
	point offset(rnd.get_random_32bit_number() % (input_image[0].nc() - crop_rect.width()),
		rnd.get_random_32bit_number() % (input_image[0].nr() - crop_rect.height()));

	rectangle rect = move_rect(crop_rect, offset);

	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth));

	// Crop the input image.
	matrix<unsigned char> in_layer;
	matrix<unsigned char> out_layer;
	for (int i = 0; i < input_image.size(); i++)
	{
		in_layer = input_image[i];
		extract_image_chip(in_layer, chip_details, out_layer, interpolate_bilinear());
		output_image[i] = out_layer;
	}

}


void RandomlyCropImage(
	const matrix<rgb_pixel>& input_image,
	matrix<rgb_pixel>& output_image, 
	dlib::rand& rnd, 
	int nCropWidth, 
	int nCropHeight)
{
	rectangle crop_rect(nCropWidth, nCropHeight);

	// randomly shift the box around
	point offset(rnd.get_random_32bit_number() % (input_image.nc() - crop_rect.width()),
		rnd.get_random_32bit_number() % (input_image.nr() - crop_rect.height()));

	rectangle rect = move_rect(crop_rect, offset);
	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth));

	// Crop the input image.
	extract_image_chip(input_image, chip_details, output_image, interpolate_bilinear());

}

void CenterRandomlyCropImage(
	const matrix<rgb_pixel>& input_image, 
	matrix<rgb_pixel>& output_image, 
	dlib::rand& rnd, 
	int nCropWidth, 
	int nCropHeight)
{
	rectangle crop_rect(nCropWidth, nCropHeight);
	point offset((input_image.nc() - crop_rect.width()) / 2, (input_image.nr() - crop_rect.height()) / 2);
	rectangle rect = move_rect(crop_rect, offset);

	double dAngles = rnd.get_random_double() * 360;
	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth), dAngles);
	extract_image_chip(input_image, chip_details, output_image, interpolate_bilinear());
}


void CenterRandomlyCropImage(
	const std::array<matrix<unsigned char>, LAYER_NUM>& input_image, 
	std::array<matrix<unsigned char>, LAYER_NUM>& output_image, 
	dlib::rand& rnd, 
	int nCropWidth, 
	int nCropHeight)
{
	rectangle crop_rect(nCropWidth, nCropHeight);
	point offset((input_image[0].nc() - crop_rect.width()) / 2, (input_image[0].nr() - crop_rect.height()) / 2);
	rectangle rect = move_rect(crop_rect, offset);

	double dAngles = rnd.get_random_double() * 360;
	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth), dAngles);

	matrix<unsigned char> in_layer;
	matrix<unsigned char> out_layer;
	for (int i = 0; i < input_image.size(); i++)
	{
		in_layer = input_image[i];
		extract_image_chip(in_layer, chip_details, out_layer, interpolate_bilinear());
		output_image[i] = out_layer;
	}
}

void PaddingZero(const matrix<unsigned char>& input_image, matrix<unsigned char>& output_image, int nCropWidth, int nCropHeight)
{
	output_image.set_size(nCropHeight, nCropWidth);

	for (int i = 0; i < input_image.nr(); i++)
	{
		for (int j = 0; j < input_image.nc(); j++)
		{
			output_image(i,j) = input_image(i, j);
		}
	}

	for (int i = input_image.nr(); i < nCropHeight; i++)
	{
		for (int j = input_image.nc(); j < nCropWidth; j++)
		{
			output_image(i, j) = 0;
		}
	}
}

void ClipImage(const matrix<unsigned char>& input_image, matrix<unsigned char>& output_image, int nCropWidth, int nCropHeight)
{
	output_image.set_size(nCropHeight, nCropWidth);

	for (int i = 0; i < nCropHeight; i++)
	{
		for (int j = 0; j < nCropWidth; j++)
			output_image(i, j) = input_image(i, j);
	}

}

void InterpolateImage(const matrix<unsigned char>& input_image, matrix<unsigned char>& output_image, int nCropWidth, int nCropHeight)
{
	rectangle rect(nCropWidth, nCropHeight);

	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth));

	// Crop the input image.
	extract_image_chip(input_image, chip_details, output_image, interpolate_bilinear());
}

void InterpolateImage(const matrix<rgb_pixel>& input_image, matrix<rgb_pixel>& output_image, int nCropWidth, int nCropHeight)
{
	rectangle rect(nCropWidth, nCropHeight);

	const chip_details chip_details(rect, chip_dims(nCropHeight, nCropWidth));

	// Crop the input image.
	extract_image_chip(input_image, chip_details, output_image, interpolate_bilinear());
}


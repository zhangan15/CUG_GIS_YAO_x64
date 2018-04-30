#include <QtCore>
#include "dnn_semantic_segmentation_label.h"

#include <iostream>
#include <dlib/data_io.h>
#include <dlib/gui_widgets.h>

using namespace std;
using namespace dlib;

// ----------------------------------------------------------------------------------------

// The PASCAL VOC2012 dataset contains 20 ground-truth classes + background.  Each class
// is represented using an RGB color value.  We associate each class also to an index in the
// range [0, 20], used internally by the network.  To generate nice RGB representations of
// inference results, we need to be able to convert the index values to the corresponding
// RGB values.

// Given an index in the range [0, 20], find the corresponding PASCAL VOC2012 class
// (e.g., 'dog').

const std::string& find_type(const uint16_t& index_label)
{
	for (int i=0; i<classes.size(); i++)
	{
		if (classes[i].index == index_label)
			return classes[i].classlabel;
	}
	return "unknown";
}

// Convert an index in the range [0, 20] to a corresponding RGB class label.
/*
inline rgb_pixel index_label_to_rgb_label(uint16_t index_label)
{
	return find_voc2012_class(index_label).rgb_label;
}*/

// Convert an image containing indexes in the range [0, 20] to a corresponding
// image containing RGB class labels.
/*
void index_label_image_to_rgb_label_image(
	const matrix<uint16_t>& index_label_image,
	matrix<rgb_pixel>& rgb_label_image
)
{
	const long nr = index_label_image.nr();
	const long nc = index_label_image.nc();

	rgb_label_image.set_size(nr, nc);

	for (long r = 0; r < nr; ++r)
	{
		for (long c = 0; c < nc; ++c)
		{
			rgb_label_image(r, c) = index_label_to_rgb_label(index_label_image(r, c));
		}
	}
}*/

// Find the most prominent class label from amongst the per-pixel predictions.
uint16_t get_most_prominent_non_background_classlabel(const matrix<uint16_t>& index_label_image)
{
	const long nr = index_label_image.nr();
	const long nc = index_label_image.nc();

	std::vector<unsigned int> counters(class_count);

	for (long r = 0; r < nr; ++r)
	{
		for (long c = 0; c < nc; ++c)
		{
			const uint16_t label = index_label_image(r, c);
			++counters[label];
		}
	}

	const auto max_element = std::max_element(counters.begin() + 1, counters.end());
	const uint16_t most_prominent_index_label = max_element - counters.begin();

	return most_prominent_index_label;
}

// ----------------------------------------------------------------------------------------

int testSemanticSeg(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "You call this program like this: " << endl;
		cout << "./dnn_semantic_segmentation_train_ex /path/to/images" << endl;
		cout << endl;
		cout << "You will also need a trained 'semantic_segmentation_voc2012net.dnn' file." << endl;
		cout << "You can either train it yourself (see example program" << endl;
		cout << "dnn_semantic_segmentation_train_ex), or download a" << endl;
		cout << "copy from here: http://dlib.net/files/semantic_segmentation_voc2012net.dnn" << endl;
		return 1;
	}

	// Read the file containing the trained network from the working directory.
	anet_type net;
	deserialize("./semantic_segmentation_voc2012net.dnn") >> net;

	// Show inference results in a window.
	image_window win;

	matrix<rgb_pixel> input_image;
	matrix<uint16_t> index_label_image;
	//matrix<rgb_pixel> rgb_label_image;

	// Find supported image files.

	const std::vector<file> files = dlib::get_files_in_directory_tree(argv[1],
		dlib::match_endings(".jpeg .jpg .png"));

	cout << "Found " << files.size() << " images, processing..." << endl;

	for (const file& file : files)
	{
		// Load the input image.
		load_image(input_image, file.full_name());

		// Create predictions for each pixel. At this point, the type of each prediction
		// is an index (a value between 0 and 20). Note that the net may return an image
		// that is not exactly the same size as the input.
		const matrix<uint16_t> temp = net(input_image);

		// Crop the returned image to be exactly the same size as the input.
		const chip_details chip_details(
			centered_rect(temp.nc() / 2, temp.nr() / 2, input_image.nc(), input_image.nr()),
			chip_dims(input_image.nr(), input_image.nc())
		);
		extract_image_chip(temp, chip_details, index_label_image); // , interpolate_nearest_neighbor());

		//
		QFileInfo fi(file.full_name().data());
		QString pngfilename = QString("%1/%2_label.png").arg(fi.absolutePath()).arg(fi.completeBaseName());
		dlib::save_png(index_label_image, pngfilename.toLocal8Bit().data());
																   // Convert the indexes to RGB values.
		//index_label_image_to_rgb_label_image(index_label_image, rgb_label_image);

		// Find the most prominent class label from amongst the per-pixel predictions.
		uint16_t classlabel = get_most_prominent_non_background_classlabel(index_label_image);

		cout << file.name() << " : " << (int)classlabel << " - hit enter to process the next image";
		//cin.get();
	}

	return 0;
}



uint16_t SementicSegImage(anet_type& net, matrix<rgb_pixel> input_image, matrix<uint16_t>& index_label_image)
{
	// Create predictions for each pixel. At this point, the type of each prediction
	// is an index (a value between 0 and 20). Note that the net may return an image
	// that is not exactly the same size as the input.
	const matrix<uint16_t> temp = net(input_image);

	// Crop the returned image to be exactly the same size as the input.
	const chip_details chip_details(
		centered_rect(temp.nc() / 2, temp.nr() / 2, input_image.nc(), input_image.nr()),
		chip_dims(input_image.nr(), input_image.nc())
	);
	extract_image_chip(temp, chip_details, index_label_image); // , interpolate_nearest_neighbor());
	
	// Find the most prominent class label from amongst the per-pixel predictions.
	uint16_t classlabel = get_most_prominent_non_background_classlabel(index_label_image);

	return classlabel;
}

int main(int argc, char** argv) try
{
	anet_type net;
	deserialize("./semantic_segmentation_ADE20K_batch2_Window5_net.dnn") >> net;
	cout << "load net success." << endl;

	QString inputFileName = "./data_jpg/113.223719_23.108423_0.png";

	//load image
	matrix<rgb_pixel> input_image;
	matrix<uint16_t> index_label_image;
	load_image(input_image, inputFileName.toLocal8Bit().data());
	cout << "load image success." << endl;

	//Êä³öµ½index_label_img
	SementicSegImage(net, input_image, index_label_image);
	cout << index_label_image.nr() << " * " << index_label_image.nc() << endl;
	
	//save png
	QFileInfo fi(inputFileName);
	QString pngfilename = QString("%1/%2_label.png").arg(fi.absolutePath()).arg(fi.completeBaseName());
	dlib::save_png(index_label_image, pngfilename.toLocal8Bit().data());
	
	return 0;
}
catch (std::exception& e)
{
	cout << e.what() << endl;
}



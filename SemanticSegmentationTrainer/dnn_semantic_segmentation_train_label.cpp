#include "dnn_semantic_segmentation_label.h"

#include <iostream>
#include <dlib/data_io.h>
#include <dlib/image_transforms.h>
#include <dlib/dir_nav.h>
#include <iterator>
#include <thread>
#include <QtCore>
#include <cuda_runtime.h>


using namespace std;
using namespace dlib;

int getClassFromFile(char* sfilename, std::vector<SegDataset>& classes)
{
	int cc = 0;
	//classes.clear();

	QFile file(sfilename);
	if (!file.open(QIODevice::ReadOnly))
	{
		cout << "read attribute file fail." << endl;
		return 0;
	}

	QTextStream _in(&file);
	
	//remove title
	_in.readLine();

	while (!_in.atEnd())
	{
		QString s = _in.readLine();
		QStringList slist = s.split(",");
		if (slist.size() < 5)
			continue;

		int nId = slist[0].trimmed().toInt();
		QString sTitle = slist[4].trimmed();
		SegDataset sg(nId, sTitle.toLocal8Bit().data());
		classes.push_back(sg);
		cc++;
	}

	file.close();
	cout << "input class count = " << cc << endl;
	
	return cc;
}

// A single training sample. A mini-batch comprises many of these.
struct training_sample
{
    matrix<rgb_pixel> input_image;
    matrix<uint16_t> label_image; // The ground-truth label of each pixel.
};

// ----------------------------------------------------------------------------------------

rectangle make_random_cropping_rect_resnet(
    const matrix<rgb_pixel>& img,
    dlib::rand& rnd
)
{
    // figure out what rectangle we want to crop from the image
    double mins = 0.466666666, maxs = 0.875;
    auto scale = mins + rnd.get_random_double()*(maxs-mins);
    auto size = scale*std::min(img.nr(), img.nc());
    rectangle rect(size, size);
    // randomly shift the box around
    point offset(rnd.get_random_32bit_number()%(img.nc()-rect.width()),
                 rnd.get_random_32bit_number()%(img.nr()-rect.height()));
    return move_rect(rect, offset);
}

// ----------------------------------------------------------------------------------------

void randomly_crop_image (
    const matrix<rgb_pixel>& input_image,
    const matrix<uint16_t>& label_image,
    training_sample& crop,
    dlib::rand& rnd
)
{
    const auto rect = make_random_cropping_rect_resnet(input_image, rnd);

    const chip_details chip_details(rect, chip_dims(227, 227));

    // Crop the input image.
    extract_image_chip(input_image, chip_details, crop.input_image, interpolate_bilinear());

    // Crop the labels correspondingly. However, note that here bilinear
    // interpolation would make absolutely no sense - you wouldn't say that
    // a bicycle is half-way between an aeroplane and a bird, would you?
    extract_image_chip(label_image, chip_details, crop.label_image, interpolate_nearest_neighbor());

    // Also randomly flip the input image and the labels.
    if (rnd.get_random_double() > 0.5)
    {
        crop.input_image = fliplr(crop.input_image);
        crop.label_image = fliplr(crop.label_image);
    }

    // And then randomly adjust the colors.
    apply_random_color_offset(crop.input_image, rnd);
}

// ----------------------------------------------------------------------------------------

// The names of the input image and the associated RGB label image in the PASCAL VOC 2012
// data set.
struct image_info
{
    string image_filename;
    string label_filename;
};

// Read the list of image files belonging to either the "train", "trainval", or "val" set
// of the PASCAL VOC2012 data.
std::vector<image_info> get_seg_data_listing(
    const std::string& folder,
    const std::string& file = "training" // "train", "trainval", or "val"
)
{
    std::ifstream in(folder + "/" + file + ".txt");

    std::vector<image_info> results;

    while (in)
    {
        std::string basename;
        in >> basename;

        if (!basename.empty())
        {
            image_info image_info;
            image_info.image_filename = folder + "/images/" + file + "/" + basename + ".jpg";
            image_info.label_filename = folder + "/annotations/" + file + "/" + basename + ".png";	//here should be uint16_t

			if (!QFile::exists(image_info.image_filename.data()) || !QFile::exists(image_info.label_filename.data()))
				continue;

            results.push_back(image_info);
        }
    }

	cout << file << " : input file count = " << results.size() << endl;

    return results;
}

// Read the list of image files belong to the "train" set of the PASCAL VOC2012 data.
std::vector<image_info> get_seg_data_train_listing(
    const std::string& folder
)
{
    return get_seg_data_listing(folder, "training");
}

// Read the list of image files belong to the "val" set of the PASCAL VOC2012 data.
std::vector<image_info> get_seg_data_val_listing(
    const std::string& folder
)
{
    return get_seg_data_listing(folder, "validation");
}

// ----------------------------------------------------------------------------------------

// The PASCAL VOC2012 dataset contains 20 ground-truth classes + background.  Each class
// is represented using an RGB color value.  We associate each class also to an index in the
// range [0, 20], used internally by the network.  To convert the ground-truth data to
// something that the network can efficiently digest, we need to be able to map the RGB
// values to the corresponding indexes.

// Given an RGB representation, find the corresponding PASCAL VOC2012 class
// (e.g., 'dog').
/*
const SegDataset& find_voc2012_class(const dlib::rgb_pixel& rgb_label)
{
    return find_voc2012_class(
        [&rgb_label](const SegDataset& SegDataset)
        {
            return rgb_label == SegDataset.rgb_label;
        }
    );
}*/

// Convert an RGB class label to an index in the range [0, 20].
/*
inline uint16_t rgb_label_to_index_label(const dlib::rgb_pixel& rgb_label)
{
    return find_voc2012_class(rgb_label).index;
}
*/

// Convert an image containing RGB class labels to a corresponding
// image containing indexes in the range [0, 20].
/*
void rgb_label_image_to_index_label_image(
    const dlib::matrix<dlib::rgb_pixel>& rgb_label_image,
    dlib::matrix<uint16_t>& index_label_image
)
{
    const long nr = rgb_label_image.nr();
    const long nc = rgb_label_image.nc();

    index_label_image.set_size(nr, nc);

    for (long r = 0; r < nr; ++r)
    {
        for (long c = 0; c < nc; ++c)
        {
            index_label_image(r, c) = rgb_label_to_index_label(rgb_label_image(r, c));
        }
    }
}
*/

// ----------------------------------------------------------------------------------------

// Calculate the per-pixel accuracy on a dataset whose file names are supplied as a parameter.
double calculate_accuracy(anet_type& anet, const std::vector<image_info>& dataset)
{
    int num_right = 0;
    int num_wrong = 0;

    matrix<rgb_pixel> input_image;
    //matrix<rgb_pixel> rgb_label_image;
    matrix<uint16_t> index_label_image;
    matrix<uint16_t> net_output;

    for (const auto& image_info : dataset)
    {
        // Load the input image.
        load_image(input_image, image_info.image_filename);

        // Load the ground-truth (RGB) labels.
        load_image(index_label_image, image_info.label_filename);

        // Create predictions for each pixel. At this point, the type of each prediction
        // is an index (a value between 0 and 20). Note that the net may return an image
        // that is not exactly the same size as the input.
        const matrix<uint16_t> temp = anet(input_image);

        // Convert the indexes to RGB values.
        //rgb_label_image_to_index_label_image(rgb_label_image, index_label_image);

        // Crop the net output to be exactly the same size as the input.
        const chip_details chip_details(
            centered_rect(temp.nc() / 2, temp.nr() / 2, input_image.nc(), input_image.nr()),
            chip_dims(input_image.nr(), input_image.nc())
        );
        extract_image_chip(temp, chip_details, net_output, interpolate_nearest_neighbor());

        const long nr = index_label_image.nr();
        const long nc = index_label_image.nc();

        // Compare the predicted values to the ground-truth values.
        for (long r = 0; r < nr; ++r)
        {
            for (long c = 0; c < nc; ++c)
            {
                const uint16_t truth = index_label_image(r, c);
                if (truth != dlib::loss_multiclass_log_per_pixel_::label_to_ignore)
                {
                    const uint16_t prediction = net_output(r, c);
                    if (prediction == truth)
                    {
                        ++num_right;
                    }
                    else
                    {
                        ++num_wrong;
                    }
                }
            }
        }
    }

    // Return the accuracy estimate.
    return num_right / static_cast<double>(num_right + num_wrong);
}

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv) try
{
    if (argc < 5)
    {
        cout << "./dnn_semantic_segmentation_train_ex ./ADEChallengeData2016_DIR ./Semantic_segmentation_ADE20K_net.dnn batch_size window_size" << endl;
        return 1;
    }

// 	cudaError_t cudaStatus = cudaSetDevice(1);
// 	if (cudaStatus != cudaSuccess) {
// 		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
// 		return -1;
// 	}

	char sfolder[2048];
	strcpy(sfolder, argv[1]);
	cout << "data folder: " << sfolder << endl;
	char sOutputDnn[2048];
	strcpy(sOutputDnn, argv[2]);
	cout << "output DNN file: " << sOutputDnn << endl;

	int batch_size = atoi(argv[3]);
	int window_size = atoi(argv[4]);

	if (batch_size <= 1) batch_size = 2;
	if (window_size <= 10) window_size = 10;
	cout << "batch size = " << batch_size << endl;
	cout << "window size = " << window_size << endl;

	
// 	

	
	//int nCount = getClassFromFile(QString(QString(sfolder) + "/objectInfo150.txt").toLocal8Bit().data(), classes);

    cout << "\nSCANNING SEGMENTATION DATASET\n" << endl;

    const auto listing = get_seg_data_train_listing(sfolder);
    cout << "images in dataset: " << listing.size() << endl;
    if (listing.size() == 0)
    {
        cout << "Didn't find the segmentation dataset. " << endl;
        return 1;
    }
        

    const double initial_learning_rate = 0.1;
    const double weight_decay = 0.0001;
    const double momentum = 0.9;
	const int min_batch = batch_size;		//sensitive param (1, 100)

	set_dnn_prefer_smallest_algorithms();

    net_type net;
    dnn_trainer<net_type> trainer(net,sgd(weight_decay, momentum));
    trainer.be_verbose();
    trainer.set_learning_rate(initial_learning_rate);
    trainer.set_synchronization_file("./trainer_state_file.dat", std::chrono::minutes(10));
    // This threshold is probably excessively large.
    trainer.set_iterations_without_progress_threshold(10000);
    // Since the progress threshold is so large might as well set the batch normalization
    // stats window to something big too.
    set_all_bn_running_stats_window_sizes(net, window_size); //sensitive param, recommendation = 1000

    // Output training parameters.
    cout << endl << trainer << endl;

    std::vector<matrix<rgb_pixel>> samples;
    std::vector<matrix<uint16_t>> labels;

    // Start a bunch of threads that read images from disk and pull out random crops.  It's
    // important to be sure to feed the GPU fast enough to keep it busy.  Using multiple
    // thread for this kind of data preparation helps us do that.  Each thread puts the
    // crops into the data queue.
    dlib::pipe<training_sample> data(200);
    auto f = [&data, &listing](time_t seed)
    {
        dlib::rand rnd(time(0)+seed);
        matrix<rgb_pixel> input_image;
        //matrix<rgb_pixel> rgb_label_image;
        matrix<uint16_t> index_label_image;
        training_sample temp;
        while(data.is_enabled())
        {
            // Pick a random input image.
            const image_info& image_info = listing[rnd.get_random_32bit_number()%listing.size()];

            // Load the input image.
            load_image(input_image, image_info.image_filename);

            // Load the ground-truth (RGB) labels.
            load_image(index_label_image, image_info.label_filename);

			//output
			//cout << "Thread No. " << seed << ", Load image:" << image_info.image_filename << endl;

            // Convert the indexes to RGB values.
            //rgb_label_image_to_index_label_image(rgb_label_image, index_label_image);

            // Randomly pick a part of the image.
            randomly_crop_image(input_image, index_label_image, temp, rnd);

            // Push the result to be used by the trainer.
            data.enqueue(temp);
        }
    };
    std::thread data_loader1([f](){ f(1); });
    std::thread data_loader2([f](){ f(2); });
    std::thread data_loader3([f](){ f(3); });
    std::thread data_loader4([f](){ f(4); });

	cout << "Step 1: load file success." << endl;

    // The main training loop.  Keep making mini-batches and giving them to the trainer.
    // We will run until the learning rate has dropped by a factor of 1e-4.
    while(trainer.get_learning_rate() >= 1e-4)
    {
        samples.clear();
        labels.clear();

        // set mini-batch
        training_sample temp;
        while(samples.size() < min_batch)
        {
            data.dequeue(temp);

            samples.push_back(std::move(temp.input_image));
            labels.push_back(std::move(temp.label_image));
        }

        trainer.train_one_step(samples, labels);
    }

    // Training done, tell threads to stop and make sure to wait for them to finish before
    // moving on.
    data.disable();
    data_loader1.join();
    data_loader2.join();
    data_loader3.join();
    data_loader4.join();

	cout << "Step 2: data loader init success." << endl;

    // also wait for threaded processing to stop in the trainer.
    trainer.get_net();

	cout << "Step 3: get net success." << endl;

    net.clean();
    cout << "saving network" << endl;
    serialize(sOutputDnn) << net;

	cout << "Step 4: saving net success." << endl;

    // Make a copy of the network to use it for inference.
    anet_type anet = net;

    cout << "Testing the network..." << endl;

    // Find the accuracy of the newly trained network on both the training and the validation sets.
    cout << "train accuracy  :  " << calculate_accuracy(anet, get_seg_data_train_listing(sfolder)) << endl;
    cout << "val accuracy    :  " << calculate_accuracy(anet, get_seg_data_val_listing(sfolder)) << endl;
}
catch(std::exception& e)
{
    cout << e.what() << endl;
}


#include <QtCore>
#include <iostream>

#include <dlib/dnn.h>
#include <iostream>
#include <dlib/data_io.h>
#include <dlib/image_transforms.h>
#include <dlib/dir_nav.h>
#include <iterator>
#include <thread>

using namespace std;
using namespace dlib;


using net_type = loss_multiclass_log<
	fc<10,
	dropout<relu<fc<84,
	relu<fc<120,
	max_pool<2, 2, 2, 2, relu<con<16, 5, 5, 1, 1,
	max_pool<2, 2, 2, 2, relu<con<6, 5, 5, 1, 1,
	//input<matrix<unsigned char>>
	input<std::array<matrix<unsigned char>, 10>>//网络的输入定义为10个波段的影像
	//input_rgb_image_sized<32>
	>>>>>>>>>>>>>;

int main(int argc, char *argv[])
{
	//输入影像为十个波段
	std::vector<std::array<matrix<unsigned char>, 10>> training_images;
	std::vector<unsigned long>         training_labels;
	
	std::vector<std::array<matrix<unsigned char>, 10>> testing_images;
	std::vector<unsigned long>         testing_labels;

	//对100张10个波段的影像赋值，每张影像大小为32*32
	for (int i = 0; i < 100; i++)
	{
		//定义一个波段的二维矩阵
		matrix<unsigned char> per_band;
		per_band.set_size(32, 32);

		//定义一张影像，有10个波段
		std::array<matrix<unsigned char>, 10> _image;
		
		//对一张影像的10个波段进行赋值
		for (int _band = 0; _band < 10; _band++)
		{
			for (int _height = 0; _height < 32; _height++)
			{
				for (int _width = 0; _width < 32; _width++)
				{
					per_band(_height, _width) = i + _band + _height + _width;
				}
			}
			_image[_band] = per_band;
		}

		training_images.push_back(_image);
		training_labels.push_back(i / 10);
	}


	//输出第一张影像的十个波段的值
	for (int _band = 0; _band < 10; _band++)
	{
		for (int _height = 0; _height < 32; _height++)
		{
			for (int _width = 0; _width < 32; _width++)
			{
				std::cout << (int)training_images[0][_band](_height, _width) << ", ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl << std::endl;
	}

	

	net_type net;
	cout << "The net has " << net.num_layers << " layers in it." << endl;
	cout << net << endl;


	cout << "Traning NN..." << endl;
	dnn_trainer<net_type> trainer(net);
	trainer.set_learning_rate(0.01);
	trainer.set_min_learning_rate(0.00001);
	trainer.set_mini_batch_size(16);
	trainer.be_verbose();
	trainer.set_synchronization_file("test_sync", std::chrono::seconds(20));
	// Train the network.  This might take a few minutes...
	trainer.train(training_images, training_labels);

	net.clean();
	serialize("cifar_network_inception.dat") << net;

	std::vector<unsigned long> predicted_labels = net(training_images);
	int num_right = 0;
	int num_wrong = 0;
	// And then let's see if it classified them correctly.
	for (size_t i = 0; i < training_images.size(); ++i)
	{
		if (predicted_labels[i] == training_labels[i])
			++num_right;
		else
			++num_wrong;

	}
	cout << "training num_right: " << num_right << endl;
	cout << "training num_wrong: " << num_wrong << endl;
	cout << "training accuracy:  " << num_right / (double)(num_right + num_wrong) << endl;


	return 0;
}

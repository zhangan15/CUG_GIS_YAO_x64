#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <time.h>

// OpenNN includes

#include "opennn/opennn.h"
using namespace OpenNN;


int main(int argc, char *argv[])
{
	try
	{
		std::cout << "OpenNN. Airfoil Self-Noise Application." << std::endl;

		srand((unsigned)time(NULL));

		// Data set

		DataSet data_set;

		data_set.set_data_file_name("airfoil_self_noise.dat");

		data_set.set_separator("Tab");

		data_set.load_data();

		// Variables

		Variables* variables_pointer = data_set.get_variables_pointer();

		Vector< Variables::Item > variables_items(6);

		variables_items[0].name = "frequency";
		variables_items[0].units = "hertzs";
		variables_items[0].use = Variables::Input;

		variables_items[1].name = "angle_of_attack";
		variables_items[1].units = "degrees";
		variables_items[1].use = Variables::Input;

		variables_items[2].name = "chord_length";
		variables_items[2].units = "meters";
		variables_items[2].use = Variables::Input;

		variables_items[3].name = "free_stream_velocity";
		variables_items[3].units = "meters per second";
		variables_items[3].use = Variables::Input;

		variables_items[4].name = "suction_side_displacement_thickness";
		variables_items[4].units = "meters";
		variables_items[4].use = Variables::Input;

		variables_items[5].name = "scaled_sound_pressure_level";
		variables_items[5].units = "decibels";
		variables_items[5].use = Variables::Target;

		variables_pointer->set_items(variables_items);

		const Matrix<std::string> inputs_information = variables_pointer->arrange_inputs_information();
		const Matrix<std::string> targets_information = variables_pointer->arrange_targets_information();

		// Instances

		Instances* instances_pointer = data_set.get_instances_pointer();

		instances_pointer->split_random_indices();

		const Vector< Statistics<double> > inputs_statistics = data_set.scale_inputs_minimum_maximum();
		const Vector< Statistics<double> > targets_statistics = data_set.scale_targets_minimum_maximum();

		// Neural network

		const size_t inputs_number = variables_pointer->count_inputs_number();
		const size_t hidden_perceptrons_number = 9;
		const size_t outputs_number = variables_pointer->count_targets_number();

		NeuralNetwork neural_network(inputs_number, hidden_perceptrons_number, outputs_number);

		Inputs* inputs = neural_network.get_inputs_pointer();

		inputs->set_information(inputs_information);

		Outputs* outputs = neural_network.get_outputs_pointer();

		outputs->set_information(targets_information);

		neural_network.construct_scaling_layer();

		ScalingLayer* scaling_layer_pointer = neural_network.get_scaling_layer_pointer();

		scaling_layer_pointer->set_statistics(inputs_statistics);

		scaling_layer_pointer->set_scaling_methods(ScalingLayer::NoScaling);

		neural_network.construct_unscaling_layer();

		UnscalingLayer* unscaling_layer_pointer = neural_network.get_unscaling_layer_pointer();

		unscaling_layer_pointer->set_statistics(targets_statistics);

		unscaling_layer_pointer->set_unscaling_method(UnscalingLayer::NoUnscaling);

		// Loss index

		LossIndex loss_index(&neural_network, &data_set);

		loss_index.get_normalized_squared_error_pointer()->set_normalization_coefficient();

		loss_index.set_regularization_type(LossIndex::NEURAL_PARAMETERS_NORM);

		loss_index.get_normalized_squared_error_pointer()->set_normalization_coefficient();

		// Training strategy object

		TrainingStrategy training_strategy(&loss_index);

		QuasiNewtonMethod* quasi_Newton_method_pointer = training_strategy.get_quasi_Newton_method_pointer();

		quasi_Newton_method_pointer->set_maximum_iterations_number(1000);
		quasi_Newton_method_pointer->set_display_period(10);

		quasi_Newton_method_pointer->set_minimum_loss_increase(1.0e-6);

		quasi_Newton_method_pointer->set_reserve_loss_history(true);

		TrainingStrategy::Results training_strategy_results = training_strategy.perform_training();

		// Testing analysis

		TestingAnalysis testing_analysis(&neural_network, &data_set);

		//        TestingAnalysis::LinearRegressionAnalysis linear_regression_results = testing_analysis.perform_linear_regression_analysis();

		// Save results

		data_set.save("data_set.xml");

		neural_network.save("neural_network.xml");
		neural_network.save_expression("expression.txt");

		loss_index.save("loss_index.xml");

		training_strategy.save("training_strategy.xml");
		training_strategy_results.save("training_strategy_results.dat");

		//        linear_regression_results.save("../data/linear_regression_analysis_results.dat");

		return(0);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;

		return(1);
	}
}

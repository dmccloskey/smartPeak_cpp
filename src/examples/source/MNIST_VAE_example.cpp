/**TODO:  Add copyright*/

#include <SmartPeak/ml/PopulationTrainer.h>
#include <SmartPeak/ml/ModelTrainer.h>
#include <SmartPeak/ml/ModelReplicator.h>
#include <SmartPeak/ml/ModelBuilder.h>
#include <SmartPeak/ml/Model.h>
#include <SmartPeak/io/PopulationTrainerFile.h>
#include <SmartPeak/io/ModelFile.h>

#include <SmartPeak/simulator/MNISTSimulator.h>

#include <fstream>

#include <unsupported/Eigen/CXX11/Tensor>

using namespace SmartPeak;

/**
 * EXAMPLES using the MNIST data set
 * 
 * EXAMPLE1:
 * - reconstruction on MNIST using a VAE
 * - whole image pixels (linearized) 28x28 normalized to 0 to 1
 */

// Extended classes
class ModelTrainerExt : public ModelTrainer
{
public:
	/*
	@brief Basic VAE with	Xavier initialization

	Notes: 
	Model input nodes: "Input_0, Input_1, ... Input_784" up to n_inputs
	Model encoding input nodes: "Encoding_0, Encoding_1, ... Encoding 20" up to n_encodings
	Model output nodes: "Output_0, Output_1, ... Output_784" up to n_inputs

	References:
	Based on Kingma et al, 2014: https://arxiv.org/pdf/1312.6114
	https://github.com/pytorch/examples/blob/master/vae/main.py
	*/
	Model makeVAE(int n_inputs = 784, int n_encodings = 20, int n_hidden_0 = 512) {
		Model model;
		model.setId(0);
		model.setName("VAE");
		model.setLossFunction(std::shared_ptr<LossFunctionOp<float>>(new BCEOp<float>()));
		model.setLossFunctionGrad(std::shared_ptr<LossFunctionGradOp<float>>(new BCEGradOp<float>()));

		ModelBuilder model_builder;

		// Add the inputs
		std::vector<std::string> node_names_input = model_builder.addInputNodes(model, "Input", n_inputs);


		// Add the Endocer FC layers
		std::vector<std::string> node_names, node_names_mu, node_names_logvar;	
		node_names = model_builder.addFullyConnected(model, "EN0", "EN0", node_names_input, n_hidden_0,
			std::shared_ptr<ActivationOp<float>>(new ELUOp<float>(1.0)),
			std::shared_ptr<ActivationOp<float>>(new ELUGradOp<float>(1.0)),
			std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()),
			std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()),
			std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()),
			std::shared_ptr<WeightInitOp>(new RandWeightInitOp((int)(node_names_input.size() + node_names.size())/2, 1)),
			std::shared_ptr<SolverOp>(new AdamOp(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "EN1", "EN1", node_names, n_hidden_0,
			std::shared_ptr<ActivationOp<float>>(new ELUOp<float>(1.0)),
			std::shared_ptr<ActivationOp<float>>(new ELUGradOp<float>(1.0)),
			std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()),
			std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()),
			std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()),
			std::shared_ptr<WeightInitOp>(new RandWeightInitOp((int)(node_names.size() + node_names.size()) / 2, 1)),
			std::shared_ptr<SolverOp>(new AdamOp(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names_mu = model_builder.addFullyConnected(model, "Mu", "Mu", node_names, n_encodings,
			std::shared_ptr<ActivationOp<float>>(new ELUOp<float>(1.0)),
			std::shared_ptr<ActivationOp<float>>(new ELUGradOp<float>(1.0)),
			std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()),
			std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()),
			std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()),
			std::shared_ptr<WeightInitOp>(new RandWeightInitOp((int)(node_names.size() + n_encodings)/2, 1)),
			std::shared_ptr<SolverOp>(new AdamOp(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names_logvar = model_builder.addFullyConnected(model, "LogVar", "LogVar", node_names, 20,
			std::shared_ptr<ActivationOp<float>>(new ELUOp<float>(1.0)),
			std::shared_ptr<ActivationOp<float>>(new ELUGradOp<float>(1.0)),
			std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()),
			std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()),
			std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()),
			std::shared_ptr<WeightInitOp>(new RandWeightInitOp((int)(node_names.size() + n_encodings)/2, 1)),
			std::shared_ptr<SolverOp>(new AdamOp(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);

		// Add the Encoding layers
		std::vector<std::string> node_names_encoder = model_builder.addVAEEncoding(model, "Encoding", "Encoding", node_names_mu, node_names_logvar);

		// Add the Decoder FC layers
		node_names = model_builder.addFullyConnected(model, "DE0", "DE0", node_names_encoder, n_hidden_0,
			std::shared_ptr<ActivationOp<float>>(new ELUOp<float>(1.0)),
			std::shared_ptr<ActivationOp<float>>(new ELUGradOp<float>(1.0)),
			std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()),
			std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()),
			std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()),
			std::shared_ptr<WeightInitOp>(new RandWeightInitOp((int)(node_names_encoder.size() + n_hidden_0)/2, 1)),
			std::shared_ptr<SolverOp>(new AdamOp(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "DE1", "DE2", node_names, n_hidden_0,
			std::shared_ptr<ActivationOp<float>>(new ELUOp<float>(1.0)),
			std::shared_ptr<ActivationOp<float>>(new ELUGradOp<float>(1.0)),
			std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()),
			std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()),
			std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()),
			std::shared_ptr<WeightInitOp>(new RandWeightInitOp((int)(node_names.size() + n_hidden_0) / 2, 1)),
			std::shared_ptr<SolverOp>(new AdamOp(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "Output", "Output", node_names, n_inputs,
			std::shared_ptr<ActivationOp<float>>(new SigmoidOp<float>()),
			std::shared_ptr<ActivationOp<float>>(new SigmoidGradOp<float>()),
			std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()),
			std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()),
			std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()),
			std::shared_ptr<WeightInitOp>(new RandWeightInitOp(node_names.size(), 1)),
			std::shared_ptr<SolverOp>(new AdamOp(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);

		// Specify the output node types manually
		// [NOTE: we do not specify the Encoding node types as outputs so that they remain "active" after CETT
		//			  so that back propogation begins at the decoder output and is propogated through the encoder layers
		for (const std::string& node_name : node_names)
			model.getNodesMap().at(node_name)->setType(NodeType::output);

		model.initWeights();
		return model;
	}
	Model makeModel() { return Model(); }
	void adaptiveTrainerScheduler(
		const int& n_generations,
		const int& n_epochs,
		Model& model,
		const std::vector<float>& model_errors) {
		if (n_epochs > 500) {
			// update the solver parameters
			std::shared_ptr<SolverOp> solver;
			for (auto& weight_map : model.getWeightsMap())
				if (weight_map.second->getSolverOp()->getName() == "AdamOp")
					weight_map.second->getSolverOp()->setLearningRate(1e-4);
		}
		if (n_epochs % 50 == 0) {
			// save the model every 100 epochs
			ModelFile data;
			data.storeModelCsv(model.getName() + "_" + std::to_string(n_epochs) + "_nodes.csv",
				model.getName() + "_" + std::to_string(n_epochs) + "_links.csv",
				model.getName() + "_" + std::to_string(n_epochs) + "_weights.csv", model);
		}
	}
};

class DataSimulatorExt : public MNISTSimulator
{
public:
	void simulateEvaluationData(Eigen::Tensor<float, 4>& input_data, Eigen::Tensor<float, 3>& time_steps) {};
	void simulateTrainingData(Eigen::Tensor<float, 4>& input_data, Eigen::Tensor<float, 4>& output_data, Eigen::Tensor<float, 3>& time_steps)
	{
		// infer data dimensions based on the input tensors
		const int batch_size = input_data.dimension(0);
		const int memory_size = input_data.dimension(1);
		const int n_input_nodes = input_data.dimension(2);
		const int n_output_nodes = output_data.dimension(2);
		const int n_epochs = input_data.dimension(3);
		const int n_input_pixels = validation_data.dimension(1);
		const int n_encodings = 20; // not ideal to have this hard coded...

		assert(n_output_nodes == n_input_pixels + 2*n_encodings);
		assert(n_input_nodes == n_input_pixels + n_encodings);

		// make the start and end sample indices [BUG FREE]
		mnist_sample_start_training = mnist_sample_end_training;
		mnist_sample_end_training = mnist_sample_start_training + batch_size*n_epochs;
		if (mnist_sample_end_training > training_data.dimension(0) - 1)
			mnist_sample_end_training = mnist_sample_end_training - batch_size*n_epochs;

		// make a vector of sample_indices [BUG FREE]
		std::vector<int> sample_indices;
		for (int i = 0; i<batch_size*n_epochs; ++i)
		{
			int sample_index = i + mnist_sample_start_training;
			if (sample_index > training_data.dimension(0) - 1)
			{
				sample_index = sample_index - batch_size*n_epochs;
			}
			sample_indices.push_back(sample_index);
		}

		std::random_device rd{};
		std::mt19937 gen{ rd() };
		std::normal_distribution<> d{ 0.0f, 1.0f };
		// Reformat the MNIST image data for training
		for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
			for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
				for (int nodes_iter = 0; nodes_iter < n_input_pixels + 2*n_encodings; ++nodes_iter) {
					for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {
						if (nodes_iter < n_input_pixels) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = training_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = training_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							//output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = training_data(sample_indices[0], nodes_iter); // test on only 1 sample
							//input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = training_data(sample_indices[0], nodes_iter);  // test on only 1 sample
						}
						else if (nodes_iter >= n_input_pixels && nodes_iter < n_encodings) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = d(gen); // sample from a normal distribution
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // Dummy data for KL divergence mu
						}
						else {
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // Dummy data for KL divergence logvar
						}
					}
				}
			}
		}

		time_steps.setConstant(1.0f);
	}
	void simulateValidationData(Eigen::Tensor<float, 4>& input_data, Eigen::Tensor<float, 4>& output_data, Eigen::Tensor<float, 3>& time_steps)
	{
		// infer data dimensions based on the input tensors
		const int batch_size = input_data.dimension(0);
		const int memory_size = input_data.dimension(1);
		const int n_input_nodes = input_data.dimension(2);
		const int n_output_nodes = output_data.dimension(2);
		const int n_epochs = input_data.dimension(3);
		const int n_input_pixels = validation_data.dimension(1);
		const int n_encodings = 20; // not ideal to have this hard coded...

		assert(n_output_nodes == n_input_pixels + 2 * n_encodings);
		assert(n_input_nodes == n_input_pixels + n_encodings);

		// make the start and end sample indices [BUG FREE]
		mnist_sample_start_training = mnist_sample_end_training;
		mnist_sample_end_training = mnist_sample_start_training + batch_size * n_epochs;
		if (mnist_sample_end_training > training_data.dimension(0) - 1)
			mnist_sample_end_training = mnist_sample_end_training - batch_size * n_epochs;

		// make a vector of sample_indices [BUG FREE]
		std::vector<int> sample_indices;
		for (int i = 0; i < batch_size*n_epochs; ++i)
		{
			int sample_index = i + mnist_sample_start_training;
			if (sample_index > training_data.dimension(0) - 1)
			{
				sample_index = sample_index - batch_size * n_epochs;
			}
			sample_indices.push_back(sample_index);
		}

		// Reformat the MNIST image data for training
		for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
			for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
				for (int nodes_iter = 0; nodes_iter < n_input_pixels + 2 * n_encodings; ++nodes_iter) {
					for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {
						if (nodes_iter < n_input_pixels) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = validation_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = validation_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							//output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = validation_data(sample_indices[0], nodes_iter); // test on only 1 sample
							//input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = validation_data(sample_indices[0], nodes_iter);  // test on only 1 sample
						}
						else if (nodes_iter >= n_input_pixels && nodes_iter < n_encodings) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // sample from a normal distribution
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // Dummy data for KL divergence mu
						}
						else {
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // Dummy data for KL divergence logvar
						}
					}
				}
			}
		}
		time_steps.setConstant(1.0f);
	}
};

class ModelReplicatorExt : public ModelReplicator
{
public:
	void adaptiveReplicatorScheduler(
		const int& n_generations,
		std::vector<Model>& models,
		std::vector<std::vector<std::pair<int, float>>>& models_errors_per_generations)
	{
		if (n_generations > 100)
		{
			setNNodeAdditions(1);
			setNLinkAdditions(2);
			setNNodeDeletions(1);
			setNLinkDeletions(2);
		}
		else if (n_generations > 1 && n_generations < 100)
		{
			setNNodeAdditions(1);
			setNLinkAdditions(2);
			setNNodeDeletions(1);
			setNLinkDeletions(2);
		}
		else if (n_generations == 0)
		{
			setNNodeAdditions(10);
			setNLinkAdditions(20);
			setNNodeDeletions(0);
			setNLinkDeletions(0);
		}
	}
};

class PopulationTrainerExt : public PopulationTrainer
{
public:
	void adaptivePopulationScheduler(
		const int& n_generations,
		std::vector<Model>& models,
		std::vector<std::vector<std::pair<int, float>>>& models_errors_per_generations)
	{
		// Population size of 16
		if (n_generations == 0)
		{
			setNTop(3);
			setNRandom(3);
			setNReplicatesPerModel(15);
		}
		else
		{
			setNTop(3);
			setNRandom(3);
			setNReplicatesPerModel(3);
		}
	}
};

void main_VAE() {

	const int n_hard_threads = std::thread::hardware_concurrency();

	// define the populatin trainer
	PopulationTrainerExt population_trainer;
	population_trainer.setNGenerations(1);
	population_trainer.setNTop(1);
	population_trainer.setNRandom(1);
	population_trainer.setNReplicatesPerModel(1);

	// define the model logger
	ModelLogger model_logger(true, true, true, false, false, false, false, false);

	// define the data simulator
	const std::size_t input_size = 784;
	const std::size_t encoding_size = 20;
	const std::size_t training_data_size = 10000; //60000;
	const std::size_t validation_data_size = 100; //10000;
	DataSimulatorExt data_simulator;

	// read in the training data
	//const std::string training_data_filename = "C:/Users/domccl/GitHub/mnist/train-images.idx3-ubyte";
	//const std::string training_labels_filename = "C:/Users/domccl/GitHub/mnist/train-labels.idx1-ubyte";
	//const std::string training_data_filename = "C:/Users/dmccloskey/Documents/GitHub/mnist/train-images-idx3-ubyte";
	//const std::string training_labels_filename = "C:/Users/dmccloskey/Documents/GitHub/mnist/train-labels-idx1-ubyte";
	const std::string training_data_filename = "/home/user/data/train-images-idx3-ubyte";
	const std::string training_labels_filename = "/home/user/data/train-labels-idx1-ubyte";
	data_simulator.readData(training_data_filename, training_labels_filename, true, training_data_size, input_size);

	// read in the validation data
	//const std::string validation_data_filename = "C:/Users/domccl/GitHub/mnist/t10k-images.idx3-ubyte";
	//const std::string validation_labels_filename = "C:/Users/domccl/GitHub/mnist/t10k-labels.idx1-ubyte";
	//const std::string validation_data_filename = "C:/Users/dmccloskey/Documents/GitHub/mnist/t10k-images-idx3-ubyte";
	//const std::string validation_labels_filename = "C:/Users/dmccloskey/Documents/GitHub/mnist/t10k-labels-idx1-ubyte";
	const std::string validation_data_filename = "/home/user/data/t10k-images-idx3-ubyte";
	const std::string validation_labels_filename = "/home/user/data/t10k-labels-idx1-ubyte";
	data_simulator.readData(validation_data_filename, validation_labels_filename, false, validation_data_size, input_size);
	data_simulator.unitScaleData();

	// Make the input nodes
	std::vector<std::string> input_nodes;
	for (int i = 0; i < input_size; ++i)
		input_nodes.push_back("Input_" + std::to_string(i));

	// Make the encoding nodes and add them to the input
	for (int i = 0; i < encoding_size; ++i)
		input_nodes.push_back("Encoding_" + std::to_string(i) + "-Sampler");

	// Make the output nodes
	std::vector<std::string> output_nodes;
	for (int i = 0; i < input_size; ++i)
		output_nodes.push_back("Output_" + std::to_string(i));

	// Make the mu nodes
	std::vector<std::string> encoding_nodes_mu;
	for (int i = 0; i < encoding_size; ++i)
		encoding_nodes_mu.push_back("Mu_" + std::to_string(i));

	// Make the encoding nodes
	std::vector<std::string> encoding_nodes_logvar;
	for (int i = 0; i < encoding_size; ++i)
		encoding_nodes_logvar.push_back("LogVar_" + std::to_string(i));

	// define the model trainer
	ModelTrainerExt model_trainer;
	model_trainer.setBatchSize(8);
	model_trainer.setMemorySize(1);
	model_trainer.setNEpochsTraining(5000);
	model_trainer.setNEpochsValidation(10);
	model_trainer.setVerbosityLevel(1);
	model_trainer.setNThreads(n_hard_threads);
	model_trainer.setLogging(true, false);
	model_trainer.setLossFunctions({ 
		std::shared_ptr<LossFunctionOp<float>>(new MSEOp<float>()),
		//std::shared_ptr<LossFunctionOp<float>>(new BCEWithLogitsOp<float>()),
		std::shared_ptr<LossFunctionOp<float>>(new KLDivergenceMuOp<float>()),
		std::shared_ptr<LossFunctionOp<float>>(new KLDivergenceLogVarOp<float>()) });
	model_trainer.setLossFunctionGrads({ 
		std::shared_ptr<LossFunctionGradOp<float>>(new MSEGradOp<float>()),
		//std::shared_ptr<LossFunctionGradOp<float>>(new BCEWithLogitsGradOp<float>()),
		std::shared_ptr<LossFunctionGradOp<float>>(new KLDivergenceMuGradOp<float>()), 
		std::shared_ptr<LossFunctionGradOp<float>>(new KLDivergenceLogVarGradOp<float>()) });
	model_trainer.setOutputNodes({ output_nodes, encoding_nodes_mu, encoding_nodes_logvar });

	// define the model replicator for growth mode
	ModelReplicatorExt model_replicator;

	// define the initial population [BUG FREE]
	std::cout << "Initializing the population..." << std::endl;
	std::vector<Model> population = { model_trainer.makeVAE(input_size, encoding_size) };

	// Evolve the population
	std::vector<std::vector<std::pair<int, float>>> models_validation_errors_per_generation = population_trainer.evolveModels(
		population, model_trainer, model_replicator, data_simulator, model_logger, input_nodes, 1);

	PopulationTrainerFile population_trainer_file;
	population_trainer_file.storeModels(population, "MNIST");
	population_trainer_file.storeModelValidations("MNISTErrors.csv", models_validation_errors_per_generation.back());
}

int main(int argc, char** argv)
{
	// run the application
	main_VAE();

  return 0;
}
/**TODO:  Add copyright*/

#include <SmartPeak/ml/PopulationTrainerDefaultDevice.h>
#include <SmartPeak/ml/ModelTrainerDefaultDevice.h>
#include <SmartPeak/ml/ModelReplicator.h>
#include <SmartPeak/ml/ModelBuilder.h>
#include <SmartPeak/ml/Model.h>
#include <SmartPeak/io/PopulationTrainerFile.h>
#include <SmartPeak/io/ModelFile.h>

#include <SmartPeak/simulator/MNISTSimulator.h>

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
template<typename TensorT>
class ModelTrainerExt : public ModelTrainerDefaultDevice<TensorT>
{
public:
	/*
	@brief AAE variation with	Xavier initialization

	n_hidden = 1000
	n_encodings = 2

	Notes: 
	Model input nodes: "Input_0, Input_1, ... Input_784" up to n_inputs
	Model encoding input nodes: "Encoding_0, Encoding_1, ... Encoding 20" up to n_encodings
	Model output nodes: "Output_0, Output_1, ... Output_784" up to n_inputs

	References:
	Alireza Makhzani, Jonathon Shlens, Navdeep Jaitly, Ian Goodfellow, Brendan Frey. "Adversarial Autoencoders" 2015.  arXiv:1511.05644
	https://github.com/musyoku/adversarial-autoencoder/blob/master/run/semi-supervised/regularize_z/model.py
	*/
	Model<TensorT> makeAAELatentZ(const int& n_inputs, int n_hidden_0 = 50, int n_encodings = 2) {
		Model<TensorT> model;
		model.setId(0);
		model.setName("AAELatentZ");

		ModelBuilder<TensorT> model_builder;

		// Add the inputs
		std::vector<std::string> node_names_input = model_builder.addInputNodes(model, "Input", n_inputs);

		// Add the Endoder FC layers
		std::vector<std::string> node_names, node_names_z, node_names_logvar;	
		node_names = model_builder.addFullyConnected(model, "EC0", "EC0", node_names_input, n_hidden_0,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names_input.size() + node_names.size())/2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "EC1", "EC1", node_names, n_hidden_0,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names.size() + node_names.size()) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names_z = model_builder.addFullyConnected(model, "LatentZ", "LatentZ", node_names, n_encodings,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names.size() + n_encodings)/2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);

		// Add the Discriminator Layer
		node_names = model_builder.addDiscriminator(model, "DS", "DS", node_names_z);

		// Add the Decoder FC layers
		node_names = model_builder.addFullyConnected(model, "DE0", "DE0", node_names_z, n_hidden_0,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names_z.size() + n_hidden_0)/2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "DE1", "DE1", node_names, n_hidden_0,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names.size() + n_hidden_0) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "DE-Output", "DE-Output", node_names, n_inputs,
			std::shared_ptr<ActivationOp<TensorT>>(new SigmoidOp<TensorT>()),
			std::shared_ptr<ActivationOp<TensorT>>(new SigmoidGradOp<TensorT>()),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>(node_names.size(), 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);

		// Specify the output node types manually
		// [NOTE: we do not specify the Encoding node types as outputs so that they remain "active" after CETT
		//			  so that back propogation begins at the decoder output and is propogated through the encoder layers
		for (const std::string& node_name : node_names)
			model.getNodesMap().at(node_name)->setType(NodeType::output);

		return model;
	}
	Model<TensorT> makeModel() { return Model<TensorT>(); }
	void adaptiveTrainerScheduler(
		const int& n_generations,
		const int& n_epochs,
		Model<TensorT>& model,
		ModelInterpreterDefaultDevice<TensorT>& model_interpreter,
		const std::vector<float>& model_errors) {
		if (n_epochs > 200) {
			// update the solver parameters
			std::shared_ptr<SolverOp<TensorT>> solver;
			for (auto& weight_map : model.getWeightsMap())
				if (weight_map.second->getSolverOp()->getName() == "AdamOp")
					weight_map.second->getSolverOp()->setLearningRate(1e-4);
		}
		if (n_epochs % 50 == 0) {
			// save the model every 100 epochs
			ModelFile<TensorT> data;
			data.storeModelCsv(model.getName() + "_" + std::to_string(n_epochs) + "_nodes.csv",
				model.getName() + "_" + std::to_string(n_epochs) + "_links.csv",
				model.getName() + "_" + std::to_string(n_epochs) + "_weights.csv", model);
		}
	}
};

template<typename TensorT>
class DataSimulatorExt : public MNISTSimulator<TensorT>
{
public:
	/*
	GAN training data:
	time-step 0: n_encodings and n_labels values are equal to 0
	time-step 1: input pixels are equal to 0
	*/
	void simulateTrainingData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
	{
		// infer data dimensions based on the input tensors
		const int batch_size = input_data.dimension(0);
		const int memory_size = input_data.dimension(1);
		const int n_input_nodes = input_data.dimension(2);
		const int n_output_nodes = output_data.dimension(2);
		const int n_epochs = input_data.dimension(3);
		const int n_input_pixels = this->validation_data.dimension(1);
		const int n_encodings = 2; // not ideal to have this hard coded...

		assert(n_output_nodes == n_input_pixels + n_encodings);
		assert(n_input_nodes == n_input_pixels + n_encodings);

		// make a vector of sample_indices [BUG FREE]
		Eigen::Tensor<int, 1> sample_indices = this->getTrainingIndices(batch_size, n_epochs);

		// Gaussian noise
		std::random_device rd{};
		std::mt19937 gen{ rd() };
		std::normal_distribution<> d{ 0.0f, 0.3f };

		// Reformat the MNIST image data for training
		for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
			for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
				for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {

					// Mixed Gaussian sampler
					Eigen::Tensor<float, 1> mixed_gaussian = GaussianMixture<float>(n_encodings, this->training_labels.size(), sample_indices[epochs_iter*batch_size + batch_iter]);
					//Eigen::Tensor<float, 1> mixed_gaussian = GaussianMixture<float>(n_encodings, this->training_labels.size(), sample_indices[0]); // test on only 1 sample

					for (int nodes_iter = 0; nodes_iter < n_input_pixels + n_encodings; ++nodes_iter) {
						if (nodes_iter < n_input_pixels) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							//output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[0], nodes_iter); // test on only 1 sample
							//input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[0], nodes_iter);  // test on only 1 sample
						}
						else {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = d(gen) + mixed_gaussian(nodes_iter - n_input_pixels); // sampler distribution + noise
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // expected value if distributions match
						}
					}
				}
			}
		}

		time_steps.setConstant(1.0f);
	}
	void simulateValidationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
	{
		// infer data dimensions based on the input tensors
		const int batch_size = input_data.dimension(0);
		const int memory_size = input_data.dimension(1);
		const int n_input_nodes = input_data.dimension(2);
		const int n_output_nodes = output_data.dimension(2);
		const int n_epochs = input_data.dimension(3);
		const int n_input_pixels = this->validation_data.dimension(1);
		const int n_encodings = 2; // not ideal to have this hard coded...

		assert(n_output_nodes == n_input_pixels + n_encodings);
		assert(n_input_nodes == n_input_pixels + n_encodings);

		// make the start and end sample indices [BUG FREE]
		Eigen::Tensor<int, 1> sample_indices = this->getValidationIndices(batch_size, n_epochs);

		// Gaussian noise
		std::random_device rd{};
		std::mt19937 gen{ rd() };
		std::normal_distribution<> d{ 0.0f, 0.3f };

		// Reformat the MNIST image data for training
		for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
			for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
				for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {

					// Mixed Gaussian sampler
					Eigen::Tensor<float, 1> mixed_gaussian = GaussianMixture<float>(n_encodings, this->validation_labels.size(), sample_indices[epochs_iter*batch_size + batch_iter]);
					//Eigen::Tensor<float, 1> mixed_gaussian = GaussianMixture<float>(n_encodings, this->validation_labels.size(), sample_indices[0]); // test on only 1 sample

					for (int nodes_iter = 0; nodes_iter < n_input_pixels + n_encodings; ++nodes_iter) {
						if (nodes_iter < n_input_pixels) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->validation_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->validation_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							//output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->validation_data(sample_indices[0], nodes_iter); // test on only 1 sample
							//input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->validation_data(sample_indices[0], nodes_iter);  // test on only 1 sample
						}
						else {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = d(gen) + mixed_gaussian(nodes_iter - n_input_pixels); // sampler distribution + noise
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // expected value if distributions match
						}
					}
				}
			}
		}
		time_steps.setConstant(1.0f);
	}
	void simulateEvaluationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 3>& time_steps)
	{
		// infer data dimensions based on the input tensors
		const int batch_size = input_data.dimension(0);
		const int memory_size = input_data.dimension(1);
		const int n_input_nodes = input_data.dimension(2);
		const int n_epochs = input_data.dimension(3);
		const int n_input_pixels = this->validation_data.dimension(1);
		const int n_encodings = 2; // not ideal to have this hard coded...

		assert(n_input_nodes == n_input_pixels + n_encodings);

		// make the start and end sample indices [BUG FREE]
		this->mnist_sample_start_training = this->mnist_sample_end_training;
		this->mnist_sample_end_training = this->mnist_sample_start_training + batch_size * n_epochs;
		if (this->mnist_sample_end_training > this->training_data.dimension(0) - 1)
			this->mnist_sample_end_training = this->mnist_sample_end_training - batch_size * n_epochs;

		// make a vector of sample_indices [BUG FREE]
		std::vector<int> sample_indices;
		for (int i = 0; i < batch_size*n_epochs; ++i)
		{
			int sample_index = i + this->mnist_sample_start_training;
			if (sample_index > this->training_data.dimension(0) - 1)
			{
				sample_index = sample_index - batch_size * n_epochs;
			}
			sample_indices.push_back(sample_index);
		}

		// Reformat the MNIST image data for training
		for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
			for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
				for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {
					for (int nodes_iter = 0; nodes_iter < n_input_pixels + n_encodings; ++nodes_iter) {
						if (nodes_iter < n_input_pixels) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							//input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[0], nodes_iter);  // test on only 1 sample
						}
						else {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // sampler distribution + noise
						}
					}
				}
			}
		}
		time_steps.setConstant(1.0f);
	}
};

template<typename TensorT>
class ModelReplicatorExt : public ModelReplicator<TensorT>
{
public:
	void adaptiveReplicatorScheduler(
		const int& n_generations,
		std::vector<Model<TensorT>>& models,
		std::vector<std::vector<std::tuple<int, std::string, TensorT>>>& models_errors_per_generations)
	{
		if (n_generations > 100)
		{
			this->setNNodeAdditions(1);
			this->setNLinkAdditions(2);
			this->setNNodeDeletions(1);
			this->setNLinkDeletions(2);
		}
		else if (n_generations > 1 && n_generations < 100)
		{
			this->setNNodeAdditions(1);
			this->setNLinkAdditions(2);
			this->setNNodeDeletions(1);
			this->setNLinkDeletions(2);
		}
		else if (n_generations == 0)
		{
			this->setNNodeAdditions(10);
			this->setNLinkAdditions(20);
			this->setNNodeDeletions(0);
			this->setNLinkDeletions(0);
		}
	}
};

template<typename TensorT>
class PopulationTrainerExt : public PopulationTrainerDefaultDevice<TensorT>
{
public:
	void adaptivePopulationScheduler(
		const int& n_generations,
		std::vector<Model<TensorT>>& models,
		std::vector<std::vector<std::tuple<int, std::string, TensorT>>>& models_errors_per_generations)
	{
		// Population size of 16
		if (n_generations == 0)
		{
			this->setNTop(3);
			this->setNRandom(3);
			this->setNReplicatesPerModel(15);
		}
		else
		{
			this->setNTop(3);
			this->setNRandom(3);
			this->setNReplicatesPerModel(3);
		}
	}
};

void main_AAELatentZTrain() {

	const int n_hard_threads = std::thread::hardware_concurrency();

	// define the populatin trainer
	PopulationTrainerExt<float> population_trainer;
	population_trainer.setNGenerations(1);
	population_trainer.setNTop(1);
	population_trainer.setNRandom(1);
	population_trainer.setNReplicatesPerModel(1);

	// define the model logger
	ModelLogger<float> model_logger(true, true, true, false, false, false, false, false);

	// define the data simulator
	const std::size_t input_size = 784;
	const std::size_t encoding_size = 2;
	const std::size_t hidden_size = 512;
	const std::size_t training_data_size = 60000; //60000;
	const std::size_t validation_data_size = 1000; //10000;
	DataSimulatorExt<float> data_simulator;

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
		input_nodes.push_back("DS-Sampler-" + std::to_string(i));

	// Make the output nodes
	std::vector<std::string> decoder_output_nodes;
	for (int i = 0; i < input_size; ++i)
		decoder_output_nodes.push_back("DE-Output_" + std::to_string(i));

	// Make the output nodes
	std::vector<std::string> discriminator_output_nodes;
	for (int i = 0; i < encoding_size; ++i)
		discriminator_output_nodes.push_back("DS-Output-" + std::to_string(i));

	// define the model trainers and resources for the trainers
	std::vector<ModelInterpreterDefaultDevice<float>> model_interpreters;
	for (size_t i = 0; i < n_hard_threads; ++i) {
		ModelResources model_resources = { ModelDevice(0, 1) };
		ModelInterpreterDefaultDevice<float> model_interpreter(model_resources);
		model_interpreters.push_back(model_interpreter);
	}
	ModelTrainerExt<float> model_trainer;
	model_trainer.setBatchSize(8);
	model_trainer.setMemorySize(1);
	model_trainer.setNEpochsTraining(5000);
	model_trainer.setNEpochsValidation(50);
	model_trainer.setVerbosityLevel(1);
	model_trainer.setLogging(true, false);
	model_trainer.setLossFunctions({
		std::shared_ptr<LossFunctionOp<float>>(new MSEOp<float>()),
		std::shared_ptr<LossFunctionOp<float>>(new MSEOp<float>()) });
	model_trainer.setLossFunctionGrads({
		std::shared_ptr<LossFunctionGradOp<float>>(new MSEGradOp<float>()),
		std::shared_ptr<LossFunctionGradOp<float>>(new MSEGradOp<float>()) });
	model_trainer.setOutputNodes({ decoder_output_nodes, discriminator_output_nodes });

	// define the model replicator for growth mode
	ModelReplicatorExt<float> model_replicator;

	// define the initial population [BUG FREE]
	std::cout << "Initializing the population..." << std::endl;
	std::vector<Model<float>> population = { ModelTrainerExt<float>().makeAAELatentZ(input_size, hidden_size, encoding_size) };

	// Evolve the population
	std::vector<std::vector<std::tuple<int, std::string, float>>> models_validation_errors_per_generation = population_trainer.evolveModels(
		population, model_trainer, model_interpreters, model_replicator, data_simulator, model_logger, input_nodes);

	PopulationTrainerFile<float> population_trainer_file;
	population_trainer_file.storeModels(population, "MNIST");
	population_trainer_file.storeModelValidations("MNISTErrors.csv", models_validation_errors_per_generation.back());
}

void main_AAELatentZEvaluate() {
	const int n_hard_threads = std::thread::hardware_concurrency();

	// define the populatin trainer
	PopulationTrainerExt<float> population_trainer;
	population_trainer.setNGenerations(1);
	population_trainer.setNTop(1);
	population_trainer.setNRandom(1);
	population_trainer.setNReplicatesPerModel(1);

	// define the data simulator
	const std::size_t input_size = 784;
	const std::size_t encoding_size = 2;
	const std::size_t hidden_size = 25;
	const std::size_t training_data_size = 10000; //60000;
	const std::size_t validation_data_size = 100; //10000;
	DataSimulatorExt<float> data_simulator;

	// define the model logger
	ModelLogger<float> model_logger(false, false, false, false, false, false, true, false);

	// read in the training data
	const std::string training_data_filename = "C:/Users/domccl/GitHub/mnist/train-images.idx3-ubyte";
	const std::string training_labels_filename = "C:/Users/domccl/GitHub/mnist/train-labels.idx1-ubyte";
	//const std::string training_data_filename = "C:/Users/dmccloskey/Documents/GitHub/mnist/train-images-idx3-ubyte";
	//const std::string training_labels_filename = "C:/Users/dmccloskey/Documents/GitHub/mnist/train-labels-idx1-ubyte";
	//const std::string training_data_filename = "/home/user/data/train-images-idx3-ubyte";
	//const std::string training_labels_filename = "/home/user/data/train-labels-idx1-ubyte";
	data_simulator.readData(training_data_filename, training_labels_filename, true, training_data_size, input_size);

	// read in the validation data
	const std::string validation_data_filename = "C:/Users/domccl/GitHub/mnist/t10k-images.idx3-ubyte";
	const std::string validation_labels_filename = "C:/Users/domccl/GitHub/mnist/t10k-labels.idx1-ubyte";
	//const std::string validation_data_filename = "C:/Users/dmccloskey/Documents/GitHub/mnist/t10k-images-idx3-ubyte";
	//const std::string validation_labels_filename = "C:/Users/dmccloskey/Documents/GitHub/mnist/t10k-labels-idx1-ubyte";
	//const std::string validation_data_filename = "/home/user/data/t10k-images-idx3-ubyte";
	//const std::string validation_labels_filename = "/home/user/data/t10k-labels-idx1-ubyte";
	data_simulator.readData(validation_data_filename, validation_labels_filename, false, validation_data_size, input_size);
	data_simulator.unitScaleData();

	// Make the input nodes
	std::vector<std::string> input_nodes;
	for (int i = 0; i < input_size; ++i) {
		char name_char[512];
		sprintf(name_char, "Input_%012d", i);
		std::string name(name_char);
		input_nodes.push_back(name);
	}

	// Make the output nodes
	std::vector<std::string> decoder_output_nodes;
	for (int i = 0; i < input_size; ++i) {
		char name_char[512];
		sprintf(name_char, "DE-Output_%012d", i);
		std::string name(name_char);
		decoder_output_nodes.push_back(name);
	}

	// Make the output nodes
	std::vector<std::string> discriminator_output_nodes;
	for (int i = 0; i < encoding_size; ++i) {
		char name_char[512];
		sprintf(name_char, "DS-Output-%012d", i);
		std::string name(name_char);
		discriminator_output_nodes.push_back(name);
	}

	// define the model trainers and resources for the trainers
	std::vector<ModelInterpreterDefaultDevice<float>> model_interpreters;
	for (size_t i = 0; i < n_hard_threads; ++i) {
		ModelResources model_resources = { ModelDevice(0, 1) };
		ModelInterpreterDefaultDevice<float> model_interpreter(model_resources);
		model_interpreters.push_back(model_interpreter);
	}
	ModelTrainerExt<float> model_trainer;
	model_trainer.setBatchSize(1);
	model_trainer.setMemorySize(1);
	model_trainer.setNEpochsTraining(0);
	model_trainer.setNEpochsValidation(0);
	model_trainer.setNEpochsEvaluation(2);
	model_trainer.setVerbosityLevel(1);
	model_trainer.setLogging(false, false, true);
	model_trainer.setLossFunctions({
		std::shared_ptr<LossFunctionOp<float>>(new MSEOp<float>()),
		std::shared_ptr<LossFunctionOp<float>>(new MSEOp<float>()) });
	model_trainer.setLossFunctionGrads({
		std::shared_ptr<LossFunctionGradOp<float>>(new MSEGradOp<float>()),
		std::shared_ptr<LossFunctionGradOp<float>>(new MSEGradOp<float>()) });
	model_trainer.setOutputNodes({ decoder_output_nodes, discriminator_output_nodes });

	// define the model replicator for growth mode
	ModelReplicatorExt<float> model_replicator;

	// read in the trained model
	std::cout << "Reading in the model..." << std::endl;
	const std::string data_dir = "C:/Users/domccl/GitHub/smartPeak_cpp/build_win/bin/Debug/";
	const std::string nodes_filename = data_dir + "0_MNIST_Nodes.csv";
	const std::string links_filename = data_dir + "0_MNIST_Links.csv";
	const std::string weights_filename = data_dir + "0_MNIST_Weights.csv";
	Model<float> model;
	model.setId(1);
	model.setName("AAELatentZ");

	ModelFile<float> model_file;
	model_file.loadModelCsv(nodes_filename, links_filename, weights_filename, model);
	
	std::vector<Model<float>> population = { model };

	// evaluate the trained model
	std::cout << "Evaluating the model..." << std::endl;
	population_trainer.evaluateModels(
		population, model_trainer, model_interpreters, model_replicator, data_simulator, model_logger, input_nodes);
}

int main(int argc, char** argv)
{
	// run the application
	main_AAELatentZTrain();
	//main_AAELatentZEvaluate();

  return 0;
}
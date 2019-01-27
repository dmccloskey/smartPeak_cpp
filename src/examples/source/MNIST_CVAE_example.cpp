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

 // Extended 
template<typename TensorT>
class ModelTrainerExt : public ModelTrainerDefaultDevice<TensorT>
{
public:
	/*
	@brief Basic VAE with	Xavier initialization

	Notes:
	Model input nodes: "Input_0, Input_1, ... Input_784" up to n_inputs
	Model encoding input nodes: "Encoding_0, Encoding_1, ... Encoding 64" up to n_encodings
	Model output nodes: "Output_0, Output_1, ... Output_784" up to n_inputs

	References:
	*/
	void makeCVAE(Model<TensorT>& model, int n_inputs = 784, int n_categorical = 10, int n_encodings = 64, int n_hidden_0 = 512) {
		model.setId(0);
		model.setName("VAE");

		ModelBuilder<TensorT> model_builder;

		// Add the inputs
		std::vector<std::string> node_names_input = model_builder.addInputNodes(model, "Input", "Input", n_inputs);

		// Add the Endocer FC layers
		std::vector<std::string> node_names, node_names_mu, node_names_logvar, node_names_logalpha;
		node_names = model_builder.addFullyConnected(model, "EN0", "EN0", node_names_input, n_hidden_0,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names_input.size() + node_names.size()) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "EN1", "EN1", node_names, n_hidden_0,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names.size() + node_names.size()) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names_mu = model_builder.addFullyConnected(model, "Mu", "Mu", node_names, n_encodings,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names.size() + n_encodings) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names_logvar = model_builder.addFullyConnected(model, "LogVar", "LogVar", node_names, n_encodings,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names.size() + n_encodings) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names_logalpha = model_builder.addFullyConnected(model, "LogAlpha", "LogAlpha", node_names, n_categorical,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names.size() + n_categorical) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);

		// Add the Encoding layers
		std::vector<std::string> node_names_Gencoder = model_builder.addGaussianEncoding(model, "Gaussian_encoding", "Gaussian_encoding", node_names_mu, node_names_logvar, true);
		std::vector<std::string> node_names_Cencoder = model_builder.addCategoricalEncoding(model, "Categorical_encoding", "Categorical_encoding", node_names_logalpha, true);

		// Add the Decoder FC layers
		node_names = model_builder.addFullyConnected(model, "DE0", "DE0", node_names_Gencoder, n_hidden_0,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names_Gencoder.size() + n_hidden_0) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		model_builder.addFullyConnected(model, "DE0", node_names_Cencoder, node_names,
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names_Cencoder.size() + n_hidden_0) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "DE1", "DE1", node_names, n_hidden_0,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
			std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()),
			std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()),
			std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()),
			std::shared_ptr<WeightInitOp<TensorT>>(new RandWeightInitOp<TensorT>((int)(node_names.size() + n_hidden_0) / 2, 1)),
			std::shared_ptr<SolverOp<TensorT>>(new AdamOp<TensorT>(0.001, 0.9, 0.999, 1e-8)), 0.0f, 0.0f);
		node_names = model_builder.addFullyConnected(model, "Output", "Output", node_names, n_inputs,
			std::shared_ptr<ActivationOp<TensorT>>(new ELUOp<TensorT>(1.0)),
			std::shared_ptr<ActivationOp<TensorT>>(new ELUGradOp<TensorT>(1.0)),
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
	}
	Model<TensorT> makeModel() { return Model<TensorT>(); }
	void adaptiveTrainerScheduler(
		const int& n_generations,
		const int& n_epochs,
		Model<TensorT>& model,
		ModelInterpreterDefaultDevice<TensorT>& model_interpreter,
		const std::vector<float>& model_errors) {
		//if (n_epochs = 1000) {
		//	// anneal the learning rate to 1e-4
		//}
		//if (n_epochs % 1000 == 0 && n_epochs != 0) {
		//	// save the model every 1000 epochs
		//	ModelFile<TensorT> data;
		//	data.storeModelCsv(model.getName() + "_" + std::to_string(n_epochs) + "_nodes.csv",
		//		model.getName() + "_" + std::to_string(n_epochs) + "_links.csv",
		//		model.getName() + "_" + std::to_string(n_epochs) + "_weights.csv", model);
		//}
	}
};

template<typename TensorT>
class DataSimulatorExt : public MNISTSimulator<TensorT>
{
public:
	void simulateEvaluationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 3>& time_steps) {
		// infer data dimensions based on the input tensors
		const int batch_size = input_data.dimension(0);
		const int memory_size = input_data.dimension(1);
		const int n_input_nodes = input_data.dimension(2);
		const int n_epochs = input_data.dimension(3);
		const int n_input_pixels = this->validation_data.dimension(1);
		const int n_encodings = 64; // not ideal to have this hard coded...
		const int n_categorical = 10;

		assert(n_input_nodes == n_input_pixels + n_encodings + 2 * n_categorical); // Guassian sampler, Gumbel sampler, inverse tau

		// make a vector of sample_indices [BUG FREE]
		Eigen::Tensor<int, 1> sample_indices = this->getTrainingIndices(batch_size, n_epochs);

		std::random_device rd{};
		std::mt19937 gen{ rd() };
		std::normal_distribution<> d{ 0.0f, 1.0f };
		// Reformat the MNIST image data for training
		for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
			for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
				for (int nodes_iter = 0; nodes_iter < n_input_pixels + 2 * n_encodings; ++nodes_iter) {
					for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {
						if (nodes_iter < n_input_pixels) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							//input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[0], nodes_iter);  // test on only 1 sample
						}
						else if (nodes_iter >= n_input_pixels && nodes_iter < n_encodings) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0;// d(gen); // sample from a normal distribution
						}
					}
				}
			}
		}

		time_steps.setConstant(1.0f);
	};
	void simulateTrainingData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
	{
		// infer data dimensions based on the input tensors
		const int batch_size = input_data.dimension(0);
		const int memory_size = input_data.dimension(1);
		const int n_input_nodes = input_data.dimension(2);
		const int n_output_nodes = output_data.dimension(2);
		const int n_epochs = input_data.dimension(3);
		const int n_input_pixels = this->validation_data.dimension(1);
		const int n_encodings = 64; // not ideal to have this hard coded...
		const int n_categorical = 10;

		assert(n_output_nodes == n_input_pixels + 2 * n_encodings + n_categorical); // mu, logvar, logalpha
		assert(n_input_nodes == n_input_pixels + n_encodings + 2 * n_categorical); // Guassian sampler, Gumbel sampler, inverse tau

		// make a vector of sample_indices [BUG FREE]
		Eigen::Tensor<int, 1> sample_indices = this->getTrainingIndices(batch_size, n_epochs);

		// Reformat the MNIST image data for training
		for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
			for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
				for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {
					// Input and Output: Pixels
					for (int nodes_iter = 0; nodes_iter < n_input_pixels; ++nodes_iter) {
						input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
						output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
						//output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[0], nodes_iter); // test on only 1 sample
						//input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->training_data(sample_indices[0], nodes_iter);  // test on only 1 sample
					}

					// Gaussian Sampler
					Eigen::Tensor<TensorT, 2> gaussian_samples = GaussianSampler<TensorT>(1, n_encodings);

					// Input and Output: encodings
					for (int nodes_iter = 0; nodes_iter < n_encodings; ++nodes_iter) {
						input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = gaussian_samples(0, nodes_iter); // sample from a normal distribution
						output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0; // Dummy data for KL divergence mu
						output_data(batch_iter, memory_iter, n_encodings + nodes_iter, epochs_iter) = 0; // Dummy data for KL divergence logvar
					}

					// Concrete Sampler
					Eigen::Tensor<TensorT, 2> categorical_samples = GumbelSampler<TensorT>(1, n_categorical);
					TensorT inverse_tau = 1.0 / 0.5; // Madison 2017 recommended 2/3 for tau

					// Input and Output: categorical
					for (int nodes_iter = 0; nodes_iter < n_categorical; ++nodes_iter) {
						input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = categorical_samples(0, nodes_iter); // sample from gumbel distribution
						input_data(batch_iter, memory_iter, n_categorical + nodes_iter, epochs_iter) = inverse_tau; // inverse tau
						output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = (TensorT)this->training_labels(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter); // Expected label
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
		const int n_encodings = 64; // not ideal to have this hard coded...
		const int n_categorical = 10;

		assert(n_output_nodes == n_input_pixels + 2 * n_encodings + n_categorical); // mu, logvar, logalpha
		assert(n_input_nodes == n_input_pixels + n_encodings + 2 * n_categorical); // Guassian sampler, Gumbel sampler, inverse tau

		// make a vector of sample_indices [BUG FREE]
		Eigen::Tensor<int, 1> sample_indices = this->getValidationIndices(batch_size, n_epochs);

		// Reformat the MNIST image data for training
		for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
			for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
				for (int nodes_iter = 0; nodes_iter < n_input_pixels + 2 * n_encodings; ++nodes_iter) {
					for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {
						if (nodes_iter < n_input_pixels) {
							input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->validation_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->validation_data(sample_indices[epochs_iter*batch_size + batch_iter], nodes_iter);
							//output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->validation_data(sample_indices[0], nodes_iter); // test on only 1 sample
							//input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = this->validation_data(sample_indices[0], nodes_iter);  // test on only 1 sample
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
			this->setNNodeDownAdditions(1);
			this->setNLinkAdditions(2);
			this->setNNodeDeletions(1);
			this->setNLinkDeletions(2);
		}
		else if (n_generations > 1 && n_generations < 100)
		{
			this->setNNodeDownAdditions(1);
			this->setNLinkAdditions(2);
			this->setNNodeDeletions(1);
			this->setNLinkDeletions(2);
		}
		else if (n_generations == 0)
		{
			this->setNNodeDownAdditions(10);
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

void main_VAE(const bool& make_model, const bool& load_weight_values, const bool& train_model) {

	const int n_hard_threads = std::thread::hardware_concurrency();
	const int n_threads = 1;

	// define the populatin trainer
	PopulationTrainerExt<float> population_trainer;
	population_trainer.setNGenerations(1);
	population_trainer.setNTop(1);
	population_trainer.setNRandom(1);
	population_trainer.setNReplicatesPerModel(1);

	// define the model logger
	ModelLogger<float> model_logger(true, true, false, false, false, false, false, false);

	// define the data simulator
	const std::size_t input_size = 784;
	const std::size_t encoding_size = 64;
	const std::size_t categorical_size = 10;
	const std::size_t n_hidden = 256;
	const std::size_t training_data_size = 60000; //60000;
	const std::size_t validation_data_size = 10000; //10000;
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
	for (int i = 0; i < input_size; ++i) {
		char name_char[512];
		sprintf(name_char, "Input_%012d", i);
		std::string name(name_char);
		input_nodes.push_back(name);
	}

	// Make the encoding nodes and add them to the input
	for (int i = 0; i < encoding_size; ++i) {
		char name_char[512];
		sprintf(name_char, "Gaussian_encoding_%012d-Sampler", i);
		std::string name(name_char);
		input_nodes.push_back(name);
	}
	for (int i = 0; i < categorical_size; ++i) {
		char name_char[512];
		sprintf(name_char, "Categorical_encoding_%012d-GumbelSampler", i);
		std::string name(name_char);
		input_nodes.push_back(name);
	}
	for (int i = 0; i < categorical_size; ++i) {
		char name_char[512];
		sprintf(name_char, "Categorical_encoding_%012d-InverseTau", i);
		std::string name(name_char);
		input_nodes.push_back(name);
	}

	// Make the output nodes
	std::vector<std::string> output_nodes;
	for (int i = 0; i < input_size; ++i) {
		char name_char[512];
		sprintf(name_char, "Output_%012d", i);
		std::string name(name_char);
		output_nodes.push_back(name);
	}

	// Make the mu nodes
	std::vector<std::string> encoding_nodes_mu;
	for (int i = 0; i < encoding_size; ++i) {
		char name_char[512];
		sprintf(name_char, "Mu_%012d", i);
		std::string name(name_char);
		encoding_nodes_mu.push_back(name);
	}

	// Make the encoding nodes
	std::vector<std::string> encoding_nodes_logvar;
	for (int i = 0; i < encoding_size; ++i) {
		char name_char[512];
		sprintf(name_char, "LogVar_%012d", i);
		std::string name(name_char);
		encoding_nodes_logvar.push_back(name);
	}

	// Make the encoding nodes
	std::vector<std::string> encoding_nodes_logalpha;
	for (int i = 0; i < categorical_size; ++i) {
		char name_char[512];
		sprintf(name_char, "Categorical_encoding-SoftMax-Out_%012d", i); // LogAlpha_%012d
		std::string name(name_char);
		encoding_nodes_logalpha.push_back(name);
	}

	// define the model trainers and resources for the trainers
	std::vector<ModelInterpreterDefaultDevice<float>> model_interpreters;
	for (size_t i = 0; i < n_threads; ++i) {
		ModelResources model_resources = { ModelDevice(0, 1) };
		ModelInterpreterDefaultDevice<float> model_interpreter(model_resources);
		model_interpreters.push_back(model_interpreter);
	}
	ModelTrainerExt<float> model_trainer;
	//model_trainer.setBatchSize(1); // evaluation only
	model_trainer.setBatchSize(32);
	model_trainer.setNEpochsTraining(10000);
	model_trainer.setNEpochsValidation(25);
	model_trainer.setNEpochsEvaluation(0);
	model_trainer.setMemorySize(1);
	model_trainer.setVerbosityLevel(1);
	model_trainer.setLogging(true, false, true);
	model_trainer.setFindCycles(false);
	model_trainer.setLossFunctions({
		//std::shared_ptr<LossFunctionOp<float>>(new MSEOp<float>()),
		std::shared_ptr<LossFunctionOp<float>>(new BCEWithLogitsOp<float>()),
		std::shared_ptr<LossFunctionOp<float>>(new KLDivergenceMuOp<float>()),
		std::shared_ptr<LossFunctionOp<float>>(new KLDivergenceLogVarOp<float>()),
		std::shared_ptr<LossFunctionOp<float>>(new NegativeLogLikelihoodOp<float>()) });
	model_trainer.setLossFunctionGrads({
		//std::shared_ptr<LossFunctionGradOp<float>>(new MSEGradOp<float>()),
		std::shared_ptr<LossFunctionGradOp<float>>(new BCEWithLogitsGradOp<float>()),
		std::shared_ptr<LossFunctionGradOp<float>>(new KLDivergenceMuGradOp<float>()),
		std::shared_ptr<LossFunctionGradOp<float>>(new KLDivergenceLogVarGradOp<float>()),
		std::shared_ptr<LossFunctionGradOp<float>>(new NegativeLogLikelihoodGradOp<float>()) });
	model_trainer.setOutputNodes({ output_nodes, encoding_nodes_mu, encoding_nodes_logvar, encoding_nodes_logalpha });

	// define the model replicator for growth mode
	ModelReplicatorExt<float> model_replicator;

	// define the initial population
	std::cout << "Initializing the population..." << std::endl;
	Model<float> model;
	if (make_model) {
		model_trainer.makeCVAE(model, input_size, categorical_size, encoding_size, n_hidden);
	}
	else {
		// read in the trained model
		std::cout << "Reading in the model..." << std::endl;
		const std::string data_dir = "C:/Users/domccl/GitHub/smartPeak_cpp/build_win_cuda/bin/Debug/";
		const std::string nodes_filename = data_dir + "0_MNIST_Nodes.csv";
		const std::string links_filename = data_dir + "0_MNIST_Links.csv";
		const std::string weights_filename = data_dir + "0_MNIST_Weights.csv";
		model.setId(1);
		model.setName("VAE1");
		ModelFile<float> model_file;
		model_file.loadModelCsv(nodes_filename, links_filename, weights_filename, model);
	}
	if (load_weight_values) {
		// read in the trained model weights only
		std::cout << "Reading in the model weight values..." << std::endl;
		const std::string data_dir = "C:/Users/domccl/GitHub/smartPeak_cpp/build_win_cuda/bin/Debug/";
		const std::string weights_filename = data_dir + "2_MNIST_Weights.csv";
		model.setId(2);
		model.setName("VAE2");
		WeightFile<float> weight_file;
		weight_file.loadWeightValuesCsv(weights_filename, model.weights_);
	}
	std::vector<Model<float>> population = { model };

	if (train_model) {
		// Evolve the population
		std::vector<std::vector<std::tuple<int, std::string, float>>> models_validation_errors_per_generation = population_trainer.evolveModels(
			population, model_trainer, model_interpreters, model_replicator, data_simulator, model_logger, input_nodes);

		PopulationTrainerFile<float> population_trainer_file;
		population_trainer_file.storeModels(population, "MNIST");
		population_trainer_file.storeModelValidations("MNISTErrors.csv", models_validation_errors_per_generation.back());
	}
	else {
		// Evaluate the population
		population_trainer.evaluateModels(
			population, model_trainer, model_interpreters, model_replicator, data_simulator, model_logger, input_nodes);
	}
}

int main(int argc, char** argv)
{
	// run the application
	main_VAE(true, false, true);

	return 0;
}
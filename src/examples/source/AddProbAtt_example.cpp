/**TODO:  Add copyright*/

#include <EvoNet/ml/PopulationTrainerExperimentalDefaultDevice.h>
#include <EvoNet/ml/ModelTrainerExperimentalDefaultDevice.h>
#include <EvoNet/ml/ModelReplicatorExperimental.h>
#include <EvoNet/ml/ModelBuilder.h>
#include <EvoNet/ml/Model.h>
#include <EvoNet/simulator/AddProbSimulator.h>
#include <EvoNet/io/Parameters.h>

#include <unsupported/Eigen/CXX11/Tensor>

using namespace EvoNet;

template<typename TensorT>
class DataSimulatorExt : public AddProbSimulator<TensorT>
{
public:
	void simulateData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
	{
		// infer data dimensions based on the input tensors
		const int batch_size = input_data.dimension(0);
		const int memory_size = input_data.dimension(1);
		const int n_input_nodes = input_data.dimension(2);
		const int n_output_nodes = output_data.dimension(2);
		const int n_epochs = input_data.dimension(3);

    // sequence length
    const int sequence_length = n_input_nodes / 2;
    assert(sequence_length == this->sequence_length_);

		//// generate a new sequence 
		//Eigen::Tensor<TensorT, 1> random_sequence(this->sequence_length_);
		//Eigen::Tensor<TensorT, 1> mask_sequence(this->sequence_length_);
		//float result = this->AddProb(random_sequence, mask_sequence, this->n_mask_);

		// Generate the input and output data for training [BUG FREE]
		for (int batch_iter = 0; batch_iter<batch_size; ++batch_iter) {
			for (int epochs_iter = 0; epochs_iter<n_epochs; ++epochs_iter) {

				// generate a new sequence 
        Eigen::Tensor<TensorT, 1> random_sequence(this->sequence_length_);
        Eigen::Tensor<TensorT, 1> mask_sequence(this->sequence_length_);
        float result = this->AddProb(random_sequence, mask_sequence, this->n_mask_);

				for (int memory_iter = 0; memory_iter<memory_size; ++memory_iter) {
          for (int nodes_iter = 0; nodes_iter < n_input_nodes/2; ++nodes_iter) {
            input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = random_sequence(nodes_iter); // random sequence
            input_data(batch_iter, memory_iter, nodes_iter + n_input_nodes/2, epochs_iter) = mask_sequence(nodes_iter); // mask sequence
            //std::cout << "Node: " << nodes_iter << ";Rand: " << input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) << ";Mask: " << input_data(batch_iter, memory_iter, nodes_iter + n_input_nodes / 2, epochs_iter) << std::endl;
          }
          for (int nodes_iter = 0; nodes_iter < n_output_nodes; ++nodes_iter) {
            output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = result;
          }
				}
			}
		}
		//std::cout << "Input data: " << input_data << std::endl; // [TESTS: convert to a test!]
		//std::cout << "Output data: " << output_data << std::endl; // [TESTS: convert to a test!]

		time_steps.setConstant(1.0f);
	}
	void simulateTrainingData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)override	{		simulateData(input_data, output_data, time_steps);	}
	void simulateValidationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)override	{		simulateData(input_data, output_data, time_steps);	}
	void simulateEvaluationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 3>& time_steps) override {};
  void simulateData(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& output_data, Eigen::Tensor<TensorT, 3>& metric_data, Eigen::Tensor<TensorT, 2>& time_steps)
  {
    // infer data dimensions based on the input tensors
    const int batch_size = input_data.dimension(0);
    const int memory_size = input_data.dimension(1);
    const int n_input_nodes = input_data.dimension(2);
    const int n_output_nodes = output_data.dimension(2);

    // sequence length
    const int sequence_length = n_input_nodes / 2;
    assert(sequence_length == this->sequence_length_);

    // Generate the input and output data for training
    for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {

      // generate a new sequence 
      Eigen::Tensor<TensorT, 1> random_sequence(this->sequence_length_);
      Eigen::Tensor<TensorT, 1> mask_sequence(this->sequence_length_);
      float result = this->AddProb(random_sequence, mask_sequence, this->n_mask_);

      for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
        for (int nodes_iter = 0; nodes_iter < n_input_nodes / 2; ++nodes_iter) {
          input_data(batch_iter, memory_iter, nodes_iter) = random_sequence(nodes_iter); // random sequence
          input_data(batch_iter, memory_iter, nodes_iter + n_input_nodes / 2) = mask_sequence(nodes_iter); // mask sequence
        }
        for (int nodes_iter = 0; nodes_iter < n_output_nodes; ++nodes_iter) {
          output_data(batch_iter, memory_iter, nodes_iter) = result;
          metric_data(batch_iter, memory_iter, nodes_iter) = result;
        }
      }
    }

    time_steps.setConstant(1.0f);
  }
  void simulateTrainingData(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& output_data, Eigen::Tensor<TensorT, 3>& metric_data, Eigen::Tensor<TensorT, 2>& time_steps)override { simulateData(input_data, output_data, metric_data, time_steps); }
  void simulateValidationData(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& output_data, Eigen::Tensor<TensorT, 3>& metric_data, Eigen::Tensor<TensorT, 2>& time_steps)override { simulateData(input_data, output_data, metric_data, time_steps); }
  void simulateEvaluationData(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& metric_data, Eigen::Tensor<TensorT, 2>& time_steps)override { simulateData(input_data, metric_data, Eigen::Tensor<TensorT, 3>(), time_steps); }
};

// Extended classes
template<typename TensorT>
class ModelTrainerExt : public ModelTrainerExperimentalDefaultDevice<TensorT>
{
public:
	/*
	@brief Minimal network 
	*/
	void makeModelMinimal(Model<TensorT>& model, const int& n_inputs, const int& n_outputs, int n_hidden_0 = 1, bool specify_layers = false)
	{
    model.setId(0);
    model.setName("AddProbAtt-Min");
    ModelBuilder<TensorT> model_builder;
    // Add the inputs
    std::vector<std::string> node_names_random = model_builder.addInputNodes(model, "Random", "Random", n_inputs, specify_layers);
    std::vector<std::string> node_names_mask = model_builder.addInputNodes(model, "Mask", "Mask", n_inputs, specify_layers);
    // Define the activation 
    std::shared_ptr<ActivationOp<TensorT>> activation = std::make_shared<LeakyReLUOp<TensorT>>(LeakyReLUOp<TensorT>());
    std::shared_ptr<ActivationOp<TensorT>> activation_grad = std::make_shared<LeakyReLUGradOp<TensorT>>(LeakyReLUGradOp<TensorT>());
    std::shared_ptr<ActivationOp<TensorT>> activation_output = std::make_shared<LeakyReLUOp<TensorT>>(LeakyReLUOp<TensorT>());
    std::shared_ptr<ActivationOp<TensorT>> activation_output_grad = std::make_shared<LeakyReLUGradOp<TensorT>>(LeakyReLUGradOp<TensorT>());
    // Define the node integration
    auto integration_op = std::make_shared<SumOp<TensorT>>(SumOp<TensorT>());
    auto integration_error_op = std::make_shared<SumErrorOp<TensorT>>(SumErrorOp<TensorT>());
    auto integration_weight_grad_op = std::make_shared<SumWeightGradOp<TensorT>>(SumWeightGradOp<TensorT>());
    // Define the solver
    auto solver_op = std::make_shared<SGDOp<TensorT>>(SGDOp<TensorT>(1e-3, 0.9, 10));
    // Add the hidden layer
    std::vector<std::string> node_names = model_builder.addFullyConnected(model, "HiddenR", "HiddenR", node_names_random, n_hidden_0,
      activation, activation_grad, integration_op, integration_error_op, integration_weight_grad_op,
      std::make_shared<RandWeightInitOp<TensorT>>(RandWeightInitOp<TensorT>((int)(node_names_random.size() + n_hidden_0) / 2, 1)),
      solver_op, 0.0f, 0.0f, false, specify_layers);
    model_builder.addFullyConnected(model, "HiddenR", node_names_mask, node_names,
      std::make_shared<RandWeightInitOp<TensorT>>(RandWeightInitOp<TensorT>((int)(node_names_mask.size() + n_hidden_0) / 2, 1)),
      solver_op, 0.0f, specify_layers);
    // Add the output layer
    node_names = model_builder.addFullyConnected(model, "FC-Out", "FC-Out", node_names, n_outputs,
      activation_output, activation_output_grad, integration_op, integration_error_op, integration_weight_grad_op,
      std::make_shared<RandWeightInitOp<TensorT>>(RandWeightInitOp<TensorT>(node_names.size(), 2)),
      solver_op, 0.0f, 0.0f, false, true);
    for (const std::string& node_name : node_names)
      model.getNodesMap().at(node_name)->setType(NodeType::unmodifiable);
    node_names = model_builder.addSinglyConnected(model, "Output", "Output", node_names, n_outputs,
      std::make_shared<LinearOp<TensorT>>(LinearOp<TensorT>()),
      std::make_shared<LinearGradOp<TensorT>>(LinearGradOp<TensorT>()),
      integration_op, integration_error_op, integration_weight_grad_op,
      std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1)),
      std::make_shared<DummySolverOp<TensorT>>(DummySolverOp<TensorT>()), 0.0f, 0.0f, false, true);
    for (const std::string& node_name : node_names)
      model.getNodesMap().at(node_name)->setType(NodeType::output);
    model.setInputAndOutputNodes();
	}
	/*
	@brief Minimal network required to solve the addition problem
	*/
	void makeModelSolution(Model<TensorT>& model, const int& n_inputs, const int& n_outputs, bool init_weight_soln = true, bool specify_layers = false)
	{
    model.setId(0);
    model.setName("AddProbAtt-Solution");
    ModelBuilder<TensorT> model_builder;
    // Add the inputs
    std::vector<std::string> node_names_random = model_builder.addInputNodes(model, "Random", "Random", n_inputs, specify_layers);
    std::vector<std::string> node_names_mask = model_builder.addInputNodes(model, "Mask", "Mask", n_inputs, specify_layers);
    // Define the activation 
    std::shared_ptr<ActivationOp<TensorT>> activation = std::make_shared<LeakyReLUOp<TensorT>>(LeakyReLUOp<TensorT>());
    std::shared_ptr<ActivationOp<TensorT>> activation_grad = std::make_shared<LeakyReLUGradOp<TensorT>>(LeakyReLUGradOp<TensorT>());
    std::shared_ptr<ActivationOp<TensorT>> activation_output = std::make_shared<LeakyReLUOp<TensorT>>(LeakyReLUOp<TensorT>());
    std::shared_ptr<ActivationOp<TensorT>> activation_output_grad = std::make_shared<LeakyReLUGradOp<TensorT>>(LeakyReLUGradOp<TensorT>());
    // Define the node integration
    auto integration_op = std::make_shared<SumOp<TensorT>>(SumOp<TensorT>());
    auto integration_error_op = std::make_shared<SumErrorOp<TensorT>>(SumErrorOp<TensorT>());
    auto integration_weight_grad_op = std::make_shared<SumWeightGradOp<TensorT>>(SumWeightGradOp<TensorT>());
    // Define the solver and weight init ops
    std::shared_ptr<SolverOp<TensorT>> solver_op;
    std::shared_ptr<WeightInitOp<TensorT>> weight_init_1, weight_init_2;
    if (init_weight_soln) {
      solver_op = std::make_shared<DummySolverOp<TensorT>>(DummySolverOp<TensorT>());
      weight_init_1 = std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1));
      weight_init_2 = std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1));
    }
    else {
      solver_op = std::make_shared<SGDOp<TensorT>>(SGDOp<TensorT>(1e-3, 0.9, 10));
      weight_init_1 = std::make_shared<RandWeightInitOp<TensorT>>(RandWeightInitOp<TensorT>((int)(node_names_random.size() + n_inputs) / 2, 1));
      weight_init_2 = std::make_shared<RandWeightInitOp<TensorT>>(RandWeightInitOp<TensorT>(n_inputs, 2));
    }
    // Add the hidden layer
    std::vector<std::string> node_names = model_builder.addSinglyConnected(model, "HiddenR", "HiddenR", node_names_random, n_inputs,
      activation, activation_grad,
      std::make_shared<ProdOp<TensorT>>(ProdOp<TensorT>()),
      std::make_shared<ProdErrorOp<TensorT>>(ProdErrorOp<TensorT>()),
      std::make_shared<ProdWeightGradOp<TensorT>>(ProdWeightGradOp<TensorT>()),
      weight_init_1, solver_op, 0.0f, 0.0f, false, specify_layers);
    model_builder.addSinglyConnected(model, "HiddenR", node_names_mask, node_names,
      weight_init_1, solver_op, 0.0f, specify_layers);
    // Add the output layer
    node_names = model_builder.addFullyConnected(model, "FC-Out", "FC-Out", node_names, n_outputs,
      activation_output, activation_output_grad, integration_op, integration_error_op, integration_weight_grad_op,
      weight_init_2, solver_op, 0.0f, 0.0f, false, specify_layers);
    for (const std::string& node_name : node_names)
      model.getNodesMap().at(node_name)->setType(NodeType::unmodifiable);
    node_names = model_builder.addSinglyConnected(model, "Output", "Output", node_names, n_outputs,
      std::make_shared<LinearOp<TensorT>>(LinearOp<TensorT>()),
      std::make_shared<LinearGradOp<TensorT>>(LinearGradOp<TensorT>()),
      integration_op, integration_error_op, integration_weight_grad_op,
      std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1)),
      std::make_shared<DummySolverOp<TensorT>>(DummySolverOp<TensorT>()), 0.0f, 0.0f, false, true);
    for (const std::string& node_name : node_names)
      model.nodes_.at(node_name)->setType(NodeType::output);
    model.setInputAndOutputNodes();
	}
};

template<typename TensorT>
class ModelReplicatorExt : public ModelReplicatorExperimental<TensorT>
{};

template<typename TensorT>
class PopulationTrainerExt : public PopulationTrainerExperimentalDefaultDevice<TensorT>
{};

template<class ...ParameterTypes>
void main_(const ParameterTypes& ...args) {
  auto parameters = std::make_tuple(args...);

  // define the population trainer parameters
  PopulationTrainerExt<float> population_trainer;
  setPopulationTrainerParameters(population_trainer, args...);

  // define the population logger
  PopulationLogger<float> population_logger(true, true);
  
  // define the data simulator
  DataSimulatorExt<float> data_simulator;
  data_simulator.n_mask_ = std::get<EvoNetParameters::Examples::NMask>(parameters).get();
  data_simulator.sequence_length_ = std::get<EvoNetParameters::Examples::SequenceLength>(parameters).get();

  // define the input/output nodes
  std::vector<std::string> input_nodes;
  for (int i = 0; i < data_simulator.sequence_length_; ++i) {
    char name_char[512];
    sprintf(name_char, "Random_%012d", i);
    std::string name(name_char);
    input_nodes.push_back(name);
  }
  for (int i = 0; i < data_simulator.sequence_length_; ++i) {
    char name_char[512];
    sprintf(name_char, "Mask_%012d", i);
    std::string name(name_char);
    input_nodes.push_back(name);
  }
  std::vector<std::string> output_nodes = { "Output_000000000000" };

  // define the model interpreters
  std::vector<ModelInterpreterDefaultDevice<float>> model_interpreters;
  setModelInterpreterParameters(model_interpreters, args...);

  // define the model trainer
  ModelTrainerExt<float> model_trainer;
  setModelTrainerParameters(model_trainer, args...);

  std::vector<LossFunctionHelper<float>> loss_function_helpers;
  LossFunctionHelper<float> loss_function_helper2;
  loss_function_helper2.output_nodes_ = output_nodes;
  loss_function_helper2.loss_functions_ = { std::make_shared<MSELossOp<float>>(MSELossOp<float>(1e-24, 1.0)) };
  loss_function_helper2.loss_function_grads_ = { std::make_shared<MSELossGradOp<float>>(MSELossGradOp<float>(1e-24, 1.0)) };
  loss_function_helpers.push_back(loss_function_helper2);
  model_trainer.setLossFunctionHelpers(loss_function_helpers);

  std::vector<MetricFunctionHelper<float>> metric_function_helpers;
  MetricFunctionHelper<float> metric_function_helper1;
  metric_function_helper1.output_nodes_ = output_nodes;
  metric_function_helper1.metric_functions_ = { std::make_shared<EuclideanDistOp<float>>(EuclideanDistOp<float>("Mean")), std::make_shared<EuclideanDistOp<float>>(EuclideanDistOp<float>("Var")) };
  metric_function_helper1.metric_names_ = { "EuclideanDist-Mean", "EuclideanDist-Var" };
  metric_function_helpers.push_back(metric_function_helper1);
  model_trainer.setMetricFunctionHelpers(metric_function_helpers);

  // define the model logger
  ModelLogger<float> model_logger(true, true, false, false, false, false, false);

  // define the model replicator for growth mode
  ModelReplicatorExt<float> model_replicator;
  setModelReplicatorParameters(model_replicator, args...);

  // define the initial population
  Model<float> model;
  if (std::get<EvoNetParameters::Main::MakeModel>(parameters).get()) {
    std::cout << "Making the model..." << std::endl;
    if (std::get<EvoNetParameters::Examples::ModelType>(parameters).get() == "Minimal") model_trainer.makeModelMinimal(model, input_nodes.size() / 2, output_nodes.size());
    else if (std::get<EvoNetParameters::Examples::ModelType>(parameters).get() == "Solution") model_trainer.makeModelSolution(model, input_nodes.size() / 2, output_nodes.size(), false, false);
    //else if (std::get<EvoNetParameters::Examples::ModelType>(parameters).get() == "DotProdAtt") model_trainer.makeModelDotProdAtt(model, (int)(input_nodes.size() / 2), output_nodes.size(), { 4 }, { 8 }, { 16 }, false, false, false, true);
    model.setId(0);
  }
  else {
    ModelFile<float> model_file;
    ModelInterpreterFileDefaultDevice<float> model_interpreter_file;
    loadModelFromParameters(model, model_interpreters.at(0), model_file, model_interpreter_file, args...);
  }
  model.setName(std::get<EvoNetParameters::General::DataDir>(parameters).get() + std::get<EvoNetParameters::Main::ModelName>(parameters).get()); //So that all output will be written to a specific directory

  // Run the training, evaluation, or evolution
  runTrainEvalEvoFromParameters<float>(model, model_interpreters, model_trainer, population_trainer, model_replicator, data_simulator, model_logger, population_logger, input_nodes, args...);
}

// Main
int main(int argc, char** argv)
{
  // Parse the user commands
  int id_int = -1;
  std::string parameters_filename = "";
  parseCommandLineArguments(argc, argv, id_int, parameters_filename);

  // Set the parameter names and defaults
  EvoNetParameters::General::ID id("id", -1);
  EvoNetParameters::General::DataDir data_dir("data_dir", std::string(""));
  EvoNetParameters::Main::DeviceId device_id("device_id", 0);
  EvoNetParameters::Main::ModelName model_name("model_name", "");
  EvoNetParameters::Main::MakeModel make_model("make_model", true);
  EvoNetParameters::Main::LoadModelCsv load_model_csv("load_model_csv", false);
  EvoNetParameters::Main::LoadModelBinary load_model_binary("load_model_binary", false);
  EvoNetParameters::Main::TrainModel train_model("train_model", true);
  EvoNetParameters::Main::EvolveModel evolve_model("evolve_model", false);
  EvoNetParameters::Main::EvaluateModel evaluate_model("evaluate_model", false);
  EvoNetParameters::Main::EvaluateModels evaluate_models("evaluate_models", false);
  EvoNetParameters::Examples::NMask n_mask("n_mask", 2);
  EvoNetParameters::Examples::SequenceLength sequence_length("sequence_length", 25);
  EvoNetParameters::Examples::ModelType model_type("model_type", "Solution");
  EvoNetParameters::Examples::SimulationType simulation_type("simulation_type", "");
  EvoNetParameters::Examples::BiochemicalRxnsFilename biochemical_rxns_filename("biochemical_rxns_filename", "iJO1366.csv");
  EvoNetParameters::PopulationTrainer::PopulationName population_name("population_name", "");
  EvoNetParameters::PopulationTrainer::NGenerations n_generations("n_generations", 1);
  EvoNetParameters::PopulationTrainer::NInterpreters n_interpreters("n_interpreters", 1);
  EvoNetParameters::PopulationTrainer::PruneModelNum prune_model_num("prune_model_num", 10);
  EvoNetParameters::PopulationTrainer::RemoveIsolatedNodes remove_isolated_nodes("remove_isolated_nodes", true);
  EvoNetParameters::PopulationTrainer::CheckCompleteModelInputToOutput check_complete_model_input_to_output("check_complete_model_input_to_output", true);
  EvoNetParameters::PopulationTrainer::PopulationSize population_size("population_size", 128);
  EvoNetParameters::PopulationTrainer::NTop n_top("n_top", 8);
  EvoNetParameters::PopulationTrainer::NRandom n_random("n_random", 8);
  EvoNetParameters::PopulationTrainer::NReplicatesPerModel n_replicates_per_model("n_replicates_per_model", 1);
  EvoNetParameters::PopulationTrainer::ResetModelCopyWeights reset_model_copy_weights("reset_model_copy_weights", true);
  EvoNetParameters::PopulationTrainer::ResetModelTemplateWeights reset_model_template_weights("reset_model_template_weights", true);
  EvoNetParameters::PopulationTrainer::Logging population_logging("population_logging", true);
  EvoNetParameters::PopulationTrainer::SetPopulationSizeFixed set_population_size_fixed("set_population_size_fixed", false);
  EvoNetParameters::PopulationTrainer::SetPopulationSizeDoubling set_population_size_doubling("set_population_size_doubling", true);
  EvoNetParameters::PopulationTrainer::SetTrainingStepsByModelSize set_training_steps_by_model_size("set_training_steps_by_model_size", false);
  EvoNetParameters::ModelTrainer::BatchSize batch_size("batch_size", 32);
  EvoNetParameters::ModelTrainer::MemorySize memory_size("memory_size", 64);
  EvoNetParameters::ModelTrainer::NEpochsTraining n_epochs_training("n_epochs_training", 1000);
  EvoNetParameters::ModelTrainer::NEpochsValidation n_epochs_validation("n_epochs_validation", 25);
  EvoNetParameters::ModelTrainer::NEpochsEvaluation n_epochs_evaluation("n_epochs_evaluation", 10);
  EvoNetParameters::ModelTrainer::NTBTTSteps n_tbtt_steps("n_tbtt_steps", 64);
  EvoNetParameters::ModelTrainer::NTETTSteps n_tett_steps("n_tett_steps", 64);
  EvoNetParameters::ModelTrainer::Verbosity verbosity("verbosity", 1);
  EvoNetParameters::ModelTrainer::LoggingTraining logging_training("logging_training", true);
  EvoNetParameters::ModelTrainer::LoggingValidation logging_validation("logging_validation", false);
  EvoNetParameters::ModelTrainer::LoggingEvaluation logging_evaluation("logging_evaluation", true);
  EvoNetParameters::ModelTrainer::FindCycles find_cycles("find_cycles", true);
  EvoNetParameters::ModelTrainer::FastInterpreter fast_interpreter("fast_interpreter", true);
  EvoNetParameters::ModelTrainer::PreserveOoO preserve_ooo("preserve_ooo", true);
  EvoNetParameters::ModelTrainer::InterpretModel interpret_model("interpret_model", true);
  EvoNetParameters::ModelTrainer::ResetModel reset_model("reset_model", false);
  EvoNetParameters::ModelTrainer::ResetInterpreter reset_interpreter("reset_interpreter", true);
  EvoNetParameters::ModelReplicator::NNodeDownAdditionsLB n_node_down_additions_lb("n_node_down_additions_lb", 0);
  EvoNetParameters::ModelReplicator::NNodeRightAdditionsLB n_node_right_additions_lb("n_node_right_additions_lb", 0);
  EvoNetParameters::ModelReplicator::NNodeDownCopiesLB n_node_down_copies_lb("n_node_down_copies_lb", 0);
  EvoNetParameters::ModelReplicator::NNodeRightCopiesLB n_node_right_copies_lb("n_node_right_copies_lb", 0);
  EvoNetParameters::ModelReplicator::NLinkAdditionsLB n_link_additons_lb("n_link_additons_lb", 0);
  EvoNetParameters::ModelReplicator::NLinkCopiesLB n_link_copies_lb("n_link_copies_lb", 0);
  EvoNetParameters::ModelReplicator::NNodeDeletionsLB n_node_deletions_lb("n_node_deletions_lb", 0);
  EvoNetParameters::ModelReplicator::NLinkDeletionsLB n_link_deletions_lb("n_link_deletions_lb", 0);
  EvoNetParameters::ModelReplicator::NNodeActivationChangesLB n_node_activation_changes_lb("n_node_activation_changes_lb", 0);
  EvoNetParameters::ModelReplicator::NNodeIntegrationChangesLB n_node_integration_changes_lb("n_node_integration_changes_lb", 0);
  EvoNetParameters::ModelReplicator::NModuleAdditionsLB n_module_additions_lb("n_module_additions_lb", 0);
  EvoNetParameters::ModelReplicator::NModuleCopiesLB n_module_copies_lb("n_module_copies_lb", 0);
  EvoNetParameters::ModelReplicator::NModuleDeletionsLB n_module_deletions_lb("n_module_deletions_lb", 0);
  EvoNetParameters::ModelReplicator::NNodeDownAdditionsUB n_node_down_additions_ub("n_node_down_additions_ub", 0);
  EvoNetParameters::ModelReplicator::NNodeRightAdditionsUB n_node_right_additions_ub("n_node_right_additions_ub", 0);
  EvoNetParameters::ModelReplicator::NNodeDownCopiesUB n_node_down_copies_ub("n_node_down_copies_ub", 0);
  EvoNetParameters::ModelReplicator::NNodeRightCopiesUB n_node_right_copies_ub("n_node_right_copies_ub", 0);
  EvoNetParameters::ModelReplicator::NLinkAdditionsUB n_link_additons_ub("n_link_additons_ub", 0);
  EvoNetParameters::ModelReplicator::NLinkCopiesUB n_link_copies_ub("n_link_copies_ub", 0);
  EvoNetParameters::ModelReplicator::NNodeDeletionsUB n_node_deletions_ub("n_node_deletions_ub", 0);
  EvoNetParameters::ModelReplicator::NLinkDeletionsUB n_link_deletions_ub("n_link_deletions_ub", 0);
  EvoNetParameters::ModelReplicator::NNodeActivationChangesUB n_node_activation_changes_ub("n_node_activation_changes_ub", 0);
  EvoNetParameters::ModelReplicator::NNodeIntegrationChangesUB n_node_integration_changes_ub("n_node_integration_changes_ub", 0);
  EvoNetParameters::ModelReplicator::NModuleAdditionsUB n_module_additions_ub("n_module_additions_ub", 0);
  EvoNetParameters::ModelReplicator::NModuleCopiesUB n_module_copies_ub("n_module_copies_ub", 0);
  EvoNetParameters::ModelReplicator::NModuleDeletionsUB n_module_deletions_ub("n_module_deletions_ub", 0);
  EvoNetParameters::ModelReplicator::SetModificationRateFixed set_modification_rate_fixed("set_modification_rate_fixed", false);
  EvoNetParameters::ModelReplicator::SetModificationRateByPrevError set_modification_rate_by_prev_error("set_modification_rate_by_prev_error", false);
  auto parameters = std::make_tuple(id, data_dir,
    device_id, model_name, make_model, load_model_csv, load_model_binary, train_model, evolve_model, evaluate_model, evaluate_models,
    n_mask, sequence_length, model_type, simulation_type, biochemical_rxns_filename,
    population_name, n_generations, n_interpreters, prune_model_num, remove_isolated_nodes, check_complete_model_input_to_output, population_size, n_top, n_random, n_replicates_per_model, reset_model_copy_weights, reset_model_template_weights, population_logging, set_population_size_fixed, set_population_size_doubling, set_training_steps_by_model_size,
    batch_size, memory_size, n_epochs_training, n_epochs_validation, n_epochs_evaluation, n_tbtt_steps, n_tett_steps, verbosity, logging_training, logging_validation, logging_evaluation, find_cycles, fast_interpreter, preserve_ooo, interpret_model, reset_model, reset_interpreter,
    n_node_down_additions_lb, n_node_right_additions_lb, n_node_down_copies_lb, n_node_right_copies_lb, n_link_additons_lb, n_link_copies_lb, n_node_deletions_lb, n_link_deletions_lb, n_node_activation_changes_lb, n_node_integration_changes_lb, n_module_additions_lb, n_module_copies_lb, n_module_deletions_lb, n_node_down_additions_ub, n_node_right_additions_ub, n_node_down_copies_ub, n_node_right_copies_ub, n_link_additons_ub, n_link_copies_ub, n_node_deletions_ub, n_link_deletions_ub, n_node_activation_changes_ub, n_node_integration_changes_ub, n_module_additions_ub, n_module_copies_ub, n_module_deletions_ub, set_modification_rate_fixed, set_modification_rate_by_prev_error);

  // Read in the parameters
  LoadParametersFromCsv loadParametersFromCsv(id_int, parameters_filename);
  parameters = EvoNet::apply([&loadParametersFromCsv](auto&& ...args) { return loadParametersFromCsv(args...); }, parameters);

  // Run the application
  EvoNet::apply([](auto&& ...args) { main_(args ...); }, parameters);
	return 0;
}
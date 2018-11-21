/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE ModelTrainer test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/ml/ModelTrainerDefaultDevice.h>

#include <SmartPeak/ml/Model.h>
#include <SmartPeak/ml/ModelInterpreterDefaultDevice.h>

using namespace SmartPeak;
using namespace std;

template<typename TensorT>
class ModelTrainerExt : public ModelTrainerDefaultDevice<TensorT><TensorT>
{
public:
	Model<TensorT> makeModel() { return Model<TensorT>(); }
	void adaptiveTrainerScheduler(
		const int& n_generations,
		const int& n_epochs,
		Model<TensorT>& model,
		const std::vector<TensorT>& model_errors) {}
};

BOOST_AUTO_TEST_SUITE(trainer)

BOOST_AUTO_TEST_CASE(constructor) 
{
  ModelTrainerExt<float>* ptr = nullptr;
  ModelTrainerExt<float>* nullPointer = nullptr;
	ptr = new ModelTrainerExt<float>();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  ModelTrainerExt<float>* ptr = nullptr;
	ptr = new ModelTrainerExt<float>();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(gettersAndSetters) 
{
  ModelTrainerExt<float> trainer;
  trainer.setBatchSize(4);
  trainer.setMemorySize(1);
  trainer.setNEpochsTraining(100);
	trainer.setNEpochsValidation(10);
	trainer.setNEpochsEvaluation(2);
	trainer.setVerbosityLevel(1);
	trainer.setLogging(true, true, true);
	trainer.setNTBPTTSteps(1);
	trainer.setNTETTSteps(2);

  BOOST_CHECK_EQUAL(trainer.getBatchSize(), 4);
  BOOST_CHECK_EQUAL(trainer.getMemorySize(), 1);
  BOOST_CHECK_EQUAL(trainer.getNEpochsTraining(), 100);
	BOOST_CHECK_EQUAL(trainer.getNEpochsValidation(), 10);
	BOOST_CHECK_EQUAL(trainer.getNEpochsEvaluation(), 2);
	BOOST_CHECK_EQUAL(trainer.getVerbosityLevel(), 1);
	BOOST_CHECK_EQUAL(trainer.getNTBPTTSteps(), 1);
	BOOST_CHECK_EQUAL(trainer.getNTETTSteps(), 2);
}

BOOST_AUTO_TEST_CASE(checkInputData) 
{
  ModelTrainerExt<float> trainer;
  trainer.setBatchSize(4);
  trainer.setMemorySize(1);
  trainer.setNEpochsTraining(100);
	trainer.setNEpochsValidation(100);

  const std::vector<std::string> input_nodes = {"0", "1", "6", "7"};
  Eigen::Tensor<float, 4> input_data(trainer.getBatchSize(), trainer.getMemorySize(), (int)input_nodes.size(), trainer.getNEpochsTraining());

  BOOST_CHECK(trainer.checkInputData(trainer.getNEpochsTraining(),
    input_data, trainer.getBatchSize(), trainer.getMemorySize(), input_nodes));

  BOOST_CHECK(!trainer.checkInputData(90,
    input_data, trainer.getBatchSize(), trainer.getMemorySize(), input_nodes));

  const std::vector<std::string> input_nodes2 = {"0", "1"};
  BOOST_CHECK(!trainer.checkInputData(trainer.getNEpochsTraining(),
    input_data, trainer.getBatchSize(), trainer.getMemorySize(), input_nodes2));

  BOOST_CHECK(!trainer.checkInputData(trainer.getNEpochsTraining(),
    input_data, trainer.getBatchSize(), 3, input_nodes));

  BOOST_CHECK(!trainer.checkInputData(trainer.getNEpochsTraining(),
    input_data, 3, trainer.getMemorySize(), input_nodes));
}

BOOST_AUTO_TEST_CASE(checkOutputData) 
{
  ModelTrainerExt<float> trainer;
  trainer.setBatchSize(4);
  trainer.setMemorySize(1);
  trainer.setNEpochsTraining(100);

  const std::vector<std::string> output_nodes = {"4", "5"};
  Eigen::Tensor<float, 4> output_data(trainer.getBatchSize(), trainer.getMemorySize(), (int)output_nodes.size(), trainer.getNEpochsTraining());

  BOOST_CHECK(trainer.checkOutputData(trainer.getNEpochsTraining(),
    output_data, trainer.getBatchSize(), trainer.getMemorySize(), output_nodes));

  BOOST_CHECK(!trainer.checkOutputData(90,
    output_data, trainer.getBatchSize(), trainer.getMemorySize(), output_nodes));

  const std::vector<std::string> output_nodes2 = {"0"};
  BOOST_CHECK(!trainer.checkOutputData(trainer.getNEpochsTraining(),
    output_data, trainer.getBatchSize(), trainer.getMemorySize(), output_nodes2));

  BOOST_CHECK(!trainer.checkOutputData(trainer.getNEpochsTraining(),
    output_data, 3, trainer.getMemorySize(), output_nodes));

	BOOST_CHECK(!trainer.checkOutputData(trainer.getNEpochsTraining(),
		output_data, trainer.getBatchSize(), 0, output_nodes));
}

template<typename TensorT>
class DAGToyModelTrainer : public ModelTrainerDefaultDevice<TensorT><TensorT>
{
public:
	Model<TensorT> makeModel()
	{
		// CopyNPasted from Model_DAG_Test
		Node<TensorT> i1, i2, h1, h2, o1, o2, b1, b2;
		Link l1, l2, l3, l4, lb1, lb2, l5, l6, l7, l8, lb3, lb4;
		Weight<TensorT> w1, w2, w3, w4, wb1, wb2, w5, w6, w7, w8, wb3, wb4;
		Model<TensorT> model1;

		// Toy network: 1 hidden layer, fully connected, DAG
		i1 = Node<TensorT>("0", NodeType::input, NodeStatus::activated, std::shared_ptr<ActivationOp<float>>(new LinearOp<float>()), std::shared_ptr<ActivationOp<float>>(new LinearGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		i2 = Node<TensorT>("1", NodeType::input, NodeStatus::activated, std::shared_ptr<ActivationOp<float>>(new LinearOp<float>()), std::shared_ptr<ActivationOp<float>>(new LinearGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		h1 = Node<TensorT>("2", NodeType::hidden, NodeStatus::initialized, std::shared_ptr<ActivationOp<float>>(new ReLUOp<float>()), std::shared_ptr<ActivationOp<float>>(new ReLUGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		h2 = Node<TensorT>("3", NodeType::hidden, NodeStatus::initialized, std::shared_ptr<ActivationOp<float>>(new ReLUOp<float>()), std::shared_ptr<ActivationOp<float>>(new ReLUGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		o1 = Node<TensorT>("4", NodeType::output, NodeStatus::initialized, std::shared_ptr<ActivationOp<float>>(new ReLUOp<float>()), std::shared_ptr<ActivationOp<float>>(new ReLUGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		o2 = Node<TensorT>("5", NodeType::output, NodeStatus::initialized, std::shared_ptr<ActivationOp<float>>(new ReLUOp<float>()), std::shared_ptr<ActivationOp<float>>(new ReLUGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		b1 = Node<TensorT>("6", NodeType::bias, NodeStatus::activated, std::shared_ptr<ActivationOp<float>>(new LinearOp<float>()), std::shared_ptr<ActivationOp<float>>(new LinearGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		b2 = Node<TensorT>("7", NodeType::bias, NodeStatus::activated, std::shared_ptr<ActivationOp<float>>(new LinearOp<float>()), std::shared_ptr<ActivationOp<float>>(new LinearGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));

		// weights  
		std::shared_ptr<WeightInitOp<TensorT>> weight_init;
		std::shared_ptr<SolverOp<TensorT>> solver;
		// weight_init.reset(new RandWeightInitOp(1.0)); // No random init for testing
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		w1 = Weight<TensorT>("0", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		w2 = Weight<TensorT>("1", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		w3 = Weight<TensorT>("2", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		w4 = Weight<TensorT>("3", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		wb1 = Weight<TensorT>("4", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		wb2 = Weight<TensorT>("5", weight_init, solver);
		// input layer + bias
		l1 = Link("0", "0", "2", "0");
		l2 = Link("1", "0", "3", "1");
		l3 = Link("2", "1", "2", "2");
		l4 = Link("3", "1", "3", "3");
		lb1 = Link("4", "6", "2", "4");
		lb2 = Link("5", "6", "3", "5");
		// weights
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		w5 = Weight<TensorT>("6", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		w6 = Weight<TensorT>("7", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		w7 = Weight<TensorT>("8", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		w8 = Weight<TensorT>("9", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		wb3 = Weight<TensorT>("10", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<float>(0.001, 0.9, 0.999, 1e-8));
		wb4 = Weight<TensorT>("11", weight_init, solver);
		// hidden layer + bias
		l5 = Link("6", "2", "4", "6");
		l6 = Link("7", "2", "5", "7");
		l7 = Link("8", "3", "4", "8");
		l8 = Link("9", "3", "5", "9");
		lb3 = Link("10", "7", "4", "10");
		lb4 = Link("11", "7", "5", "11");
		model1.setId(1);
		model1.addNodes({ i1, i2, h1, h2, o1, o2, b1, b2 });
		model1.addWeights({ w1, w2, w3, w4, wb1, wb2, w5, w6, w7, w8, wb3, wb4 });
		model1.addLinks({ l1, l2, l3, l4, lb1, lb2, l5, l6, l7, l8, lb3, lb4 });
		return model1;
	}
	void adaptiveTrainerScheduler(
		const int& n_generations,
		const int& n_epochs,
		Model<TensorT>& model,
		const std::vector<TensorT>& model_errors) {}
};
BOOST_AUTO_TEST_CASE(DAGToy) 
{
  // Define the makeModel and trainModel scripts
  DAGToyModelTrainer<float> trainer;

	// Define the model resources
	ModelResources model_resources = { ModelDevice(0, 1) };

  // Test parameters
  trainer.setBatchSize(4);
  trainer.setMemorySize(1);
  trainer.setNEpochsTraining(20);
	trainer.setNEpochsValidation(20);
	trainer.setLogging(false, false);
  const std::vector<std::string> input_nodes = {"0", "1", "6", "7"}; // true inputs + biases
  const std::vector<std::string> output_nodes = {"4", "5"};
	trainer.setLossFunctions({ std::shared_ptr<LossFunctionOp<float>>(new MSEOp<float>()) });
	trainer.setLossFunctionGrads({ std::shared_ptr<LossFunctionGradOp<float>>(new MSEGradOp<float>()) });
	trainer.setOutputNodes({ output_nodes });

  // Make the input data
  Eigen::Tensor<float, 4> input_data(trainer.getBatchSize(), trainer.getMemorySize(), (int)input_nodes.size(), trainer.getNEpochsTraining());
  Eigen::Tensor<float, 3> input_tmp(trainer.getBatchSize(), trainer.getMemorySize(), (int)input_nodes.size()); 
  input_tmp.setValues({{{1, 5, 1, 1}}, {{2, 6, 1, 1}}, {{3, 7, 1, 1}}, {{4, 8, 1, 1}}});
  for (int batch_iter=0; batch_iter<trainer.getBatchSize(); ++batch_iter)
    for (int memory_iter=0; memory_iter<trainer.getMemorySize(); ++memory_iter)
      for (int nodes_iter=0; nodes_iter<(int)input_nodes.size(); ++nodes_iter)
        for (int epochs_iter=0; epochs_iter<trainer.getNEpochsTraining(); ++epochs_iter)
          input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = input_tmp(batch_iter, memory_iter, nodes_iter);
  
  // Make the output data
  Eigen::Tensor<float, 4> output_data(trainer.getBatchSize(), trainer.getMemorySize(), (int)output_nodes.size(), trainer.getNEpochsTraining());
  Eigen::Tensor<float, 2> output_tmp(trainer.getBatchSize(), (int)output_nodes.size());
  output_tmp.setValues({{0, 1}, {0, 1}, {0, 1}, {0, 1}});
  for (int batch_iter=0; batch_iter<trainer.getBatchSize(); ++batch_iter){
		for (int memory_iter = 0; memory_iter<trainer.getMemorySize(); ++memory_iter){
			for (int nodes_iter=0; nodes_iter<(int)output_nodes.size(); ++nodes_iter){
				for (int epochs_iter=0; epochs_iter<trainer.getNEpochsTraining(); ++epochs_iter){
					if (memory_iter == 0)
						output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = output_tmp(batch_iter, nodes_iter);
					else
						output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = 0.0;
				}
			}
		}
	}

  // Make the simulation time_steps
	Eigen::Tensor<float, 3> time_steps(trainer.getBatchSize(), trainer.getMemorySize(), trainer.getNEpochsTraining());
	Eigen::Tensor<float, 2> time_steps_tmp(trainer.getBatchSize(), trainer.getMemorySize());
	time_steps_tmp.setValues({
		{ 1 },
		{ 1 },
		{ 1 },
		{ 1 }}
	);
	for (int batch_iter = 0; batch_iter<trainer.getBatchSize(); ++batch_iter)
		for (int memory_iter = 0; memory_iter<trainer.getMemorySize(); ++memory_iter)
			for (int epochs_iter = 0; epochs_iter<trainer.getNEpochsTraining(); ++epochs_iter)
				time_steps(batch_iter, memory_iter, epochs_iter) = time_steps_tmp(batch_iter, memory_iter);

  Model<float> model1 = trainer.makeModel();
  trainer.trainModel(model1, input_data, output_data, time_steps,
    input_nodes, ModelLogger<float>(), ModelInterpreterDefaultDevice<float>(model_resources));

  const Eigen::Tensor<float, 0> total_error = model1.getError().sum();
  BOOST_CHECK(total_error(0) <= 757.0);

	// TODO validateModel
	// TODO evaluateModel
}

template<typename TensorT>
class DCGToyModelTrainer : public ModelTrainerDefaultDevice<TensorT><TensorT>
{
public:
	Model<TensorT> makeModel()
	{
		// CopyNPasted from Model_DCG_Test
		Node<TensorT> i1, h1, o1, b1, b2;
		Link l1, l2, l3, lb1, lb2;
		Weight<TensorT> w1, w2, w3, wb1, wb2;
		Model<TensorT> model2;
		// Toy network: 1 hidden layer, fully connected, DCG
		i1 = Node<TensorT>("0", NodeType::input, NodeStatus::activated, std::shared_ptr<ActivationOp<float>>(new LinearOp<float>()), std::shared_ptr<ActivationOp<float>>(new LinearGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		h1 = Node<TensorT>("1", NodeType::hidden, NodeStatus::initialized, std::shared_ptr<ActivationOp<float>>(new ELUOp<float>()), std::shared_ptr<ActivationOp<float>>(new ELUGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		o1 = Node<TensorT>("2", NodeType::output, NodeStatus::initialized, std::shared_ptr<ActivationOp<float>>(new ELUOp<float>()), std::shared_ptr<ActivationOp<float>>(new ELUGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		b1 = Node<TensorT>("3", NodeType::bias, NodeStatus::activated, std::shared_ptr<ActivationOp<float>>(new LinearOp<float>()), std::shared_ptr<ActivationOp<float>>(new LinearGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		b2 = Node<TensorT>("4", NodeType::bias, NodeStatus::activated, std::shared_ptr<ActivationOp<float>>(new LinearOp<float>()), std::shared_ptr<ActivationOp<float>>(new LinearGradOp<float>()), std::shared_ptr<IntegrationOp<float>>(new SumOp<float>()), std::shared_ptr<IntegrationErrorOp<float>>(new SumErrorOp<float>()), std::shared_ptr<IntegrationWeightGradOp<float>>(new SumWeightGradOp<float>()));
		// weights  
		std::shared_ptr<WeightInitOp<TensorT>> weight_init;
		std::shared_ptr<SolverOp<TensorT>> solver;
		// weight_init.reset(new RandWeightInitOp(1.0)); // No random init for testing
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<TensorT>(0.01, 0.9, 0.999, 1e-8));
		w1 = Weight<TensorT>("0", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<TensorT>(0.01, 0.9, 0.999, 1e-8));
		w2 = Weight<TensorT>("1", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<TensorT>(0.01, 0.9, 0.999, 1e-8));
		w3 = Weight<TensorT>("2", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<TensorT>(0.01, 0.9, 0.999, 1e-8));
		wb1 = Weight<TensorT>("3", weight_init, solver);
		weight_init.reset(new ConstWeightInitOp<TensorT>(1.0));
		solver.reset(new AdamOp<TensorT>(0.01, 0.9, 0.999, 1e-8));
		wb2 = Weight<TensorT>("4", weight_init, solver);
		weight_init.reset();
		solver.reset();
		// links
		l1 = Link("0", "0", "1", "0");
		l2 = Link("1", "1", "2", "1");
		l3 = Link("2", "1", "1", "2");
		lb1 = Link("3", "3", "1", "3");
		lb2 = Link("4", "4", "2", "4");
		model2.setId(2);
		model2.addNodes({ i1, h1, o1, b1, b2 });
		model2.addWeights({ w1, w2, w3, wb1, wb2 });
		model2.addLinks({ l1, l2, l3, lb1, lb2 });
		return model2;
	}
	void adaptiveTrainerScheduler(
		const int& n_generations,
		const int& n_epochs,
		Model<TensorT>& model,
		const std::vector<TensorT>& model_errors) {}
};

BOOST_AUTO_TEST_CASE(DCGToy) 
{

  // Define the makeModel and trainModel scripts
  DCGToyModelTrainer<float> trainer;

	// Define the model resources
	ModelResources model_resources = { ModelDevice(0, 1) };

  // Test parameters
  trainer.setBatchSize(5);
  trainer.setMemorySize(8);
  trainer.setNEpochsTraining(50);
	trainer.setNEpochsValidation(50);
  const std::vector<std::string> input_nodes = {"0", "3", "4"}; // true inputs + biases
  const std::vector<std::string> output_nodes = {"2"};
	trainer.setLossFunctions({ std::shared_ptr<LossFunctionOp<float>>(new MSEOp<float>()) });
	trainer.setLossFunctionGrads({ std::shared_ptr<LossFunctionGradOp<float>>(new MSEGradOp<float>()) });
	trainer.setOutputNodes({ output_nodes });

  // Make the input data
  Eigen::Tensor<float, 4> input_data(trainer.getBatchSize(), trainer.getMemorySize(), (int)input_nodes.size(), trainer.getNEpochsTraining());
  Eigen::Tensor<float, 3> input_tmp(trainer.getBatchSize(), trainer.getMemorySize(), (int)input_nodes.size()); 
  input_tmp.setValues(
		{ {{8, 0, 0}, {7, 0, 0}, {6, 0, 0}, {5, 0, 0}, {4, 0, 0}, {3, 0, 0}, {2, 0, 0}, {1, 0, 0}},
		{{9, 0, 0}, {8, 0, 0}, {7, 0, 0}, {6, 0, 0}, {5, 0, 0}, {4, 0, 0}, {3, 0, 0}, {2, 0, 0}},
		{{10, 0, 0}, {9, 0, 0}, {8, 0, 0}, {7, 0, 0}, {6, 0, 0}, {5, 0, 0}, {4, 0, 0}, {3, 0, 0}},
		{{11, 0, 0}, {10, 0, 0}, {9, 0, 0}, {8, 0, 0}, {7, 0, 0}, {6, 0, 0}, {5, 0, 0}, {4, 0, 0}},
		{{12, 0, 0}, {11, 0, 0}, {10, 0, 0}, {9, 0, 0}, {8, 0, 0}, {7, 0, 0}, {6, 0, 0}, {5, 0, 0}} }
  );
  for (int batch_iter=0; batch_iter<trainer.getBatchSize(); ++batch_iter)
    for (int memory_iter=0; memory_iter<trainer.getMemorySize(); ++memory_iter)
      for (int nodes_iter=0; nodes_iter<(int)input_nodes.size(); ++nodes_iter)
        for (int epochs_iter=0; epochs_iter<trainer.getNEpochsTraining(); ++epochs_iter)
          input_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = input_tmp(batch_iter, memory_iter, nodes_iter);
  
  // Make the output data
  Eigen::Tensor<float, 4> output_data(trainer.getBatchSize(), trainer.getMemorySize(), (int)output_nodes.size(), trainer.getNEpochsTraining());
  Eigen::Tensor<float, 3> output_tmp(trainer.getBatchSize(), trainer.getMemorySize(), (int)output_nodes.size()); 
  output_tmp.setValues(
		{ { { 4 },{ 4 },{ 3 },{ 3 },{ 2 },{ 2 },{ 1 },{ 1 } },
		{ { 5 },{ 4 },{ 4 },{ 3 },{ 3 },{ 2 },{ 2 },{ 1 } },
		{ { 5 },{ 5 },{ 4 },{ 4 },{ 3 },{ 3 },{ 2 },{ 2 } },
		{ { 6 },{ 5 },{ 5 },{ 4 },{ 4 },{ 3 },{ 3 },{ 2 } },
		{ { 6 },{ 6 },{ 5 },{ 5 },{ 4 },{ 4 },{ 3 },{ 3 } } });
  for (int batch_iter=0; batch_iter<trainer.getBatchSize(); ++batch_iter)
		for (int memory_iter = 0; memory_iter<trainer.getMemorySize(); ++memory_iter)
			for (int nodes_iter=0; nodes_iter<(int)output_nodes.size(); ++nodes_iter)
				for (int epochs_iter=0; epochs_iter<trainer.getNEpochsTraining(); ++epochs_iter)
					output_data(batch_iter, memory_iter, nodes_iter, epochs_iter) = output_tmp(batch_iter, memory_iter, nodes_iter);

  // Make the simulation time_steps
  Eigen::Tensor<float, 3> time_steps(trainer.getBatchSize(), trainer.getMemorySize(), trainer.getNEpochsTraining());
  Eigen::Tensor<float, 2> time_steps_tmp(trainer.getBatchSize(), trainer.getMemorySize()); 
  time_steps_tmp.setValues({
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}}
  );
  for (int batch_iter=0; batch_iter<trainer.getBatchSize(); ++batch_iter)
    for (int memory_iter=0; memory_iter<trainer.getMemorySize(); ++memory_iter)
      for (int epochs_iter=0; epochs_iter<trainer.getNEpochsTraining(); ++epochs_iter)
        time_steps(batch_iter, memory_iter, epochs_iter) = time_steps_tmp(batch_iter, memory_iter);

  Model<float> model1 = trainer.makeModel();

  trainer.trainModel(model1, input_data, output_data, time_steps,
    input_nodes, ModelLogger<float>(), ModelInterpreterDefaultDevice<float>(model_resources));

  const Eigen::Tensor<float, 0> total_error = model1.getError().sum();
  BOOST_CHECK(total_error(0) <= 1492.6);

	// TODO validateModel
	// TODO evaluateModel
}

BOOST_AUTO_TEST_SUITE_END()
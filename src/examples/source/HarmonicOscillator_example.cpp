/**TODO:  Add copyright*/

#include <SmartPeak/ml/PopulationTrainerDefaultDevice.h>
#include <SmartPeak/ml/ModelTrainerDefaultDevice.h>
#include <SmartPeak/ml/ModelReplicator.h>
#include <SmartPeak/ml/ModelBuilder.h>
#include <SmartPeak/ml/Model.h>
#include <SmartPeak/io/PopulationTrainerFile.h>
#include <SmartPeak/io/ModelInterpreterFileDefaultDevice.h>
#include <SmartPeak/io/Parameter.h>
#include <SmartPeak/simulator/HarmonicOscillatorSimulator.h>

#include <unsupported/Eigen/CXX11/Tensor>

using namespace SmartPeak;

template<typename TensorT>
class DataSimulatorExt : public HarmonicOscillatorSimulator<TensorT>
{
public:
  std::string simulation_name_ = std::string("WeightSpring1W1S1DwDamping");
  void simulateDataWeightSpring3W2S1D(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
  {
    // infer data dimensions based on the input tensors
    const int batch_size = input_data.dimension(0);
    const int memory_size = input_data.dimension(1);
    const int n_input_nodes = input_data.dimension(2);
    const int n_output_nodes = output_data.dimension(2);
    const int n_epochs = input_data.dimension(3);

    HarmonicOscillatorSimulator<float> WeightSpring;
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution<> dist{ 0.0f, 1.0f };

    // Generate the input and output data for training
    for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
      for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {

        // Simulate a 3 weight and 2 spring 1D harmonic system
        // where the middle weight has been displaced by a random amount
        Eigen::Tensor<float, 1> time_steps(memory_size);
        Eigen::Tensor<float, 2> displacements(memory_size, 3);
        WeightSpring.WeightSpring3W2S1D(time_steps, displacements, memory_size, 0.1,
          1, 1, 1, //A
          1, 1, 1, //m
          0, dist(gen), 0, //xo
          1);

        for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
          if (memory_iter >= memory_size - 1)	input_data(batch_iter, memory_iter, 0, epochs_iter) = displacements(memory_size - 1 - memory_iter, 1); // m2
          else input_data(batch_iter, memory_iter, 0, epochs_iter) = TensorT(0);
          output_data(batch_iter, memory_iter, 0, epochs_iter) = displacements(memory_size - 1 - memory_iter, 0); // m1
          output_data(batch_iter, memory_iter, 1, epochs_iter) = displacements(memory_size - 1 - memory_iter, 2); // m3
        }
      }
    }
    time_steps.setConstant(1.0f);
  }
  void simulateDataWeightSpring3W2S1D(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& output_data, Eigen::Tensor<TensorT, 3>& metric_output_data, Eigen::Tensor<TensorT, 2>& time_steps)
  {
    // infer data dimensions based on the input tensors
    const int batch_size = input_data.dimension(0);
    const int memory_size = input_data.dimension(1);
    const int n_input_nodes = input_data.dimension(2);
    const int n_output_nodes = output_data.dimension(2);

    HarmonicOscillatorSimulator<float> WeightSpring;
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution<> dist{ 0.0f, 1.0f };

    // Generate the input and output data for training
    for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {

      // Simulate a 3 weight and 2 spring 1D harmonic system
      // where the middle weight has been displaced by a random amount
      Eigen::Tensor<float, 1> time_steps(memory_size);
      Eigen::Tensor<float, 2> displacements(memory_size, 3);
      WeightSpring.WeightSpring3W2S1D(time_steps, displacements, memory_size, 0.1,
        1, 1, 1, //A
        1, 1, 1, //m
        0, dist(gen), 0, //xo
        1);

      for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
        if (memory_iter >= memory_size - 1)	input_data(batch_iter, memory_iter, 0) = displacements(memory_size - 1 - memory_iter, 1); // m2
        else input_data(batch_iter, memory_iter, 0) = TensorT(0);
        output_data(batch_iter, memory_iter, 0) = displacements(memory_size - 1 - memory_iter, 0); // m1
        output_data(batch_iter, memory_iter, 1) = displacements(memory_size - 1 - memory_iter, 2); // m3
      }
    }
    time_steps.setConstant(1.0f);
  }
  void simulateDataWeightSpring1W1S1D(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
  {
    // infer data dimensions based on the input tensors
    const int batch_size = input_data.dimension(0);
    const int memory_size = input_data.dimension(1);
    const int n_input_nodes = input_data.dimension(2);
    const int n_output_nodes = output_data.dimension(2);
    const int n_epochs = input_data.dimension(3);

    HarmonicOscillatorSimulator<float> WeightSpring;
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution<> dist{ 0.0f, 1.0f };

    const int time_course_multiplier = 2; // How long to make the time course based on the memory size
    const int n_batches_per_time_course = 4; // The number of chunks each simulation time course is chopped into
    const int time_steps_size = ((memory_size > n_batches_per_time_course) ? memory_size : n_batches_per_time_course)* time_course_multiplier + 1; // The total number of time_steps per simulation time course
    Eigen::Tensor<float, 1> time_steps_displacements(time_steps_size);
    Eigen::Tensor<float, 2> displacements_all(time_steps_size, 1);

    // Generate the input and output data for training
    for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
      for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {

        // Simulate a 1 weight and 1 spring 1D harmonic system where the weight has been displaced by a random amount
        const int remainder = batch_iter % n_batches_per_time_course;
        const int increment = (time_course_multiplier * memory_size - memory_size) / (n_batches_per_time_course - 1);
        if (remainder == 0) {
          WeightSpring.WeightSpring1W1S1D(time_steps_displacements, displacements_all, time_steps_size, 0.1,
            1, 1, dist(gen), 0);
        }
        Eigen::array<Eigen::Index, 2> offset = { increment * remainder, 0 };
        Eigen::array<Eigen::Index, 2> span = { memory_size + 1, 1 };
        Eigen::Tensor<float, 2> displacements = displacements_all.slice(offset, span);

        for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
          if (memory_iter < 1)	input_data(batch_iter, memory_size - 1 - memory_iter, 0, epochs_iter) = displacements(memory_iter, 0);
          else input_data(batch_iter, memory_size - 1 - memory_iter, 0, epochs_iter) = TensorT(0);
          output_data(batch_iter, memory_size - 1 - memory_iter, 0, epochs_iter) = displacements(memory_iter + 1, 0);
        }
      }
    }
    time_steps.setConstant(1.0f);
  }
  void simulateDataWeightSpring1W1S1D(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& output_data, Eigen::Tensor<TensorT, 3>& metric_output_data, Eigen::Tensor<TensorT, 2>& time_steps)
  {
    // infer data dimensions based on the input tensors
    const int batch_size = input_data.dimension(0);
    const int memory_size = input_data.dimension(1);
    const int n_input_nodes = input_data.dimension(2);
    const int n_output_nodes = output_data.dimension(2);

    HarmonicOscillatorSimulator<float> WeightSpring;
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution<> dist{ 0.0f, 1.0f };

    const int time_course_multiplier = 2; // How long to make the time course based on the memory size
    const int n_batches_per_time_course = 4; // The number of chunks each simulation time course is chopped into
    const int time_steps_size = ((memory_size > n_batches_per_time_course) ? memory_size : n_batches_per_time_course)* time_course_multiplier + 1; // The total number of time_steps per simulation time course
    Eigen::Tensor<float, 1> time_steps_displacements(time_steps_size);
    Eigen::Tensor<float, 2> displacements_all(time_steps_size, 1);

    // Generate the input and output data for training
    for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {

      // Simulate a 1 weight and 1 spring 1D harmonic system where the weight has been displaced by a random amount
      const int remainder = batch_iter % n_batches_per_time_course;
      const int increment = (time_course_multiplier * memory_size - memory_size) / (n_batches_per_time_course - 1);
      if (remainder == 0) {
        WeightSpring.WeightSpring1W1S1D(time_steps_displacements, displacements_all, time_steps_size, 0.1,
          1, 1, dist(gen), 0);
      }
      Eigen::array<Eigen::Index, 2> offset = { increment * remainder, 0 };
      Eigen::array<Eigen::Index, 2> span = { memory_size + 1, 1 };
      Eigen::Tensor<float, 2> displacements = displacements_all.slice(offset, span);

      for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
        if (memory_iter < 1)	input_data(batch_iter, memory_size - 1 - memory_iter, 0) = displacements(memory_iter, 0);
        else input_data(batch_iter, memory_size - 1 - memory_iter, 0) = TensorT(0);
        output_data(batch_iter, memory_size - 1 - memory_iter, 0) = displacements(memory_iter + 1, 0); // The next time point
        metric_output_data(batch_iter, memory_size - 1 - memory_iter, 0) = displacements(memory_iter + 1, 0); // The next time point
      }
    }
    time_steps.setConstant(1.0f);
  }
  void simulateDataWeightSpring1W1S1DwDamping(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
  {
    // infer data dimensions based on the input tensors
    const int batch_size = input_data.dimension(0);
    const int memory_size = input_data.dimension(1);
    const int n_input_nodes = input_data.dimension(2);
    const int n_output_nodes = output_data.dimension(2);
    const int n_epochs = input_data.dimension(3);

    HarmonicOscillatorSimulator<float> WeightSpring;
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution<> dist{ 0.0f, 1.0f };

    // Generate the input and output data for training
    for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {
      for (int epochs_iter = 0; epochs_iter < n_epochs; ++epochs_iter) {

        // Simulate a 1 weight and 1 spring 1D harmonic system
        // where the weight has been displaced by a random amount
        Eigen::Tensor<float, 1> time_steps(memory_size + 1);
        Eigen::Tensor<float, 2> displacements(memory_size + 1, 1);
        WeightSpring.WeightSpring1W1S1DwDamping(time_steps, displacements, memory_size + 1, 0.1,
          1, 1, 0.5, dist(gen), 0);

        for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
          if (memory_iter < 1)	input_data(batch_iter, memory_size - 1 - memory_iter, 0, epochs_iter) = displacements(memory_iter, 0);
          else input_data(batch_iter, memory_size - 1 - memory_iter, 0, epochs_iter) = TensorT(0);
          output_data(batch_iter, memory_size - 1 - memory_iter, 0, epochs_iter) = displacements(memory_iter + 1, 0);
        }
      }
    }
    time_steps.setConstant(1.0f);
  }
  void simulateDataWeightSpring1W1S1DwDamping(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& output_data, Eigen::Tensor<TensorT, 3>& metric_output_data, Eigen::Tensor<TensorT, 2>& time_steps)
  {
    // infer data dimensions based on the input tensors
    const int batch_size = input_data.dimension(0);
    const int memory_size = input_data.dimension(1);
    const int n_input_nodes = input_data.dimension(2);
    const int n_output_nodes = output_data.dimension(2);

    HarmonicOscillatorSimulator<float> WeightSpring;
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution<> dist{ 0.0f, 1.0f };

    const int time_course_multiplier = 2; // How long to make the time course based on the memory size
    const int n_batches_per_time_course = 4; // The number of chunks each simulation time course is chopped into
    const int time_steps_size = ((memory_size > n_batches_per_time_course) ? memory_size : n_batches_per_time_course)* time_course_multiplier + 1; // The total number of time_steps per simulation time course
    Eigen::Tensor<float, 1> time_steps_displacements(time_steps_size);
    Eigen::Tensor<float, 2> displacements_all(time_steps_size, 1);

    // Generate the input and output data for training
    for (int batch_iter = 0; batch_iter < batch_size; ++batch_iter) {

      // Simulate a 1 weight and 1 spring 1D harmonic system where the weight has been displaced by a random amount
      const int remainder = batch_iter % n_batches_per_time_course;
      const int increment = (time_steps_size - 1 - memory_size) / (n_batches_per_time_course - 1);
      if (remainder == 0) {
        WeightSpring.WeightSpring1W1S1DwDamping(time_steps_displacements, displacements_all, time_steps_size, 0.1,
          1, 1, 0.5, dist(gen), 0);
      }
      Eigen::array<Eigen::Index, 2> offset = { increment * remainder, 0 };
      Eigen::array<Eigen::Index, 2> span = { memory_size + 1, 1 };
      Eigen::Tensor<float, 2> displacements = displacements_all.slice(offset, span);

      for (int memory_iter = 0; memory_iter < memory_size; ++memory_iter) {
        if (memory_iter < 1)	input_data(batch_iter, memory_size - 1 - memory_iter, 0) = displacements(memory_iter, 0);
        else input_data(batch_iter, memory_size - 1 - memory_iter, 0) = TensorT(0);
        output_data(batch_iter, memory_size - 1 - memory_iter, 0) = displacements(memory_iter + 1, 0); // The next time point
      }
    }
    time_steps.setConstant(1.0f);
  }

  void simulateTrainingData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
  {
    if (simulation_name_ == "WeightSpring1W1S1D")	simulateDataWeightSpring1W1S1D(input_data, output_data, time_steps);
    else if (simulation_name_ == "WeightSpring1W1S1DwDamping")	simulateDataWeightSpring1W1S1DwDamping(input_data, output_data, time_steps);
    else if (simulation_name_ == "WeightSpring3W2S1D")	simulateDataWeightSpring3W2S1D(input_data, output_data, time_steps);
  }
  void simulateTrainingData(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& output_data, Eigen::Tensor<TensorT, 3>& metric_output_data, Eigen::Tensor<TensorT, 2>& time_steps)
  {
    if (simulation_name_ == "WeightSpring1W1S1D")	simulateDataWeightSpring1W1S1D(input_data, output_data, metric_output_data, time_steps);
    else if (simulation_name_ == "WeightSpring1W1S1DwDamping")	simulateDataWeightSpring1W1S1DwDamping(input_data, output_data, metric_output_data, time_steps);
    else if (simulation_name_ == "WeightSpring3W2S1D")	simulateDataWeightSpring3W2S1D(input_data, output_data, metric_output_data, time_steps);
  }
  void simulateValidationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps)
  {
    if (simulation_name_ == "WeightSpring1W1S1D")	simulateDataWeightSpring1W1S1D(input_data, output_data, time_steps);
    else if (simulation_name_ == "WeightSpring1W1S1DwDamping")	simulateDataWeightSpring1W1S1DwDamping(input_data, output_data, time_steps);
    else if (simulation_name_ == "WeightSpring3W2S1D")	simulateDataWeightSpring3W2S1D(input_data, output_data, time_steps);
  }
  void simulateValidationData(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& output_data, Eigen::Tensor<TensorT, 3>& metric_output_data, Eigen::Tensor<TensorT, 2>& time_steps)
  {
    if (simulation_name_ == "WeightSpring1W1S1D")	simulateDataWeightSpring1W1S1D(input_data, output_data, metric_output_data, time_steps);
    else if (simulation_name_ == "WeightSpring1W1S1DwDamping")	simulateDataWeightSpring1W1S1DwDamping(input_data, output_data, metric_output_data, time_steps);
    else if (simulation_name_ == "WeightSpring3W2S1D")	simulateDataWeightSpring3W2S1D(input_data, output_data, metric_output_data, time_steps);
  }
  void simulateEvaluationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 3>& time_steps) {};
  void simulateEvaluationData(Eigen::Tensor<TensorT, 3>& input_data, Eigen::Tensor<TensorT, 3>& metric_output_data, Eigen::Tensor<TensorT, 2>& time_steps) {
    // HACK: using output_data as metric_output_data
    if (simulation_name_ == "WeightSpring1W1S1D")	simulateDataWeightSpring1W1S1D(input_data, metric_output_data, Eigen::Tensor<TensorT, 3>(), time_steps);
    else if (simulation_name_ == "WeightSpring1W1S1DwDamping")	simulateDataWeightSpring1W1S1DwDamping(input_data, metric_output_data, Eigen::Tensor<TensorT, 3>(), time_steps);
    else if (simulation_name_ == "WeightSpring3W2S1D")	simulateDataWeightSpring3W2S1D(input_data, metric_output_data, Eigen::Tensor<TensorT, 3>(), time_steps);
  };
};

// Extended classes
template<typename TensorT>
class ModelTrainerExt : public ModelTrainerDefaultDevice<TensorT>
{
public:
  /*
  @brief Interaction graph network for linear harmonic oscillator systems consisting of springs, masses, and a fixed wall tethered to one of the springs with or without damping

  each mass will get its own input and output

  @param[in] model
  @param[in] n_masses The number of masses
  @param[in] n_springs The number of springs
  @param[in] n_fc_0 (Optional) The number of layers in the first fully connected layer
  @param[in] n_fc_1 (Optional) The number of layers in the second fully connected layer
  */
  void makeHarmonicOscillator1D(Model<TensorT>& model, const int& n_masses, const int& n_fc_1, const int& n_fc_2, const bool& add_biases, const bool& specify_layers) {
    model.setId(0);
    model.setName("HarmonicOscillator1D");
    ModelBuilder<TensorT> model_builder;

    // Define the node activation
    auto activation = std::make_shared<LeakyReLUOp<TensorT>>(LeakyReLUOp<TensorT>());
    auto activation_grad = std::make_shared<LeakyReLUGradOp<TensorT>>(LeakyReLUGradOp<TensorT>());
    auto activation_masses = std::make_shared<LinearOp<TensorT>>(LinearOp<TensorT>());
    auto activation_masses_grad = std::make_shared<LinearGradOp<TensorT>>(LinearGradOp<TensorT>());

    // Define the node integration
    auto integration_op = std::make_shared<SumOp<TensorT>>(SumOp<TensorT>());
    auto integration_error_op = std::make_shared<SumErrorOp<TensorT>>(SumErrorOp<TensorT>());
    auto integration_weight_grad_op = std::make_shared<SumWeightGradOp<TensorT>>(SumWeightGradOp<TensorT>());

    // Define the solver and weight init
    auto weight_init = std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1.0));
    auto solver_op = std::make_shared<AdamOp<TensorT>>(AdamOp<TensorT>(1e-5, 0.9, 0.999, 1e-8, 10));

    // Make the input nodes
    std::vector<std::string> node_names_input = model_builder.addInputNodes(model, "Input", "Input", n_masses, specify_layers);

    // Connect the input nodes to the masses
    std::vector<std::string> node_names_masses_t0 = model_builder.addSinglyConnected(model, "Mass(t)", "Mass(t)", node_names_input, n_masses,
      activation_masses, activation_masses_grad,
      integration_op, integration_error_op, integration_weight_grad_op,
      std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1)),
      std::make_shared<DummySolverOp<TensorT>>(DummySolverOp<TensorT>()), 0.0f, 0.0f, add_biases, specify_layers);
    for (const std::string& node_name : node_names_masses_t0)
      model.getNodesMap().at(node_name)->setType(NodeType::unmodifiable);

    // Make the mass(t+1) nodes
    std::vector<std::string> node_names_masses_t1 = model_builder.addHiddenNodes(model, "Mass(t+1)", "Mass(t+1)", n_masses,
      activation_masses, activation_masses_grad,
      integration_op, integration_error_op, integration_weight_grad_op,
      solver_op, 0.0f, 0.0f, add_biases, specify_layers);
    //// Connect the mass(t) nodes to the mass(t+1) nodes
    //std::vector<std::string> node_names_masses_t1 = model_builder.addSinglyConnected(model, "Mass(t+1)", "Mass(t+1)", node_names_masses_t0, n_masses,
    //  activation_masses, activation_masses_grad,
    //  integration_op, integration_error_op, integration_weight_grad_op,
    //  std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1)),
    //  std::make_shared<DummySolverOp<TensorT>>(DummySolverOp<TensorT>()), 0.0f, 0.0f, add_biases, specify_layers);
    for (const std::string& node_name : node_names_masses_t1)
      model.getNodesMap().at(node_name)->setType(NodeType::unmodifiable);

    // Connect the mass(t+1) nodes to the mass(t) nodes
    model_builder.addSinglyConnected(model, "Mass", node_names_masses_t1, node_names_masses_t0,
      std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1)),
      std::make_shared<DummySolverOp<TensorT>>(DummySolverOp<TensorT>()), 0.0f, specify_layers);
    for (int i = 0; i < n_masses; ++i)
      model.addCyclicPairs(std::make_pair(node_names_masses_t1.at(i), node_names_masses_t0.at(i)));

    // Connect the mass to the output nodes
    std::vector<std::string> node_names_output = model_builder.addSinglyConnected(model, "Output", "Output", node_names_masses_t1, n_masses,
      std::make_shared<LinearOp<TensorT>>(LinearOp<TensorT>()),
      std::make_shared<LinearGradOp<TensorT>>(LinearGradOp<TensorT>()),
      integration_op, integration_error_op, integration_weight_grad_op,
      std::make_shared<ConstWeightInitOp<TensorT>>(ConstWeightInitOp<TensorT>(1)),
      std::make_shared<DummySolverOp<TensorT>>(DummySolverOp<TensorT>()), 0.0f, 0.0f, add_biases, specify_layers);
    for (const std::string& node_name : node_names_output)
      model.getNodesMap().at(node_name)->setType(NodeType::output);

    // Make the gravity and wall nodes
    std::vector<std::string> node_names_wall = model_builder.addInputNodes(model, "Wall", "Input", 1, specify_layers);
    model.getNodesMap().at(node_names_wall.front())->setType(NodeType::bias);

    // Make the deep learning layers between each of the masses
    for (int mass_iter = 0; mass_iter < n_masses; ++mass_iter) {
      std::vector<std::string> node_names;
      // determine the input nodes
      if (mass_iter == 0 && mass_iter == n_masses - 1) {
        node_names = { node_names_wall.front(), node_names_masses_t0.at(mass_iter) };
      }
      else if (mass_iter == 0) {
        node_names = { node_names_wall.front(), node_names_masses_t0.at(mass_iter), node_names_masses_t0.at(mass_iter + 1) };
      }
      else if (mass_iter == n_masses - 1) {
        node_names = { node_names_masses_t0.at(mass_iter - 1), node_names_masses_t0.at(mass_iter) };
      }
      else {
        node_names = { node_names_masses_t0.at(mass_iter - 1), node_names_masses_t0.at(mass_iter), node_names_masses_t0.at(mass_iter + 1) };
      }

      // make the FC layers between input nodes
      if (n_fc_1 > 0) {
        node_names = model_builder.addFullyConnected(model, "FC1Forward", "FC1Forward", node_names, n_fc_1,
          activation, activation_grad, integration_op, integration_error_op, integration_weight_grad_op,
          std::make_shared<RandWeightInitOp<TensorT>>(RandWeightInitOp<TensorT>(node_names.size() + n_fc_1, 2)), //weight_init, 
          solver_op, 0.0f, 0.0f, add_biases, specify_layers);
      }
      if (n_fc_2 > 0) {
        node_names = model_builder.addFullyConnected(model, "FC2Forward", "FC2Forward", node_names, n_fc_2,
          activation, activation_grad, integration_op, integration_error_op, integration_weight_grad_op,
          std::make_shared<RandWeightInitOp<TensorT>>(RandWeightInitOp<TensorT>(node_names.size() + n_fc_2, 2)), //weight_init,
          solver_op, 0.0f, 0.0f, add_biases, specify_layers);
      }
      model_builder.addFullyConnected(model, "FC0Forward", node_names, std::vector<std::string>({ node_names_masses_t1.at(mass_iter) }),
        std::make_shared<RandWeightInitOp<TensorT>>(RandWeightInitOp<TensorT>(node_names.size() + 1, 2)), //weight_init,
        solver_op, 0.0f, specify_layers);
    }

    model.setInputAndOutputNodes();
  }
  void adaptiveTrainerScheduler(
    const int& n_generations,
    const int& n_epochs,
    Model<TensorT>& model,
    ModelInterpreterDefaultDevice<TensorT>& model_interpreter,
    const std::vector<float>& model_errors) {
    // Check point the model every 1000 epochs
    if (n_epochs % 1000 == 0 && n_epochs != 0) {
      model_interpreter.getModelResults(model, false, true, false, false);
      ModelFile<TensorT> data;
      data.storeModelBinary(model.getName() + "_" + std::to_string(n_epochs) + "_model.binary", model);
      ModelInterpreterFileDefaultDevice<TensorT> interpreter_data;
      interpreter_data.storeModelInterpreterBinary(model.getName() + "_" + std::to_string(n_epochs) + "_interpreter.binary", model_interpreter);
    }
  }
  void trainingModelLogger(const int& n_epochs, Model<TensorT>& model, ModelInterpreterDefaultDevice<TensorT>& model_interpreter, ModelLogger<TensorT>& model_logger,
    const Eigen::Tensor<TensorT, 3>& expected_values, const std::vector<std::string>& output_nodes, const std::vector<std::string>& input_nodes, const TensorT& model_error_train, const TensorT& model_error_test,
    const Eigen::Tensor<TensorT, 1>& model_metrics_train, const Eigen::Tensor<TensorT, 1>& model_metrics_test) override {
    // Set the defaults
    model_logger.setLogTimeEpoch(true);
    model_logger.setLogTrainValMetricEpoch(true);
    model_logger.setLogExpectedEpoch(false);
    model_logger.setLogNodeInputsEpoch(false);
    model_logger.setLogNodeOutputsEpoch(false);

    // initialize all logs
    if (n_epochs == 0) {
      model_logger.setLogExpectedEpoch(true);
      model_logger.setLogNodeInputsEpoch(true);
      model_logger.setLogNodeOutputsEpoch(true);
      model_logger.initLogs(model);
    }

    // Per n epoch logging
    if (n_epochs % 1000 == 0) { // FIXME
      model_logger.setLogExpectedEpoch(true);
      model_logger.setLogNodeInputsEpoch(true);
      model_logger.setLogNodeOutputsEpoch(true);
      model_interpreter.getModelResults(model, true, false, false, true);
    }

    // Create the metric headers and data arrays
    std::vector<std::string> log_train_headers = { "Train_Error" };
    std::vector<std::string> log_test_headers = { "Test_Error" };
    std::vector<TensorT> log_train_values = { model_error_train };
    std::vector<TensorT> log_test_values = { model_error_test };
    int metric_iter = 0;
    for (const std::string& metric_name : this->getMetricNamesLinearized()) {
      log_train_headers.push_back(metric_name);
      log_test_headers.push_back(metric_name);
      log_train_values.push_back(model_metrics_train(metric_iter));
      log_test_values.push_back(model_metrics_test(metric_iter));
      ++metric_iter;
    }
    model_logger.writeLogs(model, n_epochs, log_train_headers, log_test_headers, log_train_values, log_test_values, output_nodes, expected_values, {}, output_nodes, {}, input_nodes, {});
  }
  void evaluationModelLogger(const int& n_epochs, Model<TensorT>& model, ModelInterpreterDefaultDevice<TensorT>& model_interpreter, ModelLogger<TensorT>& model_logger,
    const Eigen::Tensor<TensorT, 3>& expected_values, const std::vector<std::string>& output_nodes, const std::vector<std::string>& input_nodes, const Eigen::Tensor<TensorT, 1>& model_metrics) override
  {
    // Set the defaults
    model_logger.setLogTimeEpoch(true);
    model_logger.setLogTrainValMetricEpoch(true);
    model_logger.setLogExpectedEpoch(false);
    model_logger.setLogNodeInputsEpoch(false);
    model_logger.setLogNodeOutputsEpoch(false);

    // initialize all logs
    if (n_epochs == 0) {
      model_logger.setLogExpectedEpoch(true);
      model_logger.setLogNodeInputsEpoch(true);
      model_logger.setLogNodeOutputsEpoch(true);
      model_logger.initLogs(model);
    }

    // Per n epoch logging
    if (n_epochs % 1000 == 0) { // FIXME
      model_logger.setLogExpectedEpoch(true);
      model_logger.setLogNodeInputsEpoch(true);
      model_logger.setLogNodeOutputsEpoch(true);
      model_interpreter.getModelResults(model, true, false, false, true);
    }

    // Create the metric headers and data arrays
    std::vector<std::string> log_headers;
    std::vector<TensorT> log_values;
    int metric_iter = 0;
    for (const std::string& metric_name : this->getMetricNamesLinearized()) {
      log_headers.push_back(metric_name);
      log_values.push_back(model_metrics(metric_iter));
      ++metric_iter;
    }
    model_logger.writeLogs(model, n_epochs, log_headers, {}, log_values, {}, output_nodes, expected_values, {}, output_nodes, {}, input_nodes, {});
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
    if (n_generations > 0)
    {
      this->setRandomModifications(
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 2), // node activation changes
        std::make_pair(0, 0), // node integration changes
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0));
    }
    else
    {
      this->setRandomModifications(
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 2), // node activation changes
        std::make_pair(0, 0), // node integration changes
        std::make_pair(0, 0),
        std::make_pair(0, 0),
        std::make_pair(0, 0));
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
    //// Population size of 16
    //if (n_generations == 0)	{
    //	this->setNTop(3);
    //	this->setNRandom(3);
    //	this->setNReplicatesPerModel(15);
    //}
    //else {
    //	this->setNTop(3);
    //	this->setNRandom(3);
    //	this->setNReplicatesPerModel(3);
    //}
    // Population size of 30
    if (n_generations == 0) {
      this->setNTop(5);
      this->setNRandom(5);
      this->setNReplicatesPerModel(29);
    }
    else {
      this->setNTop(5);
      this->setNRandom(5);
      this->setNReplicatesPerModel(5);
    }
  }
  void trainingPopulationLogger(
    const int& n_generations,
    std::vector<Model<TensorT>>& models,
    PopulationLogger<TensorT>& population_logger,
    const std::vector<std::tuple<int, std::string, TensorT>>& models_validation_errors_per_generation) {
    // Export the selected models
    for (auto& model : models) {
      ModelFile<TensorT> data;
      data.storeModelCsv(model.getName() + "_" + std::to_string(n_generations) + "_nodes.csv",
        model.getName() + "_" + std::to_string(n_generations) + "_links.csv",
        model.getName() + "_" + std::to_string(n_generations) + "_weights.csv", model);
    }
    // Log the population statistics
    population_logger.writeLogs(n_generations, models_validation_errors_per_generation);
  }
};

template<class ...ParameterTypes>
void main_HarmonicOscillator1D(const ParameterTypes& ...args) {
  auto parameters = std::make_tuple(args...);

  // define the population trainer parameters
  PopulationTrainerExt<float> population_trainer;
  population_trainer.setNGenerations(std::get<NGenerations>(parameters).get()); // population training
  population_trainer.setLogging(true);
  population_trainer.setResetModelCopyWeights(true);

  // define the population logger
  PopulationLogger<float> population_logger(true, true);

  // define the multithreading parameters
  const int n_hard_threads = std::thread::hardware_concurrency();
  const int n_threads = (std::get<NInterpreters>(parameters).get() > n_hard_threads) ? n_hard_threads : std::get<NInterpreters>(parameters).get(); // the number of threads

  // Make the input nodes
  const int n_masses = 1;
  std::vector<std::string> input_nodes;
  for (int i = 0; i < n_masses; ++i) {
    char name_char[512];
    sprintf(name_char, "Input_%012d", i);
    std::string name(name_char);
    input_nodes.push_back(name);
  }

  // Make the output nodes
  std::vector<std::string> output_nodes;
  for (int i = 0; i < n_masses; ++i) {
    char name_char[512];
    sprintf(name_char, "Output_%012d", i);
    std::string name(name_char);
    output_nodes.push_back(name);
  }

  // define the data simulator
  DataSimulatorExt<float> data_simulator;
  data_simulator.simulation_name_ = std::get<SimulationType>(parameters).get();

  // define the model trainers and resources for the trainers
  std::vector<ModelInterpreterDefaultDevice<float>> model_interpreters;
  for (size_t i = 0; i < n_threads; ++i) {
    ModelResources model_resources = { ModelDevice(std::get<DeviceId>(parameters).get(), 0) };
    ModelInterpreterDefaultDevice<float> model_interpreter(model_resources);
    model_interpreters.push_back(model_interpreter);
  }
  ModelTrainerExt<float> model_trainer;
  model_trainer.setBatchSize(std::get<BatchSize>(parameters).get());
  model_trainer.setMemorySize(std::get<MemorySize>(parameters).get());
  model_trainer.setNEpochsTraining(std::get<NEpochsTraining>(parameters).get());
  model_trainer.setNEpochsValidation(std::get<NEpochsValidation>(parameters).get());
  model_trainer.setNEpochsEvaluation(std::get<NEpochsValidation>(parameters).get());
  model_trainer.setNTBPTTSteps(std::get<NTBTTSteps>(parameters).get());
  model_trainer.setNTETTSteps(std::get<NTBTTSteps>(parameters).get());
  model_trainer.setVerbosityLevel(1);
  model_trainer.setLogging(true, false, true);
  model_trainer.setFindCycles(false); // Specified in the model
  model_trainer.setFastInterpreter(true); // IG default
  model_trainer.setPreserveOoO(true);

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
  metric_function_helper1.metric_functions_ = { std::make_shared<PearsonROp<float>>(PearsonROp<float>("Mean")), std::make_shared<PearsonROp<float>>(PearsonROp<float>("Var")),
    std::make_shared<EuclideanDistOp<float>>(EuclideanDistOp<float>("Mean")), std::make_shared<EuclideanDistOp<float>>(EuclideanDistOp<float>("Var")) };
  metric_function_helper1.metric_names_ = { "PearsonR-Mean", "PearsonR-Var",
    "EuclideanDist-Mean", "EuclideanDist-Var" };
  metric_function_helpers.push_back(metric_function_helper1);
  model_trainer.setMetricFunctionHelpers(metric_function_helpers);

  // define the model logger
  ModelLogger<float> model_logger(true, true, true, false, false, true, false, true);

  // define the model replicator for growth mode
  ModelReplicatorExt<float> model_replicator;
  model_replicator.setNodeActivations({ std::make_pair(std::make_shared<ReLUOp<float>>(ReLUOp<float>()), std::make_shared<ReLUGradOp<float>>(ReLUGradOp<float>())),
    std::make_pair(std::make_shared<LinearOp<float>>(LinearOp<float>()), std::make_shared<LinearGradOp<float>>(LinearGradOp<float>())),
    std::make_pair(std::make_shared<ELUOp<float>>(ELUOp<float>()), std::make_shared<ELUGradOp<float>>(ELUGradOp<float>())),
    std::make_pair(std::make_shared<SigmoidOp<float>>(SigmoidOp<float>()), std::make_shared<SigmoidGradOp<float>>(SigmoidGradOp<float>())),
    std::make_pair(std::make_shared<TanHOp<float>>(TanHOp<float>()), std::make_shared<TanHGradOp<float>>(TanHGradOp<float>())),
    //std::make_pair(std::make_shared<ExponentialOp<float>>(ExponentialOp<float>()), std::make_shared<ExponentialGradOp<float>>(ExponentialGradOp<float>())),
    //std::make_pair(std::make_shared<LogOp<float>>(LogOp<float>()), std::make_shared<LogGradOp<float>>(LogGradOp<float>())),
    //std::make_pair(std::shared_ptr<ActivationOp<float>>(new InverseOp<float>()), std::shared_ptr<ActivationOp<float>>(new InverseGradOp<float>()))
    });
  model_replicator.setNodeIntegrations({ std::make_tuple(std::make_shared<SumOp<float>>(SumOp<float>()), std::make_shared<SumErrorOp<float>>(SumErrorOp<float>()), std::make_shared<SumWeightGradOp<float>>(SumWeightGradOp<float>())),
    std::make_tuple(std::make_shared<ProdOp<float>>(ProdOp<float>()), std::make_shared<ProdErrorOp<float>>(ProdErrorOp<float>()), std::make_shared<ProdWeightGradOp<float>>(ProdWeightGradOp<float>())),
    //std::make_tuple(std::make_shared<MeanOp<float>>(MeanOp<float>()), std::make_shared<MeanErrorOp<float>>(MeanErrorOp<float>()), std::make_shared<MeanWeightGradO<float>>(MeanWeightGradOp<float>())),
    //std::make_tuple(std::make_shared<VarModOp<float>>(VarModOp<float>()), std::make_shared<VarModErrorOp<float>>(VarModErrorOp<float>()), std::make_shared<VarModWeightGradOp<float>>(VarModWeightGradOp<float>())),
    //std::make_tuple(std::make_shared<CountOp<float>>(CountOp<float>()), std::make_shared<CountErrorOp<float>>(CountErrorOp<float>()), std::make_shared<CountWeightGradOp<float>>(CountWeightGradOp<float>()))
    });

  // define the initial population
  Model<float> model;
  std::string model_name = "HarmonicOscillator";
  if (std::get<MakeModel>(parameters).get()) {
    std::cout << "Making the model..." << std::endl;
    ModelTrainerExt<float>().makeHarmonicOscillator1D(model, 1, 32, 0, false, true);
  }
  else {
    // read in the trained model
    std::cout << "Reading in the model..." << std::endl;
    ModelFile<float> model_file;
    model_file.loadModelBinary(std::get<DataDir>(parameters).get() + std::get<ModelName>(parameters).get() + "_model.binary", model);
    model.setId(1);
    model.setName(std::get<ModelName>(parameters).get());
    ModelInterpreterFileDefaultDevice<float> model_interpreter_file;
    model_interpreter_file.loadModelInterpreterBinary(std::get<DataDir>(parameters).get() + std::get<ModelName>(parameters).get() + "_interpreter.binary", model_interpreters[0]); // FIX ME!
  }
  model.setName(std::get<DataDir>(parameters).get() + model_name); //So that all output will be written to a specific directory

  if (std::get<TrainModel>(parameters).get()) {
    // Train the model
    model.setName(model.getName() + "_train");
    std::pair<std::vector<float>, std::vector<float>> model_errors = model_trainer.trainModel(model, data_simulator,
      input_nodes, model_logger, model_interpreters.front());
  }
  else if (std::get<EvolveModel>(parameters).get()) {
    // Evolve the population
    std::vector<Model<float>> population = { model };
    std::vector<std::vector<std::tuple<int, std::string, float>>> models_validation_errors_per_generation = population_trainer.evolveModels(
      population, model_trainer, model_interpreters, model_replicator, data_simulator, model_logger, population_logger, input_nodes);

    PopulationTrainerFile<float> population_trainer_file;
    population_trainer_file.storeModels(population, "HarmonicOscillator");
    population_trainer_file.storeModelValidations("HarmonicOscillatorErrors.csv", models_validation_errors_per_generation);
  }
  else if (std::get<EvaluateModel>(parameters).get()) {
    //// Evaluate the population
    //std::vector<Model<float>> population = { model };
    //population_trainer.evaluateModels(
    //  population, model_trainer, model_interpreters, model_replicator, data_simulator, model_logger, input_nodes);
    // Evaluate the model
    model.setName(model.getName() + "_evaluation");
    Eigen::Tensor<float, 4> model_output = model_trainer.evaluateModel(model, data_simulator, input_nodes, model_logger, model_interpreters.front());
  }
}

/*
@brief Run the training/evolution/evaluation from the command line

Example:
./HarmonicOscillator_DefaultDevice_example "C:/Users/dmccloskey/Documents/GitHub/EvoNetData/MNIST_examples/HarmonicOscillator/DefaultDevice1-0a" true true false "WeightSpring1W1S1DwDamping" 32 64 100000

@param data_dir The data director
@param make_model Whether to make the model or read in a trained model/interpreter called 'HarmonicOscillator_model'/'HarmonicOscillator_interpreter'
@param train_model Whether to train the model
@param evolve_model Whether to evolve the model
@param simulation_type The type of simulation to run
*/
int main(int argc, char** argv)
{
  // Parse the user commands
  int id_int = -1;
  std::string parameters_file = "C:/Users/dmccloskey/Documents/GitHub/EvoNetData/MNIST_examples/HarmonicOscillator/Parameters.csv";
  parseCommandLineArguments(argc, argv, id_int, parameters_file);

  // Set the parameter names and defaults
  ID id("id", -1);
  DataDir data_dir("data_dir", std::string(""));
  NInterpreters n_interpreters("n_interpreters", 16);
  NGenerations n_generations("n_generations", 50);
  MakeModel make_model("make_model", true);
  TrainModel train_model("train_model", true);
  EvolveModel evolve_model("evolve_model", false);
  EvaluateModel evaluate_model("evaluate_model", false);
  SimulationType simulation_type("simulation_type", "WeightSpring1W1S1DwDamping");
  BatchSize batch_size("batch_size", 32);
  MemorySize memory_size("memory_size", 64);
  NEpochsTraining n_epochs_training("n_epochs_training", 100000);
  NEpochsValidation n_epochs_validation("n_epochs_validation", 25);
  NEpochsEvaluation n_epochs_evaluation("n_epochs_evaluation", 10);
  NTBTTSteps n_tbtt_steps("n_tbtt_steps", 64);
  DeviceId device_id("device_id", 0);
  ModelName model_name("model_name", "");
  auto parameters = std::make_tuple(id, data_dir, n_interpreters, n_generations, make_model, train_model, evolve_model, evaluate_model,
    simulation_type, batch_size, memory_size, n_epochs_training, n_epochs_validation, n_epochs_evaluation, n_tbtt_steps, device_id, model_name);

  // Read in the parameters
  std::apply([&id, &parameters_file](auto&& ...args) { loadParametersFromCsv(id, parameters_file, args ...); }, parameters);

  // Run the application
  std::apply([](auto&& ...args) { main_HarmonicOscillator1D(args ...); }, parameters);
  return 0;
}
/**TODO:  Add copyright*/

#ifndef SMARTPEAK_MODELTRAINER_H
#define SMARTPEAK_MODELTRAINER_H

// .h
#include <SmartPeak/ml/Model.h>
#include <SmartPeak/ml/LossFunction.h>
#include <SmartPeak/ml/ModelLogger.h>
#include <SmartPeak/simulator/DataSimulator.h>
#include <vector>
#include <string>

// .cpp

namespace SmartPeak
{
  /**
    @brief Class to train a network model
  */
	template<typename TensorT, typename InterpreterT>
  class ModelTrainer
  {
public:
    ModelTrainer() = default; ///< Default constructor
    ~ModelTrainer() = default; ///< Default destructor

    void setBatchSize(const int& batch_size); ///< batch_size setter
    void setMemorySize(const int& memory_size); ///< memory_size setter
    void setNEpochsTraining(const int& n_epochs); ///< n_epochs setter
		void setNEpochsValidation(const int& n_epochs); ///< n_epochs setter
		void setNEpochsEvaluation(const int& n_epochs); ///< n_epochs setter
		void setVerbosityLevel(const int& verbosity_level); ///< verbosity_level setter
		void setLogging(bool log_training = false, bool log_validation = false, bool log_evaluation = false); ///< enable_logging setter
		void setLossFunctions(const std::vector<std::shared_ptr<LossFunctionOp<TensorT>>>& loss_functions); ///< loss_functions setter [TODO: tests]
		void setLossFunctionGrads(const std::vector<std::shared_ptr<LossFunctionGradOp<TensorT>>>& loss_function_grads); ///< loss_functions setter [TODO: tests]
		void setOutputNodes(const std::vector<std::vector<std::string>>& output_nodes); ///< output_nodes setter [TODO: tests]
		void setNTBPTTSteps(const int& n_TBPTT); ///< n_TBPTT setter
		void setNTETTSteps(const int& n_TETT); ///< n_TETT setter
		void setFindCycles(const bool& find_cycles); ///< find_cycles setter [TODO: tests]
		void setFastInterpreter(const bool& fast_interpreter); ///< fast_interpreter setter [TODO: tests]
		void setPreserveOoO(const bool& preserve_OoO); ///< preserve_OoO setter [TODO: test]

    int getBatchSize() const; ///< batch_size setter
    int getMemorySize() const; ///< memory_size setter
    int getNEpochsTraining() const; ///< n_epochs setter
		int getNEpochsValidation() const; ///< n_epochs setter
		int getNEpochsEvaluation() const; ///< n_epochs setter
		int getVerbosityLevel() const; ///< verbosity_level setter
		bool getLogTraining() const;
		bool getLogValidation() const;
		bool getLogEvaluation() const;
		std::vector<std::shared_ptr<LossFunctionOp<TensorT>>> getLossFunctions(); ///< loss_functions getter [TODO: tests]
		std::vector<std::shared_ptr<LossFunctionGradOp<TensorT>>> getLossFunctionGrads(); ///< loss_functions getter [TODO: tests]
		std::vector<std::vector<std::string>> getOutputNodes(); ///< output_nodes getter [TODO: tests]
		int getNTBPTTSteps() const; ///< n_TBPTT setter
		int getNTETTSteps() const; ///< n_TETT setter
		bool getFindCycles(); ///< find_cycles getter [TODO: tests]
		bool getFastInterpreter(); ///< fast_interpreter getter [TODO: tests]
		bool getPreserveOoO(); ///< preserve_OoO getter [TODO: tests]
 
    /**
      @brief Check input dimensions.

      @param n_epochs The number of training epochs
      @param input The input data
      @param batch_size The batch size of the nodes
      @param memory_size The memory size of the nodes
      @param input_nodes The node names

      @returns True on success, False if not
    */ 
    bool checkInputData(const int& n_epochs,
      const Eigen::Tensor<TensorT, 4>& input,
      const int& batch_size,
      const int& memory_size,
      const std::vector<std::string>& input_nodes);
 
    /**
      @brief Check output dimensions.

      @param n_epochs The number of training epochs
      @param output The output data
      @param batch_size The batch size of the nodes
      @param output_nodes The node names

      @returns True on success, False if not
    */ 
    bool checkOutputData(const int& n_epochs,
      const Eigen::Tensor<TensorT, 4>& output,
      const int& batch_size,
			const int& memory_size,
      const std::vector<std::string>& output_nodes);
 
    /**
      @brief Check time step dimensions required for FPTT.

      @param n_epochs The number of training epochs
      @param time_steps The time step spacing
      @param batch_size The batch size of the nodes
      @param memory_size The memory size of the nodes

      @returns True on success, False if not
    */ 
    bool checkTimeSteps(const int& n_epochs,
      const Eigen::Tensor<TensorT, 3>& time_steps,
      const int& batch_size,
      const int& memory_size);
 
    /**
      @brief Entry point for users to code their script
        for model training

      @param[in, out] model The model to train
      @param[in] input Input data tensor of dimensions: batch_size, memory_size, input_nodes, n_epochs
      @param[in] output Expected output data tensor of dimensions: batch_size, memory_size, output_nodes, n_epochs
      @param[in] time_steps Time steps of the forward passes of dimensions: batch_size, memory_size, n_epochs
      @param[in] input_nodes Input node names
      @param[in] model_logger Model logger to log training epochs
      @param[in] model_interpreter The model interpreter

      @returns vector of average model error scores
    */ 
		virtual std::vector<TensorT> trainModel(Model<TensorT>& model,
			const Eigen::Tensor<TensorT, 4>& input,
			const Eigen::Tensor<TensorT, 4>& output,
			const Eigen::Tensor<TensorT, 3>& time_steps,
			const std::vector<std::string>& input_nodes,
			ModelLogger<TensorT>& model_logger,
			InterpreterT& model_interpreter);

    /**
      @brief Entry point for users to code their script
        for model training

      @param[in, out] model The model to train
      @param[in] data_simulator The training, validation, and test data generator
      @param[in] input_nodes Input node names
      @param[in] model_logger Model logger to log training epochs
      @param[in] model_interpreter The model interpreter

      @returns vector of average model error scores from training and testing/validation
    */
    virtual std::pair<std::vector<TensorT>, std::vector<TensorT>> trainModel(Model<TensorT>& model,
      DataSimulator<TensorT> &data_simulator,
      const std::vector<std::string>& input_nodes,
      ModelLogger<TensorT>& model_logger,
      InterpreterT& model_interpreter);
 
    /**
      @brief Entry point for users to code their script
        for model validation

      @param[in, out] model The model to train
      @param[in] model_resources The hardware available for training the model
      @param[in] input Input data tensor of dimensions: batch_size, memory_size, input_nodes, n_epochs
      @param[in] output Expected output data tensor of dimensions: batch_size, memory_size, output_nodes, n_epochs
      @param[in] time_steps Time steps of the forward passes of dimensions: batch_size, memory_size, n_epochs
      @param[in] input_nodes Input node names

      @returns vector of average model error scores
    */ 
		virtual std::vector<TensorT> validateModel(Model<TensorT>& model,
			const Eigen::Tensor<TensorT, 4>& input,
			const Eigen::Tensor<TensorT, 4>& output,
			const Eigen::Tensor<TensorT, 3>& time_steps,
			const std::vector<std::string>& input_nodes,
			ModelLogger<TensorT>& model_logger,
			InterpreterT& model_interpreter);

		/**
			@brief Entry point for users to code their script
				for model forward evaluations

			@param[in, out] model The model to train
      @param[in] model_resources The hardware available for training the model
			@param[in] input Input data tensor of dimensions: batch_size, memory_size, input_nodes, n_epochs
			@param[in] time_steps Time steps of the forward passes of dimensions: batch_size, memory_size, n_epochs
			@param[in] input_nodes Input node names

			@returns vector of vectors corresponding to output nodes
		*/
		virtual std::vector<std::vector<Eigen::Tensor<TensorT, 2>>> evaluateModel(Model<TensorT>& model,
			const Eigen::Tensor<TensorT, 4>& input,
			const Eigen::Tensor<TensorT, 3>& time_steps,
			const std::vector<std::string>& input_nodes,
			ModelLogger<TensorT>& model_logger,
			InterpreterT& model_interpreter);
 
    /**
      @brief Entry point for users to code their script
        to build the model

      @returns The constructed model
    */ 
    virtual Model<TensorT> makeModel();

		/**
		@brief Entry point for users to code their adaptive scheduler
		to modify training parameters based on a given trigger

		@param[in] n_generations The number of evolution generations
		@param[in] n_epochs The number of training/validation epochs
		@param[in, out] model The model
		@param[in, out] model_interpreter The model interpreter
		@param[in] model_errors The trace of model errors from training/validation

		*/
		virtual void adaptiveTrainerScheduler(
			const int& n_generations,
			const int& n_epochs,
			Model<TensorT>& model,
			InterpreterT& model_interpreter,
			const std::vector<TensorT>& model_errors);

		/**
		@brief Entry point for users to code their training logger

		[TODO: add tests]

		@param[in] n_generations The number of evolution generations
		@param[in] n_epochs The number of training/validation epochs
		@param[in, out] model The model
		@param[in, out] model_interpreter The model interpreter
		@param[in, out] model_logger The model logger
		@param[in] expected_values The expected values

		*/
		virtual void trainingModelLogger(
			const int& n_epochs,
			Model<TensorT>& model,
			InterpreterT& model_interpreter,
			ModelLogger<TensorT>& model_logger,
			const Eigen::Tensor<TensorT, 3>& expected_values,
			const std::vector<std::string>& output_nodes,
			const TensorT& model_error);

    /**
    @brief Entry point for users to code their training logger

    [TODO: add tests]

    @param[in] n_generations The number of evolution generations
    @param[in] n_epochs The number of training/validation epochs
    @param[in, out] model The model
    @param[in, out] model_interpreter The model interpreter
    @param[in, out] model_logger The model logger
    @param[in] expected_values The expected values

    */
    virtual void trainingModelLogger(
      const int& n_epochs,
      Model<TensorT>& model,
      InterpreterT& model_interpreter,
      ModelLogger<TensorT>& model_logger,
      const Eigen::Tensor<TensorT, 3>& expected_values,
      const std::vector<std::string>& output_nodes,
      const TensorT& model_error_train,
      const TensorT& model_error_test);

		/**
		@brief Entry point for users to code their validation logger

		[TODO: add tests]

		@param[in] n_generations The number of evolution generations
		@param[in] n_epochs The number of training/validation epochs
		@param[in, out] model The model
		@param[in, out] model_interpreter The model interpreter
		@param[in, out] model_logger The model logger
		@param[in] expected_values The expected values

		*/
		virtual void validationModelLogger(
			const int& n_epochs,
			Model<TensorT>& model,
			InterpreterT& model_interpreter,
			ModelLogger<TensorT>& model_logger,
			const Eigen::Tensor<TensorT, 3>& expected_values,
			const std::vector<std::string>& output_nodes,
			const TensorT& model_error);

		/**
		@brief Entry point for users to code their evaluation logger

		[TODO: add tests]

		@param[in] n_generations The number of evolution generations
		@param[in] n_epochs The number of training/validation epochs
		@param[in, out] model The model
		@param[in, out] model_interpreter The model interpreter
		@param[in, out] model_logger The model logger

		*/
		virtual void evaluationModelLogger(
			const int& n_epochs,
			Model<TensorT>& model,
			InterpreterT& model_interpreter,
			ModelLogger<TensorT>& model_logger,
			const std::vector<std::string>& output_nodes);

    /*
    @brief Determine the decay factor to reduce the learning rate by if the model_errors has not
      increases/decreased by a specified threshold for a period of epochs
      
    TODO: could be more nicely implemented as a functor?

    @param[in] model_errors The history of model errors
    @param[in] decay A scalar to multiple the current learning rate to get the new learning rate
    @param[in] cur_epoch The current epoch number
    @param[in] n_epochs_avg The number of epochs to determine the average model error
    @param[in] n_epochs_win The number of epochs to determine a recent window to compare to the average model error
    @param[in] min_per_error_diff The minimum percent error difference to change the learning rate

    @returns The decayed factor to change the learning rate
    */
    TensorT reduceLROnPlateau(const std::vector<float>& model_errors,
      const TensorT& decay, const int& n_epochs_avg, const int& n_epochs_win,
      const TensorT& min_perc_error_diff);

protected:
		std::vector<std::shared_ptr<LossFunctionOp<TensorT>>> loss_functions_;
		std::vector<std::shared_ptr<LossFunctionGradOp<TensorT>>> loss_function_grads_;
		std::vector<std::vector<std::string>> output_nodes_;

private:
    int batch_size_;
    int memory_size_;
    int n_epochs_training_ = 0;
		int n_epochs_validation_ = 0;
		int n_epochs_evaluation_ = 0;

		int n_TBPTT_steps_ = -1; ///< the number of truncated back propogation through time steps
		int n_TETT_steps_ = -1; ///< the number of truncated error through time calculation steps

		int verbosity_level_ = 0; ///< level of verbosity (0=none, 1=test/validation errors, 2=test/validation node values
		bool log_training_ = false; ///< whether to log training epochs or not
		bool log_validation_ = false; ///< whether to log validation epochs or not
		bool log_evaluation_ = false; ///< whether to log evaluation epochs or not

		bool find_cycles_ = true; ///< whether to find cycles prior to interpreting the model (see `ModelInterpreter`)
		bool fast_interpreter_ = false; ///< whether to skip certain checks when interpreting the model (see `ModelInterpreter`)
		bool preserve_OoO_ = true; ///< whether to preserve the order of operations (see `ModelInterpreter`)
  };
	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setBatchSize(const int& batch_size)
	{
		batch_size_ = batch_size;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setMemorySize(const int& memory_size)
	{
		memory_size_ = memory_size;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setNEpochsTraining(const int& n_epochs)
	{
		n_epochs_training_ = n_epochs;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setNEpochsValidation(const int & n_epochs)
	{
		n_epochs_validation_ = n_epochs;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setNEpochsEvaluation(const int & n_epochs)
	{
		n_epochs_evaluation_ = n_epochs;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setVerbosityLevel(const int & verbosity_level)
	{
		verbosity_level_ = verbosity_level;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setLogging(bool log_training, bool log_validation, bool log_evaluation)
	{
		log_training_ = log_training;
		log_validation_ = log_validation;
		log_evaluation_ = log_evaluation;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setLossFunctions(const std::vector<std::shared_ptr<LossFunctionOp<TensorT>>>& loss_functions)
	{
		loss_functions_ = loss_functions;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setLossFunctionGrads(const std::vector<std::shared_ptr<LossFunctionGradOp<TensorT>>>& loss_function_grads)
	{
		loss_function_grads_ = loss_function_grads;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setOutputNodes(const std::vector<std::vector<std::string>>& output_nodes)
	{
		output_nodes_ = output_nodes;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setNTBPTTSteps(const int & n_TBPTT)
	{
		n_TBPTT_steps_ = n_TBPTT;
	}

	template<typename TensorT, typename InterpreterT>
	void ModelTrainer<TensorT, InterpreterT>::setNTETTSteps(const int & n_TETT)
	{
		n_TETT_steps_ = n_TETT;
	}

	template<typename TensorT, typename InterpreterT>
	inline void ModelTrainer<TensorT, InterpreterT>::setFindCycles(const bool & find_cycles)
	{
		find_cycles_ = find_cycles;
	}

	template<typename TensorT, typename InterpreterT>
	inline void ModelTrainer<TensorT, InterpreterT>::setPreserveOoO(const bool & preserve_OoO)
	{
		preserve_OoO_ = preserve_OoO;
	}

	template<typename TensorT, typename InterpreterT>
	inline void ModelTrainer<TensorT, InterpreterT>::setFastInterpreter(const bool & fast_interpreter)
	{
		fast_interpreter_ = fast_interpreter;
	}

	template<typename TensorT, typename InterpreterT>
	int ModelTrainer<TensorT, InterpreterT>::getBatchSize() const
	{
		return batch_size_;
	}

	template<typename TensorT, typename InterpreterT>
	int ModelTrainer<TensorT, InterpreterT>::getMemorySize() const
	{
		return memory_size_;
	}

	template<typename TensorT, typename InterpreterT>
	int ModelTrainer<TensorT, InterpreterT>::getNEpochsTraining() const
	{
		return n_epochs_training_;
	}

	template<typename TensorT, typename InterpreterT>
	int ModelTrainer<TensorT, InterpreterT>::getNEpochsValidation() const
	{
		return n_epochs_validation_;
	}

	template<typename TensorT, typename InterpreterT>
	int ModelTrainer<TensorT, InterpreterT>::getNEpochsEvaluation() const
	{
		return n_epochs_evaluation_;
	}

	template<typename TensorT, typename InterpreterT>
	int ModelTrainer<TensorT, InterpreterT>::getVerbosityLevel() const
	{
		return verbosity_level_;
	}

	template<typename TensorT, typename InterpreterT>
	inline bool ModelTrainer<TensorT, InterpreterT>::getLogTraining() const
	{
		return log_training_;
	}

	template<typename TensorT, typename InterpreterT>
	inline bool ModelTrainer<TensorT, InterpreterT>::getLogValidation() const
	{
		return log_validation_;
	}

	template<typename TensorT, typename InterpreterT>
	inline bool ModelTrainer<TensorT, InterpreterT>::getLogEvaluation() const
	{
		return log_evaluation_;
	}

	template<typename TensorT, typename InterpreterT>
	std::vector<std::shared_ptr<LossFunctionOp<TensorT>>> ModelTrainer<TensorT, InterpreterT>::getLossFunctions()
	{
		return loss_functions_;
	}

	template<typename TensorT, typename InterpreterT>
	std::vector<std::shared_ptr<LossFunctionGradOp<TensorT>>> ModelTrainer<TensorT, InterpreterT>::getLossFunctionGrads()
	{
		return loss_function_grads_;
	}

	template<typename TensorT, typename InterpreterT>
	std::vector<std::vector<std::string>> ModelTrainer<TensorT, InterpreterT>::getOutputNodes()
	{
		return output_nodes_;
	}

	template<typename TensorT, typename InterpreterT>
	int ModelTrainer<TensorT, InterpreterT>::getNTBPTTSteps() const
	{
		return n_TBPTT_steps_;
	}

	template<typename TensorT, typename InterpreterT>
	int ModelTrainer<TensorT, InterpreterT>::getNTETTSteps() const
	{
		return n_TETT_steps_;
	}

	template<typename TensorT, typename InterpreterT>
	inline bool ModelTrainer<TensorT, InterpreterT>::getFindCycles()
	{
		return find_cycles_;
	}

	template<typename TensorT, typename InterpreterT>
	inline bool ModelTrainer<TensorT, InterpreterT>::getFastInterpreter()
	{
		return fast_interpreter_;
	}

	template<typename TensorT, typename InterpreterT>
	inline bool ModelTrainer<TensorT, InterpreterT>::getPreserveOoO()
	{
		return preserve_OoO_;
	}

	template<typename TensorT, typename InterpreterT>
	bool ModelTrainer<TensorT, InterpreterT>::checkInputData(const int& n_epochs,
		const Eigen::Tensor<TensorT, 4>& input,
		const int& batch_size,
		const int& memory_size,
		const std::vector<std::string>& input_nodes)
	{
		if (input.dimension(0) != batch_size)
		{
			printf("batch_size of %d is not compatible with the input dim 0 of %d\n", batch_size, (int)input.dimension(0));
			return false;
		}
		else if (input.dimension(1) != memory_size)
		{
			printf("memory_size of %d is not compatible with the input dim 1 of %d\n", memory_size, (int)input.dimension(1));
			return false;
		}
		else if (input.dimension(2) != input_nodes.size())
		{
			printf("input_nodes size of %d is not compatible with the input dim 2 of %d\n", input_nodes.size(), (int)input.dimension(2));
			return false;
		}
		else if (input.dimension(3) != n_epochs)
		{
			printf("n_epochs of %d is not compatible with the input dim 3 of %d\n", n_epochs, (int)input.dimension(3));
			return false;
		}
		else
		{
			return true;
		}
	}

	template<typename TensorT, typename InterpreterT>
	bool ModelTrainer<TensorT, InterpreterT>::checkOutputData(const int& n_epochs,
		const Eigen::Tensor<TensorT, 4>& output,
		const int& batch_size,
		const int& memory_size,
		const std::vector<std::string>& output_nodes)
	{
		if (output.dimension(0) != batch_size)
		{
			printf("batch_size of %d is not compatible with the output dim 0 of %d\n", batch_size, (int)output.dimension(0));
			return false;
		}
		else if (output.dimension(1) != memory_size)
		{
			printf("memory_size of %d is not compatible with the output dim 1 of %d\n", memory_size, (int)output.dimension(1));
			return false;
		}
		else if (output.dimension(2) != output_nodes.size())
		{
			printf("output_nodes size of %d is not compatible with the output dim 2 of %d\n", output_nodes.size(), (int)output.dimension(2));
			return false;
		}
		else if (output.dimension(3) != n_epochs)
		{
			printf("n_epochs of %d is not compatible with the output dim 3 of %d\n", n_epochs, (int)output.dimension(3));
			return false;
		}
		else
		{
			return true;
		}
	}

	template<typename TensorT, typename InterpreterT>
	bool ModelTrainer<TensorT, InterpreterT>::checkTimeSteps(const int & n_epochs, const Eigen::Tensor<TensorT, 3>& time_steps, const int & batch_size, const int & memory_size)
	{
		if (time_steps.dimension(0) != batch_size)
		{
			printf("batch_size of %d is not compatible with the time_steps dim 0 of %d\n", batch_size, (int)time_steps.dimension(0));
			return false;
		}
		else if (time_steps.dimension(1) != memory_size)
		{
			printf("memory_size of %d is not compatible with the time_steps dim 1 of %d\n", memory_size, (int)time_steps.dimension(1));
			return false;
		}
		else if (time_steps.dimension(2) != n_epochs)
		{
			printf("n_epochs of %d is not compatible with the time_steps dim 3 of %d\n", n_epochs, (int)time_steps.dimension(2));
			return false;
		}
		else
		{
			return true;
		}
	}

	template<typename TensorT, typename InterpreterT>
	inline std::vector<TensorT> ModelTrainer<TensorT, InterpreterT>::trainModel(Model<TensorT>& model, const Eigen::Tensor<TensorT, 4>& input, const Eigen::Tensor<TensorT, 4>& output, const Eigen::Tensor<TensorT, 3>& time_steps,
		const std::vector<std::string>& input_nodes,
		ModelLogger<TensorT>& model_logger,
		InterpreterT& model_interpreter)
	{
		std::vector<TensorT> model_error;

		// Check input and output data
		if (!this->checkInputData(this->getNEpochsTraining(), input, this->getBatchSize(), this->getMemorySize(), input_nodes))
		{
			return model_error;
		}
		std::vector<std::string> output_nodes;
		for (const std::vector<std::string>& output_nodes_vec : this->output_nodes_)
			for (const std::string& output_node : output_nodes_vec)
				output_nodes.push_back(output_node);
		if (!this->checkOutputData(this->getNEpochsTraining(), output, this->getBatchSize(), this->getMemorySize(), output_nodes))
		{
			return model_error;
		}
		if (!this->checkTimeSteps(this->getNEpochsTraining(), time_steps, this->getBatchSize(), this->getMemorySize()))
		{
			return model_error;
		}
		if (!model.checkNodeNames(input_nodes))
		{
			return model_error;
		}
		if (!model.checkNodeNames(output_nodes))
		{
			return model_error;
		}

		// Initialize the model
		if (this->getVerbosityLevel() >= 2)
			std::cout << "Intializing the model..." << std::endl;
		if (this->getFindCycles())
			model.findCycles();

		// compile the graph into a set of operations and allocate all tensors
		if (this->getVerbosityLevel() >= 2)
			std::cout << "Interpreting the model..." << std::endl;
		model_interpreter.checkMemory(model, this->getBatchSize(), this->getMemorySize());
		model_interpreter.getForwardPropogationOperations(model, this->getBatchSize(), this->getMemorySize(), true, this->getFastInterpreter(), this->getFindCycles(), this->getPreserveOoO());
		model_interpreter.allocateModelErrorTensor(this->getBatchSize(), this->getMemorySize());

		for (int iter = 0; iter < this->getNEpochsTraining(); ++iter) // use n_epochs here
		{
			// update the model hyperparameters
			this->adaptiveTrainerScheduler(0, iter, model, model_interpreter, model_error);

			// assign the input data
			model_interpreter.initBiases(model); // create the bias	
      model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "input"); // Needed for OoO/IG with DAG and DCG

			// forward propogate
			if (this->getVerbosityLevel() >= 2)
				std::cout << "Foward Propogation..." << std::endl;
			model_interpreter.FPTT(this->getMemorySize());

			// calculate the model error and node output 
			if (this->getVerbosityLevel() >= 2)
				std::cout << "Error Calculation..." << std::endl;
			int output_node_cnt = 0;
			for (size_t loss_iter = 0; loss_iter < this->output_nodes_.size(); loss_iter++) {
				const Eigen::Tensor<TensorT, 3> expected_tmp = output.chip(iter, 3);
				Eigen::Tensor<TensorT, 3> expected(this->getBatchSize(), this->getMemorySize(), (int)this->output_nodes_[loss_iter].size());
				for (int batch_iter = 0; batch_iter < this->getBatchSize(); ++batch_iter)
					for (int memory_iter = 0; memory_iter < this->getMemorySize(); ++memory_iter)
						for (int node_iter = 0; node_iter < this->output_nodes_[loss_iter].size(); ++node_iter)
							expected(batch_iter, memory_iter, node_iter) = expected_tmp(batch_iter, memory_iter, (int)(node_iter + output_node_cnt));
				if (this->getNTETTSteps() < 0)
					model_interpreter.CETT(model, expected, this->output_nodes_[loss_iter], this->loss_functions_[loss_iter].get(), this->loss_function_grads_[loss_iter].get(), this->getMemorySize());
				else
					model_interpreter.CETT(model, expected, this->output_nodes_[loss_iter], this->loss_functions_[loss_iter].get(), this->loss_function_grads_[loss_iter].get(), this->getNTETTSteps());
				output_node_cnt += this->output_nodes_[loss_iter].size();
			}

			// back propogate
			if (this->getVerbosityLevel() >= 2)
				std::cout << "Back Propogation..." << std::endl;
			if (this->getNTBPTTSteps() < 0)
				model_interpreter.TBPTT(this->getMemorySize());
			else
				model_interpreter.TBPTT(this->getNTBPTTSteps());

			// update the weights
			if (this->getVerbosityLevel() >= 2)
				std::cout << "Weight Update..." << std::endl;
			model_interpreter.updateWeights();

			model_interpreter.getModelResults(model, false, false, true);
			const Eigen::Tensor<TensorT, 0> total_error = model.getError().sum();
			model_error.push_back(total_error(0));
			if (this->getVerbosityLevel() >= 1)
				std::cout << "Model " << model.getName() << " error: " << total_error(0) << std::endl;

			// log epoch
			if (this->getLogTraining()) {
				if (this->getVerbosityLevel() >= 2)
					std::cout << "Logging..." << std::endl;
				this->trainingModelLogger(iter, model, model_interpreter, model_logger, output.chip(iter, 3), output_nodes, total_error(0));
			}

			// reinitialize the model
			if (iter != this->getNEpochsTraining() - 1) {
				model_interpreter.reInitNodes();
				model_interpreter.reInitModelError();
			}
		}
		// copy out results
		model_interpreter.getModelResults(model, true, true, true);
		model_interpreter.clear_cache();
		model.initNodeTensorIndices();
		model.initWeightTensorIndices();
		return model_error;
	}

  template<typename TensorT, typename InterpreterT>
  inline std::pair<std::vector<TensorT>, std::vector<TensorT>> ModelTrainer<TensorT, InterpreterT>::trainModel(Model<TensorT>& model, DataSimulator<TensorT>& data_simulator, const std::vector<std::string>& input_nodes, ModelLogger<TensorT>& model_logger, InterpreterT & model_interpreter)
  {
    std::vector<TensorT> model_error_training;
    std::vector<TensorT> model_error_validation;

    // Check the model
    if (!model.checkNodeNames(input_nodes))
    {
      return std::make_pair(model_error_training, model_error_validation);
    }
    std::vector<std::string> output_nodes;
    for (const std::vector<std::string>& output_nodes_vec : this->output_nodes_)
      for (const std::string& output_node : output_nodes_vec)
        output_nodes.push_back(output_node);
    if (!model.checkNodeNames(output_nodes))
    {
      return std::make_pair(model_error_training, model_error_validation);
    }

    // Initialize the model
    if (this->getVerbosityLevel() >= 2)
      std::cout << "Intializing the model..." << std::endl;
    if (this->getFindCycles())
      model.findCycles();

    // compile the graph into a set of operations and allocate all tensors
    if (this->getVerbosityLevel() >= 2)
      std::cout << "Interpreting the model..." << std::endl;
    model_interpreter.checkMemory(model, this->getBatchSize(), this->getMemorySize());
    model_interpreter.getForwardPropogationOperations(model, this->getBatchSize(), this->getMemorySize(), true, this->getFastInterpreter(), this->getFindCycles(), this->getPreserveOoO());
    model_interpreter.allocateModelErrorTensor(this->getBatchSize(), this->getMemorySize());

    for (int iter = 0; iter < this->getNEpochsTraining(); ++iter) // use n_epochs here
    {
      // update the model hyperparameters
      this->adaptiveTrainerScheduler(0, iter, model, model_interpreter, model_error_training);

      // Generate the input and output data for validation
      if (this->getVerbosityLevel() >= 2)
        std::cout << "Generating the input/output data for validation..." << std::endl;
      Eigen::Tensor<TensorT, 3> input_data_validation(this->getBatchSize(), this->getMemorySize(), (int)input_nodes.size());
      Eigen::Tensor<TensorT, 3> output_data_validation(this->getBatchSize(), this->getMemorySize(), (int)output_nodes.size());
      Eigen::Tensor<TensorT, 2> time_steps_validation(this->getBatchSize(), this->getMemorySize());
      data_simulator.simulateValidationData(input_data_validation, output_data_validation, time_steps_validation);

      // assign the input data
      model_interpreter.initBiases(model); // create the bias	
      model_interpreter.mapValuesToLayers(model, input_data_validation, input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      model_interpreter.mapValuesToLayers(model, input_data_validation, input_nodes, "input"); // Needed for OoO/IG with DAG and DCG

      // forward propogate
      if (this->getVerbosityLevel() >= 2)
        std::cout << "Validation Foward Propogation..." << std::endl;
      model_interpreter.FPTT(this->getMemorySize());

      // calculate the model error and node output 
      if (this->getVerbosityLevel() >= 2)
        std::cout << "Validation Error Calculation..." << std::endl;
      int output_node_cnt = 0;
      for (size_t loss_iter = 0; loss_iter < this->output_nodes_.size(); loss_iter++) {
        Eigen::Tensor<TensorT, 3> expected(this->getBatchSize(), this->getMemorySize(), (int)this->output_nodes_[loss_iter].size());
        for (int batch_iter = 0; batch_iter < this->getBatchSize(); ++batch_iter)
          for (int memory_iter = 0; memory_iter < this->getMemorySize(); ++memory_iter)
            for (int node_iter = 0; node_iter < this->output_nodes_[loss_iter].size(); ++node_iter)
              expected(batch_iter, memory_iter, node_iter) = output_data_validation(batch_iter, memory_iter, (int)(node_iter + output_node_cnt));
        if (this->getNTETTSteps() < 0)
          model_interpreter.CETT(model, expected, this->output_nodes_[loss_iter], this->loss_functions_[loss_iter].get(), this->loss_function_grads_[loss_iter].get(), this->getMemorySize());
        else
          model_interpreter.CETT(model, expected, this->output_nodes_[loss_iter], this->loss_functions_[loss_iter].get(), this->loss_function_grads_[loss_iter].get(), this->getNTETTSteps());
        output_node_cnt += this->output_nodes_[loss_iter].size();
      }

      // get the model validation error
      model_interpreter.getModelResults(model, false, false, true);
      const Eigen::Tensor<TensorT, 0> total_error_validation = model.getError().sum();
      model_error_validation.push_back(total_error_validation(0));

      // re-initialize the model
      model_interpreter.reInitNodes();
      model_interpreter.reInitModelError();

      // Generate the input and output data for training
      if (this->getVerbosityLevel() >= 2)
        std::cout << "Generating the input/output data for training..." << std::endl;
      Eigen::Tensor<TensorT, 3> input_data_training(this->getBatchSize(), this->getMemorySize(), (int)input_nodes.size());
      Eigen::Tensor<TensorT, 3> output_data_training(this->getBatchSize(), this->getMemorySize(), (int)output_nodes.size());
      Eigen::Tensor<TensorT, 2> time_steps_training(this->getBatchSize(), this->getMemorySize());
      data_simulator.simulateTrainingData(input_data_training, output_data_training, time_steps_training);

      // assign the input data
      model_interpreter.initBiases(model); // create the bias	
      model_interpreter.mapValuesToLayers(model, input_data_training, input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      model_interpreter.mapValuesToLayers(model, input_data_training, input_nodes, "input"); // Needed for OoO/IG with DAG and DCG

      // forward propogate
      if (this->getVerbosityLevel() >= 2)
        std::cout << "Training Foward Propogation..." << std::endl;
      model_interpreter.FPTT(this->getMemorySize());

      // calculate the model error and node output 
      if (this->getVerbosityLevel() >= 2)
        std::cout << "Training Error Calculation..." << std::endl;
      output_node_cnt = 0;
      for (size_t loss_iter = 0; loss_iter < this->output_nodes_.size(); loss_iter++) {
        Eigen::Tensor<TensorT, 3> expected(this->getBatchSize(), this->getMemorySize(), (int)this->output_nodes_[loss_iter].size());
        for (int batch_iter = 0; batch_iter < this->getBatchSize(); ++batch_iter)
          for (int memory_iter = 0; memory_iter < this->getMemorySize(); ++memory_iter)
            for (int node_iter = 0; node_iter < this->output_nodes_[loss_iter].size(); ++node_iter)
              expected(batch_iter, memory_iter, node_iter) = output_data_training(batch_iter, memory_iter, (int)(node_iter + output_node_cnt));
        if (this->getNTETTSteps() < 0)
          model_interpreter.CETT(model, expected, this->output_nodes_[loss_iter], this->loss_functions_[loss_iter].get(), this->loss_function_grads_[loss_iter].get(), this->getMemorySize());
        else
          model_interpreter.CETT(model, expected, this->output_nodes_[loss_iter], this->loss_functions_[loss_iter].get(), this->loss_function_grads_[loss_iter].get(), this->getNTETTSteps());
        output_node_cnt += this->output_nodes_[loss_iter].size();
      }

      // get the model training error
      model_interpreter.getModelResults(model, false, false, true);
      const Eigen::Tensor<TensorT, 0> total_error_training = model.getError().sum();
      model_error_training.push_back(total_error_training(0));
      if (this->getVerbosityLevel() >= 1)
        std::cout << "Model " << model.getName() << " error: " << total_error_training(0) << std::endl;

      // back propogate
      if (this->getVerbosityLevel() >= 2)
        std::cout << "Back Propogation..." << std::endl;
      if (this->getNTBPTTSteps() < 0)
        model_interpreter.TBPTT(this->getMemorySize());
      else
        model_interpreter.TBPTT(this->getNTBPTTSteps());

      // update the weights
      if (this->getVerbosityLevel() >= 2)
        std::cout << "Weight Update..." << std::endl;
      model_interpreter.updateWeights();

      // log epoch
      if (this->getLogTraining()) {
        if (this->getVerbosityLevel() >= 2)
          std::cout << "Logging..." << std::endl;
        this->trainingModelLogger(iter, model, model_interpreter, model_logger, output_data_training, output_nodes, total_error_training(0), total_error_validation(0));
      }

      // reinitialize the model
      if (iter != this->getNEpochsTraining() - 1) {
        model_interpreter.reInitNodes();
        model_interpreter.reInitModelError();
      }
    }
    // copy out results
    model_interpreter.getModelResults(model, true, true, true);
    model_interpreter.clear_cache();
    model.initNodeTensorIndices();
    model.initWeightTensorIndices();
    return std::make_pair(model_error_training, model_error_validation);
  }

	template<typename TensorT, typename InterpreterT>
	inline std::vector<TensorT> ModelTrainer<TensorT, InterpreterT>::validateModel(Model<TensorT>& model, const Eigen::Tensor<TensorT, 4>& input, const Eigen::Tensor<TensorT, 4>& output, const Eigen::Tensor<TensorT, 3>& time_steps,
		const std::vector<std::string>& input_nodes,
		ModelLogger<TensorT>& model_logger,
		InterpreterT& model_interpreter)
	{
		std::vector<TensorT> model_error;

		// Check input and output data
		if (!this->checkInputData(this->getNEpochsValidation(), input, this->getBatchSize(), this->getMemorySize(), input_nodes))
		{
			return model_error;
		}
		std::vector<std::string> output_nodes;
		for (const std::vector<std::string>& output_nodes_vec : this->output_nodes_)
			for (const std::string& output_node : output_nodes_vec)
				output_nodes.push_back(output_node);
		if (!this->checkOutputData(this->getNEpochsValidation(), output, this->getBatchSize(), this->getMemorySize(), output_nodes))
		{
			return model_error;
		}
		if (!this->checkTimeSteps(this->getNEpochsValidation(), time_steps, this->getBatchSize(), this->getMemorySize()))
		{
			return model_error;
		}
		if (!model.checkNodeNames(input_nodes))
		{
			return model_error;
		}
		if (!model.checkNodeNames(output_nodes))
		{
			return model_error;
		}

		// Initialize the model
		if (this->getFindCycles())
			model.findCycles();

		// compile the graph into a set of operations and allocate all tensors
		model_interpreter.getForwardPropogationOperations(model, this->getBatchSize(), this->getMemorySize(), false, this->getFastInterpreter(), this->getFindCycles(), this->getPreserveOoO());
		model_interpreter.allocateModelErrorTensor(this->getBatchSize(), this->getMemorySize());

		for (int iter = 0; iter < this->getNEpochsValidation(); ++iter) // use n_epochs here
		{
			// assign the input data
			model_interpreter.initBiases(model); // create the bias	
      model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "input"); // Needed for IG with DAG and DCG
      //if (this->getPreserveOoO()) {
      //  model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      //}
      //else {
      //  model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      //  model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "input"); // Needed for IG with DAG and DCG
      //}

			// forward propogate
			if (this->getVerbosityLevel() >= 2)
				std::cout << "Foward Propogation..." << std::endl;
			model_interpreter.FPTT(this->getMemorySize());

			// calculate the model error and node output 
			if (this->getVerbosityLevel() >= 2)
				std::cout << "Error Calculation..." << std::endl;
			int output_node_cnt = 0;
			for (size_t loss_iter = 0; loss_iter < this->output_nodes_.size(); loss_iter++) {
				Eigen::Tensor<TensorT, 3> expected_tmp = output.chip(iter, 3);
				Eigen::Tensor<TensorT, 3> expected(this->getBatchSize(), this->getMemorySize(), (int)this->output_nodes_[loss_iter].size());
				for (int batch_iter = 0; batch_iter < this->getBatchSize(); ++batch_iter)
					for (int memory_iter = 0; memory_iter < this->getMemorySize(); ++memory_iter)
						for (int node_iter = 0; node_iter < this->output_nodes_[loss_iter].size(); ++node_iter)
							expected(batch_iter, memory_iter, node_iter) = expected_tmp(batch_iter, memory_iter, (int)(node_iter + output_node_cnt));
				if (this->getNTETTSteps() < 0)
					model_interpreter.CETT(model, expected, this->output_nodes_[loss_iter], this->loss_functions_[loss_iter].get(), this->loss_function_grads_[loss_iter].get(), this->getMemorySize());
				else
					model_interpreter.CETT(model, expected, this->output_nodes_[loss_iter], this->loss_functions_[loss_iter].get(), this->loss_function_grads_[loss_iter].get(), this->getNTETTSteps());
				output_node_cnt += this->output_nodes_[loss_iter].size();
			}

			model_interpreter.getModelResults(model, false, false, true);
			const Eigen::Tensor<TensorT, 0> total_error = model.getError().sum();
			model_error.push_back(total_error(0));
			if (this->getVerbosityLevel() >= 1)
				std::cout << "Model " << model.getName() << " error: " << total_error(0) << std::endl;

			// log epoch
			if (this->getLogValidation()) {
				if (this->getVerbosityLevel() >= 2)
					std::cout << "Logging..." << std::endl;
				this->validationModelLogger(iter, model, model_interpreter, model_logger, output.chip(iter, 3), output_nodes, total_error(0));
			}

			// reinitialize the model
			if (iter != this->getNEpochsValidation() - 1) {
				model_interpreter.reInitNodes();
				model_interpreter.reInitModelError();
			}
		}
		// copy out results
		model_interpreter.getModelResults(model, true, true, true);
		model_interpreter.clear_cache();
		model.initNodeTensorIndices();
		model.initWeightTensorIndices();
		return model_error;
	}

	template<typename TensorT, typename InterpreterT>
	inline std::vector<std::vector<Eigen::Tensor<TensorT, 2>>> ModelTrainer<TensorT, InterpreterT>::evaluateModel(Model<TensorT>& model, const Eigen::Tensor<TensorT, 4>& input, const Eigen::Tensor<TensorT, 3>& time_steps, const std::vector<std::string>& input_nodes,
		ModelLogger<TensorT>& model_logger,
		InterpreterT& model_interpreter)
	{
		std::vector<std::vector<Eigen::Tensor<TensorT, 2>>> model_output;

		// Check input data
		if (!this->checkInputData(this->getNEpochsEvaluation(), input, this->getBatchSize(), this->getMemorySize(), input_nodes))
		{
			return model_output;
		}
		if (!this->checkTimeSteps(this->getNEpochsEvaluation(), time_steps, this->getBatchSize(), this->getMemorySize()))
		{
			return model_output;
		}
		if (!model.checkNodeNames(input_nodes))
		{
			return model_output;
		}
		std::vector<std::string> output_nodes;
		for (const std::vector<std::string>& output_nodes_vec : this->output_nodes_)
			for (const std::string& output_node : output_nodes_vec)
				output_nodes.push_back(output_node);
		if (!model.checkNodeNames(output_nodes))
		{
			return model_output;
		}

		// Initialize the model
		if (this->getFindCycles())
			model.findCycles();

		// compile the graph into a set of operations and allocate all tensors
		model_interpreter.getForwardPropogationOperations(model, this->getBatchSize(), this->getMemorySize(), false, this->getFastInterpreter(), this->getFindCycles(), this->getPreserveOoO());
		model_interpreter.allocateModelErrorTensor(this->getBatchSize(), this->getMemorySize());

		for (int iter = 0; iter < this->getNEpochsEvaluation(); ++iter) // use n_epochs here
		{
			// assign the input data
			model_interpreter.initBiases(model); // create the bias	
      model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "input"); // Needed for IG with DAG and DCG
      //if (this->getPreserveOoO()) {
      //  model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      //}
      //else {
      //  model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "output"); // Needed for OoO/IG with DAG and DCG
      //  model_interpreter.mapValuesToLayers(model, input.chip(iter, 3), input_nodes, "input"); // Needed for IG with DAG and DCG
      //}

			// forward propogate
			if (this->getVerbosityLevel() >= 2)
				std::cout << "Foward Propogation..." << std::endl;
			model_interpreter.FPTT(this->getMemorySize());

			// extract out the model output
			std::vector<Eigen::Tensor<TensorT, 2>> output;
			for (const std::vector<std::string>& output_nodes_vec : this->output_nodes_) {
				for (const std::string& output_node : output_nodes_vec) {
					output.push_back(model.getNode(output_node).getOutput());
				}
			}

			// log epoch
			if (this->getLogEvaluation()) {
				if (this->getVerbosityLevel() >= 2)
					std::cout << "Logging..." << std::endl;
				this->evaluationModelLogger(iter, model, model_interpreter, model_logger, output_nodes);
			}

			// reinitialize the model
			if (iter != this->getNEpochsEvaluation() - 1) {
				model_interpreter.reInitNodes();
			}
		}
		// copy out results
		model_interpreter.getModelResults(model, true, true, true);
		model_interpreter.clear_cache();
		model.initNodeTensorIndices();
		model.initWeightTensorIndices();
		return model_output;
	}
	template<typename TensorT, typename InterpreterT>
	inline Model<TensorT> ModelTrainer<TensorT, InterpreterT>::makeModel()
	{
		return Model<TensorT>();  // USER:  create your own overload method
	}
	template<typename TensorT, typename InterpreterT>
	inline void ModelTrainer<TensorT, InterpreterT>::adaptiveTrainerScheduler(const int & n_generations, const int & n_epochs, Model<TensorT>& model, InterpreterT & model_interpreter, const std::vector<TensorT>& model_errors)
	{
		// USER: create your own overload method
	}
	template<typename TensorT, typename InterpreterT>
	inline void ModelTrainer<TensorT, InterpreterT>::trainingModelLogger(const int & n_epochs, Model<TensorT>& model, InterpreterT & model_interpreter, ModelLogger<TensorT>& model_logger, 
		const Eigen::Tensor<TensorT, 3>& expected_values,
		const std::vector<std::string>& output_nodes,
		const TensorT& model_error)
	{
		if (n_epochs == 0) {
			model_logger.initLogs(model);
		}
		if (n_epochs % 10 == 0) {
			if (model_logger.getLogExpectedPredictedEpoch())
				model_interpreter.getModelResults(model, true, false, false);
			model_logger.writeLogs(model, n_epochs, { "Error" }, {}, { model_error }, {}, output_nodes, expected_values);
		}
	}
  template<typename TensorT, typename InterpreterT>
  inline void ModelTrainer<TensorT, InterpreterT>::trainingModelLogger(const int & n_epochs, Model<TensorT>& model, InterpreterT & model_interpreter, ModelLogger<TensorT>& model_logger, const Eigen::Tensor<TensorT, 3>& expected_values, const std::vector<std::string>& output_nodes, const TensorT & model_error_train, const TensorT & model_error_test)
  {
    if (n_epochs == 0) {
      model_logger.initLogs(model);
    }
    if (n_epochs % 10 == 0) {
      if (model_logger.getLogExpectedPredictedEpoch())
        model_interpreter.getModelResults(model, true, false, false);
      model_logger.writeLogs(model, n_epochs, { "Train_Error" }, { "Test_Error" }, { model_error_train }, { model_error_test }, output_nodes, expected_values);
    }
  }
	template<typename TensorT, typename InterpreterT>
	inline void ModelTrainer<TensorT, InterpreterT>::validationModelLogger(const int & n_epochs, Model<TensorT>& model, InterpreterT & model_interpreter, ModelLogger<TensorT>& model_logger, 
		const Eigen::Tensor<TensorT, 3>& expected_values,
		const std::vector<std::string>& output_nodes,
		const TensorT& model_error)
	{
		if (n_epochs == 0) {
			model_logger.initLogs(model);
		}
		if (n_epochs % 10 == 0) {
			if (model_logger.getLogExpectedPredictedEpoch())
				model_interpreter.getModelResults(model, true, false, false);
			model_logger.writeLogs(model, n_epochs, {}, { "Error" }, {}, { model_error }, output_nodes, expected_values);
		}
	}
	template<typename TensorT, typename InterpreterT>
	inline void ModelTrainer<TensorT, InterpreterT>::evaluationModelLogger(const int & n_epochs, Model<TensorT>& model, InterpreterT & model_interpreter, ModelLogger<TensorT>& model_logger,
		const std::vector<std::string>& output_nodes)
	{
		if (n_epochs == 0) {
			model_logger.initLogs(model);
		}
		if (n_epochs % 1 == 0) {
			model_interpreter.getModelResults(model, true, true, false);
			model_logger.writeLogs(model, n_epochs, {}, {}, {}, {}, output_nodes, Eigen::Tensor<TensorT, 3>(), output_nodes, {}, {});
		}
	}
  template<typename TensorT, typename InterpreterT>
  inline TensorT ModelTrainer<TensorT, InterpreterT>::reduceLROnPlateau(const std::vector<float>& model_errors, const TensorT & decay, const int & n_epochs_avg, const int & n_epochs_win, const TensorT & min_perc_error_diff)
  {
    assert(n_epochs_avg > n_epochs_win); // The number of average epochs is less than the number of windowed epochs.
    int cur_epoch = model_errors.size() - 1;
    // Check that enough epochs has elapsed
    if (cur_epoch < n_epochs_avg)
      return (TensorT)1;

    // Calculate the averages
    TensorT avg_error = 0;
    for (int i = 0; i < n_epochs_avg; ++i) {
      avg_error += model_errors.at(cur_epoch - i);
    }
    avg_error /= (TensorT)n_epochs_avg;
    TensorT win_error = 0;
    for (int i = 0; i < n_epochs_win; ++i) {
      win_error += model_errors.at(cur_epoch - i);
    }
    win_error /= (TensorT)n_epochs_win;

    // Check if the threshold has been met
    TensorT percent_diff = (avg_error - win_error) / avg_error;
    if (percent_diff < min_perc_error_diff) {
      return decay;
    }
    else
      return (TensorT)1;
  }
}
#endif //SMARTPEAK_MODELTRAINER_H
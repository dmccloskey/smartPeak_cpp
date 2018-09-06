/**TODO:  Add copyright*/

#ifndef SMARTPEAK_MODELLOGGER_H
#define SMARTPEAK_MODELLOGGER_H

#include <SmartPeak/ml/Model.h>
#include <SmartPeak/io/CSVWriter.h>
#include <vector>

namespace SmartPeak
{
  /**
    @brief Class to log model training metrics
  */
  class ModelLogger
  {
public:
    ModelLogger() = default; ///< Default constructor
		ModelLogger(bool log_time_epoch, bool log_train_val_metric_epoch, bool log_expected_predicted_epoch, bool log_weights_epoch, bool log_nodes_epoch, bool log_module_variance_epoch);
    ~ModelLogger() = default; ///< Default destructor

		bool getLogTimeEpoch() { return log_time_epoch_; }
		bool getLogTrainValMetricEpoch() { return log_train_val_metric_epoch_; }
		bool getLogExpectedPredictedEpoch() { return log_expected_predicted_epoch_; }
		bool getLogWeightsEpoch() { return log_weights_epoch_; }
		bool getLogNodesEpoch() { return log_nodes_epoch_; }
		bool getLogModuleVarianceEpoch() { return log_module_variance_epoch_; }

		CSVWriter getLogTimeEpochCSVWriter() { return log_time_epoch_csvwriter_; }
		CSVWriter getLogTrainValMetricEpochCSVWriter() { return log_train_val_metric_epoch_csvwriter_; }
		CSVWriter getLogExpectedPredictedEpochCSVWriter() { return log_expected_predicted_epoch_csvwriter_; }
		CSVWriter getLogWeightsEpochCSVWriter() { return log_weights_epoch_csvwriter_; }
		CSVWriter getLogNodesEpochCSVWriter() { return log_nodes_epoch_csvwriter_; }
		CSVWriter getLogModuleVarianceEpochCSVWriter() { return log_module_variance_epoch_csvwriter_; }

		/**
		@brief Initialize the log files

		@param[in] model

		@returns True for a successfull write operation
		*/
		bool initLogs(const Model & model);

		/**
		@brief Initialize the log files

		@param[in] model

		@returns True for a successfull write operation
		*/
		bool writeLogs(const Model & model, const int& n_epochs, const std::vector<std::string>& training_metric_names, const std::vector<std::string>& validation_metric_names,
			const std::vector<float>& training_metrics, const std::vector<float>& validation_metrics, const std::vector<std::string>& output_node_names, const Eigen::Tensor<float, 3>& expected_values);

		/**
		@brief Log epoch iteration number vs. time

		@param[in] n_epoch
		@param[in] time_stamp

		@returns True for a successfull write operation
		*/
		bool logTimePerEpoch(const Model & model, const int& n_epoch);

		/**
		@brief Log training/validation metrics per epoch

		@param[in] model
		@param[in] training_metric_names
		...
		@param[in] n_epoch

		@returns True for a successfull write operation
		*/
		bool logTrainValMetricsPerEpoch(const Model& model, const std::vector<std::string>& training_metric_names, const std::vector<std::string>& validation_metric_names,
			const std::vector<float>& training_metrics, const std::vector<float>& validation_metrics, const int& n_epoch);

		/**
		@brief Model predicted output and expected output for each batch for each time step per epoch

		@param[in] model
		@param[in] output_node_names Names of the output nodes
		@param[in] n_epoch

		@returns True for a successfull write operation
		*/
		bool logExpectedAndPredictedOutputPerEpoch(const Model& model, const std::vector<std::string>& output_node_names, const Eigen::Tensor<float, 3>& expected_values, const int& n_epoch);

		/**
		@brief Model weight update ratio for each link for each time step per epoch

		@param[in] model
		@param[in] n_epoch

		@returns True for a successfull write operation
		*/
		bool logWeightsPerEpoch(const Model& model, const int& n_epoch);

		/**
		@brief Model node values for each time step per epoch

		@param[in] model
		@param[in] n_epoch

		@returns True for a successfull write operation
		*/
		bool logNodesPerEpoch(const Model& model, const int& n_epoch);

		/**
		@brief The mean and variance of each module output and error for each time step for each mini batch per epoch

		@param[in] model
		@param[in] n_epoch

		@returns True for a successfull write operation
		*/
		bool logModuleMeanAndVariancePerEpoch(const Model& model, const int& n_epoch);

	private:
		bool log_time_epoch_ = false; ///< log ...
		CSVWriter log_time_epoch_csvwriter_;
		bool log_train_val_metric_epoch_ = false; ///< log 
		CSVWriter log_train_val_metric_epoch_csvwriter_;
		bool log_expected_predicted_epoch_ = false;
		CSVWriter log_expected_predicted_epoch_csvwriter_;
		bool log_weights_epoch_ = false;
		CSVWriter log_weights_epoch_csvwriter_;
		bool log_nodes_epoch_ = false;
		CSVWriter log_nodes_epoch_csvwriter_;
		bool log_module_variance_epoch_ = false;
		CSVWriter log_module_variance_epoch_csvwriter_;

		// internal variables
		std::map<std::string, std::vector<std::string>> module_to_node_names_;

  };
}

#endif //SMARTPEAK_MODELLOGGER_H
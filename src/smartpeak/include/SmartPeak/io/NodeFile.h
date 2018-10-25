/**TODO:  Add copyright*/

#ifndef SMARTPEAK_NODEFILE_H
#define SMARTPEAK_NODEFILE_H

// .h
#include <SmartPeak/ml/Node.h>
#include <unsupported/Eigen/CXX11/Tensor>
#include <iostream>
#include <fstream>
#include <vector>

// .cpp
#include <SmartPeak/io/csv.h>
#include <SmartPeak/io/CSVWriter.h>

namespace SmartPeak
{

  /**
    @brief NodeFile
  */
	template<typename TensorT>
  class NodeFile
  {
public:
    NodeFile() = default; ///< Default constructor
    ~NodeFile() = default; ///< Default destructor
 
    /**
      @brief Load nodes from file

      @param filename The name of the nodes file
      @param nodes The nodes to load data into

      @returns Status True on success, False if not
    */ 
    bool loadNodesBinary(const std::string& filename, std::vector<Node<TensorT>>& nodes);
    bool loadNodesCsv(const std::string& filename, std::vector<Node<TensorT>>& nodes);
 
    /**
      @brief Load nodes from file

      @param filename The name of the nodes file
      @param nodes The nodes to load data into

      @returns Status True on success, False if not
    */ 
    bool storeNodesBinary(const std::string& filename, const std::vector<Node<TensorT>>& nodes);
    bool storeNodesCsv(const std::string& filename, const std::vector<Node<TensorT>>& nodes);
  };
	template<typename TensorT>
	bool NodeFile<TensorT>::loadNodesBinary(const std::string& filename, std::vector<Node<TensorT>>& nodes) { return true; }

	template<typename TensorT>
	bool NodeFile<TensorT>::loadNodesCsv(const std::string& filename, std::vector<Node<TensorT>>& nodes)
	{
		nodes.clear();

		io::CSVReader<9> nodes_in(filename);
		nodes_in.read_header(io::ignore_extra_column,
			"node_name", "node_type", "node_status", "node_activation", "node_activation_grad", "node_integration", "node_integration_error", "node_integration_weight_grad", "module_name");
		std::string node_name, node_type_str, node_status_str, node_activation_str, node_activation_grad_str, node_integration_str, node_integration_error_str, node_integration_weight_grad_str, module_name_str = "";

		while (nodes_in.read_row(node_name, node_type_str, node_status_str, node_activation_str, node_activation_grad_str, node_integration_str, node_integration_error_str, node_integration_weight_grad_str, module_name_str))
		{
			// parse the node_type
			NodeType node_type;
			if (node_type_str == "hidden") node_type = NodeType::hidden;
			else if (node_type_str == "output") node_type = NodeType::output;
			else if (node_type_str == "input") node_type = NodeType::input;
			else if (node_type_str == "bias") node_type = NodeType::bias;
			else if (node_type_str == "recursive") node_type = NodeType::recursive;
			else std::cout << "NodeType for node_name " << node_name << " was not recognized." << std::endl;

			// parse the node_status
			NodeStatus node_status;
			if (node_status_str == "deactivated") node_status = NodeStatus::deactivated;
			else if (node_status_str == "initialized") node_status = NodeStatus::initialized;
			else if (node_status_str == "activated") node_status = NodeStatus::activated;
			else if (node_status_str == "corrected") node_status = NodeStatus::corrected;
			else std::cout << "NodeStatus for node_name " << node_name << " was not recognized." << std::endl;

			// parse the node_activation
			std::shared_ptr<ActivationOp<TensorT>> node_activation;
			if (node_activation_str == "ReLUOp") node_activation.reset(new ReLUOp<TensorT>());
			else if (node_activation_str == "ELUOp") node_activation.reset(new ELUOp<TensorT>());
			else if (node_activation_str == "LinearOp") node_activation.reset(new LinearOp<TensorT>());
			else if (node_activation_str == "SigmoidOp") node_activation.reset(new SigmoidOp<TensorT>());
			else if (node_activation_str == "TanHOp") node_activation.reset(new TanHOp<TensorT>());
			else if (node_activation_str == "ExponentialOp") node_activation.reset(new ExponentialOp<TensorT>());
			else if (node_activation_str == "InverseOp") node_activation.reset(new InverseOp<TensorT>());
			else if (node_activation_str == "LinearOp") node_activation.reset(new LinearOp<TensorT>());
			else if (node_activation_str == "LeakyReLUOp") node_activation.reset(new LeakyReLUOp<TensorT>()); // TODO define values
			else if (node_activation_str == "PowOp") node_activation.reset(new PowOp<TensorT>(-0.5));  // TODO define values
			else std::cout << "NodeActivation for node_name " << node_name << " was not recognized." << std::endl;

			// parse the node_activation
			std::shared_ptr<ActivationOp<TensorT>> node_activation_grad;
			if (node_activation_grad_str == "ReLUGradOp") node_activation_grad.reset(new ReLUGradOp<TensorT>());
			else if (node_activation_grad_str == "ELUGradOp") node_activation_grad.reset(new ELUGradOp<TensorT>());
			else if (node_activation_grad_str == "LinearGradOp") node_activation_grad.reset(new LinearGradOp<TensorT>());
			else if (node_activation_grad_str == "SigmoidGradOp") node_activation_grad.reset(new SigmoidGradOp<TensorT>());
			else if (node_activation_grad_str == "TanHGradOp") node_activation_grad.reset(new TanHGradOp<TensorT>());
			else if (node_activation_grad_str == "ExponentialGradOp") node_activation_grad.reset(new ExponentialGradOp<TensorT>());
			else if (node_activation_grad_str == "InverseGradOp") node_activation_grad.reset(new InverseGradOp<TensorT>());
			else if (node_activation_grad_str == "LinearGradOp") node_activation_grad.reset(new LinearGradOp<TensorT>());
			else if (node_activation_grad_str == "LeakyReLUGradOp") node_activation_grad.reset(new LeakyReLUGradOp<TensorT>());
			else if (node_activation_grad_str == "PowGradOp") node_activation_grad.reset(new PowGradOp<TensorT>(-0.5));
			else std::cout << "NodeActivationGrad for node_name " << node_name << " was not recognized." << std::endl;

			// parse the node_integration
			std::shared_ptr<IntegrationOp<TensorT>> node_integration;
			if (node_integration_str == "SumOp") node_integration.reset(new SumOp<TensorT>());
			else if (node_integration_str == "ProdOp") node_integration.reset(new ProdOp<TensorT>());
			else if (node_integration_str == "MaxOp") node_integration.reset(new MaxOp<TensorT>());
			else if (node_integration_str == "MeanOp") node_integration.reset(new MeanOp<TensorT>());
			else if (node_integration_str == "VarModOp") node_integration.reset(new VarModOp<TensorT>());
			else if (node_integration_str == "CountOp") node_integration.reset(new CountOp<TensorT>());
			else std::cout << "NodeIntegration for node_name " << node_name << " was not recognized." << std::endl;

			// parse the node_integration_error
			std::shared_ptr<IntegrationErrorOp<TensorT>> node_integration_error;
			if (node_integration_error_str == "SumErrorOp") node_integration_error.reset(new SumErrorOp<TensorT>());
			else if (node_integration_error_str == "ProdErrorOp") node_integration_error.reset(new ProdErrorOp<TensorT>());
			else if (node_integration_error_str == "MaxErrorOp") node_integration_error.reset(new MaxErrorOp<TensorT>());
			else if (node_integration_error_str == "MeanErrorOp") node_integration_error.reset(new MeanErrorOp<TensorT>());
			else if (node_integration_error_str == "VarModErrorOp") node_integration_error.reset(new VarModErrorOp<TensorT>());
			else if (node_integration_error_str == "CountErrorOp") node_integration_error.reset(new CountErrorOp<TensorT>());
			else std::cout << "NodeIntegrationError for node_name " << node_name << " was not recognized." << std::endl;

			// parse the node_integration_weight_grad
			std::shared_ptr<IntegrationWeightGradOp<TensorT>> node_integration_weight_grad;
			if (node_integration_weight_grad_str == "SumWeightGradOp") node_integration_weight_grad.reset(new SumWeightGradOp<TensorT>());
			else if (node_integration_weight_grad_str == "ProdWeightGradOp") node_integration_weight_grad.reset(new ProdWeightGradOp<TensorT>());
			else if (node_integration_weight_grad_str == "MaxWeightGradOp") node_integration_weight_grad.reset(new MaxWeightGradOp<TensorT>());
			else if (node_integration_weight_grad_str == "MeanWeightGradOp") node_integration_weight_grad.reset(new MeanWeightGradOp<TensorT>());
			else if (node_integration_weight_grad_str == "VarModWeightGradOp") node_integration_weight_grad.reset(new VarModWeightGradOp<TensorT>());
			else if (node_integration_weight_grad_str == "CountWeightGradOp") node_integration_weight_grad.reset(new CountWeightGradOp<TensorT>());
			else std::cout << "NodeIntegrationWeightGrad for node_name " << node_name << " was not recognized." << std::endl;

			Node<TensorT> node(node_name, node_type, node_status, node_activation, node_activation_grad, node_integration, node_integration_error, node_integration_weight_grad);
			node.setModuleName(module_name_str);
			nodes.push_back(node);
		}
		return true;
	}

	template<typename TensorT>
	bool NodeFile<TensorT>::storeNodesBinary(const std::string& filename, const std::vector<Node<TensorT>>& nodes) { return true; }

	template<typename TensorT>
	bool NodeFile<TensorT>::storeNodesCsv(const std::string& filename, const std::vector<Node<TensorT>>& nodes)
	{
		CSVWriter csvwriter(filename);

		// write the headers to the first line
		const std::vector<std::string> headers = { "node_name", "node_type", "node_status", "node_activation", "node_activation_grad", "node_integration", "node_integration_error", "node_integration_weight_grad", "module_name" };
		csvwriter.writeDataInRow(headers.begin(), headers.end());

		for (const Node<TensorT>& node : nodes)
		{
			std::vector<std::string> row;
			row.push_back(node.getName());

			// parse the node_type
			std::string node_type_str = "";
			if (node.getType() == NodeType::hidden) node_type_str = "hidden";
			else if (node.getType() == NodeType::output) node_type_str = "output";
			else if (node.getType() == NodeType::input) node_type_str = "input";
			else if (node.getType() == NodeType::bias) node_type_str = "bias";
			else if (node.getType() == NodeType::recursive) node_type_str = "recursive";
			else std::cout << "NodeType for node_name " << node.getName() << " was not recognized." << std::endl;
			row.push_back(node_type_str);

			// parse the node_status
			std::string node_status_str = "";
			if (node.getStatus() == NodeStatus::deactivated) node_status_str = "deactivated";
			else if (node.getStatus() == NodeStatus::initialized) node_status_str = "initialized";
			else if (node.getStatus() == NodeStatus::activated) node_status_str = "activated";
			else if (node.getStatus() == NodeStatus::corrected) node_status_str = "corrected";
			else std::cout << "NodeStatus for node_name " << node.getName() << " was not recognized." << std::endl;
			row.push_back(node_status_str);

			// parse the node_activation
			std::string node_activation_str = node.getActivation()->getName();
			row.push_back(node_activation_str);
			std::string node_activation_grad_str = node.getActivationGrad()->getName();
			row.push_back(node_activation_grad_str);

			// parse the node_integration
			std::string node_integration_str = node.getIntegration()->getName();
			row.push_back(node_integration_str);
			std::string node_integration_error_str = node.getIntegrationError()->getName();
			row.push_back(node_integration_error_str);
			std::string node_integration_weight_grad_str = node.getIntegrationWeightGrad()->getName();
			row.push_back(node_integration_weight_grad_str);

			row.push_back(node.getModuleName());

			// write to file
			csvwriter.writeDataInRow(row.begin(), row.end());
		}
		return true;
	}
}

#endif //SMARTPEAK_NODEFILE_H
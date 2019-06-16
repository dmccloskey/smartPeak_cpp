﻿/**TODO:  Add copyright*/

#ifndef SMARTPEAK_MODELBUILDEREXPERIMENTAL_H
#define SMARTPEAK_MODELBUILDEREXPERIMENTAL_H

// .h
#include <SmartPeak/ml/Model.h>
#include <SmartPeak/simulator/BiochemicalReaction.h> // AddBiochemicalReactions

#include <unsupported/Eigen/CXX11/Tensor>

// .cpp
#include <SmartPeak/core/Preprocessing.h>

namespace SmartPeak
{

  /**
    @brief Class to help create complex network models

		NOTE: the ModelInterpreter class arranges the Tensor layers according to node name ascending order.
			Therefore, the node name indices are buffered with 0's of length 12 to ensure proper sorting of
			nodes within a tensor layer.
  */
	template<typename TensorT>
  class ModelBuilderExperimental
  {
public:
    ModelBuilderExperimental() = default; ///< Default constructor
    ~ModelBuilderExperimental() = default; ///< Default destructor

		/*
		@brief Convert a biochemical interaction graph into a network model

    EXPERIMENTAL

		@param[in] elementary_graph A map of vectores of source/sink pairs where the key is the connection name
		@param[in, out] Model
		@param[in] node_activation The activation function of the input node to create
		@param[in] node_activation_grad The activation function gradient of the input node to create
		@param[in] node_integration The integration function of the input node to create
		@param[in] node_integration_error The integration function of the input node to create
		@param[in] node_integration_weight_grad The integration function of the input node to create
		**/
		void addInteractionGraph(const std::map<std::string, std::vector<std::pair<std::string, std::string>>>& elementary_graph,
			Model<TensorT> & model, const std::string & name, const std::string& module_name,
			const std::shared_ptr<ActivationOp<TensorT>>& node_activation, const std::shared_ptr<ActivationOp<TensorT>>& node_activation_grad,
			const std::shared_ptr<IntegrationOp<TensorT>>& node_integration, const std::shared_ptr<IntegrationErrorOp<TensorT>>& node_integration_error, const std::shared_ptr<IntegrationWeightGradOp<TensorT>>& node_integration_weight_grad,
			const std::shared_ptr<WeightInitOp<TensorT>> & weight_init, const std::shared_ptr<SolverOp<TensorT>> & solver);

    /*
    @brief Convert and add Biochemical reactions to the network model

    EXPERIMENTAL

    @param[in, out] Model
    @param[in] biochemicalReaction The set of biochemical reactions to convert and add
    @param[in] name Base node names
    @param[in] module_name Module name
    @param[in] weight_init The weight initialization for learnable parameters
    @param[in] solver The solver for learnable parameters
    **/
    void addBiochemicalReactions(Model<TensorT> & model, const BiochemicalReactions& biochemicalReactions, const std::string & name, const std::string & module_name,
      const std::shared_ptr<WeightInitOp<TensorT>> & weight_init, const std::shared_ptr<SolverOp<TensorT>> & solver, const int& version, bool specify_layer = false, bool specify_cycles = false);
    void addReactants_1(Model<TensorT> & model, const BiochemicalReaction& reaction, const std::string & name, const std::string & module_name,
      const std::shared_ptr<WeightInitOp<TensorT>> & weight_init, const std::shared_ptr<SolverOp<TensorT>> & solver,
      std::string& enzyme_complex_name, std::string& enzyme_complex_name_tmp1, std::string& enzyme_complex_name_tmp2, std::string& enzyme_complex_name_result,
      const bool& is_reverse, bool specify_layer = false, bool specify_cycles = false);
    void addProducts_1(Model<TensorT> & model, const BiochemicalReaction& reaction, const std::string & name, const std::string & module_name,
      const std::shared_ptr<WeightInitOp<TensorT>> & weight_init, const std::shared_ptr<SolverOp<TensorT>> & solver,
      std::string& enzyme_complex_name, std::string& enzyme_complex_name_tmp1, std::string& enzyme_complex_name_tmp2, std::string& enzyme_complex_name_result,
      const bool& is_reverse, bool specify_layer = false, bool specify_cycles = false);
    void addReactants_2(Model<TensorT> & model, const BiochemicalReaction& reaction, const std::string & name, const std::string & module_name,
      const std::shared_ptr<WeightInitOp<TensorT>> & weight_init, const std::shared_ptr<SolverOp<TensorT>> & solver,
      std::string& enzyme_complex_name, std::string& enzyme_complex_name_tmp1, std::string& enzyme_complex_name_tmp2, std::string& enzyme_complex_name_result,
      const bool& is_reverse, bool specify_layer = false, bool specify_cycles = false);
    void addProducts_2(Model<TensorT> & model, const BiochemicalReaction& reaction, const std::string & name, const std::string & module_name,
      const std::shared_ptr<WeightInitOp<TensorT>> & weight_init, const std::shared_ptr<SolverOp<TensorT>> & solver,
      std::string& enzyme_complex_name, std::string& enzyme_complex_name_tmp1, std::string& enzyme_complex_name_tmp2, std::string& enzyme_complex_name_result,
      const bool& is_reverse, bool specify_layer = false, bool specify_cycles = false);
  };

	template<typename TensorT>
	inline void ModelBuilderExperimental<TensorT>::addInteractionGraph(const std::map<std::string, std::vector<std::pair<std::string, std::string>>>& elementary_graph,
		Model<TensorT> & model, const std::string & name, const std::string& module_name,
		const std::shared_ptr<ActivationOp<TensorT>>& node_activation, const std::shared_ptr<ActivationOp<TensorT>>& node_activation_grad,
		const std::shared_ptr<IntegrationOp<TensorT>>& node_integration, const std::shared_ptr<IntegrationErrorOp<TensorT>>& node_integration_error, const std::shared_ptr<IntegrationWeightGradOp<TensorT>>& node_integration_weight_grad,
		const std::shared_ptr<WeightInitOp<TensorT>> & weight_init, const std::shared_ptr<SolverOp<TensorT>> & solver)
	{
		for (const auto& element : elementary_graph) {
			for (const std::pair<std::string, std::string>& source_sink : element.second) {
				Node<TensorT> source_node(source_sink.first, NodeType::hidden, NodeStatus::initialized, node_activation, node_activation_grad, node_integration, node_integration_error, node_integration_weight_grad);
				Node<TensorT> sink_node(source_sink.second, NodeType::hidden, NodeStatus::initialized, node_activation, node_activation_grad, node_integration, node_integration_error, node_integration_weight_grad);
				source_node.setModuleName(module_name);
				sink_node.setModuleName(module_name);
				Weight<TensorT> weight(element.first, weight_init, solver);
				weight.setModuleName(module_name);
				Link link(element.first, source_sink.first, source_sink.second, element.first);
				link.setModuleName(module_name);
				model.addNodes({ source_node, sink_node });
				model.addLinks({ link });
				model.addWeights({ weight });
			}
		}
	}
  template<typename TensorT>
  inline void ModelBuilderExperimental<TensorT>::addBiochemicalReactions(Model<TensorT>& model, const BiochemicalReactions& biochemicalReactions, const std::string & name, const std::string & module_name, const std::shared_ptr<WeightInitOp<TensorT>>& weight_init, const std::shared_ptr<SolverOp<TensorT>>& solver, const int& version, bool specify_layer, bool specify_cycles)
  {
    for (const auto& biochemicalReaction : biochemicalReactions) {
      if (!biochemicalReaction.second.used) continue; // Skip specified reactions

      // intialize the enzyme complex names
      std::string enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result;

      // parse the reactants
      if (version == 1)
        addReactants_1(model, biochemicalReaction.second, name, module_name, weight_init, solver,
          enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result, false, specify_layer, specify_cycles);
      else if (version == 2)
        addReactants_2(model, biochemicalReaction.second, name, module_name, weight_init, solver,
          enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result, false, specify_layer, specify_cycles);

      // parse the products
      if (version == 1)
        addProducts_1(model, biochemicalReaction.second, name, module_name, weight_init, solver,
          enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result, false, specify_layer, specify_cycles);
      else if (version == 2)
        addProducts_2(model, biochemicalReaction.second, name, module_name, weight_init, solver,
          enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result, false, specify_layer, specify_cycles);

      if (biochemicalReaction.second.reversibility) {
        // flip the products and reactants and repeat the above
        BiochemicalReaction reverse_reaction = biochemicalReaction.second;
        reverse_reaction.products_ids = biochemicalReaction.second.reactants_ids;
        reverse_reaction.products_stoichiometry = biochemicalReaction.second.reactants_stoichiometry;
        reverse_reaction.reactants_ids = biochemicalReaction.second.products_ids;
        reverse_reaction.reactants_stoichiometry = biochemicalReaction.second.products_stoichiometry;

        // initialize the comples names
        std::string enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result;

        // parse the reactants
        if (version == 1)
          addReactants_1(model, reverse_reaction, name, module_name, weight_init, solver,
            enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result, true, specify_layer, specify_cycles);
        else if (version == 2)
          addReactants_1(model, reverse_reaction, name, module_name, weight_init, solver,
            enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result, true, specify_layer, specify_cycles);

        // parse the products
        if (version == 1)
          addProducts_1(model, reverse_reaction, name, module_name, weight_init, solver,
            enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result, true, specify_layer, specify_cycles);
        else if (version == 2)
          addProducts_2(model, reverse_reaction, name, module_name, weight_init, solver,
            enzyme_complex_name, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, enzyme_complex_name_result, true, specify_layer, specify_cycles);
      }
    }
  }
  template<typename TensorT>
  inline void ModelBuilderExperimental<TensorT>::addReactants_1(Model<TensorT>& model, const BiochemicalReaction & reaction, 
    const std::string & name, const std::string & module_name, 
    const std::shared_ptr<WeightInitOp<TensorT>>& weight_init, const std::shared_ptr<SolverOp<TensorT>>& solver,
    std::string & enzyme_complex_name, std::string & enzyme_complex_name_tmp1, std::string & enzyme_complex_name_tmp2, std::string & enzyme_complex_name_result,
    const bool& is_reverse, bool specify_layer, bool specify_cycles)
  {
    if (is_reverse)
      enzyme_complex_name = reaction.reaction_id + "_reverse";
    else
      enzyme_complex_name = reaction.reaction_id;

    for (int i = 0; i < reaction.reactants_ids.size(); ++i) {
      for (int stoich = 0; stoich < std::abs(reaction.reactants_stoichiometry[i]); ++stoich) {
        enzyme_complex_name_tmp1 = enzyme_complex_name + ":" + reaction.reactants_ids[i];
        enzyme_complex_name_tmp2 = enzyme_complex_name + "::" + reaction.reactants_ids[i];
        enzyme_complex_name_result = enzyme_complex_name + "&" + reaction.reactants_ids[i];

        // Add the nodes for the enzyme complex, enzyme complex tmp, reactant, and enzyme complex result
        Node<TensorT> enzyme_complex(enzyme_complex_name, NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
        enzyme_complex.setModuleName(module_name);
        //if (specify_layer) enzyme_complex.setLayerName(module_name + "-" + enzyme_complex_name + "-Enz");
        if (specify_layer) enzyme_complex.setLayerName(module_name + "-Enz");
        Node<TensorT> enzyme_complex_tmp1(enzyme_complex_name_tmp1, NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new MinOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new MinErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new MinWeightGradOp<TensorT>()));
        enzyme_complex_tmp1.setModuleName(module_name);
        //if (specify_layer) enzyme_complex_tmp1.setLayerName(module_name + "-" + enzyme_complex_name + "-EnzTmp1");
        if (specify_layer) enzyme_complex_tmp1.setLayerName(module_name + "-EnzTmp1");
        Node<TensorT> enzyme_complex_tmp2(enzyme_complex_name_tmp2, NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
        enzyme_complex_tmp2.setModuleName(module_name);
        //if (specify_layer) enzyme_complex_tmp2.setLayerName(module_name + "-" + enzyme_complex_name + "-EnzTmp2");
        if (specify_layer) enzyme_complex_tmp2.setLayerName(module_name + "-EnzTmp2");
        Node<TensorT> reactant(reaction.reactants_ids[i], NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
        reactant.setModuleName(module_name);
        //if (specify_layer) reactant.setLayerName(module_name + "-" + reaction.reactants_ids[i] + "-" + "-Met");
        if (specify_layer) reactant.setLayerName(module_name + "-Met");
        Node<TensorT> enzyme_complex_result(enzyme_complex_name_result, NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
        enzyme_complex_result.setModuleName(module_name);
        //if (specify_layer) enzyme_complex_result.setLayerName(module_name + "-" + enzyme_complex_name + "-Result");
        if (specify_layer) enzyme_complex_result.setLayerName(module_name + "-Result");

        // Add the enzyme to complex link and weight
        std::string weight_name_1 = enzyme_complex_name + "_to_" + enzyme_complex_name_tmp1;
        Weight<TensorT> weight1(weight_name_1, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight1.setModuleName(module_name);
        if (specify_layer) weight1.setLayerName(module_name + "-Enz_to_EnzTmp1");
        Link link1(weight_name_1, enzyme_complex_name, enzyme_complex_name_tmp1, weight_name_1);
        link1.setModuleName(module_name);

        // Add the reactant to complex link and weight
        std::string weight_name_2 = reaction.reactants_ids[i] + "_to_" + enzyme_complex_name_tmp1;
        Weight<TensorT> weight2(weight_name_2, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight2.setModuleName(module_name);
        if (specify_layer) weight2.setLayerName(module_name + "-Met_to_EnzTmp1");
        Link link2(weight_name_2, reaction.reactants_ids[i], enzyme_complex_name_tmp1, weight_name_2);
        link2.setModuleName(module_name);

        // Add the reactant to complex link and weight
        std::string weight_name_3 = enzyme_complex_name_tmp1 + "_to_" + enzyme_complex_name_tmp2;
        Weight<TensorT> weight3(weight_name_3, weight_init, solver);
        weight3.setModuleName(module_name);
        if (specify_layer) weight3.setLayerName(module_name + "-EnzTmp1_to_EnzTmp2");
        Link link3(weight_name_3, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, weight_name_3);
        link3.setModuleName(module_name);

        // Add the enzyme loss pseudo link and weight
        std::string weight_name_4 = enzyme_complex_name_tmp2 + "_to_" + enzyme_complex_name;
        Weight<TensorT> weight4(weight_name_4, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(-1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight4.setModuleName(module_name);
        if (specify_layer) weight4.setLayerName(module_name + "-EnzTmp2_to_Enz");
        Link link4(weight_name_4, enzyme_complex_name_tmp2, enzyme_complex_name, weight_name_4);
        link4.setModuleName(module_name);
        if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, enzyme_complex_name));

        // Add the reactant loss pseudo link and weight
        std::string weight_name_5 = enzyme_complex_name_tmp2 + "_to_" + reaction.reactants_ids[i];
        Weight<TensorT> weight5(weight_name_5, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(-1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight5.setModuleName(module_name);
        if (specify_layer) weight5.setLayerName(module_name + "-EnzTmp2_to_Met");
        Link link5(weight_name_5, enzyme_complex_name_tmp2, reaction.reactants_ids[i], weight_name_5);
        link5.setModuleName(module_name);
        if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, reaction.reactants_ids[i]));

        // Add the result enzyme complex link and weight
        std::string weight_name_result = enzyme_complex_name_tmp2 + "_to_" + enzyme_complex_name_result;
        Weight<TensorT> weight_result(weight_name_result, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight_result.setModuleName(module_name);
        if (specify_layer) weight_result.setLayerName(module_name + "-EnzTmp2_to_Result");
        Link link_result(weight_name_result, enzyme_complex_name_tmp2, enzyme_complex_name_result, weight_name_result);
        link_result.setModuleName(module_name);

        // Add all of the nodes, links, and weights to the model
        model.addNodes({ enzyme_complex, enzyme_complex_tmp1, reactant, enzyme_complex_tmp2, enzyme_complex_result });
        model.addLinks({ link1, link2, link3, link4, link5, link_result });
        model.addWeights({ weight1, weight2, weight3, weight4, weight5, weight_result });

        // Update the enzyme complex name with the result
        enzyme_complex_name = enzyme_complex_name_result;
      }
    }
  }
  template<typename TensorT>
  inline void ModelBuilderExperimental<TensorT>::addProducts_1(Model<TensorT>& model, const BiochemicalReaction & reaction, 
    const std::string & name, const std::string & module_name, 
    const std::shared_ptr<WeightInitOp<TensorT>>& weight_init, const std::shared_ptr<SolverOp<TensorT>>& solver, 
    std::string & enzyme_complex_name, std::string & enzyme_complex_name_tmp1, std::string & enzyme_complex_name_tmp2, std::string & enzyme_complex_name_result,
    const bool& is_reverse, bool specify_layer, bool specify_cycles)
  {
    // make the products enzyme complex name
    std::vector<std::string> enzyme_complex_names_tmp1, enzyme_complex_names_tmp2, enzyme_complex_names_result;
    if (is_reverse) {
      enzyme_complex_names_tmp1.push_back(reaction.reaction_id + "_reverse");
      enzyme_complex_names_tmp2.push_back(reaction.reaction_id + "_reverse");
      enzyme_complex_names_result.push_back(reaction.reaction_id + "_reverse");
    }
    else {
      enzyme_complex_names_tmp1.push_back(reaction.reaction_id);
      enzyme_complex_names_tmp2.push_back(reaction.reaction_id);
      enzyme_complex_names_result.push_back(reaction.reaction_id);
    }
    for (int i = reaction.products_ids.size() - 1; i >= 0; --i) {
      for (int stoich = 0; stoich < std::abs(reaction.products_stoichiometry[i]); ++stoich) {
        enzyme_complex_names_tmp1.push_back(enzyme_complex_names_result.back() + "::" + reaction.products_ids[i]);
        enzyme_complex_names_tmp2.push_back(enzyme_complex_names_result.back() + ":" + reaction.products_ids[i]);
        enzyme_complex_names_result.push_back(enzyme_complex_names_result.back() + "&" + reaction.products_ids[i]);
      }
    }

    // parse the products
    for (int i = 0; i < reaction.products_ids.size(); ++i) {
      for (int stoich = 0; stoich < std::abs(reaction.products_stoichiometry[i]); ++stoich) {
        enzyme_complex_name_tmp1 = enzyme_complex_names_tmp1[enzyme_complex_names_tmp1.size() - 1 - i];
        enzyme_complex_name_tmp2 = enzyme_complex_names_tmp2[enzyme_complex_names_tmp2.size() - 1 - i];
        enzyme_complex_name_result = enzyme_complex_names_result[enzyme_complex_names_result.size() - 2 - i];

        //// Experimental
        //if (i == reaction.products_ids.size() - 1)
        //  enzyme_complex_name_result = enzyme_complex_names_result[enzyme_complex_names_result.size() - 2 - i] + "_inactivated";

        // Add the nodes for the enzyme complex, enzyme complex tmp, product, and enzyme complex result
        Node<TensorT> enzyme_complex(enzyme_complex_name, NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
        enzyme_complex.setModuleName(module_name);
        //if (specify_layer) enzyme_complex.setLayerName(module_name + "-" + enzyme_complex_name + "-Enz");
        if (specify_layer) enzyme_complex.setLayerName(module_name + "-Enz");
        Node<TensorT> enzyme_complex_tmp1(enzyme_complex_name_tmp1, NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new MinOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new MinErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new MinWeightGradOp<TensorT>()));
        enzyme_complex_tmp1.setModuleName(module_name);
        //if (specify_layer) enzyme_complex_tmp1.setLayerName(module_name + "-" + enzyme_complex_name + "-EnzTmp1");
        if (specify_layer) enzyme_complex_tmp1.setLayerName(module_name + "-EnzTmp1");
        Node<TensorT> enzyme_complex_tmp2(enzyme_complex_name_tmp2, NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
        enzyme_complex_tmp2.setModuleName(module_name);
        //if (specify_layer) enzyme_complex_tmp2.setLayerName(module_name + "-" + enzyme_complex_name + "-EnzTmp2");
        if (specify_layer) enzyme_complex_tmp2.setLayerName(module_name + "-EnzTmp2");
        Node<TensorT> product(reaction.products_ids[i], NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
        product.setModuleName(module_name);
        //if (specify_layer) product.setLayerName(module_name + "-" + reaction.products_ids[i] + "-Met");
        if (specify_layer) product.setLayerName(module_name + "-Met");
        Node<TensorT> enzyme_complex_result(enzyme_complex_name_result, NodeType::hidden, NodeStatus::initialized,
          std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
          std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
        enzyme_complex_result.setModuleName(module_name);
        //if (specify_layer) enzyme_complex_result.setLayerName(module_name + "-" + enzyme_complex_name + "-Result");
        if (specify_layer) enzyme_complex_result.setLayerName(module_name + "-Result");

        // Add the enzyme to complex link and weight
        std::string weight_name_1 = enzyme_complex_name + "_to_" + enzyme_complex_name_tmp1;
        Weight<TensorT> weight1(weight_name_1, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight1.setModuleName(module_name);
        if (specify_layer) weight1.setLayerName(module_name + "-Enz_to_EnzTmp1");
        Link link1(weight_name_1, enzyme_complex_name, enzyme_complex_name_tmp1, weight_name_1);
        link1.setModuleName(module_name);

        // Add the complex tmp1 to tmp2
        std::string weight_name_3 = enzyme_complex_name_tmp1 + "_to_" + enzyme_complex_name_tmp2;
        Weight<TensorT> weight3(weight_name_3, weight_init, solver);
        weight3.setModuleName(module_name);
        if (specify_layer) weight3.setLayerName(module_name + "-EnzTmp1_to_EnzTmp2");
        Link link3(weight_name_3, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, weight_name_3);
        link3.setModuleName(module_name);

        // Add the enzyme loss pseudo link and weight
        std::string weight_name_4 = enzyme_complex_name_tmp2 + "_to_" + enzyme_complex_name;
        Weight<TensorT> weight4(weight_name_4, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(-1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight4.setModuleName(module_name);
        if (specify_layer) weight4.setLayerName(module_name + "-EnzTmp2_to_Enz");
        Link link4(weight_name_4, enzyme_complex_name_tmp2, enzyme_complex_name, weight_name_4);
        link4.setModuleName(module_name);
        if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, enzyme_complex_name));

        // Add the resulting product
        std::string weight_name_5 = enzyme_complex_name_tmp2 + "_to_" + reaction.products_ids[i];
        Weight<TensorT> weight5(weight_name_5, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight5.setModuleName(module_name);
        if (specify_layer) weight5.setLayerName(module_name + "-EnzTmp2_to_Met");
        Link link5(weight_name_5, enzyme_complex_name_tmp2, reaction.products_ids[i], weight_name_5);
        link5.setModuleName(module_name);
        if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, reaction.products_ids[i]));

        // Add the result enzyme complex link and weight
        std::string weight_name_result = enzyme_complex_name_tmp2 + "_to_" + enzyme_complex_name_result;
        Weight<TensorT> weight_result(weight_name_result, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
        weight_result.setModuleName(module_name);
        if (specify_layer) weight_result.setLayerName(module_name + "-EnzTmp2_to_Result");
        Link link_result(weight_name_result, enzyme_complex_name_tmp2, enzyme_complex_name_result, weight_name_result);
        link_result.setModuleName(module_name);
        if (i == reaction.products_ids.size()-1 && stoich == std::abs(reaction.products_stoichiometry[i]) - 1 && specify_cycles)
          model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, enzyme_complex_name_result));

        // Add all of the nodes, links, and weights to the model
        model.addNodes({ enzyme_complex, enzyme_complex_tmp1, product, enzyme_complex_tmp2, enzyme_complex_result });
        model.addLinks({ link1, link3, link4, link5, link_result });
        model.addWeights({ weight1, weight3, weight4, weight5, weight_result });

        // Update the enzyme complex name with the result
        enzyme_complex_name = enzyme_complex_name_result;
      }
    }

    //// Experimental
    //// Add the reactivated complex link and weight
    //std::string weight_name_activated = enzyme_complex_name + "_to_" + enzyme_complex_names_result[0];
    //Weight<TensorT> weight_activated(weight_name_activated, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    //weight_activated.setModuleName(module_name);
    //Link link_activated(weight_name_activated, enzyme_complex_name, enzyme_complex_names_result[0], weight_name_activated);
    //link_activated.setModuleName(module_name);
    //model.addLinks({ link_activated });
    //model.addWeights({ weight_activated });
  }
  template<typename TensorT>
  inline void ModelBuilderExperimental<TensorT>::addReactants_2(Model<TensorT>& model, const BiochemicalReaction & reaction,
    const std::string & name, const std::string & module_name,
    const std::shared_ptr<WeightInitOp<TensorT>>& weight_init, const std::shared_ptr<SolverOp<TensorT>>& solver,
    std::string & enzyme_complex_name, std::string & enzyme_complex_name_tmp1, std::string & enzyme_complex_name_tmp2, std::string & enzyme_complex_name_result,
    const bool& is_reverse, bool specify_layer, bool specify_cycles)
  {
    if (is_reverse)
      enzyme_complex_name = reaction.reaction_id + "_reverse";
    else
      enzyme_complex_name = reaction.reaction_id;

    // Create the intermediate enzyme complex names
    enzyme_complex_name_tmp1 = enzyme_complex_name;
    enzyme_complex_name_tmp2 = enzyme_complex_name;
    enzyme_complex_name_result = enzyme_complex_name;
    for (int i = 0; i < reaction.reactants_ids.size(); ++i) {
      enzyme_complex_name_tmp1 = enzyme_complex_name_tmp1 + ":" + reaction.reactants_ids[i];
      enzyme_complex_name_tmp2 = enzyme_complex_name_tmp2 + "::" + reaction.reactants_ids[i];
      enzyme_complex_name_result = enzyme_complex_name_result + "&" + reaction.reactants_ids[i];
    }

    // Add the nodes for the enzyme complex, enzyme complex tmp, reactant, and enzyme complex result
    Node<TensorT> enzyme_complex(enzyme_complex_name, NodeType::hidden, NodeStatus::initialized,
      std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
      std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
    enzyme_complex.setModuleName(module_name);
    //if (specify_layer) enzyme_complex.setLayerName(module_name + "-" + enzyme_complex_name + "-Enz");
    if (specify_layer) enzyme_complex.setLayerName(module_name + "-Enz");
    Node<TensorT> enzyme_complex_tmp1(enzyme_complex_name_tmp1, NodeType::hidden, NodeStatus::initialized,
      std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
      std::shared_ptr<IntegrationOp<TensorT>>(new MinOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new MinErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new MinWeightGradOp<TensorT>()));
    enzyme_complex_tmp1.setModuleName(module_name);
    //if (specify_layer) enzyme_complex_tmp1.setLayerName(module_name + "-" + enzyme_complex_name + "-EnzTmp1");
    if (specify_layer) enzyme_complex_tmp1.setLayerName(module_name + "-EnzTmp1");
    Node<TensorT> enzyme_complex_tmp2(enzyme_complex_name_tmp2, NodeType::hidden, NodeStatus::initialized,
      std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
      std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
    enzyme_complex_tmp2.setModuleName(module_name);
    //if (specify_layer) enzyme_complex_tmp2.setLayerName(module_name + "-" + enzyme_complex_name + "-EnzTmp2");
    if (specify_layer) enzyme_complex_tmp2.setLayerName(module_name + "-EnzTmp2");
    Node<TensorT> enzyme_complex_result(enzyme_complex_name_result, NodeType::hidden, NodeStatus::initialized,
      std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
      std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
    enzyme_complex_result.setModuleName(module_name);
    //if (specify_layer) enzyme_complex_result.setLayerName(module_name + "-" + enzyme_complex_name + "-Result");
    if (specify_layer) enzyme_complex_result.setLayerName(module_name + "-Result");

    // Add the enzyme to complex link and weight
    std::string weight_name_1 = enzyme_complex_name + "_to_" + enzyme_complex_name_tmp1;
    Weight<TensorT> weight1(weight_name_1, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight1.setModuleName(module_name);
    if (specify_layer) weight1.setLayerName(module_name + "-Enz_to_EnzTmp1");
    Link link1(weight_name_1, enzyme_complex_name, enzyme_complex_name_tmp1, weight_name_1);
    link1.setModuleName(module_name);

    // Add the reactant to complex link and weight
    std::string weight_name_3 = enzyme_complex_name_tmp1 + "_to_" + enzyme_complex_name_tmp2;
    Weight<TensorT> weight3(weight_name_3, weight_init, solver);
    weight3.setModuleName(module_name);
    if (specify_layer) weight3.setLayerName(module_name + "-EnzTmp1_to_EnzTmp2");
    Link link3(weight_name_3, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, weight_name_3);
    link3.setModuleName(module_name);

    // Add the enzyme loss pseudo link and weight
    std::string weight_name_4 = enzyme_complex_name_tmp2 + "_to_" + enzyme_complex_name;
    Weight<TensorT> weight4(weight_name_4, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(-1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight4.setModuleName(module_name);
    if (specify_layer) weight4.setLayerName(module_name + "-EnzTmp2_to_Enz");
    Link link4(weight_name_4, enzyme_complex_name_tmp2, enzyme_complex_name, weight_name_4);
    link4.setModuleName(module_name);
    if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, enzyme_complex_name));

    // Add the result enzyme complex link and weight
    std::string weight_name_result = enzyme_complex_name_tmp2 + "_to_" + enzyme_complex_name_result;
    Weight<TensorT> weight_result(weight_name_result, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight_result.setModuleName(module_name);
    if (specify_layer) weight_result.setLayerName(module_name + "-EnzTmp2_to_Result");
    Link link_result(weight_name_result, enzyme_complex_name_tmp2, enzyme_complex_name_result, weight_name_result);
    link_result.setModuleName(module_name);

    // Add "self" enzyme link
    std::string weight_name_1_self = enzyme_complex_name + "_to_" + enzyme_complex_name;
    Weight<TensorT> weight1_self(weight_name_1_self, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight1_self.setModuleName(module_name);
    if (specify_layer) weight1_self.setLayerName(module_name + "-Enz_to_Enz");
    Link link1_self(weight_name_1_self, enzyme_complex_name, enzyme_complex_name, weight_name_1_self);
    link1_self.setModuleName(module_name);
    if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name, enzyme_complex_name));

    // Add "self" result link
    std::string weight_name_result_self = enzyme_complex_name_result + "_to_" + enzyme_complex_name_result;
    Weight<TensorT> weight_result_self(weight_name_result_self, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight_result_self.setModuleName(module_name);
    if (specify_layer) weight_result_self.setLayerName(module_name + "-Result_to_Result");
    Link link_result_self(weight_name_result_self, enzyme_complex_name_result, enzyme_complex_name_result, weight_name_result_self);
    link_result_self.setModuleName(module_name);
    if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_result, enzyme_complex_name_result));

    for (int i = 0; i < reaction.reactants_ids.size(); ++i) {
      const int stoich = std::abs(reaction.reactants_stoichiometry[i]);

      Node<TensorT> reactant(reaction.reactants_ids[i], NodeType::hidden, NodeStatus::initialized,
        std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
        std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
      reactant.setModuleName(module_name);
      //if (specify_layer) reactant.setLayerName(module_name + "-" + reaction.reactants_ids[i] + "-" + "-Met");
      if (specify_layer) reactant.setLayerName(module_name + "-Met");

      // Add the reactant to complex link and weight
      std::string weight_name_2 = reaction.reactants_ids[i] + "_to_" + enzyme_complex_name_tmp1;
      Weight<TensorT> weight2(weight_name_2, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0 / (TensorT)stoich)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
      weight2.setModuleName(module_name);
      if (specify_layer) weight2.setLayerName(module_name + "-Met_to_EnzTmp1");
      Link link2(weight_name_2, reaction.reactants_ids[i], enzyme_complex_name_tmp1, weight_name_2);
      link2.setModuleName(module_name);

      // Add the reactant loss pseudo link and weight
      std::string weight_name_5 = enzyme_complex_name_tmp2 + "_to_" + reaction.reactants_ids[i];
      Weight<TensorT> weight5(weight_name_5, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(-(TensorT)stoich)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
      weight5.setModuleName(module_name);
      if (specify_layer) weight5.setLayerName(module_name + "-EnzTmp2_to_Met");
      Link link5(weight_name_5, enzyme_complex_name_tmp2, reaction.reactants_ids[i], weight_name_5);
      link5.setModuleName(module_name);
      if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, reaction.reactants_ids[i]));

      // Add the reactant "self" link
      std::string weight_name_2_self = reaction.reactants_ids[i] + "_to_" + reaction.reactants_ids[i];
      Weight<TensorT> weight2_self(weight_name_2_self, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
      weight2_self.setModuleName(module_name);
      if (specify_layer) weight2_self.setLayerName(module_name + "-Met_to_Met");
      Link link2_self(weight_name_2_self, reaction.reactants_ids[i], reaction.reactants_ids[i], weight_name_2_self);
      link2_self.setModuleName(module_name);
      if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(reaction.reactants_ids[i], reaction.reactants_ids[i]));

      // Add all of the nodes, links, and weights to the model
      model.addNodes({ reactant });
      model.addLinks({ link2, link5, link2_self });
      model.addWeights({ weight2, weight5, weight2_self });
    }

    // Add all of the nodes, links, and weights to the model
    model.addNodes({ enzyme_complex, enzyme_complex_tmp1, enzyme_complex_tmp2, enzyme_complex_result });
    model.addLinks({ link1, link3, link4, link_result, link1_self, link_result_self });
    model.addWeights({ weight1, weight3, weight4, weight_result, weight1_self, weight_result_self });

    // Update the enzyme complex name with the result
    enzyme_complex_name = enzyme_complex_name_result;
  }
  template<typename TensorT>
  inline void ModelBuilderExperimental<TensorT>::addProducts_2(Model<TensorT>& model, const BiochemicalReaction & reaction,
    const std::string & name, const std::string & module_name,
    const std::shared_ptr<WeightInitOp<TensorT>>& weight_init, const std::shared_ptr<SolverOp<TensorT>>& solver,
    std::string & enzyme_complex_name, std::string & enzyme_complex_name_tmp1, std::string & enzyme_complex_name_tmp2, std::string & enzyme_complex_name_result,
    const bool& is_reverse, bool specify_layer, bool specify_cycles)
  {
    // make the products enzyme complex name
    if (is_reverse) {
      enzyme_complex_name_tmp1 = reaction.reaction_id + "_reverse";
      enzyme_complex_name_tmp2 = reaction.reaction_id + "_reverse";
      enzyme_complex_name_result = reaction.reaction_id + "_reverse";
    }
    else {
      enzyme_complex_name_tmp1 = reaction.reaction_id;
      enzyme_complex_name_tmp2 = reaction.reaction_id;
      enzyme_complex_name_result = reaction.reaction_id;
    }
    for (int i = reaction.products_ids.size() - 1; i >= 0; --i) {
      enzyme_complex_name_tmp1 = enzyme_complex_name_tmp1 + "::" + reaction.products_ids[i];
      enzyme_complex_name_tmp2 = enzyme_complex_name_tmp2 + ":" + reaction.products_ids[i];
    }

    // Add the nodes for the enzyme complex, enzyme complex tmp, product, and enzyme complex result
    Node<TensorT> enzyme_complex(enzyme_complex_name, NodeType::hidden, NodeStatus::initialized,
      std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
      std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
    enzyme_complex.setModuleName(module_name);
    //if (specify_layer) enzyme_complex.setLayerName(module_name + "-" + enzyme_complex_name + "-Enz");
    if (specify_layer) enzyme_complex.setLayerName(module_name + "-Enz");
    Node<TensorT> enzyme_complex_tmp1(enzyme_complex_name_tmp1, NodeType::hidden, NodeStatus::initialized,
      std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
      std::shared_ptr<IntegrationOp<TensorT>>(new MinOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new MinErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new MinWeightGradOp<TensorT>()));
    enzyme_complex_tmp1.setModuleName(module_name);
    //if (specify_layer) enzyme_complex_tmp1.setLayerName(module_name + "-" + enzyme_complex_name + "-EnzTmp1");
    if (specify_layer) enzyme_complex_tmp1.setLayerName(module_name + "-EnzTmp1");
    Node<TensorT> enzyme_complex_tmp2(enzyme_complex_name_tmp2, NodeType::hidden, NodeStatus::initialized,
      std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
      std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
    enzyme_complex_tmp2.setModuleName(module_name);
    //if (specify_layer) enzyme_complex_tmp2.setLayerName(module_name + "-" + enzyme_complex_name + "-EnzTmp2");
    if (specify_layer) enzyme_complex_tmp2.setLayerName(module_name + "-EnzTmp2");
    Node<TensorT> enzyme_complex_result(enzyme_complex_name_result, NodeType::hidden, NodeStatus::initialized,
      std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
      std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
    enzyme_complex_result.setModuleName(module_name);
    //if (specify_layer) enzyme_complex_result.setLayerName(module_name + "-" + enzyme_complex_name + "-Result");
    if (specify_layer) enzyme_complex_result.setLayerName(module_name + "-Result");

    // Add the enzyme to complex link and weight
    std::string weight_name_1 = enzyme_complex_name + "_to_" + enzyme_complex_name_tmp1;
    Weight<TensorT> weight1(weight_name_1, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight1.setModuleName(module_name);
    if (specify_layer) weight1.setLayerName(module_name + "-Enz_to_EnzTmp1");
    Link link1(weight_name_1, enzyme_complex_name, enzyme_complex_name_tmp1, weight_name_1);
    link1.setModuleName(module_name);

    // Add the complex tmp1 to tmp2
    std::string weight_name_3 = enzyme_complex_name_tmp1 + "_to_" + enzyme_complex_name_tmp2;
    Weight<TensorT> weight3(weight_name_3, weight_init, solver);
    weight3.setModuleName(module_name);
    if (specify_layer) weight3.setLayerName(module_name + "-EnzTmp1_to_EnzTmp2");
    Link link3(weight_name_3, enzyme_complex_name_tmp1, enzyme_complex_name_tmp2, weight_name_3);
    link3.setModuleName(module_name);

    // Add the enzyme loss pseudo link and weight
    std::string weight_name_4 = enzyme_complex_name_tmp2 + "_to_" + enzyme_complex_name;
    Weight<TensorT> weight4(weight_name_4, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(-1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight4.setModuleName(module_name);
    if (specify_layer) weight4.setLayerName(module_name + "-EnzTmp2_to_Enz");
    Link link4(weight_name_4, enzyme_complex_name_tmp2, enzyme_complex_name, weight_name_4);
    link4.setModuleName(module_name);
    if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, enzyme_complex_name));

    // Add the result enzyme complex link and weight
    std::string weight_name_result = enzyme_complex_name_tmp2 + "_to_" + enzyme_complex_name_result;
    Weight<TensorT> weight_result(weight_name_result, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight_result.setModuleName(module_name);
    if (specify_layer) weight_result.setLayerName(module_name + "-EnzTmp2_to_Result");
    Link link_result(weight_name_result, enzyme_complex_name_tmp2, enzyme_complex_name_result, weight_name_result);
    link_result.setModuleName(module_name);
    if (specify_cycles)
      model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, enzyme_complex_name_result));

    // Add the enzyme "self" link and weight
    std::string weight_name_1_self = enzyme_complex_name + "_to_" + enzyme_complex_name;
    Weight<TensorT> weight1_self(weight_name_1_self, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight1_self.setModuleName(module_name);
    if (specify_layer) weight1_self.setLayerName(module_name + "-Enz_to_Enz");
    Link link1_self(weight_name_1_self, enzyme_complex_name, enzyme_complex_name, weight_name_1_self);
    link1_self.setModuleName(module_name);
    if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name, enzyme_complex_name));

    // Add the result enzyme complex "self" link and weight
    std::string weight_name_result_self = enzyme_complex_name_result + "_to_" + enzyme_complex_name_result;
    Weight<TensorT> weight_result_self(weight_name_result_self, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1.0)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
    weight_result_self.setModuleName(module_name);
    if (specify_layer) weight_result_self.setLayerName(module_name + "-Result_to_Result");
    Link link_result_self(weight_name_result_self, enzyme_complex_name_result, enzyme_complex_name_result, weight_name_result_self);
    link_result_self.setModuleName(module_name);
    if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_result, enzyme_complex_name_result));

    // parse the products
    for (int i = 0; i < reaction.products_ids.size(); ++i) {
      const int stoich = std::abs(reaction.products_stoichiometry[i]);

      Node<TensorT> product(reaction.products_ids[i], NodeType::hidden, NodeStatus::initialized,
        std::shared_ptr<ActivationOp<TensorT>>(new ReLUOp<TensorT>()), std::shared_ptr<ActivationOp<TensorT>>(new ReLUGradOp<TensorT>()),
        std::shared_ptr<IntegrationOp<TensorT>>(new SumOp<TensorT>()), std::shared_ptr<IntegrationErrorOp<TensorT>>(new SumErrorOp<TensorT>()), std::shared_ptr<IntegrationWeightGradOp<TensorT>>(new SumWeightGradOp<TensorT>()));
      product.setModuleName(module_name);
      //if (specify_layer) product.setLayerName(module_name + "-" + reaction.products_ids[i] + "-Met");
      if (specify_layer) product.setLayerName(module_name + "-Met");

      // Add the resulting product
      std::string weight_name_5 = enzyme_complex_name_tmp2 + "_to_" + reaction.products_ids[i];
      Weight<TensorT> weight5(weight_name_5, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>((TensorT)stoich)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
      weight5.setModuleName(module_name);
      if (specify_layer) weight5.setLayerName(module_name + "-EnzTmp2_to_Met");
      Link link5(weight_name_5, enzyme_complex_name_tmp2, reaction.products_ids[i], weight_name_5);
      link5.setModuleName(module_name);
      if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(enzyme_complex_name_tmp2, reaction.products_ids[i]));

      // Add the resulting product "self" link
      std::string weight_name_5_self = reaction.products_ids[i] + "_to_" + reaction.products_ids[i];
      Weight<TensorT> weight5_self(weight_name_5_self, std::shared_ptr<WeightInitOp<TensorT>>(new ConstWeightInitOp<TensorT>(1)), std::shared_ptr<SolverOp<TensorT>>(new DummySolverOp<TensorT>()));
      weight5_self.setModuleName(module_name);
      if (specify_layer) weight5_self.setLayerName(module_name + "-Met_to_Met");
      Link link5_self(weight_name_5_self, reaction.products_ids[i], reaction.products_ids[i], weight_name_5_self);
      link5_self.setModuleName(module_name);
      if (specify_cycles) model.getCyclicPairs().insert(std::make_pair(reaction.products_ids[i], reaction.products_ids[i]));

      // Add all of the nodes, links, and weights to the model
      model.addNodes({ product });
      model.addLinks({ link5, link5_self });
      model.addWeights({ weight5, weight5_self });
    }

    // Add all of the nodes, links, and weights to the model
    model.addNodes({ enzyme_complex, enzyme_complex_tmp1, enzyme_complex_tmp2, enzyme_complex_result });
    model.addLinks({ link1, link3, link4, link_result, link1_self, link_result_self });
    model.addWeights({ weight1, weight3, weight4, weight_result, weight1_self, weight_result_self });

    // Update the enzyme complex name with the result
    enzyme_complex_name = enzyme_complex_name_result;
  }
}

#endif //SMARTPEAK_MODELBUILDEREXPERIMENTAL_H
/**TODO:  Add copyright*/

#ifndef SMARTPEAK_MODELREPLICATOR_H
#define SMARTPEAK_MODELREPLICATOR_H

#include <SmartPeak/ml/Model.h>

#include <vector>
#include <string>

namespace SmartPeak
{

  /**
    @brief Replicates a model with or without modification (i.e., mutation)
  */
  class ModelReplicator
  {
public:
    ModelReplicator(); ///< Default constructor
    ~ModelReplicator(); ///< Default destructor

    void setNNodeCopies(const int& n_node_copies); ///< n_node_copies setter
    void setNNodeAdditions(const int& n_node_additions); ///< n_node_additions setter
    void setNLinkAdditions(const int& n_link_additions); ///< n_link_additions setter
    void setNNodeDeletions(const int& n_node_deletions); ///< n_node_deletions setter
    void setNLinkDeletions(const int& n_link_deletions); ///< n_link_deletions setter
    void setNWeightChanges(const int& n_weight_changes); ///< n_weight_changes setter
    void setWeightChangeStDev(const float& weight_change_stdev); ///< weight_change_stdev setter

    int getNNodeCopies() const; ///< n_node_copies setter
    int getNNodeAdditions() const; ///< n_node_additions setter
    int getNLinkAdditions() const; ///< n_link_additions setter
    int getNNodeDeletions() const; ///< n_node_deletions setter
    int getNLinkDeletions() const; ///< n_link_deletions setter
    int getNWeightChanges() const; ///< n_weight_changes setter
    float getWeightChangeStDev() const; ///< weight_change_stdev setter
 
    /**
      @brief Make a new baseline model where all layers are fully connected

      @param n_input_nodes The number of input nodes the model should have
      @param n_hidden_nodes The number of hidden nodes the model should have
      @param n_output_nodes The number of output nodes the model should have
      @param hidden_node_activation The activation function of the hidden node to create
      @param output_node_activation The activation function of the output node to create
      @param weight_init Weight init operator to use for hidden and output nodes
      @param solver Solver operator to use for hidden and output nodes
      @param error_function Model loss function
      @param unique_str Optional string to make the model name unique

      @returns A baseline model
    */ 
    Model makeBaselineModel(const int& n_input_nodes, const int& n_hidden_nodes, const int& n_output_nodes,
      const NodeActivation& hidden_node_activation, const NodeActivation& output_node_activation,
      const std::shared_ptr<WeightInitOp>& weight_init, const std::shared_ptr<SolverOp>& solver,
      const ModelLossFunction& error_function, std::string unique_str = "");
 
    /**
      @brief Modify (i.e., mutate) an existing model in place

      @param[in, out] model The model to modify
    */ 
    void modifyModel(Model& model, std::string unique_str = "");
 
    /**
      @brief Select nodes given a set of conditions

      @param[in, out] model The model
      @param node_type_exclude Node types to exclude
      @param node_type_include Node types to include

      @returns A node name
    */ 
    std::vector<std::string> selectNodes(
      const Model& model,
      const std::vector<NodeType>& node_type_exclude,
      const std::vector<NodeType>& node_type_include);

    template<typename T>
    T selectRandomElement(std::vector<T> elements);
 
    /**
      @brief Select random node given a set of conditions

      @param[in, out] model The model
      @param node_type_exclude Node types to exclude
      @param node_type_include Node types to include
      @param node Previous node selected (for distance calculation)
      @param distance_weight Probability weighting to punish more "distant" nodes
      @param direction Source to Sink node direction; options are "forward, reverse"

      @returns A node name
    */ 
    std::string selectRandomNode(
      const Model& model,
      const std::vector<NodeType>& node_type_exclude,
      const std::vector<NodeType>& node_type_include,
      const Node& node, 
      const float& distance_weight,
      const std::string& direction);
    std::string selectRandomNode(
      const Model& model,
      const std::vector<NodeType>& node_type_exclude,
      const std::vector<NodeType>& node_type_include);
 
    /**
      @brief Select random link given a set of conditions

      @param[in, out] model The model
      @param source_node_type_exclude Source node types to exclude
      @param source_node_type_include Source node types to include
      @param sink_node_type_exclude Sink node types to exclude
      @param sink_node_type_include Sink node types to include
      @param direction Source to Sink node direction; options are "forward, reverse"

      @returns A link name
    */ 
    std::string selectRandomLink(
      const Model& model,
      const std::vector<NodeType>& source_node_type_exclude,
      const std::vector<NodeType>& source_node_type_include,
      const std::vector<NodeType>& sink_node_type_exclude,
      const std::vector<NodeType>& sink_node_type_include,
      const std::string& direction);
    std::string selectRandomLink(
      const Model& model,
      const std::vector<NodeType>& source_node_type_exclude,
      const std::vector<NodeType>& source_node_type_include,
      const std::vector<NodeType>& sink_node_type_exclude,
      const std::vector<NodeType>& sink_node_type_include);

    // Model modification operators

    /**
      @brief Copy a node in the model (Layer expansion to the left or right)

      @param[in, out] model The model
    */ 
    void copyNode(Model& model);

    /**
      @brief Add node to the model (Layer injection up or down).
        The method utilizes a modified version of the NEAT algorithm whereby a random
        link is chosen and bifurcated with a new node.  Instead, new nodes are added
        using the following procedure:
        1. an existing node is randomly chosen from the model.
        2. a randomly connected input link to the node is chosen.
          Note that an input link is chose because it is easier
          to exclude input nodes than output nodes.
        3. the chosen node is copied and a new link is added
          between the new node and the existing node.
        4. the new link becomes the input link of the existing node and the output link of the new node, 
          and existing link becomes the input link of the new node.

      References:
        Kenneth O. Stanley & Risto Miikkulainen (2002). "Evolving Neural Networks Through Augmenting Topologies". 
        Evolutionary Computation. 10 (2): 99–127. doi:10.1162/106365602320169811

      @param[in, out] model The model
    */ 
    void addNode(Model& model, std::string unique_str = "");

    /**
      @brief add link to the model

      @param[in, out] model The model
    */ 
    void addLink(Model& model, std::string unique_str = "");

    /**
      @brief delete node to the model

      @param[in, out] model The model
      @param[in] prune_iterations The number of model recursive prune iterations
    */ 
    void deleteNode(Model& model, int prune_iterations = 1e6);

    /**
      @brief delete link to the model

      @param[in, out] model The model
      @param[in] prune_iterations The number of model recursive prune iterations
    */ 
    void deleteLink(Model& model, int prune_iterations = 1e6);

    /**
      @brief modify weights in the model

      @param[in, out] model The model
    */ 
    void modifyWeight(Model& model);

    /**
      @brief Make a unique time stampped hash of the form
        left_str + right_str + timestamp

      @param[in] left_str
      @param[in] right_str

      @returns A unique string hash
    */ 
    std::string makeUniqueHash(const std::string& left_str, const std::string& right_str);    

    /**
      @brief randomly order the mutations

      @returns A random list of mutations types
    */ 
    std::vector<std::string> makeRandomModificationOrder();

private:
    // modification parameters
    int n_node_copies_ = 0; ///< nodes to duplicate in the model (nodes are created through replication)
    int n_node_additions_ = 0; ///< new nodes to add to the model (with a random source and sink connection)
    int n_link_additions_ = 0; ///< new links to add to the model
    int n_node_deletions_ = 0; ///< nodes to remove from the model
    int n_link_deletions_ = 0; ///< links to remove from the model
    int n_weight_changes_ = 0; ///< the number of weights to change in the model
    float weight_change_stdev_ = 0; ///< the standard deviation to change the weights in the model
  };
}

#endif //SMARTPEAK_MODELREPLICATOR_H
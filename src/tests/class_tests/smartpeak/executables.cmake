set(core_executables_list
  Helloworld_test
  Preprocessing_test
  Statistics_test
  StringParsing_test
)

set(io_executables_list
  CSVWriter_test
  DataFile_test
  LinkFile_test
  ModelFile_test
  NodeFile_test
  PopulationTrainerFile_test
  WeightFile_test
)

set(graph_executables_list
  CircuitFinder_test
)

set(ml_executables_list
  ActivationFunction_test
  IntegrationFunction_test
  Link_test
  Lossfunction_test
  ModelBuilder_test
  ModelLogger_test
  ModelReplicator_test
  ModelTrainer_test
  Model_test
  Model_DAG_test
  Model_DCG_test
  Node_test
  PopulationTrainer_test
  SharedFunctions_test
  Solver_test
  Weight_test
  WeightInit_test
)

set(simulator_executables_list
  ChromatogramSimulator_test
  DataSimulator_test
  EMGModel_test
  PeakSimulator_test
)

### collect test executables
set(TEST_executables
    ${core_executables_list}
    ${io_executables_list}
    ${ml_executables_list}
	${graph_executables_list}
    ${simulator_executables_list}
)

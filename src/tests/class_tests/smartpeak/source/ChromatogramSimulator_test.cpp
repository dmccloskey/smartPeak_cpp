/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE ChromatogramSimulator test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/simulator/ChromatogramSimulator.h>
#include <SmartPeak/simulator/PeakSimulator.h>
#include <SmartPeak/simulator/EMGModel.h>

#include <iostream>

using namespace SmartPeak;
using namespace std;

template<typename TensorT>
class ChromatogramSimulatorExt : public ChromatogramSimulator<TensorT>
{
public:
	void simulateTrainingData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps) override {};
	void simulateValidationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 4>& output_data, Eigen::Tensor<TensorT, 3>& time_steps) override {};
	void simulateEvaluationData(Eigen::Tensor<TensorT, 4>& input_data, Eigen::Tensor<TensorT, 3>& time_steps) override {};
};

BOOST_AUTO_TEST_SUITE(chromatogramsimulator)

BOOST_AUTO_TEST_CASE(constructor) 
{
  ChromatogramSimulatorExt<double>* ptr = nullptr;
  ChromatogramSimulatorExt<double>* nullPointer = nullptr;
	ptr = new ChromatogramSimulatorExt<double>();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  ChromatogramSimulatorExt<double>* ptr = nullptr;
	ptr = new ChromatogramSimulatorExt<double>();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(findPeakOverlap)
{
  ChromatogramSimulatorExt<double> chromsimulator;
  PeakSimulator<double> peak_left, peak_right;
  EMGModel<double> emg_left, emg_right;  

 // Overlapping windows; left and right baseline are equal;
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    0.0, 12.0, 
    0.0, 0.0,
    1.0, 5.0, //bl, br
    15);
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    8.0, 20.0, 
    0.0, 0.0,
    5.0, 1.0, //bl, br
    15);
  emg_left = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel<double>(10.0, 0.0, 15.0, 1.0);
  chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right), 12.0);

 // Merged peaks: both baselines overlap
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    0.0, 15.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    5.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel<double>(10.0, 0.0, 9.0, 1.0);
  emg_right = EMGModel<double>(10.0, 0.0, 11.0, 1.0);
  chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right), 10.0);

 // Merged peaks: both baselines do not overlap
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    0.0, 13.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    5.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel<double>(10.0, 0.0, 9.0, 1.0);
  emg_right = EMGModel<double>(10.0, 0.0, 11.0, 1.0);
  chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right), 10.0);
}

BOOST_AUTO_TEST_CASE(joinPeakWindows1) 
{
  ChromatogramSimulatorExt<double> chromsimulator;
  PeakSimulator<double> peak_left, peak_right;
  EMGModel<double> emg_left, emg_right;

  // Perfect overlap; no differences in baseline
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel<double>(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 1.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 1.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 10.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 10.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

  // Perfect overlap; no differences in baseline
  // swapped peaks
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_right = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  emg_left = EMGModel<double>(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 1.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 1.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowStart(), 0.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 10.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 10.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowEnd(), 20.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

 // Non overlapping windows; Left baseline is higher
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    0.0, 8.0, 
    0.0, 0.0,
    1.0, 5.0, //bl, br
    15);
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    12.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel<double>(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 5.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 12.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 12.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

 // Non overlapping windows; Right baseline is higher
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    0.0, 8.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    12.0, 20.0, 
    0.0, 0.0,
    5.0, 1.0, //bl, br
    15);
  emg_left = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel<double>(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 5.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 12.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 12.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

 // Overlapping windows; Left baseline is higher
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    0.0, 12.0, 
    0.0, 0.0,
    1.0, 5.0, //bl, br
    15);
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    8.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel<double>(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 5.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 12.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 12.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

 // Overlapping windows; Right baseline is higher
  peak_left = PeakSimulator<double>(1.0, 0.0, 
    0.0, 12.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator<double>(1.0, 0.0, 
    8.0, 20.0, 
    0.0, 0.0,
    5.0, 1.0, //bl, br
    15);
  emg_left = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel<double>(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 5.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 12.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 12.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);
}

BOOST_AUTO_TEST_CASE(makeChromatogram) 
{
  ChromatogramSimulatorExt<double> chromsimulator;
  PeakSimulator<double> peak1, peak2, peak3;
  EMGModel<double> emg1, emg2, emg3;
  std::vector<double> chrom_time, chrom_intensity, x_test, y_test;
	std::vector<std::pair<double, double>> best_lr, best_lr_test;
  std::vector<double> peak_apices, peak_apices_test;

  std::vector<PeakSimulator<double>> peaks;
  std::vector<EMGModel<double>> emgs;

  // Perfect gaussian peak
  peak1 = PeakSimulator<double>(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg1 = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  peaks = {peak1};
  emgs = {emg1};

  chromsimulator.makeChromatogram(chrom_time, chrom_intensity, best_lr, peak_apices,
    peaks, emgs);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  y_test = {1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1};
  for (int i=0; i<chrom_time.size(); ++i)
  {
    BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
    BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
  }
	best_lr_test = { std::make_pair(2,8) };
	for (int i = 0; i < best_lr_test.size(); ++i)
	{
		BOOST_CHECK_CLOSE(best_lr[i].first, best_lr_test[i].first, 1e-3);
		BOOST_CHECK_CLOSE(best_lr[i].second, best_lr_test[i].second, 1e-3);
	}
  peak_apices_test = { 5 };
  for (int i = 0; i < peak_apices_test.size(); ++i)
  {
    BOOST_CHECK_CLOSE(peak_apices[i], peak_apices_test[i], 1e-3);
  }

  // Perfect gaussian peaks
  peak1 = PeakSimulator<double>(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg1 = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  peak2 = PeakSimulator<double>(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg2 = EMGModel<double>(10.0, 0.0, 15.0, 1.0);
  peak3 = PeakSimulator<double>(1.0, 0.0, 
    20.0, 30.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg3 = EMGModel<double>(10.0, 0.0, 25.0, 1.0);
  peaks = {peak1, peak2, peak3};
  emgs = {emg1, emg2, emg3};

  chromsimulator.makeChromatogram(chrom_time, chrom_intensity, best_lr, peak_apices,
    peaks, emgs);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
  y_test = {1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1};
  for (int i=0; i<chrom_time.size(); ++i)
  {
    BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
    BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
  }
	best_lr_test = { std::make_pair(2,8),std::make_pair(12,18),std::make_pair(22,28) };
	for (int i = 0; i < best_lr_test.size(); ++i)
	{
		BOOST_CHECK_CLOSE(best_lr[i].first, best_lr_test[i].first, 1e-3);
		BOOST_CHECK_CLOSE(best_lr[i].second, best_lr_test[i].second, 1e-3);
	}
  peak_apices_test = { 5, 15, 25 };
  for (int i = 0; i < peak_apices_test.size(); ++i)
  {
    BOOST_CHECK_CLOSE(peak_apices[i], peak_apices_test[i], 1e-3);
  }

  // Increase tailing
  peak1 = PeakSimulator<double>(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg1 = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  peak2 = PeakSimulator<double>(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg2 = EMGModel<double>(10.0, 0.2, 15.0, 1.0);
  peak3 = PeakSimulator<double>(1.0, 0.0, 
    20.0, 30.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg3 = EMGModel<double>(10.0, 1.0, 25.0, 1.0);
  peaks = {peak1, peak2, peak3};
  emgs = {emg1, emg2, emg3};

  chromsimulator.makeChromatogram(chrom_time, chrom_intensity, best_lr, peak_apices,
    peaks, emgs);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
  y_test = {1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1, 4.92435, 9.64041, 7.17685, 2.06109, 1, 1, 1, 1, 1, 1, 1, 2.55573, 6.5568, 7.60173, 4.70568, 2.01076, 1, 1};
  for (int i=0; i<chrom_time.size(); ++i)
  {
    BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
    BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
  }
	best_lr_test = { std::make_pair(2,8),std::make_pair(13,18),std::make_pair(23,29) };
	for (int i = 0; i < best_lr_test.size(); ++i)
	{
		BOOST_CHECK_CLOSE(best_lr[i].first, best_lr_test[i].first, 1e-3);
		BOOST_CHECK_CLOSE(best_lr[i].second, best_lr_test[i].second, 1e-3);
	}
  peak_apices_test = { 5, 15, 25 };
  for (int i = 0; i < peak_apices_test.size(); ++i)
  {
    BOOST_CHECK_CLOSE(peak_apices[i], peak_apices_test[i], 1e-3);
  }

  // Overlap and cutoff peak
  peak1 = PeakSimulator<double>(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg1 = EMGModel<double>(10.0, 0.0, 5.0, 1.0);
  peak2 = PeakSimulator<double>(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg2 = EMGModel<double>(10.0, 0.0, 7.0, 1.0);
  peak3 = PeakSimulator<double>(1.0, 0.0, 
    20.0, 30.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg3 = EMGModel<double>(10.0, 0.0, 29.0, 1.0);
  peaks = {peak1, peak2, peak3};
  emgs = {emg1, emg2, emg3};

  chromsimulator.makeChromatogram(chrom_time, chrom_intensity, best_lr, peak_apices,
    peaks, emgs);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
  y_test = {1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.35335, 6.06531, 10, 6.06531};
  for (int i=0; i<chrom_time.size(); ++i)
  {
    BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
    BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
  }
	best_lr_test = { std::make_pair(2,8),std::make_pair(26,30) };
	for (int i = 0; i < best_lr_test.size(); ++i)
	{
		BOOST_CHECK_CLOSE(best_lr[i].first, best_lr_test[i].first, 1e-3);
		BOOST_CHECK_CLOSE(best_lr[i].second, best_lr_test[i].second, 1e-3);
	}
  peak_apices_test = { 5, 7, 29 };
  for (int i = 0; i < peak_apices_test.size(); ++i)
  {
    BOOST_CHECK_CLOSE(peak_apices[i], peak_apices_test[i], 1e-3);
  }

  // // UNCOMMENT to print out new test values
  // for (int i=0; i<chrom_time.size(); ++i)
  // {
  //   std::cout<< chrom_time[i] << "  " << chrom_intensity[i] <<std::endl;
  // }
  // std::cout<< ";" <<std::endl;
  // for (int i=0; i<chrom_time.size(); ++i)
  // {
  //   std::cout<< chrom_time[i] << ", ";
  // }
  // std::cout<< ";" <<std::endl;
  // for (int i=0; i<chrom_intensity.size(); ++i)
  // {
  //   std::cout<< chrom_intensity[i] << ", ";
  // }
  // std::cout<< ";" <<std::endl;

}

BOOST_AUTO_TEST_CASE(simulateChromatogram)
{
	ChromatogramSimulatorExt<double> chromsimulator;
	std::vector<double> chrom_time, chrom_intensity, chrom_time_noise, chrom_intensity_noise, x_test, y_test;
	std::vector<std::pair<double, double>> best_lr, best_lr_test;
  std::vector<double> peak_apices, peak_apices_test;
  std::vector<EMGModel<double>> emgs;

	chromsimulator.simulateChromatogram(chrom_time, chrom_intensity, chrom_time_noise, chrom_intensity_noise, best_lr, peak_apices, emgs,
		std::make_pair(1.0, 1.0), std::make_pair(0.1, 0.2), std::make_pair(10.0, 10.0), 
		std::make_pair(0.1, 0.2), std::make_pair(0.1, 0.2), std::make_pair(1.0, 1.0),
		std::make_pair(1.0, 1.0), std::make_pair(10.0, 10.0), std::make_pair(0.0, 0.0), std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0)
		);
	x_test = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	y_test = { 1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1 };
	for (int i = 0; i < chrom_time.size(); ++i)
	{
		BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
		BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
	}
	best_lr_test = { std::make_pair(2,8) };
	for (int i = 0; i < best_lr_test.size(); ++i)
	{
		BOOST_CHECK_CLOSE(best_lr[i].first, best_lr_test[i].first, 1e-3);
		BOOST_CHECK_CLOSE(best_lr[i].second, best_lr_test[i].second, 1e-3);
	}
  peak_apices_test = { 5 };
  for (int i = 0; i < peak_apices_test.size(); ++i)
  {
    BOOST_CHECK_CLOSE(peak_apices[i], peak_apices_test[i], 1e-3);
  }
  BOOST_CHECK_EQUAL(emgs.size(), 1);

	// Perfect gaussian peaks
	chromsimulator.simulateChromatogram(chrom_time, chrom_intensity, chrom_time_noise, chrom_intensity_noise, best_lr, peak_apices, emgs,
		std::make_pair(1.0, 1.0), std::make_pair(0.1, 0.2), std::make_pair(30.0, 30.0),
		std::make_pair(0.1, 0.2), std::make_pair(0.1, 0.2), std::make_pair(1.0, 1.0),
		std::make_pair(3.0, 3.0), std::make_pair(10.0, 10.0), std::make_pair(0.0, 0.0), std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0)
	);
	x_test = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
	y_test = { 1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1 };
	for (int i = 0; i < chrom_time.size(); ++i)
	{
		BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
		BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
	}
	best_lr_test = { std::make_pair(2,8),std::make_pair(12,18),std::make_pair(22,28) };
	for (int i = 0; i < best_lr_test.size(); ++i)
	{
		BOOST_CHECK_CLOSE(best_lr[i].first, best_lr_test[i].first, 1e-3);
		BOOST_CHECK_CLOSE(best_lr[i].second, best_lr_test[i].second, 1e-3);
	}
  peak_apices_test = { 5, 15, 25 };
  for (int i = 0; i < peak_apices_test.size(); ++i)
  {
    BOOST_CHECK_CLOSE(peak_apices[i], peak_apices_test[i], 1e-3);
  }
  BOOST_CHECK_EQUAL(emgs.size(), 3);

	// Random tailing
	chromsimulator.simulateChromatogram(chrom_time, chrom_intensity, chrom_time_noise, chrom_intensity_noise, best_lr, peak_apices, emgs,
		std::make_pair(1.0, 1.0), std::make_pair(0.1, 0.2), std::make_pair(30.0, 30.0),
		std::make_pair(0.1, 0.2), std::make_pair(0.1, 0.2), std::make_pair(1.0, 1.0),
		std::make_pair(3.0, 3.0), std::make_pair(10.0, 10.0), std::make_pair(0.0, 1.0), std::make_pair(0.0, 0.0), std::make_pair(1.0, 1.0)
	);
	BOOST_CHECK_EQUAL(chrom_time.size(), 33);
	BOOST_CHECK_EQUAL(chrom_intensity.size(), 33);
  BOOST_CHECK_EQUAL(best_lr.size(), 3);
  BOOST_CHECK_EQUAL(emgs.size(), 3);

	// Random peak offset
	chromsimulator.simulateChromatogram(chrom_time, chrom_intensity, chrom_time_noise, chrom_intensity_noise, best_lr, peak_apices, emgs,
		std::make_pair(1.0, 1.0), std::make_pair(0.1, 0.2), std::make_pair(30.0, 30.0),
		std::make_pair(0.1, 0.2), std::make_pair(0.1, 0.2), std::make_pair(1.0, 1.0),
		std::make_pair(3.0, 3.0), std::make_pair(10.0, 10.0), std::make_pair(0.0, 0.0), std::make_pair(0.0, 5.0), std::make_pair(1.0, 1.0)
	);
	BOOST_CHECK_EQUAL(chrom_time.size(), 33);
	BOOST_CHECK_EQUAL(chrom_intensity.size(), 33);
	BOOST_CHECK_EQUAL(best_lr.size(), 3);
  BOOST_CHECK_EQUAL(emgs.size(), 3);
}

BOOST_AUTO_TEST_SUITE_END()
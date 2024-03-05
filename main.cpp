
#define MZML_HEADER_ONLY
#include "mzml.hpp"
#include <omp.h>

// Workaround if M_2_SQRTPI is not recognized
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390
#endif

#include "external/qAlgorithms/include/qalgorithms_utils.h"
#include "external/qAlgorithms/include/qalgorithms_matrix.h"
#include "external/qAlgorithms/include/qalgorithms.h"

#include "external/qAlgorithms/src/qalgorithms_matrix.cpp"
#include "external/qAlgorithms/src/qalgorithms_utils.cpp"
#include "external/qAlgorithms/src/qalgorithms.cpp"

int main() {

  std::string file1 = "C:/Users/apoli/Documents/example_ms_files/tof_25000_mix1-r001.mzML";
  std::string file2 = "./excluded/example.mzML";
  std::string file3 = "./excluded/example.animl";
  std::string file4 = "./excluded/basic.xml";
  std::string file5 = "./excluded/mrm_pos.mzML";
  std::string file6 = "./excluded/mrm_neg.mzML";
  std::string file7 = "./excluded/example_strict.animl";
  std::string file8 = "./excluded/example.mzXML";
  std::string file9 = "./excluded/example_profile.mzML";
  std::string file10 = "./excluded/example_modified.mzML";

  // sc::hello();
  // std::cout << std::endl;

  // omp_set_num_threads(4);

  // int *arr = new int[ARRAY_SIZE];
  // std::fill_n(arr, ARRAY_SIZE, ARRAY_VALUE);

  // #pragma omp parallel for
  // for(int i = 0; i < ARRAY_SIZE; i++)
  // {
  //     arr[i] = arr[i] / arr[i] + arr[i] / 5 - 14;
  // }

  mzml::MZML z(file9);
  
  z.print();

  // mzml::SPECTRA_HEADERS hd;
  // hd = z.get_spectra_headers();

  // int number = hd.spec_index.size();
  // std::cout << "Size of vector in headers struct: " << number << std::endl;
  
  std::vector<std::vector<std::vector<double>>> spectra;

  mzml::SPECTRA_HEADERS headers;
  
  spectra = z.get_spectra();
  
  headers = z.get_spectra_headers();

  std::cout << "Number of extracted spectra: " << spectra.size() << std::endl;

  int number_traces = 0;

  for (size_t i = 0; i < spectra.size(); i++) {
    number_traces = number_traces + spectra[i][0].size();
  }

  std::cout << "Number of traces: " << number_traces << std::endl;

  q::Matrix xyData(number_traces, 3);
  
  int offset = 0;
  
  for (size_t i = 0; i < spectra.size(); i++) {
    const int& scan = headers.spec_index[i];
    const std::vector<std::vector<double>>& sp = spectra[i];

    for (size_t j = 0; j < sp[0].size(); j++) {
      xyData(offset, 0) = sp[0][j];
      xyData(offset, 1) = sp[1][j];
      xyData(offset, 2) = scan;
      offset = offset + 1;
    }
  }

  std::cout << "1st mz: " << xyData(0, 0) << std::endl;
  std::cout << "1st intensity: " << xyData(0, 1) << std::endl;
  std::cout << "1st scan: " << xyData(0, 2) << std::endl;

  q::tValues tVal;

  int maxScale = 5;

  int mode = 2;
  
  q::Peakmodel model(maxScale);
  
  switch (mode) {
    case 0:
      model.setMode(q::Mode::DEBUGGING);
      break;
      
    case 2:
      model.setMode(q::Mode::PROGRESS);
      break;
      
    default:
      break;
  }
  
  model.loadTValues(tVal);
  
  model.addMultipleMeasurements(xyData);

  model.findPeaks();

  std::vector<double> cent_scan = model.getPeakProperties(q::Peakproperties::SMPLID);
  std::vector<double> cent_mz = model.getPeakProperties(q::Peakproperties::POSITION);
  std::vector<double> cent_intensity = model.getPeakProperties(q::Peakproperties::HEIGHT);

  std::cout << "Number of traces after: " << cent_mz.size() << std::endl;
  std::cout << "1st mz after: " << cent_mz[0] << std::endl;
  std::cout << "1st intensity after: " << cent_intensity[0] << std::endl;

  std::cout << "Var Check " << std::endl;
  for (size_t i = 0; i < cent_scan.size(); i++) {
    std::cout << cent_scan[i] << " ";
  }
  std::cout << std::endl;



  // list_out["mz"] = model.getPeakProperties(q::Peakproperties::POSITION);
  // list_out["intensity"] = model.getPeakProperties(q::Peakproperties::HEIGHT);
  // list_out["area"] = model.getPeakProperties(q::Peakproperties::AREA);
  // list_out["DQS"] = model.getPeakProperties(q::Peakproperties::DQS);

  // std::vector<std::vector<double>> spectra2D(3);
  // spectra2D[0].resize(number_traces);
  // spectra2D[1].resize(number_traces);
  // spectra2D[2].resize(number_traces);

  // for (size_t i = 0; i < spectra.size(); i++) {
  //   int scan = headers.spec_scan[i];
  //   const std::vector<std::vector<double>>& sp = spectra[i];
  //   const std::vector<double>& mz = sp[0];
  //   const std::vector<double>& intensity = sp[1];

  //   for (size_t j = 0; j < mz.size(); j++) {
  //     spectra2D[0].push_back(scan);
  //     spectra2D[1].push_back(mz[j]);
  //     spectra2D[2].push_back(intensity[j]);
  //   }
  // }

  // std::cout << "Number of traces: " << spectra2D[0].size() << std::endl;


  // std::vector<std::vector<double>> matrix;

  // q::Matrix xyData(matrix.size(), 3);

  // tests::test_writing_mzml(file9);

  // tests::test_integration_mstoolkit(file2);

  // tests::test_reading_generic_xml(file4);

  // Pre conversion of a float vector to double for returning the same as input
  // std::vector<float> inputFloat = {10, 13.4, 15.6, 1000.5};
  // std::vector<double> inputDouble(inputFloat.size());
  // for (size_t i = 0; i < inputFloat.size(); i++) {
  //   inputDouble[i] = static_cast<double>(inputFloat[i]);
  // }

  // tests::test_encoding_decoding_little_endian(inputDouble, 4);


  // std::vector<double> input = {10, 13.4, 15.6, 1000.5};
  // tests::test_encoding_decoding_little_endian(input, 8);

  // tests::test_extract_spectra_mzml(file2);

  // tests::test_extract_chromatograms_mzml(file5);

  // tests::test_reading_animl(file7);




  return 0;
}
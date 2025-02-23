#ifndef STREAMCRAFT_UTILS_HPP
#define STREAMCRAFT_UTILS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <zlib.h>

namespace sc
{

  inline namespace utils
  {

    enum MS_SPECTRA_MODE
    {
      UNDEFINED,
      PROFILE,
      CENTROID
    };

    enum MS_SPECTRA_POLARITY
    {
      NEUTRAL,
      POSITIVE,
      NEGATIVE
    };

    struct MS_SPECTRUM
    {
      int index;
      int scan;
      int array_length;
      int level;
      int mode;
      int polarity;
      float lowmz;
      float highmz;
      float bpmz;
      float bpint;
      float tic;
      int configuration;
      float rt;
      float mobility;
      float window_mz;
      float window_mzlow;
      float window_mzhigh;
      float precursor_mz;
      float precursor_intensity;
      int precursor_charge;
      float activation_ce;
      int binary_arrays_count;
      std::vector<std::string> binary_names;
      std::vector<std::vector<float>> binary_data;
    };

    struct MS_SPECTRA_HEADERS
    {
      std::vector<int> index;
      std::vector<int> scan;
      std::vector<int> array_length;
      std::vector<int> level;
      std::vector<int> mode;
      std::vector<int> polarity;
      std::vector<float> lowmz;
      std::vector<float> highmz;
      std::vector<float> bpmz;
      std::vector<float> bpint;
      std::vector<float> tic;
      std::vector<int> configuration;
      std::vector<float> rt;
      std::vector<float> mobility;
      std::vector<float> window_mz;
      std::vector<float> window_mzlow;
      std::vector<float> window_mzhigh;
      std::vector<float> precursor_mz;
      std::vector<float> precursor_intensity;
      std::vector<int> precursor_charge;
      std::vector<float> activation_ce;

      void resize_all(int n)
      {
        index.resize(n);
        scan.resize(n);
        array_length.resize(n);
        level.resize(n);
        mode.resize(n);
        polarity.resize(n);
        lowmz.resize(n);
        highmz.resize(n);
        bpmz.resize(n);
        bpint.resize(n);
        tic.resize(n);
        configuration.resize(n);
        rt.resize(n);
        mobility.resize(n);
        window_mz.resize(n);
        window_mzlow.resize(n);
        window_mzhigh.resize(n);
        precursor_mz.resize(n);
        precursor_intensity.resize(n);
        precursor_charge.resize(n);
        activation_ce.resize(n);
      }

      size_t size() const
      {
        return index.size();
      }
    };

    struct MS_SUMMARY
    {
      std::string file_name;
      std::string file_path;
      std::string file_dir;
      std::string file_extension;
      int number_spectra;
      int number_chromatograms;
      int number_spectra_binary_arrays;
      std::string format;
      std::string time_stamp;
      std::vector<int> polarity;
      std::vector<int> mode;
      std::vector<int> level;
      std::vector<int> configuration;
      std::string type;
      float min_mz;
      float max_mz;
      float start_rt;
      float end_rt;
      bool has_ion_mobility;
    };

    struct MS_CHROMATOGRAMS_HEADERS
    {
      std::vector<int> index;
      std::vector<std::string> id;
      std::vector<int> array_length;
      std::vector<int> polarity;
      std::vector<float> precursor_mz;
      std::vector<float> activation_ce;
      std::vector<float> product_mz;

      void resize_all(int n)
      {
        index.resize(n);
        id.resize(n);
        array_length.resize(n);
        polarity.resize(n);
        precursor_mz.resize(n);
        activation_ce.resize(n);
        product_mz.resize(n);
      }

      size_t size() const
      {
        return index.size();
      }
    };

    struct MS_TARGETS
    {
      std::vector<int> index;
      std::vector<std::string> id;
      std::vector<int> level;
      std::vector<int> polarity;
      std::vector<bool> precursor;
      std::vector<float> mzmin;
      std::vector<float> mzmax;
      std::vector<float> rtmin;
      std::vector<float> rtmax;
      std::vector<float> mobilitymin;
      std::vector<float> mobilitymax;

      void resize_all(int n)
      {
        index.resize(n);
        id.resize(n);
        level.resize(n);
        polarity.resize(n);
        precursor.resize(n);
        mzmin.resize(n);
        mzmax.resize(n);
        rtmin.resize(n);
        rtmax.resize(n);
        mobilitymin.resize(n);
        mobilitymax.resize(n);
      }

      MS_TARGETS operator[](int i)
      {
        MS_TARGETS target;
        target.index.push_back(index[i]);
        target.id.push_back(id[i]);
        target.level.push_back(level[i]);
        target.polarity.push_back(polarity[i]);
        target.precursor.push_back(precursor[i]);
        target.mzmin.push_back(mzmin[i]);
        target.mzmax.push_back(mzmax[i]);
        target.rtmin.push_back(rtmin[i]);
        target.rtmax.push_back(rtmax[i]);
        target.mobilitymin.push_back(mobilitymin[i]);
        target.mobilitymax.push_back(mobilitymax[i]);
        return target;
      }

      size_t size() const
      {
        return index.size();
      }
    };

    struct MS_TARGETS_SPECTRA
    {
      std::vector<std::string> id;
      std::vector<int> polarity;
      std::vector<int> level;
      std::vector<float> pre_mz;
      std::vector<float> pre_mzlow;
      std::vector<float> pre_mzhigh;
      std::vector<float> pre_ce;
      std::vector<float> rt;
      std::vector<float> mobility;
      std::vector<float> mz;
      std::vector<float> intensity;

      void resize_all(int n)
      {
        id.resize(n);
        polarity.resize(n);
        level.resize(n);
        pre_mz.resize(n);
        pre_mzlow.resize(n);
        pre_mzhigh.resize(n);
        pre_ce.resize(n);
        rt.resize(n);
        mobility.resize(n);
        mz.resize(n);
        intensity.resize(n);
      }

      size_t size() const
      {
        return id.size();
      }

      int number_ids() const
      {
        std::unordered_set<std::string> unique_ids(id.begin(), id.end());
        return unique_ids.size();
      }

      MS_TARGETS_SPECTRA operator[](const std::string &unique_id) const
      {
        MS_TARGETS_SPECTRA target;
        for (size_t i = 0; i < id.size(); ++i)
        {
          if (id[i] == unique_id)
          {
            target.id.push_back(id[i]);
            target.polarity.push_back(polarity[i]);
            target.level.push_back(level[i]);
            target.pre_mz.push_back(pre_mz[i]);
            target.pre_mzlow.push_back(pre_mzlow[i]);
            target.pre_mzhigh.push_back(pre_mzhigh[i]);
            target.pre_ce.push_back(pre_ce[i]);
            target.rt.push_back(rt[i]);
            target.mobility.push_back(mobility[i]);
            target.mz.push_back(mz[i]);
            target.intensity.push_back(intensity[i]);
          }
        }
        return target;
      }
    };

    class MS_READER
    {
    public:
      MS_READER(const std::string &file) : file_(file) {}
      virtual ~MS_READER() = default;

      virtual int get_number_spectra() = 0;
      virtual int get_number_chromatograms() = 0;
      virtual int get_number_spectra_binary_arrays() = 0;
      virtual std::string get_format() = 0;
      virtual std::string get_type() = 0;
      virtual std::string get_time_stamp() = 0;
      virtual std::vector<int> get_polarity() = 0;
      virtual std::vector<int> get_mode() = 0;
      virtual std::vector<int> get_level() = 0;
      virtual std::vector<int> get_configuration() = 0;
      virtual float get_min_mz() = 0;
      virtual float get_max_mz() = 0;
      virtual float get_start_rt() = 0;
      virtual float get_end_rt() = 0;
      virtual bool has_ion_mobility() = 0;
      virtual MS_SUMMARY get_summary() = 0;
      virtual std::vector<int> get_spectra_index(std::vector<int> indices = {}) = 0;
      virtual std::vector<int> get_spectra_scan_number(std::vector<int> indices = {}) = 0;
      virtual std::vector<int> get_spectra_array_length(std::vector<int> indices = {}) = 0;
      virtual std::vector<int> get_spectra_level(std::vector<int> indices = {}) = 0;
      virtual std::vector<int> get_spectra_configuration(std::vector<int> indices = {}) = 0;
      virtual std::vector<int> get_spectra_mode(std::vector<int> indices = {}) = 0;
      virtual std::vector<int> get_spectra_polarity(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_lowmz(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_highmz(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_bpmz(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_bpint(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_tic(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_rt(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_mobility(std::vector<int> indices = {}) = 0;
      virtual std::vector<int> get_spectra_precursor_scan(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_precursor_mz(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_precursor_window_mz(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_precursor_window_mzlow(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_precursor_window_mzhigh(std::vector<int> indices = {}) = 0;
      virtual std::vector<float> get_spectra_collision_energy(std::vector<int> indices = {}) = 0;
      virtual MS_SPECTRA_HEADERS get_spectra_headers(std::vector<int> indices = {}) = 0;
      virtual MS_CHROMATOGRAMS_HEADERS get_chromatograms_headers(std::vector<int> indices = {}) = 0;
      virtual std::vector<std::vector<std::vector<float>>> get_spectra(std::vector<int> indices = {}) = 0;
      virtual std::vector<std::vector<std::vector<float>>> get_chromatograms(std::vector<int> indices = {}) = 0;
      virtual std::vector<std::vector<std::string>> get_software() = 0;
      virtual std::vector<std::vector<std::string>> get_hardware() = 0;
      virtual MS_SPECTRUM get_spectrum(const int &idx) = 0;

    protected:
      std::string file_;
    };

    std::string encode_little_endian_from_float(const std::vector<float> &input, const int &precision);

    std::string encode_little_endian_from_double(const std::vector<double> &input, const int &precision);

    std::vector<float> decode_little_endian_to_float(const std::string &str, const int &precision);

    std::vector<double> decode_little_endian_to_double(const std::string &str, const int &precision);

    std::string encode_big_endian_from_float(const std::vector<float> &input, const int &precision);

    std::string encode_big_endian_from_double(const std::vector<double> &input, const int &precision);

    std::vector<float> decode_big_endian_to_float(const std::string &str, const int &precision);

    std::vector<double> decode_big_endian_to_double(const std::string &str, const int &precision);

    std::string compress_zlib(const std::string &str);

    std::string decompress_zlib(const std::string &compressed_string);

    std::string encode_base64(const std::string &str);

    std::string decode_base64(const std::string &encoded_string);
  }; // namespace utils
};

#endif // STREAMCRAFT_UTILS_HPP

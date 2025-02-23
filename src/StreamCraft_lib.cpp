#include "StreamCraft_lib.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <set>
#include <memory>
#include "pugixml.hpp"
#include "StreamCraft_utils.hpp"
#include "StreamCraft_mzml.hpp"
#include "StreamCraft_mzxml.hpp"

std::unique_ptr<sc::MS_READER> sc::create_ms_reader(const std::string &file)
{

  std::string file_dir = file.substr(0, file.find_last_of("/\\") + 1);

  if (file_dir.back() == '/')
    file_dir.pop_back();

  std::string file_name = file.substr(file.find_last_of("/\\") + 1);

  std::string file_extension = file_name.substr(file_name.find_last_of(".") + 1);

  file_name = file_name.substr(0, file_name.find_last_of("."));

  const std::vector<std::string> possible_formats = {"mzML", "mzXML", "animl"};

  int format_case = std::distance(possible_formats.begin(), std::find(possible_formats.begin(), possible_formats.end(), file_extension));

  switch (format_case)
  {

  case 0:
  {
    return std::make_unique<MZML>(file);
  }

  case 1:
  {
    return std::make_unique<MZXML>(file);
    break;
  }

  default:
    throw std::invalid_argument("Unsupported file format");
  }
};

sc::MS_ANALYSIS::MS_ANALYSIS(const std::string &file)
{

  file_path = file;

  file_dir = file.substr(0, file.find_last_of("/\\") + 1);

  if (file_dir.back() == '/')
    file_dir.pop_back();

  file_name = file.substr(file.find_last_of("/\\") + 1);

  file_extension = file_name.substr(file_name.find_last_of(".") + 1);

  file_name = file_name.substr(0, file_name.find_last_of("."));

  format_case = std::distance(possible_formats.begin(), std::find(possible_formats.begin(), possible_formats.end(), file_extension));

  switch (format_case)
  {

  case 0:
  {
    ms = std::make_unique<MZML>(file);
    break;
  }

  case 1:
  {
    ms = std::make_unique<MZXML>(file);
    break;
  }

  default:
    break;
  }
};

sc::MS_TARGETS_SPECTRA sc::MS_ANALYSIS::get_spectra_targets(const sc::MS_TARGETS &targets, const sc::MS_SPECTRA_HEADERS &headers, const float &minIntLv1 = 0, const float &minIntLv2 = 0)
{

  const int number_spectra = get_number_spectra();

  const int number_targets = targets.index.size();

  MS_TARGETS_SPECTRA res;

  if (number_targets == 0)
    return res;

  if (number_spectra == 0)
    return res;

  const int number_spectra_binary_arrays = get_number_spectra_binary_arrays();

  if (number_spectra_binary_arrays == 0)
    return res;

  if (headers.size() == 0)
    return res;

  const int headers_size = headers.size();

  if (headers_size != number_spectra)
    return res;

  std::set<int> idx;

  for (int i = 0; i < number_targets; i++)
  {

#pragma omp parallel for shared(idx)
    for (int j = 0; j < number_spectra; j++)
    {

      // excludes higher configuration function scans
      if (headers.configuration[j] >= 3)
        continue;

      if (headers.level[j] == targets.level[i] || targets.level[i] == 0)
      {
        if ((headers.rt[j] >= targets.rtmin[i] && headers.rt[j] <= targets.rtmax[i]) || targets.rtmax[i] == 0)
        {
          if (headers.polarity[j] == targets.polarity[i])
          {
            if ((headers.mobility[j] >= targets.mobilitymin[i] && headers.mobility[j] <= targets.mobilitymax[i]) || targets.mobilitymax[i] == 0)
            {
              if (targets.precursor[i])
              {
                if ((headers.precursor_mz[j] >= targets.mzmin[i] && headers.precursor_mz[j] <= targets.mzmax[i]) || targets.mzmax[i] == 0)
                {
#pragma omp critical
                  {
                    idx.insert(j);
                  }
                }
              }
              else
              {
#pragma omp critical
                {
                  idx.insert(j);
                }
              }
            }
          }
        }
      }
    }
  }

  std::vector<int> idx_vector(idx.begin(), idx.end());

  std::sort(idx_vector.begin(), idx_vector.end());

  const int number_spectra_targets = idx_vector.size();

  if (number_spectra_targets == 0)
    return res;

  std::vector<std::string> id_out;
  std::vector<int> polarity_out;
  std::vector<int> level_out;
  std::vector<float> pre_mz_out;
  std::vector<float> pre_mzlow_out;
  std::vector<float> pre_mzhigh_out;
  std::vector<float> pre_ce_out;
  std::vector<float> rt_out;
  std::vector<float> mobility_out;
  std::vector<float> mz_out;
  std::vector<float> intensity_out;

#pragma omp parallel
  {
    std::vector<std::string> id_priv;
    std::vector<int> polarity_priv;
    std::vector<int> level_priv;
    std::vector<float> pre_mz_priv;
    std::vector<float> pre_mzlow_priv;
    std::vector<float> pre_mzhigh_priv;
    std::vector<float> pre_ce_priv;
    std::vector<float> rt_priv;
    std::vector<float> mobility_priv;
    std::vector<float> mz_priv;
    std::vector<float> intensity_priv;

#pragma omp for
    for (int i = 0; i < number_spectra_targets; i++)
    {

      const std::vector<int> i_idx = {idx_vector[i]};

      std::vector<std::vector<std::vector<float>>> spectra = get_spectra(i_idx);

      const int n_traces = spectra[0][1].size();

      if (n_traces == 0)
        continue;

      const int &i_polarity = headers.polarity[i_idx[0]];
      const int &i_level = headers.level[i_idx[0]];
      const float &i_pre_mz = headers.precursor_mz[i_idx[0]];
      const float &i_pre_mzlow = headers.window_mzlow[i_idx[0]];
      const float &i_pre_mzhigh = headers.window_mzhigh[i_idx[0]];
      const float &i_rt = headers.rt[i_idx[0]];
      const float &i_mobility = headers.mobility[i_idx[0]];

      for (int j = 0; j < number_targets; j++)
      {

        if (targets.polarity[j] == i_polarity)
        {

          if (targets.rtmax[j] == 0 || (i_rt >= targets.rtmin[j] && i_rt <= targets.rtmax[j]))
          {

            if (targets.mobilitymax[j] == 0 || (i_mobility >= targets.mobilitymin[j] && i_mobility <= targets.mobilitymax[j]))
            {

              if (targets.precursor[j])
              {

                if ((i_pre_mz >= targets.mzmin[j] && i_pre_mz <= targets.mzmax[j]) || targets.mzmax[j] == 0)
                {

                  for (int k = 0; k < n_traces; k++)
                  {

                    if (spectra[0][1][k] >= minIntLv2 && i_level == 2)
                    {
                      id_priv.push_back(targets.id[j]);
                      polarity_priv.push_back(i_polarity);
                      level_priv.push_back(i_level);
                      pre_mz_priv.push_back(i_pre_mz);
                      pre_mzlow_priv.push_back(i_pre_mzlow);
                      pre_mzhigh_priv.push_back(i_pre_mzhigh);
                      pre_ce_priv.push_back(headers.activation_ce[i_idx[0]]);
                      rt_priv.push_back(i_rt);
                      mobility_priv.push_back(i_mobility);
                      mz_priv.push_back(spectra[0][0][k]);
                      intensity_priv.push_back(spectra[0][1][k]);
                    }
                  }
                }
              }
              else
              {

                for (int k = 0; k < n_traces; k++)
                {

                  if ((spectra[0][0][k] >= targets.mzmin[j] && spectra[0][0][k] <= targets.mzmax[j]) || targets.mzmax[j] == 0)
                  {

                    if ((spectra[0][1][k] >= minIntLv2 && i_level == 2) || (spectra[0][1][k] >= minIntLv1 && i_level == 1))
                    {
                      id_priv.push_back(targets.id[j]);
                      polarity_priv.push_back(i_polarity);
                      level_priv.push_back(i_level);
                      pre_mz_priv.push_back(i_pre_mz);
                      pre_mzlow_priv.push_back(i_pre_mzlow);
                      pre_mzhigh_priv.push_back(i_pre_mzhigh);
                      pre_ce_priv.push_back(headers.activation_ce[i_idx[0]]);
                      rt_priv.push_back(i_rt);
                      mobility_priv.push_back(i_mobility);
                      mz_priv.push_back(spectra[0][0][k]);
                      intensity_priv.push_back(spectra[0][1][k]);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

#pragma omp critical
    {
      id_out.insert(id_out.end(), id_priv.begin(), id_priv.end());
      polarity_out.insert(polarity_out.end(), polarity_priv.begin(), polarity_priv.end());
      level_out.insert(level_out.end(), level_priv.begin(), level_priv.end());
      pre_mz_out.insert(pre_mz_out.end(), pre_mz_priv.begin(), pre_mz_priv.end());
      pre_mzlow_out.insert(pre_mzlow_out.end(), pre_mzlow_priv.begin(), pre_mzlow_priv.end());
      pre_mzhigh_out.insert(pre_mzhigh_out.end(), pre_mzhigh_priv.begin(), pre_mzhigh_priv.end());
      pre_ce_out.insert(pre_ce_out.end(), pre_ce_priv.begin(), pre_ce_priv.end());
      rt_out.insert(rt_out.end(), rt_priv.begin(), rt_priv.end());
      mobility_out.insert(mobility_out.end(), mobility_priv.begin(), mobility_priv.end());
      mz_out.insert(mz_out.end(), mz_priv.begin(), mz_priv.end());
      intensity_out.insert(intensity_out.end(), intensity_priv.begin(), intensity_priv.end());
    }
  }

  // make int vector of indexes for sorting the _out vector by mz_out, then rt_out, then mobility_out and then id_out

  int number_spectra_targets_out = id_out.size();

  std::vector<int> idx_sort(number_spectra_targets_out);

  std::iota(idx_sort.begin(), idx_sort.end(), 0);

  std::sort(idx_sort.begin(), idx_sort.end(), [&](int i, int j)
            {
    if (rt_out[i] != rt_out[j]) return rt_out[i] < rt_out[j];
    if (mobility_out[i] != mobility_out[j]) return mobility_out[i] < mobility_out[j];
    if (mz_out[i] != mz_out[j]) return mz_out[i] < mz_out[j];
    return id_out[i] < id_out[j]; });

  res.resize_all(id_out.size());

#pragma omp for
  for (int i = 0; i < number_spectra_targets_out; i++)
  {
    res.id[i] = id_out[idx_sort[i]];
    res.polarity[i] = polarity_out[idx_sort[i]];
    res.level[i] = level_out[idx_sort[i]];
    res.pre_mz[i] = pre_mz_out[idx_sort[i]];
    res.pre_mzlow[i] = pre_mzlow_out[idx_sort[i]];
    res.pre_mzhigh[i] = pre_mzhigh_out[idx_sort[i]];
    res.pre_ce[i] = pre_ce_out[idx_sort[i]];
    res.rt[i] = rt_out[idx_sort[i]];
    res.mobility[i] = mobility_out[idx_sort[i]];
    res.mz[i] = mz_out[idx_sort[i]];
    res.intensity[i] = intensity_out[idx_sort[i]];
  }

  return res;
};

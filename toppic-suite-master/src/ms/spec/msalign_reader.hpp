// Copyright (c) 2014 - 2019, The Trustees of Indiana University.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TOPPIC_MS_SPEC_MSALIGN_READER_HPP_
#define TOPPIC_MS_SPEC_MSALIGN_READER_HPP_

#include <limits>
#include <fstream>
#include <string>
#include <vector>

#include "ms/spec/deconv_ms.hpp"
#include "ms/spec/spectrum_set.hpp"

namespace toppic {

class MsAlignReader {
 public:
  MsAlignReader(const std::string &file_name);

  ~MsAlignReader();

  MsAlignReader(const std::string &file_name, int group_spec_num,
                ActivationPtr act_ptr, const std::set<std::string> skip_list,
                int peak_num_limit = std::numeric_limits<int>::max());

  std::vector<std::string> readOneStrSpectrum();

  DeconvMsPtr getNextMs();

  std::vector<SpectrumSetPtr> getNextSpectrumSet(SpParaPtr sp_para_ptr);

  void close();

  static void readMsOneSpectra(const std::string &file_name, DeconvMsPtrVec &ms_ptr_vec); 

 private:
  std::string file_name_;

  int group_spec_num_;

  std::ifstream input_;

  std::vector<std::string> spectrum_str_vec_;

  int current_ = 0;

  ActivationPtr activation_ptr_;

  DeconvMsPtr deconv_ms_ptr_ = nullptr;

  std::set<std::string> skip_list_;

  int peak_num_limit_ = std::numeric_limits<int>::max();

  void readNext();
};

typedef std::shared_ptr<MsAlignReader> MsAlignReaderPtr;
typedef std::vector<MsAlignReaderPtr>  MsAlignReaderPtrVec;

}  // namespace toppic
#endif

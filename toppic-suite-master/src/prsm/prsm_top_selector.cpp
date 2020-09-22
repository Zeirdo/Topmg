//Copyright (c) 2014 - 2019, The Trustees of Indiana University.
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#include <algorithm>

#include "common/util/file_util.hpp"
#include "prsm/prsm_reader.hpp"
#include "prsm/prsm_xml_writer.hpp"
#include "prsm/prsm_top_selector.hpp"

namespace toppic {

PrsmTopSelector::PrsmTopSelector(const std::string &db_file_name,
                                 const std::string &spec_file_name,
                                 const std::string &in_file_ext, 
                                 const std::string &out_file_ext, int n_top): 
    spec_file_name_(spec_file_name), 
    db_file_name_(db_file_name),
    input_file_ext_(in_file_ext),
    output_file_ext_(out_file_ext),
    n_top_(n_top) {}

bool containsSameFastaSeq(const PrsmStrPtrVec prsm_ptrs, PrsmStrPtr target_prsm_ptr) {
  for (size_t i = 0; i < prsm_ptrs.size(); i++) {
    if (prsm_ptrs[i]->getSeqName() == target_prsm_ptr->getSeqName()) {
      return true;
    }
  }
  return false;
}

PrsmStrPtrVec getTopPrsms(PrsmStrPtrVec &prsm_str_ptrs, int n_top) {
  std::sort(prsm_str_ptrs.begin(), prsm_str_ptrs.end(), PrsmStr::cmpEValueInc);
  int size = prsm_str_ptrs.size();
  int max = size > n_top? n_top:size;
  PrsmStrPtrVec result_ptrs;
  for (int i = 0; i < max; i++) {
    if (!containsSameFastaSeq(result_ptrs, prsm_str_ptrs[i])) {
      result_ptrs.push_back(prsm_str_ptrs[i]);
    }
  }
  return result_ptrs;
}

void PrsmTopSelector::process() {
  std::string base_name = file_util::basename(spec_file_name_);
  std::string input_file_name = base_name+"."+input_file_ext_;
  PrsmReader reader(input_file_name);
  PrsmStrPtr prsm_str_ptr = reader.readOnePrsmStr();

  PrsmXmlWriter writer(base_name +"."+output_file_ext_);

  int spec_id = 0;
  while (prsm_str_ptr != nullptr) {
    PrsmStrPtrVec cur_str_ptrs;
    while (prsm_str_ptr != nullptr && prsm_str_ptr->getSpectrumId() == spec_id) {
      cur_str_ptrs.push_back(prsm_str_ptr);
      prsm_str_ptr = reader.readOnePrsmStr();
    }
    PrsmStrPtrVec result_ptrs = getTopPrsms(cur_str_ptrs, n_top_);
    for (size_t i = 0; i < result_ptrs.size(); i++) {
      writer.write(result_ptrs[i]);
    }

    spec_id++;
  }

  reader.close();
  writer.close();
}

} /* namespace toppic */

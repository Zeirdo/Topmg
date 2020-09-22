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

#include <memory>
#include <vector>

#include "ms/spec/peak.hpp"
#include "ms/spec/ms.hpp"
#include "ms/spec/deconv_ms_util.hpp"

namespace toppic {

namespace deconv_ms_util {

DeconvMsPtrVec getRefineMsPtrVec(const DeconvMsPtrVec &deconv_ms_ptr_vec, 
                                 double new_prec_mass) {
  DeconvMsPtrVec result_ptrs;
  for (size_t m = 0; m < deconv_ms_ptr_vec.size(); m++) {
    DeconvMsPtr deconv_ms_ptr = deconv_ms_ptr_vec[m];
    MsHeaderPtr ori_header_ptr = deconv_ms_ptr->getMsHeaderPtr();
    MsHeaderPtr header_ptr = MsHeader::geneMsHeaderPtr(ori_header_ptr, new_prec_mass);
    std::vector<DeconvPeakPtr> peak_ptr_list;
    for (size_t p = 0; p < deconv_ms_ptr->size(); p++) {
      DeconvPeakPtr ori_peak_ptr = deconv_ms_ptr->getPeakPtr(p);
      // * is a dereference operator
      DeconvPeakPtr new_peak_ptr = std::make_shared<DeconvPeak>(*ori_peak_ptr.get());
      peak_ptr_list.push_back(new_peak_ptr);
    }
    DeconvMsPtr ms_ptr = std::make_shared<Ms<DeconvPeakPtr> >(header_ptr, peak_ptr_list);
    result_ptrs.push_back(ms_ptr);
  }
  return result_ptrs;
}

}  // namespace deconv_ms_util

}  // namespace toppic

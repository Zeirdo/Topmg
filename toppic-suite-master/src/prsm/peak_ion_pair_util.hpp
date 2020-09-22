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


#ifndef TOPPIC_PRSM_PEAK_ION_PAIR_UTIL_HPP_
#define TOPPIC_PRSM_PEAK_ION_PAIR_UTIL_HPP_

#include <vector>

#include "seq/proteoform.hpp"
#include "ms/spec/rm_break_type.hpp"
#include "ms/spec/extend_peak.hpp"
#include "ms/spec/theo_peak.hpp"
#include "ms/spec/extend_ms.hpp"
#include "prsm/peak_ion_pair.hpp"


namespace toppic {

namespace peak_ion_pair_util {

PeakIonPairPtrVec getMatchedPairs(const PeakIonPairPtrVec &pair_ptrs,
                                  int spec_id, int peak_id);

double computePairCoverage(const PeakIonPairPtrVec &pair_ptrs, int begin,
                           int end, RmBreakTypePtr type_ptr);

PeakIonPairPtrVec findPairs(ExtendMsPtr ms_three_ptr, TheoPeakPtrVec &theo_peak_ptrs,
                            int bgn, int end, double add_tolerance);

PeakIonPairPtrVec genePeakIonPairs(const ProteoformPtr &proteoform_ptr,
                                   const ExtendMsPtr &ms_three_ptr, double min_mass);

PeakIonPairPtrVec genePeakIonPairs(const ProteoformPtr &proteoform_ptr,
                                   const ExtendMsPtrVec &ms_ptr_vec, double min_mass);

double compMatchFragNum(const PeakIonPairPtrVec &pairs);

double compMatchPeakNum(PeakIonPairPtrVec &pairs);

}  // namespace peak_ion_pair_util

}  // namespace toppic
#endif

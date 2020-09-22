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


#ifndef TOPPIC_MS_SPEC_RAW_MS_UTIL_HPP_
#define TOPPIC_MS_SPEC_RAW_MS_UTIL_HPP_

#include "ms/spec/peak.hpp"

namespace toppic {

namespace raw_ms_util {

double findMaxPos(const PeakPtrVec &ptr_list);

int searchPos(const PeakPtrVec &ptr_list, double pos);

int getNearPeakIdx(const PeakPtrVec  &ptr_list, double pos, double tolerance);

PeakPtrVec rmPeaks(const PeakPtrVec &ptr_list, std::vector<bool> &keep);

PeakPtrVec getPeaksInWindow(const PeakPtrVec &ptr_list, double center, 
                            double win_size);

double getHighestPeakInte(const PeakPtrVec &ptr_list); 

double getMedianPeakInte(PeakPtrVec ptr_list);

}  // namespace raw_ms_util

}  // namespace toppic

#endif

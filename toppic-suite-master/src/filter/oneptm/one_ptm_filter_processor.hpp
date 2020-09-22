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

#ifndef TOPPIC_FILTER_ONE_PTM_ONE_PTM_FILTER_PROCESSOR_HPP_
#define TOPPIC_FILTER_ONE_PTM_ONE_PTM_FILTER_PROCESSOR_HPP_

#include "seq/db_block.hpp"
#include "filter/oneptm/one_ptm_filter_mng.hpp"

namespace toppic {

class OnePtmFilterProcessor {
 public:
  explicit OnePtmFilterProcessor(OnePtmFilterMngPtr mng_ptr): mng_ptr_(mng_ptr) {}
  void process();

 private:
  OnePtmFilterMngPtr mng_ptr_;

  void processBlock(DbBlockPtr block_ptr, const std::vector<double> & mod_mass_list);
};

typedef std::shared_ptr<OnePtmFilterProcessor> OnePtmFilterProcessorPtr;

}  // namespace toppic

#endif /* PTM_FAST_FILTER_PROCESSOR_HPP_ */

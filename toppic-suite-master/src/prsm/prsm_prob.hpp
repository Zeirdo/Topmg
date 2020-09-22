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

#ifndef TOPPIC_PRSM_PRSM_PROB_HPP_
#define TOPPIC_PRSM_PRSM_PROB_HPP_

#include "common/base/mod.hpp"

namespace toppic {

class PrsmProb {
 public:
  PrsmProb(const std::string &db_file_name,
           const std::string &spec_file_name,
           const ModPtrVec &fix_mod_ptr_vec,
           const std::string &in_file_ext,
           const std::string &out_file_ext,
           double K1, double K2,
           double pref, double inte);

  void process();

 private:
  std::string db_file_name_;
  std::string spec_file_name_;
  ModPtrVec fix_mod_ptr_vec_;
  std::string input_file_ext_;
  std::string output_file_ext_;
  double K1_;
  double K2_;
  double pref_;
  double inte_;
};

typedef std::shared_ptr<PrsmProb> PrsmProbPtr;
} /* namespace toppic */

#endif /* TOPPIC_PRSM_PROB_HPP_ */

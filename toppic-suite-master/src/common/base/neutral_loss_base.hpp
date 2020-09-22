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

#ifndef TOPPIC_COMMON_BASE_NEUTRAL_LOSS_BASE_HPP_
#define TOPPIC_COMMON_BASE_NEUTRAL_LOSS_BASE_HPP_

#include "common/base/neutral_loss.hpp"

namespace toppic {

class NeutralLossBase {
 public:
  static void initBase();

  static NeutralLossPtrVec getBaseNeutralLossPtrVec() {
    return neutral_loss_ptr_vec_;
  }

  static NeutralLossPtr getNeutralLossPtrByName(const std::string &name);

  static NeutralLossPtr getNeutralLossPtr_NONE() {
    return neutral_loss_ptr_NONE_;
  }

  static std::string getName_NONE() {return "NONE";}

 private:
  static NeutralLossPtrVec neutral_loss_ptr_vec_;

  static NeutralLossPtr neutral_loss_ptr_NONE_;
};

}  // namespace toppic

#endif /* NEUTRALLOSS_HPP_ */

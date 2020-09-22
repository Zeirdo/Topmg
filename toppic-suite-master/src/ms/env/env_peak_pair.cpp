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


#include "ms/env/env_peak_pair.hpp"

namespace toppic {

EnvPeakPair::EnvPeakPair(MatchEnvPtr env_ptr, int pos_idx) {
  env_ptr_ = env_ptr;
  pos_idx_ = pos_idx;
}

EnvPeakPair::EnvPeakPair(EnvPeakPairPtr pair_ptr) {
  env_ptr_ = pair_ptr->getMatchEnvPtr();
  pos_idx_ = pair_ptr->getPosIdx();
}

double EnvPeakPair::getTheoIntensity() {
  return env_ptr_->getTheoEnvPtr()->getIntensity(pos_idx_);

}

double EnvPeakPair::getPeakScore(double intensity_sum, double tolerance) {
  return env_ptr_->calcPeakScr(pos_idx_, intensity_sum, tolerance);
}

}

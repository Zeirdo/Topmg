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


#include "common/util/logger.hpp"
#include "ms/env/charge_cmp.hpp" 
#include "ms/env/env_filter.hpp" 

namespace toppic {


// count the number of valid matching envleops 
int cntValid(MatchEnvPtr2D &match_envs) {
  int cnt = 0;
  for (size_t i = 0; i < match_envs.size(); i++) {
    for (size_t j = 0; j < match_envs[i].size(); j++) {
      if (match_envs[i][j] != nullptr) {
        cnt++;
      }
    }
  }
  return cnt;
}

// Test match envelope has a valid real envelope 
bool EnvFilter::testRealEnvValid(MatchEnvPtr env, EnvParaPtr env_para_ptr) {
  RealEnvPtr real_env = env->getRealEnvPtr();
  int mass_group = env->getMassGroup();
  // 1. test if the number of matched peaks >= min_match_peak_num
  if (real_env->getMatchPeakNum() < env_para_ptr->min_match_peak_num_[mass_group]) {
    //LOG_DEBUG("FILETER 1" << " peak num " << real_env->getMatchPeakNum() << " thresh " << env_para_ptr->min_match_peak_num_[mass_group]);
    return false;
  }
  // 2. test if the number of missing peaks <= max_miss_peak_num
  if (real_env->getMissPeakNum() > env_para_ptr->max_miss_peak_num_) {
    //LOG_DEBUG("FILETER 2");
    return false;
  }
  // 3. test if consecutive peak number >= peak_num - 3
  if (env_para_ptr->check_consecutive_peak_num_) {
    // get threshold: peak_num - 3
    int min_cons_peak_num = env_para_ptr->compMinConsPeakNum(real_env->getPeakNum(), mass_group);
    if (real_env->getMaxConsPeakNum() < min_cons_peak_num) {
      //LOG_DEBUG("FILETER 3");
      return false;
    }
  }
  return true;
}

// Filtering by peak_num 
void filterEnvByRealEnv(MatchEnvPtr2D &match_envs, EnvParaPtr env_para_ptr) {
  for (size_t i = 0; i < match_envs.size(); i++) {
    for (size_t j = 0; j < match_envs[i].size(); j++) {
      if (match_envs[i][j] != nullptr) {
        //LOG_DEBUG("testing i " << i << " j " << j);
        if (!EnvFilter::testRealEnvValid(match_envs[i][j], env_para_ptr)) {
          //LOG_DEBUG("result filtered");
          match_envs[i][j] = nullptr;
        }
      }
    }
  }
}



// Filtering by score 
void filterEnvByScr(MatchEnvPtr2D &match_envs, EnvParaPtr env_para_ptr) {
  for (size_t i = 0; i < match_envs.size(); i++) {
    for (size_t j = 0; j < match_envs[i].size(); j++) {
      if (match_envs[i][j] != nullptr) {
        match_envs[i][j]->compScr(env_para_ptr);
        if (match_envs[i][j]->getScore() < env_para_ptr->min_match_env_score_) {
          match_envs[i][j] = nullptr;
        }
      }
    }
  }
}

// Filtering by charge, if there is another envelope with k * charge and a
// better score, the envelope is removed.
void filterEnvByChrg(MatchEnvPtr2D &match_envs) {
  for (size_t i = 0; i < match_envs.size(); i++) {
    for (size_t j = 0; j < match_envs[i].size(); j++) {
      int charge = j + 1;
      for (int k = 2 * charge - 1; k < (int)match_envs[i].size(); k += charge) {
        if (match_envs[i][k] != nullptr
            && match_envs[i][j] != nullptr
            && match_envs[i][k]->getScore() > match_envs[i][j]->getScore()) {
          match_envs[i][j] = nullptr;
        }
      }
    }
  }
}

// Filtering by comparing two envelopes with charge and charge + 1, the
// better one is kept.
void filterEnvByChrgComp(MatchEnvPtr2D &match_envs,
                         const PeakPtrVec &peak_list, EnvParaPtr env_para_ptr) {

  for (size_t i = 0; i < match_envs.size(); i++) {
    for (int j = env_para_ptr->charge_computation_bgn_ - 1; j < (int)match_envs[i].size() - 1; j++) {
      if (match_envs[i][j] != nullptr && match_envs[i][j + 1] != nullptr) {
        int result = charge_cmp::comp(peak_list, match_envs[i][j],
                                      match_envs[i][j + 1],
                                      env_para_ptr->charge_computation_mz_tolerance_);
        // rlst may be -1, 0, 1
        if (result == 1) {
          match_envs[i][j + 1] = nullptr;
        } else if (result == -1) {
          match_envs[i][j] = nullptr;
        }
      }
    }
  }
}

// compute the rank of a matchenv in an interval 
int compRank(int idx, int charge, MatchEnvPtr2D &match_envs,
             const PeakPtrVec &peak_list, EnvParaPtr env_para_ptr)  {
  int rank = 0;
  int peak_idx = match_envs[idx][charge - 1]->getRealEnvPtr()->getReferPeakIdx();
  //LOG_ERROR("peak_idx " << peak_idx << " match peak num " << real_env->getMatchPeakNum());
  if (peak_idx < 0) {
    return rank;
  }
  double score = match_envs[idx][charge - 1]->getScore();
  // check left 
  int p = peak_idx - 1;
  for (p = peak_idx - 1; p >= 0; p--) {
    double pos_dist = peak_list[peak_idx]->getPosition() - peak_list[p]->getPosition();  
    if (pos_dist * charge >= env_para_ptr->rank_peak_distance_) {
      break;
    }
    if (match_envs[p][charge - 1] != nullptr
        && match_envs[p][charge - 1]->getScore() > score) {
      rank++;
    }
  }
  // check right 
  for (p = peak_idx+1; p < (int)match_envs.size(); p++) {
    double pos_dist = peak_list[p]->getPosition() - peak_list[peak_idx]->getPosition();
    if (pos_dist * charge >= env_para_ptr->rank_peak_distance_) {
      break;
    }
    if (match_envs[p][charge - 1] != nullptr
        && match_envs[p][charge - 1]->getScore() > score) {
      rank++;
    }
  }
  return rank;
}

// Filtering by comparing the envelope with its neighboring envelopes with
// the same charge. Only the best one is kept.
void filterEnvByMz(MatchEnvPtr2D &match_envs,
                   const PeakPtrVec &peak_list, EnvParaPtr env_para_ptr) {

  for (size_t i = 0; i < match_envs.size(); i++) {
    for (size_t j = 0; j < match_envs[i].size(); j++) {
      if (match_envs[i][j] != nullptr) {
        int rank = compRank(i, j + 1, match_envs, peak_list, env_para_ptr);
        if (rank > env_para_ptr->max_similar_mz_env_rank_) {
          match_envs[i][j] = nullptr;
        }
      }
    }
  }
}

// Filtering methods 
void EnvFilter::filter(MatchEnvPtr2D &match_envs, const PeakPtrVec &peak_list, 
                       EnvParaPtr env_para_ptr) {

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
  LOG_INFO("Filtering by real envelope peaks...");
  filterEnvByRealEnv(match_envs, env_para_ptr);

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
  LOG_INFO("Filtering by score...");
  /* compute scores of matching envelopes here to peak_listeed up */
  filterEnvByScr(match_envs, env_para_ptr);

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
  LOG_INFO("Filtering by charge...");
  filterEnvByChrg(match_envs);

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
  LOG_INFO("Filtering by charge comparison...");
  filterEnvByChrgComp(match_envs, peak_list, env_para_ptr);
  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
  LOG_INFO("Filtering by mz...");
  filterEnvByMz(match_envs, peak_list, env_para_ptr);

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
}

// Filtering methods 
void EnvFilter::multipleMassFilter(MatchEnvPtr2D &match_envs, EnvParaPtr env_para_ptr) {

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
  LOG_INFO("Filtering by real envelope peaks...");
  filterEnvByRealEnv(match_envs, env_para_ptr);

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
  LOG_INFO("Filtering by score...");
  /* compute scores of matching envelopes here to peak_listeed up */
  filterEnvByScr(match_envs, env_para_ptr);

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
  LOG_INFO("Filtering by charge...");
  filterEnvByChrg(match_envs);

  LOG_DEBUG("Valid match envelope number " << cntValid(match_envs));
}

}

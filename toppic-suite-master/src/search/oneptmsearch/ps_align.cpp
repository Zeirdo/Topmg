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


#include <limits>
#include <algorithm>
#include <vector>

#include "seq/proteoform_factory.hpp"
#include "ms/spec/extend_ms_factory.hpp"
#include "search/oneptmsearch/ps_align.hpp"

namespace toppic {

void PSAlign::compute(ProteoformTypePtr align_type_ptr) {
  dp(align_type_ptr);
  backtrace();
}

void PSAlign::initDPPair() {
  // init 2d dp pairs
  dp_2d_pair_ptrs_.clear();
  segment_bgn_pair_ptrs_.clear();
  segment_end_pair_ptrs_.clear();
  for (size_t i = 0; i < diagonal_ptrs_.size(); i++) {
    DPPairPtrVec temp_dp_pair_ptrs;
    dp_2d_pair_ptrs_.push_back(temp_dp_pair_ptrs);
    for (size_t j = 0; j < diagonal_ptrs_[i]->size(); j++) {
      int x = diagonal_ptrs_[i]->getDiagPair(j)->getX();
      int y = diagonal_ptrs_[i]->getDiagPair(j)->getY();
      double score = diagonal_ptrs_[i]->getDiagPair(j)->getScore();
      double diff = diagonal_ptrs_[i]->getDiagPair(j)->getDiff();
      DPPairPtr dp_pair_ptr = std::make_shared<DPPair>(x, y, score, diff, j,
                                                       para_ptr_->n_unknown_shift_,
                                                       diagonal_ptrs_[i]->getHeader());
      dp_2d_pair_ptrs_[i].push_back(dp_pair_ptr);
    }
    segment_bgn_pair_ptrs_.push_back(dp_2d_pair_ptrs_[i][0]);
    segment_end_pair_ptrs_.push_back(dp_2d_pair_ptrs_[i][diagonal_ptrs_[i]->size() - 1]);
  }

  // init 1d dp pairs
  dp_pair_ptrs_.clear();
  first_pair_ptr_ = std::make_shared<DPPair>(-1, -1, 0, 0, -1, para_ptr_->n_unknown_shift_, nullptr);
  first_pair_ptr_->setDiagPrevPairPtr(nullptr);
  dp_pair_ptrs_.push_back(first_pair_ptr_);
  for (size_t i = 0; i < dp_2d_pair_ptrs_.size(); i++) {
    for (size_t j = 0; j < dp_2d_pair_ptrs_[i].size(); j++) {
      dp_pair_ptrs_.push_back(dp_2d_pair_ptrs_[i][j]);
      if (j > 0) {
        dp_2d_pair_ptrs_[i][j]->setDiagPrevPairPtr(dp_2d_pair_ptrs_[i][j - 1]);
      }
    }
  }
  // last pair
  double diff = ms_masses_[ms_masses_.size() - 1]
      - seq_masses_[seq_masses_.size() - 1];
  last_pair_ptr_ = std::make_shared<DPPair>(ms_masses_.size(), seq_masses_.size(), 0, diff, -1,
                                            para_ptr_->n_unknown_shift_, nullptr);
  last_pair_ptr_->setDiagPrevPairPtr(nullptr);
  dp_pair_ptrs_.push_back(last_pair_ptr_);
  std::sort(dp_pair_ptrs_.begin(), dp_pair_ptrs_.end(), Pair::cmpPosInc);
  // init indexes
  for (size_t i = 0; i < dp_pair_ptrs_.size(); i++) {
    std::vector<int> ends(diagonal_ptrs_.size(), -1);
    std::vector<bool> large_shifts(diagonal_ptrs_.size(), false);
    DPPairPtr cur_pair_ptr = dp_pair_ptrs_[i];
    if (cur_pair_ptr == last_pair_ptr_ || cur_pair_ptr == first_pair_ptr_) {
      idxes_.push_back(ends);
      penalties_.push_back(large_shifts);
      continue;
    }
    int cur_x = cur_pair_ptr->getX();
    int cur_y = cur_pair_ptr->getY();

    // cur pair is not last pair
    // first pair can not shift, so q starts from 1
    for (size_t j = 1; j < i; j++) {
      DPPairPtr prev_pair_ptr = dp_pair_ptrs_[j];

      int prev_x = prev_pair_ptr->getX();

      int prev_y = prev_pair_ptr->getY();

      double prev_pair_nterm_shift = prev_pair_ptr->getDiagonalHeader()->getProtNTermShift();

      double cur_pair_nterm_shift = cur_pair_ptr->getDiagonalHeader()->getProtNTermShift();

      double shift = cur_pair_nterm_shift - prev_pair_nterm_shift;

      if (prev_x >= cur_x || prev_y >= cur_y
          || prev_pair_ptr->getDiagonalHeader() == cur_pair_ptr->getDiagonalHeader()
          || shift > para_ptr_->align_max_shift_ || shift < para_ptr_->align_min_shift_) {
        continue;
      }

      int diag_id = prev_pair_ptr->getDiagonalHeader()->getId();

      ends[diag_id] = j;

      if (std::abs(shift) > para_ptr_-> align_large_shift_thresh_) {
        large_shifts[diag_id] = true;
      }
    }
    idxes_.push_back(ends);
    penalties_.push_back(large_shifts);
  }
}

void PSAlign::dpPrep() {
  std::sort(dp_pair_ptrs_.begin(), dp_pair_ptrs_.end(), Pair::cmpPosInc);
  for (int s = 0; s < para_ptr_->n_unknown_shift_ + 1; s++) {
    dp_pair_ptrs_[0]->updateTable(s, 0, PATH_TYPE_NULL, nullptr);
  }
}

inline DPPairPtr PSAlign::getTruncPre(DPPairPtr cur_pair_ptr, int s,
                                      ProteoformTypePtr align_type_ptr) {
  DPPairPtr trunc_prev_ptr;
  if (cur_pair_ptr == last_pair_ptr_) {
    double trunc_score = - std::numeric_limits<double>::max();
    for (size_t i = 0; i < segment_end_pair_ptrs_.size(); i++) {
      DPPairPtr prev_pair_ptr = segment_end_pair_ptrs_[i];
      if (align_type_ptr == ProteoformType::COMPLETE || align_type_ptr == ProteoformType::SUFFIX) {
        if (prev_pair_ptr->getDiagonalHeader()->isProtCTermMatch()) {
          if (prev_pair_ptr->getScore(s) > trunc_score) {
            trunc_prev_ptr = prev_pair_ptr;
            trunc_score = prev_pair_ptr->getScore(s);
          }
        }
      } else {
        if (prev_pair_ptr->getDiagonalHeader()->isPepCTermMatch()) {
          if (prev_pair_ptr->getScore(s) > trunc_score) {
            trunc_prev_ptr = prev_pair_ptr;
            trunc_score = prev_pair_ptr->getScore(s);
          }
        }
      }
    }
  } else {
    // if cur_pair_ptr is the first in a diagonal
    if (cur_pair_ptr->getDiagOrder() == 0) {
      if (align_type_ptr == ProteoformType::COMPLETE
          || align_type_ptr == ProteoformType::PREFIX) {
        if (cur_pair_ptr->getDiagonalHeader()->isProtNTermMatch()) {
          trunc_prev_ptr = first_pair_ptr_;
        }
      } else {
        if (cur_pair_ptr->getDiagonalHeader()->isPepNTermMatch()) {
          trunc_prev_ptr = first_pair_ptr_;
        }
      }
    }
  }
  return trunc_prev_ptr;
}

DPPairPtr PSAlign::getShiftPre(int p, int s) {
  if (s < 1) {
    return nullptr;
  }
  // last pair can not shift, only trunc is allowed;
  DPPairPtr shift_prev = nullptr;
  double shift_score = -std::numeric_limits<double>::max();

  // cur pair is not last pair
  // first pair can not shift, so q starts from 1
  for (size_t d = 0; d < idxes_[p].size(); d++) {
    if (idxes_[p][d] >= 0) {
      DPPairPtr prev_pair_ptr = dp_pair_ptrs_[idxes_[p][d]];
      double prev_score = prev_pair_ptr->getScore(s - 1);
      if (penalties_[p][d]) {
        prev_score = prev_score - para_ptr_-> align_large_shift_panelty_;
      }
      if (prev_score > shift_score) {
        shift_prev = prev_pair_ptr;
        shift_score = prev_score;
      }
    }
  }
  return shift_prev;
}

void PSAlign::dp(ProteoformTypePtr align_type_ptr) {
  dpPrep();
  for (size_t p = 1; p < dp_pair_ptrs_.size(); p++) {
    for (int s = 0; s <= para_ptr_->n_unknown_shift_; s++) {
      DPPairPtr trunc_prev_ptr = getTruncPre(dp_pair_ptrs_[p], s, align_type_ptr);
      double trunc_score;
      if (trunc_prev_ptr == nullptr) {
        trunc_score = - std::numeric_limits<double>::max();
      } else {
        trunc_score = trunc_prev_ptr->getScore(s);
      }

      DPPairPtr diag_prev = dp_pair_ptrs_[p]->getDiagPrevPairPtr();
      double diag_score;
      if (diag_prev != nullptr) {
        diag_score = diag_prev->getScore(s);
      } else {
        diag_score = - std::numeric_limits<double>::max();
      }
      DPPairPtr shift_prev = getShiftPre(p, s);
      double shift_score;
      if (shift_prev == nullptr) {
        shift_score =  - std::numeric_limits<double>::max();
      } else {
        shift_score = shift_prev->getScore(s - 1);
      }
      double new_score = dp_pair_ptrs_[p]->getPairScore();
      if (trunc_score >= diag_score && trunc_score >= shift_score) {
        if (trunc_score ==  - std::numeric_limits<double>::max()) {
          dp_pair_ptrs_[p]->updateTable(s, -std::numeric_limits<double>::max(),
                                        PATH_TYPE_NULL, nullptr);
        } else {
          dp_pair_ptrs_[p]->updateTable(s, trunc_score + new_score, PATH_TYPE_TRUNC,
                                        trunc_prev_ptr);
        }
      } else if (diag_score >= shift_score) {
        dp_pair_ptrs_[p]->updateTable(s, diag_score + new_score, PATH_TYPE_DIAGONAL,
                                      diag_prev);
      } else {
        dp_pair_ptrs_[p]->updateTable(s, shift_score + new_score, PATH_TYPE_SHIFT,
                                      shift_prev);
      }
    }
  }
}

void PSAlign::backtrace() {
  align_scores_.clear();
  backtrack_diagonal_ptrs_.clear();
  for (int s = 0; s <= para_ptr_->n_unknown_shift_; s++) {
    align_scores_.push_back(last_pair_ptr_->getScore(s));
    backtrack_diagonal_ptrs_.push_back(backtrace(s));
  }
}

DiagonalHeaderPtrVec PSAlign::backtrace(int s) {
  DiagonalHeaderPtrVec list;
  DiagonalHeaderPtr cur_header;
  int cur_end = -1;
  int cur_bgn = -1;
  DPPairPtr p = last_pair_ptr_;
  if (p->getPrevPairPtr(s) == nullptr || p->getPrevPairPtr(s) == first_pair_ptr_
      || p->getScore(s) <= 0) {
    return list;
  }

  // LOG_DEBUG("backtrace start ");
  while (p != first_pair_ptr_) {
    DPPairPtr pre = p->getPrevPairPtr(s);
    // LOG_DEBUG("p " << p << " pre ptr " << pre);
    if (p == last_pair_ptr_) {
      cur_header = pre->getDiagonalHeader();
      cur_end = pre->getY();
    } else if (pre == first_pair_ptr_) {
      cur_bgn = p->getY();
      list.push_back(geneDiagonalHeaderPtr(cur_bgn, cur_end, cur_header));
    } else {
      if (p->getType(s) == PATH_TYPE_SHIFT) {
        cur_bgn = p->getY();
        list.push_back(geneDiagonalHeaderPtr(cur_bgn, cur_end, cur_header));
        cur_header = pre->getDiagonalHeader();
        cur_end = pre->getY();
      }
    }
    if (p->getType(s) == PATH_TYPE_SHIFT) {
      s--;
    }
    p = pre;
  }
  std::reverse(list.begin(), list.end());
  return list;
}

PrsmPtr PSAlign::geneResult(int shift_num, ProteoformPtr proteo_ptr,
                            DeconvMsPtrVec &deconv_ms_ptr_vec,
                            ExtendMsPtrVec &ms_three_ptr_vec,
                            PrsmParaPtr prsm_para_ptr) {
  DiagonalHeaderPtrVec header_ptrs = getDiagonalHeaders(shift_num);
  // double score = ps_align_ptr_->getAlignScr(shift_num);
  // LOG_DEBUG("Shift " << shift_num << " score " << score);
  if (header_ptrs.size() == 0) {
    return nullptr;
  }
  int first_pos = header_ptrs[0]->getTruncFirstResPos();
  int last_pos = header_ptrs[header_ptrs.size()-1]->getTruncLastResPos();
  ProteoformPtr sub_proteo_ptr = proteoform_factory::geneSubProteoform(proteo_ptr, first_pos, last_pos);

  double min_mass = prsm_para_ptr->getSpParaPtr()->getMinMass();
  double ppo = prsm_para_ptr->getSpParaPtr()->getPeakTolerancePtr()->getPpo();

  double refine_prec_mass = refinePrecursorAndHeaderShift(proteo_ptr, ms_three_ptr_vec,
                                                          header_ptrs, ppo, min_mass,
                                                          para_ptr_->refine_prec_step_width_);

  SpParaPtr sp_para_ptr = prsm_para_ptr->getSpParaPtr();
  ExtendMsPtrVec refine_ms_ptr_vec = extend_ms_factory::geneMsThreePtrVec(deconv_ms_ptr_vec,
                                                                          sp_para_ptr, refine_prec_mass);

  DiagonalHeaderPtrVec refined_header_ptrs = refineHeadersBgnEnd(proteo_ptr, refine_ms_ptr_vec,
                                                                 header_ptrs, min_mass);

  if (refined_header_ptrs.size() == 0) {
    return nullptr;
  }

  MassShiftPtrVec shifts = getDiagonalMassChanges(refined_header_ptrs, first_pos,
                                                  last_pos, AlterType::UNEXPECTED);
  sub_proteo_ptr->addMassShiftPtrVec(shifts);

  return std::make_shared<Prsm>(sub_proteo_ptr, deconv_ms_ptr_vec, refine_prec_mass,
                                prsm_para_ptr->getSpParaPtr());
}

} /* namespace toppic */

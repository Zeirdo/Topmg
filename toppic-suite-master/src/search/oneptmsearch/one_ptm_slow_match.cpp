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


#include <chrono>
#include <vector>

#include "search/oneptmsearch/diagonal_header_util.hpp"
#include "search/oneptmsearch/one_ptm_slow_match.hpp"

namespace toppic {

OnePtmSlowMatch::OnePtmSlowMatch(ProteoformPtr proteo_ptr,
                                 SpectrumSetPtr spectrum_set_ptr,
                                 SimplePrsmPtr simple_prsm_ptr,
                                 ProteoformTypePtr align_type_ptr,
                                 PtmSearchMngPtr mng_ptr) {
  proteo_ptr_ = proteo_ptr;
  deconv_ms_ptr_vec_ = spectrum_set_ptr->getDeconvMsPtrVec();
  ms_six_ptr_vec_ = spectrum_set_ptr->getMsSixPtrVec();
  ms_three_ptr_vec_ = spectrum_set_ptr->getMsThreePtrVec();
  prec_mono_mass_ = spectrum_set_ptr->getPrecMonoMass();
  align_type_ptr_ = align_type_ptr;
  simple_prsm_ptr_ = simple_prsm_ptr;
  mng_ptr_ = mng_ptr;
  init();
}

inline void OnePtmSlowMatch::addPrefixDiagonals(DiagonalHeaderPtrVec &n_extend_header_ptrs) {
  std::vector<double> shifts = simple_prsm_ptr_->getNTruncShifts();
  for (size_t i = 0; i < shifts.size(); i++) {
    double new_shift = shifts[i] - proteo_ptr_->getProtModPtr()->getProtShift();
    if (!DiagonalHeaderUtil::isExistHeader(n_extend_header_ptrs, new_shift)) {
      // n_term strict; c_term nostrict; prot n_term no_match; prot c_term no_match
      // pep n_term match; pep c_term no_match
      DiagonalHeaderPtr header_ptr = std::make_shared<DiagonalHeader>(new_shift, true, false, false, false, true, false);
      n_extend_header_ptrs.push_back(header_ptr);
    }
  }
}

inline void OnePtmSlowMatch::addComplementDiagonals(DiagonalHeaderPtrVec &n_extend_header_ptrs,
                                                    DiagonalHeaderPtrVec &c_extend_header_ptrs) {
  std::vector<double> prm_masses = proteo_ptr_->getBpSpecPtr()->getPrmMasses();
  // shifts for n_term matches
  std::vector<double> n_term_match_shifts;
  for (size_t i = 1; i < prm_masses.size(); i++) {
    n_term_match_shifts.push_back(-prm_masses[i]);
  }
  // add n trunc headers that have similar shift to c trunc headers
  for (size_t i = 0; i < c_extend_header_ptrs.size(); i++) {
    double s = c_extend_header_ptrs[i]->getProtNTermShift();
    // find a similar shift in n_term_match_shifts
    int best_n_pos = DiagonalHeaderUtil::findSimilarShiftPos(n_term_match_shifts, s);
    if (best_n_pos >= 0) {
      double new_shift = n_term_match_shifts[best_n_pos];
      if (!DiagonalHeaderUtil::isExistHeader(n_extend_header_ptrs, new_shift)) {
        // n_term strict; c_term nostrict; prot n_term no_match; prot c_term no_match
        // pep n_term match; pep c_term no_match
        DiagonalHeaderPtr header_ptr = std::make_shared<DiagonalHeader>(new_shift, true, false, false, false, true, false);
        n_extend_header_ptrs.push_back(header_ptr);
      }
    }
  }

  double prec_mass_minus_water = prec_mono_mass_ - mass_constant::getWaterMass();
  // shifts for c_term matches
  std::vector<double> c_term_match_shifts;
  for (size_t i = 1; i < prm_masses.size(); i++) {
    c_term_match_shifts.push_back(prec_mass_minus_water - prm_masses[i]);
  }

  // add c trunc headers that have similar shift to n trunc headers
  for (size_t i = 0; i < n_extend_header_ptrs.size(); i++) {
    double s = n_extend_header_ptrs[i]->getProtNTermShift();
    int best_c_pos = DiagonalHeaderUtil::findSimilarShiftPos(c_term_match_shifts, s);
    // LOG_DEBUG("Shift " << s <<" C term position " << best_c_pos);
    if (best_c_pos >= 0) {
      double new_shift = c_term_match_shifts[best_c_pos];
      // LOG_DEBUG("Shift " << s <<" C term shift " << new_shift);
      if (!DiagonalHeaderUtil::isExistHeader(c_extend_header_ptrs, new_shift)) {
        // n term nostrict, c_term strict, prot n_term no match ; prot c_term no match
        // pep n_term no match, pep c_term match
        DiagonalHeaderPtr header_ptr = std::make_shared<DiagonalHeader>(new_shift, false, true, false, false, false, true);
        c_extend_header_ptrs.push_back(header_ptr);
      }
    }
  }
}

inline void OnePtmSlowMatch::addSuffixDiagonals(DiagonalHeaderPtrVec &c_extend_header_ptrs) {
  std::vector<double>shifts = simple_prsm_ptr_->getCTruncShifts();
  for (size_t i = 0; i < shifts.size(); i++) {
    double new_shift = shifts[i] - proteo_ptr_->getProtModPtr()->getProtShift();
    if (!DiagonalHeaderUtil::isExistHeader(c_extend_header_ptrs, new_shift)) {
      // n term nostrict, c_term strict, prot n_term no match ; prot c_term no match
      // pep n_term no match, pep c_term match
      DiagonalHeaderPtr header_ptr = std::make_shared<DiagonalHeader>(new_shift, false, true, false, false, false, true);
      c_extend_header_ptrs.push_back(header_ptr);
    }
  }
}

inline DiagonalHeaderPtrVec OnePtmSlowMatch::geneOnePtmNTermShiftHeaders() {
  DiagonalHeaderPtrVec n_extend_header_ptrs;
  DiagonalHeaderPtrVec c_extend_header_ptrs;

  // add corner diagonals for all types of alignments
  double seq_mass = proteo_ptr_->getResSeqPtr()->getSeqMass();
  DiagonalHeaderUtil::addCornerDiagonals(n_extend_header_ptrs, c_extend_header_ptrs, seq_mass, prec_mono_mass_);

  // if not complete alignment, find best shifts
  if (align_type_ptr_ != ProteoformType::COMPLETE) {
    if (align_type_ptr_ == ProteoformType::SUFFIX || align_type_ptr_ == ProteoformType::INTERNAL) {
      // add prefix masses
      addPrefixDiagonals(n_extend_header_ptrs);
    }
    if (align_type_ptr_ == ProteoformType::PREFIX || align_type_ptr_ == ProteoformType::INTERNAL) {
      addSuffixDiagonals(c_extend_header_ptrs);
    }
    addComplementDiagonals(n_extend_header_ptrs, c_extend_header_ptrs);
  }
  DiagonalHeaderPtrVec header_ptrs;
  header_ptrs.insert(header_ptrs.end(), n_extend_header_ptrs.begin(), n_extend_header_ptrs.end());
  header_ptrs.insert(header_ptrs.end(), c_extend_header_ptrs.begin(), c_extend_header_ptrs.end());
  for (size_t i = 0; i < header_ptrs.size(); i++) {
    double n_shift = header_ptrs[i]->getProtNTermShift();
    double c_shift = prec_mono_mass_ - proteo_ptr_->getResSeqPtr()->getSeqMass() - n_shift;
    header_ptrs[i]->initHeader(c_shift, proteo_ptr_, mng_ptr_->align_prefix_suffix_shift_thresh_);
  }
  return header_ptrs;
}

// initialize ps_align
void OnePtmSlowMatch::init() {
  // auto start = std::chrono::high_resolution_clock::now();
  DiagonalHeaderPtrVec n_term_shift_header_ptrs = geneOnePtmNTermShiftHeaders(); 
  // auto step_1 = std::chrono::high_resolution_clock::now();
  // LOG_DEBUG("Init n term diag time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(step_1-start).count());
  PeakTolerancePtr tole_ptr = mng_ptr_->prsm_para_ptr_->getSpParaPtr()->getPeakTolerancePtr();
  PrmPeakPtrVec prm_peaks = prm_ms::getPrmPeakPtrs(ms_six_ptr_vec_, tole_ptr);
  int group_spec_num = ms_six_ptr_vec_.size();
  BasicDiagonalPtrVec diagonal_ptrs = geneDiagonals(n_term_shift_header_ptrs,
                                                    prm_peaks, group_spec_num,
                                                    proteo_ptr_);
  // auto step_2 = std::chrono::high_resolution_clock::now();
  // LOG_DEBUG("Init diag time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(step_2-step_1).count());

  std::vector<double> seq_masses = proteo_ptr_->getBpSpecPtr()->getPrmMasses();
  std::vector<double> ms_masses(prm_peaks.size());
  for (size_t i = 0; i < prm_peaks.size(); i++) {
    ms_masses[i] = prm_peaks[i]->getPosition();
  }
  ps_align_ptr_ = std::make_shared<PSAlign>(ms_masses, seq_masses, diagonal_ptrs, mng_ptr_->align_para_ptr_);

  // auto step_3 = std::chrono::high_resolution_clock::now();
  // LOG_DEBUG("Init ps time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(step_3-step_2).count());
}

PrsmPtr OnePtmSlowMatch::compute(int shift_num) {
  ps_align_ptr_->compute(align_type_ptr_);
  return ps_align_ptr_->geneResult(shift_num, proteo_ptr_, deconv_ms_ptr_vec_,
                                   ms_three_ptr_vec_, mng_ptr_->prsm_para_ptr_);
}

}  // namespace toppic

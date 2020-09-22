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


#include <utility>
#include <vector>

#include "search/oneptmsearch/diagonal_header_util.hpp"
#include "search/ptmsearch/ptm_slow_match.hpp"

namespace toppic {

PtmSlowMatch::PtmSlowMatch(ProteoformPtr proteo_ptr,
                           SpectrumSetPtr spectrum_set_ptr,
                           ProteoformTypePtr align_type_ptr,
                           CompShiftLowMem comp_shift,
                           PtmSearchMngPtr mng_ptr) {
  proteo_ptr_ = proteo_ptr;
  deconv_ms_ptr_vec_ = spectrum_set_ptr->getDeconvMsPtrVec();
  ms_six_ptr_vec_ = spectrum_set_ptr->getMsSixPtrVec();
  ms_three_ptr_vec_ = spectrum_set_ptr->getMsThreePtrVec();
  prec_mono_mass_ = spectrum_set_ptr->getPrecMonoMass();
  align_type_ptr_ = align_type_ptr;
  comp_shift_ = comp_shift;
  mng_ptr_ = mng_ptr;
  init();
}

// get headers without n trunc and c trunc information
DiagonalHeaderPtrVec PtmSlowMatch::getNTermShiftListCommonHeaders() {
  double scale = mng_ptr_->ptm_fast_filter_scale_;
  // n term strict c term nonstrict
  PeakTolerancePtr tole_ptr = mng_ptr_->prsm_para_ptr_->getSpParaPtr()->getPeakTolerancePtr();
  std::vector<std::pair<int, int>> sp_masses_toles
      = prm_ms::getIntMassErrorList(ms_six_ptr_vec_, tole_ptr, scale, true, false);

  std::vector<double> best_shifts = comp_shift_.findBestShift(
      sp_masses_toles,
      proteo_ptr_->getBpSpecPtr()->getScaledPrmMasses(scale),
      mng_ptr_->n_top_diagonals_,
      mng_ptr_->min_diagonal_gap_,
      scale);

  DiagonalHeaderPtrVec header_ptrs;
  for (size_t i = 0; i < best_shifts.size(); i++) {
    // n term shift; c term nostrict; no prot nterm match; no prot cterm match,
    // no pep nterm match; no pep cterm match
    DiagonalHeaderPtr header_ptr
        = std::make_shared<DiagonalHeader>(best_shifts[i],
                                           true, false, false, false, false, false);
    header_ptrs.push_back(header_ptr);
  }
  return header_ptrs;
}

void PtmSlowMatch::addPrefixDiagonals(DiagonalHeaderPtrVec &common_header_ptrs,
                                      DiagonalHeaderPtrVec &n_extend_header_ptrs) {
  std::vector<double> prm_masses = proteo_ptr_->getBpSpecPtr()->getPrmMasses();
  // shifts for n_term matches
  std::vector<double> n_term_match_shifts;
  for (size_t i = 1; i < prm_masses.size(); i++) {
    n_term_match_shifts.push_back(-prm_masses[i]);
  }
  // add trunc headers that have similar shift to best shift headers
  for (size_t i = 0; i < common_header_ptrs.size(); i++) {
    double s = common_header_ptrs[i]->getProtNTermShift();
    // find a similar shift in n_term_match_shifts
    int best_n_pos = DiagonalHeaderUtil::findSimilarShiftPos(n_term_match_shifts, s);
    if (best_n_pos >= 0) {
      double new_shift = n_term_match_shifts[best_n_pos];
      if (!DiagonalHeaderUtil::isExistHeader(n_extend_header_ptrs, new_shift)) {
        // n_term strict; c_term nostrict; prot n_term no_match; prot c_term no_match
        // pep n_term match; pep c_term no_match
        DiagonalHeaderPtr header_ptr
            = std::make_shared<DiagonalHeader>(new_shift, true, false, false, false, true, false);
        n_extend_header_ptrs.push_back(header_ptr);
      }
    }
  }
}

void PtmSlowMatch::addSuffixDiagonals(DiagonalHeaderPtrVec &common_header_ptrs,
                                      DiagonalHeaderPtrVec &c_extend_header_ptrs) {
  double prec_mass_minus_water = prec_mono_mass_ - mass_constant::getWaterMass();
  std::vector<double> prm_masses = proteo_ptr_->getBpSpecPtr()->getPrmMasses();
  // shifts for c_term matches
  std::vector<double> c_term_match_shifts;
  for (size_t i = 1; i < prm_masses.size(); i++) {
    c_term_match_shifts.push_back(prec_mass_minus_water - prm_masses[i]);
  }

  // add trunc headers that have similar shift to best shift headers
  for (size_t i = 0; i < common_header_ptrs.size(); i++) {
    double s = common_header_ptrs[i]->getProtNTermShift();
    int best_c_pos = DiagonalHeaderUtil::findSimilarShiftPos(c_term_match_shifts, s);
    // LOG_DEBUG("Shift " << s <<" C term position " << best_c_pos);
    if (best_c_pos >= 0) {
      double new_shift = c_term_match_shifts[best_c_pos];
      // LOG_DEBUG("Shift " << s <<" C term shift " << new_shift);
      if (!DiagonalHeaderUtil::isExistHeader(c_extend_header_ptrs, new_shift)) {
        // n term nostrict, c_term strict, prot n_term no match ; prot c_term no match
        // pep n_term no match, pep c_term match
        DiagonalHeaderPtr header_ptr
            = std::make_shared<DiagonalHeader>(new_shift, false, true, false, false, false, true);
        c_extend_header_ptrs.push_back(header_ptr);
      }
    }
  }
}

DiagonalHeaderPtrVec PtmSlowMatch::geneNTermShiftHeaders() {
  DiagonalHeaderPtrVec n_extend_header_ptrs;
  DiagonalHeaderPtrVec c_extend_header_ptrs;

  // add corner diagonals for all types of alignments
  double seq_mass = proteo_ptr_->getResSeqPtr()->getSeqMass();
  DiagonalHeaderUtil::addCornerDiagonals(n_extend_header_ptrs,
                                         c_extend_header_ptrs,
                                         seq_mass,
                                         prec_mono_mass_);

  DiagonalHeaderPtrVec header_ptrs;
  // if not complete alignment, find best shifts
  if (align_type_ptr_ != ProteoformType::COMPLETE) {
    DiagonalHeaderPtrVec common_header_ptrs = getNTermShiftListCommonHeaders();

    if (align_type_ptr_ == ProteoformType::SUFFIX || align_type_ptr_ == ProteoformType::INTERNAL) {
      // add prefix masses
      addPrefixDiagonals(common_header_ptrs, n_extend_header_ptrs);
    }
    if (align_type_ptr_ == ProteoformType::PREFIX || align_type_ptr_ == ProteoformType::INTERNAL) {
      addSuffixDiagonals(common_header_ptrs, c_extend_header_ptrs);
    }
    // add original headers for ps alignment
    if (mng_ptr_->align_para_ptr_->getUnknownShiftNum() >= 2) {
      header_ptrs.insert(header_ptrs.end(), common_header_ptrs.begin(), common_header_ptrs.end());
    }
  }
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
void PtmSlowMatch::init() {
  DiagonalHeaderPtrVec n_term_shift_header_ptrs = geneNTermShiftHeaders();
  PeakTolerancePtr tole_ptr = mng_ptr_->prsm_para_ptr_->getSpParaPtr()->getPeakTolerancePtr();
  PrmPeakPtrVec prm_peaks = prm_ms::getPrmPeakPtrs(ms_six_ptr_vec_, tole_ptr);
  int group_spec_num = ms_six_ptr_vec_.size();
  BasicDiagonalPtrVec diagonal_ptrs = geneDiagonals(n_term_shift_header_ptrs,
                                                    prm_peaks, group_spec_num,
                                                    proteo_ptr_);

  std::vector<double> seq_masses = proteo_ptr_->getBpSpecPtr()->getPrmMasses();
  std::vector<double> ms_masses(prm_peaks.size());
  for (size_t i = 0; i < prm_peaks.size(); i++) {
    ms_masses[i] = prm_peaks[i]->getPosition();
  }
  ps_align_ptr_ = std::make_shared<PSAlign>(ms_masses, seq_masses,
                                            diagonal_ptrs, mng_ptr_->align_para_ptr_);
}

void PtmSlowMatch::compute(ProteoformTypePtr type_ptr, PrsmPtrVec &prsm_ptrs) {
  ps_align_ptr_->compute(type_ptr);
  for (int s = 2; s <= mng_ptr_->align_para_ptr_->getUnknownShiftNum(); s++) {
    PrsmPtr prsm_ptr = ps_align_ptr_->geneResult(s, proteo_ptr_, deconv_ms_ptr_vec_,
                                                 ms_three_ptr_vec_, mng_ptr_->prsm_para_ptr_);
    prsm_ptrs.push_back(prsm_ptr);
  }
}

PrsmPtr PtmSlowMatch::compute(ProteoformTypePtr align_type_ptr, int shift_num) {
  ps_align_ptr_->compute(align_type_ptr_);
  return ps_align_ptr_->geneResult(shift_num, proteo_ptr_, deconv_ms_ptr_vec_,
                                   ms_three_ptr_vec_, mng_ptr_->prsm_para_ptr_);
}

} /* namespace toppic */

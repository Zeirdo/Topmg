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


#ifndef TOPPIC_STAT_TDGF_COUNT_TEST_NUM_HPP_
#define TOPPIC_STAT_TDGF_COUNT_TEST_NUM_HPP_

#include "seq/proteoform.hpp"
#include "common/base/residue_freq.hpp"
#include "stat/tdgf/tdgf_mng.hpp"

namespace toppic {

class CountTestNum {
 public:
  /*
  CountTestNum(const ProteoformPtrVec &raw_proteo_ptrs, 
               const ProteoformPtrVec &mod_proteo_ptrs,
               const ResFreqPtrVec &residue_ptrs, 
               double convert_ratio, double max_prec_mass,
               double max_ptm_mass);
               */
  CountTestNum(TdgfMngPtr mng_ptr);

  ~CountTestNum();

  double compCandNum(ProteoformTypePtr type_ptr, int index, 
                     double ori_mass, double ori_tolerance);

  ResFreqPtrVec getResFreqPtrVec() {return residue_ptrs_;}
  ResFreqPtrVec getNTermResFreqPtrVec() {return prot_n_term_residue_ptrs_;}
  double getResidueAvgLen() {return residue_avg_len_;}

 private:
  static double PREFIX_SUFFIX_ADJUST() {return 0.693;}
  static double INTERNAL_ADJUST() {return 0.508;}

  //ProteoformPtrVec raw_proteo_ptrs_;
  //ProteoformPtrVec mod_proteo_ptrs_;

  double *comp_mass_cnts_;
  double *pref_mass_cnts_;
  double *suff_mass_cnts_;
  double *internal_mass_cnts_;

  ResFreqPtrVec residue_ptrs_; 
  ResFreqPtrVec prot_n_term_residue_ptrs_;

  std::vector<int> raw_proteo_lens_;
  std::vector<int> mod_proteo_lens_;

  double convert_ratio_;
  int max_sp_len_;
  int residue_avg_len_;
  double max_ptm_mass_;
  double norm_factor_;
  

  int convertMass(double m);

  void init(PrsmParaPtr para_ptr);

  void initCompMassCnt(const ProteoformPtrVec &prot_mod_forms);

  void initPrefMassCnt(const ProteoformPtrVec &prot_mod_forms);

  void initSuffMassCnt(const ProteoformPtrVec &raw_forms);

  void initInternalMassCnt();

  double compNonPtmCandNum(ProteoformTypePtr type_ptr, 
                           double ori_mass, double ori_tolerance);

  double compPtmCandNum(ProteoformTypePtr type_ptr);

  double compPtmRestrictCandNum (ProteoformTypePtr type_ptr, int shift_num, double ori_mass);

  double compSeqNum(ProteoformTypePtr type_ptr, int low, int high);

  double compMassNum(double *cnts, int low, int high);
};

typedef std::shared_ptr<CountTestNum> CountTestNumPtr;


}

#endif

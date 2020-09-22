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


#ifndef TOPPIC_SEARCH_ONE_PTM_SEARCH_DP_PAIR_HPP_
#define TOPPIC_SEARCH_ONE_PTM_SEARCH_DP_PAIR_HPP_

#include "search/oneptmsearch/pair.hpp"
#include "search/oneptmsearch/diagonal_header.hpp"

namespace toppic {

#define PATH_TYPE_NULL -1
#define PATH_TYPE_DIAGONAL 0
#define PATH_TYPE_SHIFT 1
#define PATH_TYPE_TRUNC 2

class DPPair;
typedef std::shared_ptr<DPPair>  DPPairPtr;
typedef std::vector<DPPairPtr> DPPairPtrVec;

class DPPair : public Pair{
 public:
  DPPair(int x,int y,double pair_score,double diff,
         int order,int n_shift, DiagonalHeaderPtr header_ptr);

  DPPairPtr getDiagPrevPairPtr() {return diag_prev_pair_ptr_;}

  void setDiagPrevPairPtr(DPPairPtr diag_prev_pair_ptr) {
    diag_prev_pair_ptr_ = diag_prev_pair_ptr;}

  double getDiff() {return diff_;}

  DiagonalHeaderPtr getDiagonalHeader() {return header_ptr_;}

  int getDiagOrder() {return order_;}

  double getPairScore() {return pair_score_;}

  double getScore(int s) {return scores_[s];}

  DPPairPtr getPrevPairPtr(int s){return prev_pair_ptrs_[s];}

  int getType(int s){return types_[s];}

  bool isAssisting(){ return (pair_score_==0.0); }

  void updateTable(int s,double score,int path_type,DPPairPtr prev_pair);

 private:
  DiagonalHeaderPtr header_ptr_;
  double diff_;
  double pair_score_;
  int order_;
  DPPairPtr diag_prev_pair_ptr_;
  DPPairPtrVec prev_pair_ptrs_;
  std::vector<double> scores_;
  std::vector<int> types_;
};

} /* namespace toppic */

#endif /* DP_PAIR_HPP_ */

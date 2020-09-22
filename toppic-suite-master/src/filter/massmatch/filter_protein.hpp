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

#ifndef TOPPIC_FILTER_MASS_MATCH_FILTER_PROTEIN_HPP_
#define TOPPIC_FILTER_MASS_MATCH_FILTER_PROTEIN_HPP_

#include <memory>
#include <vector>

namespace toppic {

class FilterProtein;
typedef std::shared_ptr<FilterProtein> FilterProteinPtr;
typedef std::vector<FilterProteinPtr> FilterProteinPtrVec;

class FilterProtein {
 public:
  FilterProtein(int protein_id, int score);
  int getProteinId() {return protein_id_;}
  int getScore() {return score_;}
  std::vector<double> getNTermShifts() {return n_term_shifts_;}
  std::vector<double> getCTermShifts() {return c_term_shifts_;}

  void setNTermShifts(std::vector<double> shifts) {n_term_shifts_ = shifts;}
  void setCTermShifts(std::vector<double> shifts) {c_term_shifts_ = shifts;}

  static FilterProteinPtrVec geneResults(std::vector<std::pair<int,int>> &single_type_results, 
                                         int threshold, int single_type_num);

 private:
  int protein_id_;
  int score_;
  std::vector<double> n_term_shifts_;
  std::vector<double> c_term_shifts_;
};

} /* namespace toppic */

#endif 

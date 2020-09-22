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


#ifndef TOPPIC_COMMON_BASE_RESIDUE_UTIL_HPP_
#define TOPPIC_COMMON_BASE_RESIDUE_UTIL_HPP_

#include "common/util/str_util.hpp"
#include "common/base/mod.hpp"
#include "common/base/residue.hpp"

namespace toppic {

namespace residue_util {

bool isValidResidue(char c);

char replaceResidueLetter(char c);

ResiduePtrVec convertStrToResiduePtrVec(const std::string &seq);

ResiduePtrVec convertStrToResiduePtrVec(const std::string &seq, 
                                        const ModPtrVec &fix_mod_ptr_vec);

ResiduePtrVec convertStrToResiduePtrVec(const StringPairVec &string_pair_vec);

ResiduePtrVec convertStrToResiduePtrVec(const StringPairVec &string_pair_vec,
                                        const ModPtrVec &fix_mod_ptr_vec);

int findResidue(const ResiduePtrVec &residue_list, ResiduePtr residue_ptr);

double compResiduePtrVecMass(const ResiduePtrVec &ptr_vec);

double compResiduePtrVecMass(const std::string & seq);

double compResiduePtrVecMass(const std::string & seq,
                             const ModPtrVec &fix_mod_ptr_vec);

}  // namespace residue_util

}  // namespace toppic
#endif

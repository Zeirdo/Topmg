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

#ifndef TOPPIC_SEQ_FASTA_SUB_UTIL_HPP_
#define TOPPIC_SEQ_FASTA_SUB_UTIL_HPP_

#include "seq/fasta_sub_seq.hpp"

namespace toppic {

namespace fasta_sub_util {

FastaSubSeqPtrVec breakSeq(FastaSeqPtr seq_ptr);

FastaSubSeqPtrVec breakSeq(FastaSeqPtr seq_ptr, int N);

}  // namespace fasta_sub_util

}  // namespace toppic

#endif

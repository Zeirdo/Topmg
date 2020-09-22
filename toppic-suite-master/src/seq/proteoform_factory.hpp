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

#ifndef TOPPIC_SEQ_PROTEOFORM_FACTORY_HPP_
#define TOPPIC_SEQ_PROTEOFORM_FACTORY_HPP_

#include "seq/mass_shift.hpp"
#include "seq/proteoform.hpp"
#include "seq/fasta_index_reader.hpp"

namespace toppic {

namespace proteoform_factory {

ProteoformPtr geneDbProteoformPtr(FastaSeqPtr seq_ptr, ModPtrVec fix_mod_list);

// Generate a proteoform with protein N-terminal modification 
ProteoformPtr geneProtModProteoform(ProteoformPtr db_form_ptr,
                                    ProtModPtr prot_mod_ptr);

ProteoformPtrVec geneProtModProteoform(ProteoformPtr db_form_ptr,
                                       const ProtModPtrVec &prot_mod_ptrs);

ProteoformPtrVec2D gene2DProtModProteoform(const ProteoformPtrVec &db_form_ptrs,
                                           const ProtModPtrVec &prot_mod_ptrs);

// Get subproteoform. local_start and local_end are relatively to
// the start position in the original proteoform
ProteoformPtr geneSubProteoform(ProteoformPtr proteoform_ptr,
                                int local_start, int local_end);

ProteoformPtr geneProteoform(ProteoformPtr proteoform, int start_pos, int end_pos,
                             const MassShiftPtrVec & mass_shift_vec,
                             const ModPtrVec & mod_ptr_vec);

// generate a proteoform vector with protein mod 
ProteoformPtrVec geneProtModProteoform(const ProteoformPtrVec &ori_forms,
                                       const ProtModPtrVec &prot_mods);

ProteoformPtrVec readFastaToProteoformPtrVec(const std::string &file_name, 
                                             const ModPtrVec &fix_mod_list);

ProteoformPtr readFastaToProteoformPtr(FastaIndexReaderPtr reader_ptr, 
                                       const std::string &seq_name,
                                       const std::string &seq_desc,
                                       const ModPtrVec &fix_mod_list);

}  // namespace proteoform_factory 

}  // namespace toppic

#endif

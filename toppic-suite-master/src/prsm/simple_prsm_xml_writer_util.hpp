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

#ifndef TOPPIC_PRSM_SIMPLE_PRSM_XML_WRITER_UTIL_HPP_
#define TOPPIC_PRSM_SIMPLE_PRSM_XML_WRITER_UTIL_HPP_

#include "prsm/simple_prsm_xml_writer.hpp"

namespace toppic {

namespace simple_prsm_xml_writer_util {

SimplePrsmXmlWriterPtrVec geneWriterPtrVec(std::string &file_name, int thread_num);

void closeWriterPtrVec(SimplePrsmXmlWriterPtrVec &writer_ptr_vec);

}

} /* namespace toppic */

#endif 

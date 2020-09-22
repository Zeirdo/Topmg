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

#ifndef TOPPIC_COMMON_BASE_TRUNC_DATA_HPP_
#define TOPPIC_COMMON_BASE_TRUNC_DATA_HPP_

# include <string>

namespace toppic {

std::string trunc_base_data = R"(
<truncation_list>
<truncation>
	<name>NONE</name>
	<trunc_len>0</trunc_len>
	<trunc_residues></trunc_residues>
  <allow_first_remain_residues></allow_first_remain_residues>
</truncation>
<truncation>
	<name>NME</name>
	<trunc_len>1</trunc_len>
	<trunc_residues>M</trunc_residues>
  <allow_first_remain_residues>ACGPSTV</allow_first_remain_residues>
</truncation>
</truncation_list>
)";

} 

#endif

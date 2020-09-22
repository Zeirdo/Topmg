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

#ifndef TOPPIC_TOPPIC_PROCESS_HPP
#define TOPPIC_TOPPIC_PROCESS_HPP

#include <string>
#include <map>
#include <vector>

namespace toppic {

int TopPICProgress(std::map<std::string, std::string> & arguments);

// protein filtering + database searching + E-value computation
int TopPIC_identify(std::map<std::string, std::string> & arguments);

// proteoform clustering + FDR + HTML generation
int TopPIC_post(std::map<std::string, std::string> & arguments);

int TopPICProgress_multi_file(std::map<std::string, std::string> & arguments,
                              const std::vector<std::string> & spec_file_lst);

}  // namespace toppic

#endif

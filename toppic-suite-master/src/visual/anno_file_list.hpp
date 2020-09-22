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

#ifndef TOPPIC_VISUAL_ANNO_FILE_LIST_HPP_
#define TOPPIC_VISUAL_ANNO_FILE_LIST_HPP_

#include <memory>
#include <string>
#include <vector>

#include "common/xml/xml_dom_document.hpp"

namespace toppic {

class AnnoFileList {
 public:
  std::vector<std::vector<std::string>> file_list_;

  xercesc::DOMElement* geneFileList(XmlDOMDocument* xml_doc);

  static std::vector<std::vector<std::string>> readFromXml(const std::string &file_name);
};

typedef std::shared_ptr<AnnoFileList> AnnoFileListPtr;


}  // namespace toppic

#endif /* TOPPIC_VISUAL_ANNO_FILE_LIST_HPP_ */

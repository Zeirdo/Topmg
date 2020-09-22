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

#ifndef TOPPIC_COMMON_BASE_PROT_MOD_HPP_
#define TOPPIC_COMMON_BASE_PROT_MOD_HPP_

#include "common/base/mod.hpp"
#include "common/base/trunc.hpp"

namespace toppic {

class XmlDOMDocument;

class ProtMod {
 public:
  ProtMod(const std::string &name, const std::string &type,
          TruncPtr trunc_ptr, ModPtr mod_ptr);

  ProtMod(XmlDOMElement* element); 

  const std::string& getName() { return name_;};

  const std::string& getType() { return type_;};

  TruncPtr getTruncPtr() { return trunc_ptr_;}

  ModPtr getModPtr() { return mod_ptr_;}

  int getModPos() {return mod_pos_;}

  double getProtShift() { return prot_shift_;}

  double getPepShift() { return pep_shift_;}

  bool isAcetylation();

  void appendNameToXml(XmlDOMDocument* xml_doc,XmlDOMElement* parent);

  static std::string getXmlElementName() {return "prot_mod";}

  static std::string getNameFromXml(XmlDOMElement * element);

 private:
  std::string name_;
  std::string type_;
  TruncPtr trunc_ptr_;
  ModPtr mod_ptr_;
  int mod_pos_;
  double prot_shift_;
  double pep_shift_;
};

typedef std::shared_ptr<ProtMod> ProtModPtr;
typedef std::vector<ProtModPtr> ProtModPtrVec;

}
#endif

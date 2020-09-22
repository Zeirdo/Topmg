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

#include <string>

#include "common/xml/xml_dom_document.hpp"
#include "common/xml/xml_dom_util.hpp"
#include "common/base/activation.hpp"
#include "common/base/ion_type_base.hpp"

namespace toppic {

Activation::Activation(const std::string &name, 
                       IonTypePtr n_ion_type_ptr,
                       IonTypePtr c_ion_type_ptr):
    name_(name),
    n_ion_type_ptr_(n_ion_type_ptr),
    c_ion_type_ptr_(c_ion_type_ptr) {}

Activation::Activation(XmlDOMElement * element) {
  name_ = xml_dom_util::getChildValue(element, "name", 0);
  std::string ion_type_name = xml_dom_util::getChildValue(element, "n_ion_type", 0);
  n_ion_type_ptr_ = IonTypeBase::getIonTypePtrByName(ion_type_name);
  ion_type_name = xml_dom_util::getChildValue(element, "c_ion_type", 0);
  c_ion_type_ptr_ = IonTypeBase::getIonTypePtrByName(ion_type_name);
}

void Activation::appendNameToXml(XmlDOMDocument* xml_doc,
                                 XmlDOMElement* parent) {
  std::string element_name = Activation::getXmlElementName();
  XmlDOMElement* element = xml_doc->createElement(element_name.c_str());
  xml_doc->addElement(element, "name", name_.c_str());
  parent->appendChild(element);
}

std::string Activation::getNameFromXml(XmlDOMElement * element) {
  std::string name = xml_dom_util::getChildValue(element, "name", 0);
  return name;
}

}  // namespace toppic

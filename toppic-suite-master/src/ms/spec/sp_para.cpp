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

#include "common/util/str_util.hpp"
#include "common/base/activation_base.hpp"
#include "common/xml/xml_dom_util.hpp"
#include "common/xml/xml_dom_document.hpp"
#include "ms/spec/sp_para.hpp"

namespace toppic {

SpPara::SpPara(int min_peak_num, double min_mass, double min_extend_mass,
               const std::vector<double> &ext_offsets,
               PeakTolerancePtr peak_tolerance_ptr,
               ActivationPtr activation_ptr,
               const std::set<std::string> & skip_list):
    min_peak_num_(min_peak_num),
    min_mass_(min_mass),
    extend_min_mass_(min_extend_mass),
    ext_offsets_(ext_offsets),
    peak_tolerance_ptr_(peak_tolerance_ptr),
    activation_ptr_(activation_ptr),
    skip_list_(skip_list) {}

SpPara::SpPara(xercesc::DOMElement* element) {
  min_peak_num_ = xml_dom_util::getIntChildValue(element, "min_peak_num", 0);
  min_mass_ = xml_dom_util::getDoubleChildValue(element, "min_mass", 0);
  extend_min_mass_ = xml_dom_util::getDoubleChildValue(element, "extend_min_mass", 0);
  xercesc::DOMElement* list_element = xml_dom_util::getChildElement(element, "extend_offset_list", 0);
  int offset_num =  xml_dom_util::getChildCount(list_element, "extend_offset");
  for (int i = 0; i < offset_num; i++) {
    double offset = xml_dom_util::getDoubleChildValue(list_element, "extend_offset", i);
    ext_offsets_.push_back(offset);
  }
  std::string element_name = PeakTolerance::getXmlElementName();
  xercesc::DOMElement* pt_element = xml_dom_util::getChildElement(element, element_name.c_str(), 0);
  peak_tolerance_ptr_ = std::make_shared<PeakTolerance>(pt_element);

  element_name = Activation::getXmlElementName();
  xercesc::DOMElement* ac_element = xml_dom_util::getChildElement(element, element_name.c_str(), 0);
  activation_ptr_ = ActivationBase::getActivationPtrFromXml(ac_element);
}

void SpPara::appendXml(XmlDOMDocument* xml_doc, xercesc::DOMElement* parent) {
  std::string element_name = SpPara::getXmlElementName();
  xercesc::DOMElement* element = xml_doc->createElement(element_name.c_str());
  std::string str = str_util::toString(min_peak_num_);
  xml_doc->addElement(element, "min_peak_num", str.c_str());
  str = str_util::toString(min_mass_);
  xml_doc->addElement(element, "min_mass", str.c_str());
  str = str_util::toString(extend_min_mass_).c_str();
  xml_doc->addElement(element, "extend_min_mass", str.c_str());
  xercesc::DOMElement* list_element = xml_doc->createElement("extend_offset_list");
  element->appendChild(list_element);
  for (size_t i = 0; i < ext_offsets_.size(); i++) {
    str = str_util::toString(ext_offsets_[i]);
    xml_doc->addElement(list_element, "extend_offset", str.c_str());
  }
  element->appendChild(list_element);
  peak_tolerance_ptr_->appendXml(xml_doc, element);
  activation_ptr_->appendNameToXml(xml_doc, element);
  parent->appendChild(element);
}

}  // namespace toppic

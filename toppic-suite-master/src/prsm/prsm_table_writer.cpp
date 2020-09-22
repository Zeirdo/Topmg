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

#include <iomanip>
#include <sstream>

#include "common/util/logger.hpp"
#include "common/util/file_util.hpp"
#include "ms/spec/msalign_reader.hpp"
#include "ms/spec/extend_ms_factory.hpp"
#include "prsm/prsm_reader.hpp"
#include "prsm/prsm_table_writer.hpp"

namespace toppic {

PrsmTableWriter::PrsmTableWriter(PrsmParaPtr prsm_para_ptr, 
                                 std::string argu_str,
                                 const std::string &input_file_ext, 
                                 const std::string &output_file_ext):
    prsm_para_ptr_(prsm_para_ptr),
    input_file_ext_(input_file_ext),
    argu_str_(argu_str),
    output_file_ext_(output_file_ext) {}

void PrsmTableWriter::write() {
  std::string spectrum_file_name  = prsm_para_ptr_->getSpectrumFileName();
  std::string base_name = file_util::basename(spectrum_file_name);
  std::string output_file_name = base_name + output_file_ext_;
  std::ofstream file;
  file.open(output_file_name.c_str());
  file << argu_str_;
  // write title
  file << "Data file name" << ","
      << "Prsm ID" << ","
      << "Spectrum ID"<< ","
      << "Fragmentation" << ","
      << "Scan(s)" << ","
      << "Retention time" << ","
      << "#peaks"<< ","
      << "Charge" << ","
      << "Precursor mass" << ","
      << "Adjusted precursor mass" << ","
      << "Proteoform ID" << ","
      << "Feature intensity" << ","
      << "Feature score" << ","
      << "Protein accession" << ","
      << "Protein description" << ","
      << "First residue" << ","
      << "Last residue" << ","
      << "Proteoform" << ","
      << "#unexpected modifications" << ","
      << "MIScore" << ","
      << "#variable PTMs" << ","
      << "#matched peaks" << ","
      << "#matched fragment ions" << ","
      << "P-value" << ","
      << "E-value" << ","
      << "Q-value (spectral FDR)" << ","
      << "Proteoform FDR" << std::endl;

  std::string input_file_name = file_util::basename(spectrum_file_name) + "." + input_file_ext_;
  std::string db_file_name = prsm_para_ptr_->getSearchDbFileName();
  FastaIndexReaderPtr seq_reader = std::make_shared<FastaIndexReader>(db_file_name);
  ModPtrVec fix_mod_ptr_vec = prsm_para_ptr_->getFixModPtrVec();
  PrsmReader prsm_reader(input_file_name);
  // LOG_DEBUG("start read prsm");
  PrsmPtr prsm_ptr = prsm_reader.readOnePrsm(seq_reader, fix_mod_ptr_vec);
  // LOG_DEBUG("end read prsm");

  // init variables
  std::string sp_file_name = prsm_para_ptr_->getSpectrumFileName();
  int group_spec_num = prsm_para_ptr_->getGroupSpecNum();
  MsAlignReader sp_reader(sp_file_name, group_spec_num,
                          prsm_para_ptr_->getSpParaPtr()->getActivationPtr(),
                          prsm_para_ptr_->getSpParaPtr()->getSkipList());
  SpectrumSetPtr spec_set_ptr;
  SpParaPtr sp_para_ptr = prsm_para_ptr_->getSpParaPtr();
  while ((spec_set_ptr = sp_reader.getNextSpectrumSet(sp_para_ptr)[0]) != nullptr) {
    if (spec_set_ptr->isValid()) {
      int spec_id = spec_set_ptr->getSpectrumId();
      while (prsm_ptr != nullptr && prsm_ptr->getSpectrumId() == spec_id) {
        DeconvMsPtrVec deconv_ms_ptr_vec = spec_set_ptr->getDeconvMsPtrVec();
        prsm_ptr->setDeconvMsPtrVec(deconv_ms_ptr_vec);
        double new_prec_mass = prsm_ptr->getAdjustedPrecMass();
        ExtendMsPtrVec extend_ms_ptr_vec
            = extend_ms_factory::geneMsThreePtrVec(deconv_ms_ptr_vec, sp_para_ptr, new_prec_mass);
        prsm_ptr->setRefineMsVec(extend_ms_ptr_vec);
        writePrsm(file, prsm_ptr);
        // LOG_DEBUG("start read prsm");
        prsm_ptr = prsm_reader.readOnePrsm(seq_reader, fix_mod_ptr_vec);
        // LOG_DEBUG("end read prsm");
      }
    }
  }
  sp_reader.close();
  prsm_reader.close();
  // write end;
  file.close();
}

void PrsmTableWriter::writePrsm(std::ofstream &file, PrsmPtr prsm_ptr) {
  std::string spec_ids;
  std::string spec_activations;
  std::string spec_scans;
  std::string retention_time;
  int ptm_num = prsm_ptr->getProteoformPtr()->getMassShiftNum(AlterType::UNEXPECTED);
  int peak_num = 0;
  DeconvMsPtrVec deconv_ms_ptr_vec = prsm_ptr->getDeconvMsPtrVec();
  for (size_t i = 0; i < deconv_ms_ptr_vec.size(); i++) {
    spec_ids = spec_ids + str_util::toString(deconv_ms_ptr_vec[i]->getMsHeaderPtr()->getId()) + " ";
    spec_activations = spec_activations + deconv_ms_ptr_vec[i]->getMsHeaderPtr()->getActivationPtr()->getName() + " ";
    spec_scans = spec_scans + deconv_ms_ptr_vec[i]->getMsHeaderPtr()->getScansString() + " ";
    peak_num += deconv_ms_ptr_vec[i]->size();
    retention_time = retention_time + str_util::fixedToString(deconv_ms_ptr_vec[i]->getMsHeaderPtr()->getRetentionTime(), 2) + " ";
  }

  str_util::trim(spec_ids);
  str_util::trim(spec_activations);
  str_util::trim(spec_scans);
  str_util::trim(retention_time);

  if (deconv_ms_ptr_vec[0]->getMsHeaderPtr()->getRetentionTime() <= 0.0) retention_time = "-";

  file << std::setprecision(10);
  LOG_DEBUG("start output prsm ");
  file << prsm_ptr->getFileName() << ","
      << prsm_ptr->getPrsmId() << ","
      << spec_ids << ","
      << spec_activations<< ","
      << spec_scans << ","
      << retention_time << ","
      << peak_num << ","
      << deconv_ms_ptr_vec[0]->getMsHeaderPtr()->getPrecCharge() << ","
      << prsm_ptr->getOriPrecMass()<< ","
      << prsm_ptr->getAdjustedPrecMass() << ","
      << prsm_ptr->getProteoformPtr()->getProteoClusterId() << ",";

  if (prsm_ptr->getPrecFeatureInte() > 0) {
    std::ostringstream str_stream;
    str_stream << std::scientific << std::setprecision(3);
    str_stream << prsm_ptr->getPrecFeatureInte();
    file << str_stream.str() << ",";
  } else {
    file << "-" << ",";
  }

  file << prsm_ptr->getFracFeatureScore() << ",";

  file << prsm_ptr->getProteoformPtr()->getSeqName() << ","
      << "\"" << prsm_ptr->getProteoformPtr()->getSeqDesc() << "\"" << ","
      << (prsm_ptr->getProteoformPtr()->getStartPos() + 1) << ","
      << (prsm_ptr->getProteoformPtr()->getEndPos() + 1) << ","
      << "\"" << prsm_ptr->getProteoformPtr()->getProteinMatchSeq() << "\","
      << ptm_num << ","
      << prsm_ptr->getProteoformPtr()->getMIScore() << ","
      << prsm_ptr->getProteoformPtr()->getVariablePtmNum() << ","
      << prsm_ptr->getMatchPeakNum() << ","
      << prsm_ptr->getMatchFragNum() << ","
      << prsm_ptr->getPValue() << ","
      << prsm_ptr->getEValue() << ",";

  double fdr = prsm_ptr->getFdr();
  if (fdr >= 0) {
    file << fdr << ",";
  } else {
    file << "-" << ",";
  }

  double proteoform_fdr = prsm_ptr->getProteoformFdr();
  if (proteoform_fdr >= 0) {
    file << proteoform_fdr << std::endl;
  } else {
    file << "-" << std::endl;
  }
}

}  // namespace toppic

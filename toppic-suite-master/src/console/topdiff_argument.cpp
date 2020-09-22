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

#include "common/util/logger.hpp"
#include "common/util/file_util.hpp"
#include "common/util/str_util.hpp"
#include "common/xml/xml_dom_util.hpp"
#include "console/topdiff_argument.hpp"

namespace toppic {

Argument::Argument() {
  initArguments();
}

void Argument::initArguments() {
  arguments_["databaseFileName"] = "";
  arguments_["fixedMod"] = "";
  arguments_["errorTolerance"] = "1.2";
  arguments_["toolName"] = "toppic";
  arguments_["mergedOutputFileName"] = "sample_diff.csv";
}

void Argument::outputArguments(std::ostream &output, std::map<std::string, std::string> arguments) {
  output << "******************** Parameters ********************" << std::endl;
  output << std::setw(44) << std::left << "Protein database file: " << "\t" << arguments["databaseFileName"] << std::endl;
  if (arguments["fixedMod"] == "") {
    output << std::setw(44) << std::left << "Fixed modifications: " << "\t" << "None" << std::endl;
  } 
  else if (arguments["fixedMod"] == "C57") {
    output << std::setw(44) << std::left << "Fixed modifications: " << "\t" << "C57:carbamidomethylation on cysteine" << std::endl;
  }
  else if (arguments["fixedMod"] == "C58") {
    output << std::setw(44) << std::left << "Fixed modifications: " << "\t" << "C58:carboxymethylation on cysteine" << std::endl;
  }
  else {
    output << std::setw(44) << std::left << "Fixed modifications:," << arguments["fixedMod"] << std::endl;
  }
  output << std::setw(44) << std::left << "Error tolerance: " << "\t" << arguments["errorTolerance"] << " Dalton " << std::endl;
  output << std::setw(44) << std::left << "Database search tool name: " << "\t" << arguments["toolName"] << std::endl;
  output << std::setw(44) << std::left << "Output file name: " << "\t" << arguments["mergedOutputFileName"] << std::endl;
  output << "******************** Parameters ********************" << std::endl;
}

void Argument::showUsage(boost::program_options::options_description &desc) {
  std::cout << "Usage: topdiff [options] database-file-name spectrum-file-names" << std::endl; 
  std::cout << desc << std::endl; 
}

bool Argument::parse(int argc, char* argv[]) {
  std::string fixed_mod = "";
  std::string error_tole = "1.2";
  std::string tool_name = "toppic";
  std::string database_file_name = "";
  std::string merged_output_name = "sample_diff.csv";

  /** Define and parse the program options*/
  try {
    namespace po = boost::program_options;
    po::options_description display_desc("Options");
    display_desc.add_options() 
        ("help,h", "Print the help message.")
        ("fixed-mod,f", po::value<std::string> (&fixed_mod), 
         "<C57|C58|a fixed modification file>. Fixed modifications. Three available options: C57, C58, or the name of a text file containing the information of fixed modifications. When C57 is selected, carbamidomethylation on cysteine is the only fixed modification. When C58 is selected, carboxymethylation on cysteine is the only fixed modification.")
        ("error-tolerance,e", po::value<std::string>(&error_tole) , "Error tolerance of precursor masses for mapping identified proteoforms. Default: 1.2 Dalton.")
        ("tool-name,t", po::value<std::string>(&tool_name) , "<toppic|topmg>. Database search tool name: toppic or topmg. Default: toppic.")
        ("output,o", po::value<std::string>(&merged_output_name) , "Output file name. Default: sample_diff.csv.");
    po::options_description desc("Options");

    desc.add_options() 
        ("help,h", "") 
        ("fixed-mod,f", po::value<std::string> (&fixed_mod), "")
        ("error-tolerance,e", po::value<std::string>(&error_tole) , "")
        ("tool-name,t", po::value<std::string>(&tool_name), "")
        ("output,o", po::value<std::string>(&merged_output_name) , "")
        ("database-file-name", po::value<std::string>(&database_file_name)->required(), "Database file name.")
        ("spectrum-file-name", po::value<std::vector<std::string> >()->multitoken()->required(), "Spectrum file name with its path.");

    po::positional_options_description positional_options;
    positional_options.add("database-file-name", 1);
    positional_options.add("spectrum-file-name", -1);

    po::variables_map vm;
    try {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(positional_options).run(),vm); 
      if (vm.count("help")) {
        showUsage(display_desc);
        return false;
      }
      po::notify(vm); 
      // throws on error, so do after help in case there are any problems
    } 
    catch(boost::program_options::required_option& e) {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      showUsage(display_desc);
      return false;
    }
    catch(boost::program_options::error& e) {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      showUsage(display_desc);
      return false;
    }
    std::string argv_0 (argv[0]);
    arguments_["executiveDir"] = file_util::getExecutiveDir(argv_0);
    LOG_DEBUG("Executive Dir " << arguments_["executiveDir"]);
    arguments_["resourceDir"] = file_util::getResourceDir(arguments_["executiveDir"]);

    arguments_["databaseFileName"] = database_file_name;

    if (vm.count("spectrum-file-name")) {
      spectrum_file_list_ = vm["spectrum-file-name"].as<std::vector<std::string> >(); 
    }

    if (vm.count("fixed-mod")) {
      arguments_["fixedMod"] = fixed_mod;
    }

    if (vm.count("error-tolerance")) {
      arguments_["errorTolerance"] = error_tole;
    }

    if (vm.count("tool-name")) {
      arguments_["toolName"] = tool_name;
    }

    if (vm.count("output")) {
      arguments_["mergedOutputFileName"] = merged_output_name; 
    }
  }
  catch(std::exception&e ) {
    std::cerr << "Unhandled Exception in parsing command line" << e.what() << ", application will now exit" << std::endl;
    return false;
  }

  return validateArguments();
}

bool Argument::validateArguments() {
  if (!file_util::exists(arguments_["resourceDir"])) {
    LOG_ERROR("Resource direcotry " << arguments_["resourceDir"] << " does not exist!");
    return false;
  }

  if (!file_util::exists(arguments_["databaseFileName"])) {
    LOG_ERROR("Database file " << arguments_["databaseFileName"] << " does not exist!");
    return false;
  }

  if (!str_util::endsWith(arguments_["databaseFileName"], ".fasta") &&
      !str_util::endsWith(arguments_["databaseFileName"], ".fa")) {
    LOG_ERROR("Database file " << arguments_["databaseFileName"] << " is not a fasta file!");
    return false;
  }

  if (arguments_["databaseFileName"].length() > 200) {
    LOG_ERROR("Database file " << arguments_["databaseFileName"] << " path is too long!");
    return false;
  }
  for (size_t k = 0; k < spectrum_file_list_.size(); k++) {
    if (!file_util::exists(spectrum_file_list_[k])) {
      LOG_ERROR(spectrum_file_list_[k] << " does not exist!");
      return false;
    }
  }

  return true;
}

}  // namespace toppic

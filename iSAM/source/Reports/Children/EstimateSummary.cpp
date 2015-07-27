/**
 * @file EstimateSummary.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "EstimateSummary.h"

#include "Estimates/Manager.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Default Constructor
 */
EstimateSummary::EstimateSummary() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation);
  model_state_ = State::kFinalise;
}

/**
 * Destructor
 */
EstimateSummary::~EstimateSummary() noexcept(true) {
}

/**
 * This method will execute our estimate summary report
 */
void EstimateSummary::DoExecute() {


  // Print the estimates
  niwa::estimates::Manager& estimate_manager = niwa::estimates::Manager::Instance();
  vector<EstimatePtr> estimates = estimate_manager.objects();

  /*
  // Header
  cache_ << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";
  //  cout << PARAM_REPORT << "." << PARAM_TYPE << CONFIG_RATIO_SEPARATOR << " " << parameters_.Get(PARAM_TYPE)->GetValue<string>() << "\n";

  for (EstimatePtr estimate : estimates) {
    cache_ << "\n";
    if (estimate->label() != "") {
      cache_ << "Estimate: " << estimate->label() << " (" << estimate->parameter() << ")\n";
      cache_ << "Parameter: " << estimate->parameter() << "\n";
    } else
      cache_ << "Estimate: " << estimate->parameter() << "\n";
    cache_ << "Lower Bound: " << estimate->lower_bound() << "\n";
    cache_ << "Upper Bound: " << estimate->upper_bound() << "\n";
    cache_ << "Value: " << AS_DOUBLE(estimate->value()) << "\n";
    cache_ << "parameters:\n";
    map<string, ParameterPtr> parameters = estimate->parameters().parameters();
    for (auto iter = parameters.begin(); iter != parameters.end(); ++iter) {
      cache_ << iter->first << ": ";
      for (string parameter_value : iter->second->values())
        cache_ << parameter_value << " ";
      cache_ << "\n";
    }
  }
  cache_ << CONFIG_END_REPORT << "\n" << endl;
  */

  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  for (EstimatePtr estimate : estimates) {
	cache_ << estimate->parameter() << " " << REPORT_R_LIST << "\n";
 //   cache_ << "label: " << estimate->label() << "\n";
    cache_ << "lower_bound: " << estimate->lower_bound() << "\n";
    cache_ << "upper_bound: " << estimate->upper_bound() << "\n";
    cache_ << "value: " << AS_DOUBLE(estimate->value()) << "\n";
    map<string, ParameterPtr> parameters = estimate->parameters().parameters();
    for (auto iter = parameters.begin(); iter != parameters.end(); ++iter) {
      cache_ << iter->first << ": ";
      for (string parameter_value : iter->second->values())
        cache_ << parameter_value << " ";
      cache_ << "\n";
    }
    cache_ << REPORT_R_LIST_END << "\n\n";
  }


  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */

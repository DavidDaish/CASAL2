/**
 * @file Selectivity.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Selectivity.h"

#include "Model/Model.h"
#include "AgeLengths/AgeLength.h"

#include <boost/math/distributions/normal.hpp>

// Namesapces
namespace niwa {

/**
 * Explicit Constructor
 */
Selectivity::Selectivity(Model* model)
: model_(model) {

  parameters_.Bind<string>(PARAM_LABEL, &label_, "The label for this selectivity", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "The type of selectivity", "");
  parameters_.Bind<bool>(PARAM_LENGTH_BASED, &length_based_, "Is the selectivity length based", "", false);
  parameters_.Bind<unsigned>(PARAM_INTERVALS, &n_quant_, "Number of quantiles to evaluate a length based selectivity over the age length distribution", "", 5);
  parameters_.Bind<string>(PARAM_PARTITION_TYPE, &partition_type_label_, "The type of partition this selectivity will support, Defaults to same as the model", "", PARAM_MODEL)
      ->set_allowed_values({PARAM_MODEL, PARAM_AGE, PARAM_LENGTH, PARAM_HYBRID});

  RegisterAsAddressable(PARAM_VALUES, &values_, addressable::kLookup);
  RegisterAsAddressable(PARAM_LENGTH_VALUES, &length_values_, addressable::kLookup);
}

/**
 *
 */
void Selectivity::Validate() {
  parameters_.Populate(model_);

  if (partition_type_label_ == PARAM_MODEL)
    partition_type_ = model_->partition_type();
  else if (partition_type_label_ == PARAM_AGE)
    partition_type_ = PartitionType::kAge;
  else if (partition_type_label_ == PARAM_LENGTH)
    partition_type_ = PartitionType::kLength;
  else {
    LOG_CODE_ERROR() << "Selectivity does not recognise the current partition_type. It's not length or age";
  }
  age_index_ = model_->min_age();

  DoValidate();

  if (length_based_) {
    boost::math::normal dist{ };

    for (unsigned i = 1; i <= n_quant_; ++i) {
      quantiles_.push_back((Double(i) - 0.5) / Double(n_quant_));
      LOG_FINEST() << ": Quantile value = " << quantiles_[i - 1];
      quantiles_at_.push_back(quantile(dist, AS_DOUBLE(quantiles_[i - 1])));
      LOG_FINEST() << ": Normal quantile value = " << quantiles_at_[i - 1];
    }
  }

  if (model_->partition_type() == PartitionType::kAge) {
    values_.assign(model_->age_spread(), 0.0);
  } else {
    length_values_.assign(model_->length_bins().size(), 0.0);
  }
}


/**
 *
 */
void Selectivity::Reset() {
  if (is_estimated_) {
    RebuildCache();
  }
}

/**
 * Return the cached value for the specified age or length from
 * our internal map
 *
 * @param age_or_length The age or length to get selectivity value for
 * @return The value stored in the map or 0.0 as default
 */

Double Selectivity::GetAgeResult(unsigned age, AgeLength* age_length) {
  if (!length_based_) {
    if (age - age_index_ >= values_.size())
      LOG_CODE_ERROR() << "if (age - age_index_ >= values_.size())";
    if (age < age_index_)
      LOG_CODE_ERROR() << "if (age < age_index)";
    return values_[age - age_index_];
  }

  return GetLengthBasedResult(age, age_length);
}

/**
 *
 */
Double Selectivity::GetLengthResult(unsigned length_bin_index) {
  return length_values_[length_bin_index];
}



} /* namespace niwa */

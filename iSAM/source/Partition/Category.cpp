/**
 * @file Category.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 */

// headers
#include "Category.h"

#include "Categories/Categories.h"
#include "Model/Model.h"

// namespaces
namespace niwa {
namespace partition {

/**
 * This method will populate the length data on the partition category object.
 * This is required to be called by any object that wants to update the length
 * data from the age data.
 */
void Category::UpdateMeanLengthData() {
  if (!age_length_)
    return;

  unsigned year = Model::Instance()->current_year();

  for (unsigned age = min_age_; age <= max_age_; ++age)
    length_per_[age] = age_length_->mean_length(year, age);
}

/**
 * This method will populate the age data from the length data. This is required
 * to transfer any changes in the length partition back to the age partition.
 */
void Category::ConvertLengthDataToAge() {
  data_.clear();

  for (auto iter : length_data_) {
    Double total = 0;
    for (auto x : iter.second)
      total += x.second;
    data_.push_back(total);
  }
}

/**
 *
 */
void Category::ConvertAgeDataToLength() {
  CategoriesPtr categories = Categories::Instance();
  if (!categories->HasAgeLengthKeys())
    return;

  length_data_.clear();
  categories->age_length_key(name_)->DoAgeToLengthConversion(shared_from_this());
}


/**
 * This method will update the weight data with the number of fish and weight
 * per fish for use.
 */

void Category::UpdateMeanWeightData() {
  if (!age_length_)
    return;

  unsigned year = Model::Instance()->current_year();
  age_length_->BuildCv(year);
  for (unsigned age = min_age_; age <= max_age_; ++age)
  weight_per_[age] = age_length_->mean_weight(year, age);
}

} /* namespace partitions */
} /* namespace niwa */

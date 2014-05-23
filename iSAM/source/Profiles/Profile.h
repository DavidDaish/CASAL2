/**
 * @file Profile.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * A profile is a form of estimate. When you define a profile
 * you define the usual lower and upper bounds as well as
 * a steps parameter. The steps parameter tells isam how many
 * steps it must make between the lower and upper bounds incrementing
 * the estimate value each time and then doing another minimisation.
 */
#ifndef PROFILE_H_
#define PROFILE_H_

// headers
#include <boost/shared_ptr.hpp>

#include "BaseClasses/Object.h"
#include "Estimates/Estimate.h"

// namespaces
namespace isam {

/**
 * class definition
 */
class Profile : public isam::base::Object {
public:
  // methods
  Profile();
  virtual                     ~Profile() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { };
  void                        FirstStep();
  void                        NextStep();
  void                        RestoreOriginalValue();

  // accessors
  string                      parameter() const { return parameter_; }
  unsigned                    steps() const { return steps_; }
  double                      value() const { return *target_; }

private:
  // members
  unsigned                    steps_;
  double                      lower_bound_;
  double                      upper_bound_;
  string                      parameter_;
  double                      step_size_;
  double*                     target_;
  double                      original_value_;
};

/**
 * Typedef
 */
typedef boost::shared_ptr<Profile> ProfilePtr;

} /* namespace isam */
#endif /* PROFILE_H_ */
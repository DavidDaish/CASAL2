/**
 * @file ObjectiveFunction.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The objective function is the score for the model. Each model
 * run is capable of generating a score, the lower the score
 * the better the run.
 *
 * The objective function is used when minimising the model to
 * try and find a set of parameters that fit the best.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBJECTIVEFUNCTION_H_
#define OBJECTIVEFUNCTION_H_

// Headers
#include <string>
#include <vector>

#include "Utilities/Types.h"

// Namespaces
namespace isam {

using std::string;
using std::pair;
using std::vector;
using isam::utilities::Double;

/**
 * Struct definition
 */
namespace objective {
struct Score {
  string label_;
  Double score_;
};
}


/**
 * Class definition
 */
class ObjectiveFunction {
public:
  // Methods
  ObjectiveFunction();
  virtual                     ~ObjectiveFunction() = default;
  static ObjectiveFunction&   Instance();
  void                        CalculateScore();

  // Accessors
  const vector<objective::Score>& score_list() const { return score_list_; }
  Double                          score() const { return score_; }
  double                          penalties() const { return penalties_; }
  double                          priors() const { return priors_; }
  double                          likelihoods() const { return likelihoods_; }

private:
  // Members
  Double                      score_        = 0.0;
  double                      penalties_    = 0.0;
  double                      priors_       = 0.0;
  double                      likelihoods_  = 0.0;
  vector<objective::Score>    score_list_;
};

} /* namespace isam */
#endif /* OBJECTIVEFUNCTION_H_ */
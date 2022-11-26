/**
 * \file rng.cpp
 *   \author Marcus Ritt <mrpritt@inf.ufrgs.br> 
 *   \version $Id: emacs 6578 2016-04-03 22:33:03Z ritt $
 *   \date Time-stamp: <2016-05-25 12:47:34 ritt>
 */
#include "rng.hpp"

using namespace std;

mt19937 rng;
uniform_real_distribution<double> random01(0.0,1.0);
uniform_int_distribution<> randomInt;

namespace lehmer {
  // lehmer-like generator, taken from Palubeckis code
  // for reproduction and instance generation only
  double random(double *seed, double coef) {
    double rd, rf;
    rd = 16807 * (*seed);
    rf = floor(rd / coef);
    *seed = rd - rf * coef;
    return (*seed / (coef + 1));
  }

  // dito using the fixed coefficient of Palubeckis
  double random(double *seed) {
    double coef = 2048;
    coef *= 1024;
    coef *= 1024;
    coef -= 1;
    return random(seed,coef);
  }
}


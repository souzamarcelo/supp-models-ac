/**
 * \file rng.hpp
 *   \author Marcus Ritt <mrpritt@inf.ufrgs.br> 
 *   \version $Id: emacs 6578 2016-04-03 22:33:03Z ritt $
 *   \date Time-stamp: <2016-05-25 12:47:42 ritt>
 *
 * Random numbers
 */
#pragma once

#include <random>

extern std::mt19937 rng;
extern std::uniform_real_distribution<double> random01;
extern std::uniform_int_distribution<> randomInt;

namespace lehmer {
  // lehmer-like generator, taken from Palubeckis code
  // for reproduction and instance generation only
  double random(double *seed, double coef);

  // dito using the fixed coefficient of Palubeckis
  double random(double *seed);
}


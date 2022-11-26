/**
 * \file qbp.cpp
 *   \author Marcus Ritt <mrpritt@inf.ufrgs.br>
 *   \version $Id: bqp.cpp 6915 2016-05-29 00:50:08Z ritt $
 *   \date Time-stamp: <2016-05-28 21:49:37 ritt>
 *
 * Simple local searches & randomized search for BQP.
 *
 * Exercise 4, list 1, 2014/1.
 * Exercise 2, list 2, 2014/1.
 */

#include "bqp.hpp"

bool verbose;
bool record;
unsigned timelimit;

std::discrete_distribution<> rank_proportional; // rank-proportional distribution over variables


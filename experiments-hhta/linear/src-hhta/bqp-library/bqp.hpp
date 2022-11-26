/**
 * \file bqp.hpp
 *   \author Marcus Ritt <mrpritt@inf.ufrgs.br> 
 *   \version $Id: emacs 6578 2016-04-03 22:33:03Z ritt $
 *   \date Time-stamp: <2016-06-07 11:04:41 ritt>
 *
 * Global for BQP solvers
 */
#pragma once

#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>

extern bool verbose;
extern bool record;
extern unsigned timelimit;

// global variables; TBD: must be setup, currently done in bqpsolve.cpp/main
extern std::discrete_distribution<> rank_proportional; // rank-proportional distribution over variables

inline bool termination(int bestValue, int worstValue, unsigned steps, int target, std::chrono::system_clock::time_point start, std::chrono::system_clock::time_point& last_report) {
  if (std::chrono::system_clock::now()-last_report>std::chrono::duration<int>(3)) {
    last_report = std::chrono::system_clock::now();
    if (verbose)
      std::cerr << "RC" << std::setw(7) << steps << " " << std::setw(3) << std::chrono::duration_cast<std::chrono::seconds>(last_report-start).count() << " " << std::setw(7) << bestValue << " " << std::setw(7) << worstValue << std::endl;
  }
  
  if (bestValue <= target)
    return true;
  
  if (std::chrono::system_clock::now()-start>std::chrono::duration<int>(timelimit))
    return true;
  return false;
}


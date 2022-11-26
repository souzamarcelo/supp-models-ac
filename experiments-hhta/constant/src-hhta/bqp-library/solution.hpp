/**
 * \file solution.hpp
 *   \author Marcus Ritt <mrpritt@inf.ufrgs.br>
 *   \version $Id: emacs 6578 2016-04-03 22:33:03Z ritt $
 *   \date Time-stamp: <2016-06-01 11:26:28 ritt>
 *
 * Represent a solution for the BQP.
 */
#pragma once

#include <chrono>

// solution, variable indices 0-based
struct Solution {
  const Instance& I; // corresponding instance
  vector<char> x;    // x-variables
  double value;         // value
  vector<double> d;     // deltas
  std::chrono::system_clock::time_point time; // time found

  // data fro diversity-driven search
  unsigned ones;
  vector<unsigned> freq; // number of flips; TBD: make optional?

  // data for recombiner
  bool novel;

  Solution(const Instance& _I) : I(_I), x(I.n), value(0), d(I.n), ones(0), freq(I.n,0), novel(true) {
    // generate random solution
    for(char& c : x)
      c = 0;

    //Fill T desks with null color
    for(int i = 0; i < I.uncolored; i++) {
      unsigned desk = randomInt(rng) % I.desks;
      unsigned index = I.colors * desk;
      if(!x[index]) {
        x[index] = 1;
      } else {
        i--;
      }
    }

    //Fill each table with exactly one color
    for(int i = 0; i < I.desks; i++) {
      unsigned start = I.colors * i;
      if(!x[start]) {
        unsigned index = 0;
        while(index == 0) {
          index = randomInt(rng) % I.colors;
        }
        index += start;
        x[index] = 1;
      }
    }

    evaluate();
  }

  Solution(const Instance& _I, string s) : I(_I), x(I.n), value(0), d(I.n), ones(0), freq(I.n,0), novel(true) {
    // generate random solution
    assert(x.size()==I.n);
    for(unsigned i=0; i<x.size(); i++)
      x[i]=(s[i]=='1'?1:0);

    evaluate();
  }

  Solution& operator=(const Solution& other) {
    x = other.x;
    value = other.value;
    d = other.d;
    time = other.time;
    ones = other.ones;
    freq = other.freq;
    novel = other.novel;
    return *this;
  }

  bool operator==(const Solution& other) {
    return x == other.x;
  }

  // full evalation in O(n^2)
  void evaluate() {
    value = 0;
    ones = 0;
    for(unsigned i=0; i<x.size(); i++) {
      d[i]=0;
      if (x[i])
	ones++;
      for(unsigned j=0; j<x.size(); j++) {
	if (j==i)
	  d[i] += I[i][i];
	else
	  d[i] += 2*I[i][j]*x[j];
	value += I[i][j]*x[i]*x[j];
      }
      d[i] *= (1-2*x[i]);
    }
  }

  // get delta & update delta's in O(n)
  double adelta(unsigned i) {
    double result = d[i];
    const int sign = x[i]?-1:1;
    const unsigned n = x.size();

    // TDB: could go over only the non-zero entries
    for(unsigned j=0; j<n; j++)
      if (j != i)
	d[j] += 2*I[j][i]*sign*(1-2*x[j]);
      else
	d[j] = -d[j];
    return result;
  }

  double delta(unsigned i) const {
    return d[i];
  }

  // update: set a variable in O(n)
  void set(unsigned i) {
    if (x[i])
      return;
    value += adelta(i);
    x[i]=1;    
    ones++;
    freq[i]++;
  }

  // update: unset a variable in O(n)
  void reset(unsigned i) {
    if (!x[i])
      return;
    value += adelta(i);
    x[i]=0;
    ones--;
    freq[i]++;
  }

  void flip(unsigned i) {
    if (x[i])
      reset(i);
    else
      set(i);
  }

  void swap(unsigned i, unsigned j) {
    flip(i);
    flip(j);
  }

  double flipvalue(unsigned i) const {
    return delta(i);
  }

  double swapvalue(unsigned i, unsigned j) {
    //double v = delta(i) + delta(j);
    double v = adelta(i);
    x[i] = !x[i];
    v += delta(j);
    adelta(i);
    x[i] = !x[i];
    return v;
  }

  unsigned distance(const Solution& other) const {
    unsigned result = 0;
    for(unsigned i=0; i<x.size(); i++)
      if (x[i]!=other.x[i])
	result++;
    return result;
  }
};

// a move is simply an integer `i`; by convention i==n is an invalid move
bool isValid(unsigned i, const Solution& S) {
  return i<S.x.size();
}

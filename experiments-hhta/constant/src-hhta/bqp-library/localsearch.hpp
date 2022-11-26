/**
* \file localsearch.hpp
*   \author Marcus Ritt <mrpritt@inf.ufrgs.br>
*   \version $Id: emacs 6578 2016-04-03 22:33:03Z ritt $
*   \date Time-stamp: <2016-11-23 21:01:53 ritt>
*
* Simple local search strategies for BQP.
*/
#pragma once

#include "rng.hpp"
#include "bqp.hpp"

// fi
unsigned fi(const Solution& S) {
  for(unsigned i=0; i<S.x.size(); i++)
    if (S.flipvalue(i)<0)
      return i;
  return S.x.size();
}

struct FiRR {
  unsigned i;
  FiRR() : i(0) {}

  unsigned operator()(const Solution& S) {
    for(unsigned j = i; j < S.x.size(); j++) {
      if (S.flipvalue(j) < 0) {
        i = (i+1) % S.x.size();
        return j;
      }
    }

    for(unsigned j = 0; j < i; j++) {
      if (S.flipvalue(j) < 0) {
        i = (i+1) % S.x.size();
        return j;
      }
    }

    i = (i+1) % S.x.size();
    return S.x.size();
  }
};

// bi with reservoir sampling
unsigned bi(const Solution& S) {
  pair<int,unsigned> best = make_pair(0,S.x.size()); // delta, index
  unsigned nbest = 0;
  for(unsigned i=0; i<S.x.size(); i++)
    if (S.flipvalue(i)<get<0>(best)) {
      best = make_pair(S.flipvalue(i),i);
      nbest = 1;
    } else if (get<1>(best) != S.x.size() && S.flipvalue(i)==get<0>(best)) {
      nbest++;
      if ((nbest)*random01(rng)<=1.0)
      best = make_pair(S.flipvalue(i),i);
    }
  return get<1>(best);
}

// some improvement w/ reservoir sampling
unsigned si(const Solution& S) {
  unsigned imp = S.x.size();
  unsigned nimp = 0;
  for(unsigned j = 0; j < S.x.size(); j++)
  if (S.flipvalue(j) < 0) {
    nimp++;
    if (nimp * random01(rng) <= 1)
      imp = j;
  }
  return imp;
}

struct SiPartial {
  unsigned i;
  bool rr;
  double partial_size;
  SiPartial(bool _rr, double _partial_size) : i(0), rr(_rr), partial_size(_partial_size) {}

  unsigned operator()(const Solution& S) {
    unsigned imp = S.x.size(), emp = S.x.size();
    unsigned nimp = 0, nemp = 0;
    int nt = (S.x.size() + 9) * (partial_size / 100.0);
    unsigned tt = S.x.size();
    while (nt-->0 || (tt-->0 && nimp==0)) { //  && nemp==0
      if (S.flipvalue(i)<0) {
        nimp++;
        if (nimp*random01(rng)<=1)
          imp = i;
      }
      if (S.flipvalue(i)<=0) {
        nemp++;
        if (nemp*random01(rng)<=1)
          emp = i;
      }
      i = (i + 1) % S.x.size();
    }

    if(!rr) i = 0;
    return imp;
  }
};

unsigned ri(const Solution& S) {
  vector<unsigned> positions;
  for(unsigned i = 0; i < S.x.size(); i++)
    positions.push_back(i);

  while(positions.size() > 0) {
    int test = uniform_int_distribution<>(0, positions.size() - 1)(rng);
    if(S.flipvalue(positions[test]) < 0) {
      return positions[test];
    } else {
      swap(positions[test],positions.back());
      positions.pop_back();
    }
  }

  return S.x.size();
}

// random iterative improvements
struct RII {
  double p;
  RII(double _p) : p(_p) {}

  // basic variant
  template<typename Improvement>
  unsigned blmr(const Solution& S, Improvement improve) {
    if (random01(rng) < p)
    return randomInt(rng) % S.x.size();
    else {
      //unsigned r = si(S);
      unsigned r = improve(S);
      if (isValid(r,S))
        return r;
      else
        return randomInt(rng) % S.x.size();
    }
  }
};

// simple local search
//   `improve` is a one-step improvement method
template<typename Improvement>
unsigned localsearch(Solution& S, Improvement improve, chrono::system_clock::time_point start, int target) {
  // local searches
  chrono::system_clock::time_point last_report;
  unsigned i, steps = 0;
  Solution B(S.I);
  B = S;

  do {
    if (chrono::system_clock::now()-last_report>chrono::duration<int>(3)) {
      last_report = chrono::system_clock::now();
      if (verbose)
      cerr << " " << setw(7) << steps << " " << setw(3) << chrono::duration_cast<chrono::seconds>(last_report-start).count() << " " << setw(7) << S.value << " " << setw(7) << B.value << endl;
    }
    if (S.value <= target)
    break;
    if (chrono::system_clock::now()-start>chrono::duration<int>(timelimit))
    break;
    steps++;
    i = improve(S);
    if (!isValid(i,S))
    break;
    S.flip(i);
    if (S.value < B.value) {
      B = S;
      B.time = chrono::system_clock::now();
      if (record)
      cerr << " " << setw(7) << steps << " " << setw(6) << chrono::duration_cast<chrono::milliseconds>(B.time-start).count() << " " << setw(7) << S.value << " " << setw(7) << B.value << endl;
    }
  } while (true);
  steps--;
  S = B;
  return steps;
}

// generator for the perturbation strength in iterated local searches
// TBD: solve via `bind`
struct StepGenerator {
  unsigned minv, maxv;

  StepGenerator(unsigned _minv, unsigned _maxv) : minv(_minv), maxv(_maxv) {}

  unsigned operator()() {
    unsigned v = uniform_int_distribution<unsigned>(minv,maxv)(rng);
    return v;
  }
};

struct StepGaussian {
  unsigned minv, maxv, n;
  double mean, sigma;

  StepGaussian(unsigned _minv, unsigned _maxv, unsigned _n) : minv(_minv), maxv(_maxv), n(_n) {
    mean = double(maxv+minv)/2;
    sigma = double(maxv-minv);
  }

  unsigned operator()() {
    double v = normal_distribution<double>(mean,sigma)(rng);
    return v<minv?minv:(v>n?n:v);
  }
};

struct StepExponential {
  unsigned minv, maxv, n;
  double lambda;

  StepExponential(unsigned _minv, unsigned _maxv, unsigned _n) : minv(_minv), maxv(_maxv), n(_n) {
    lambda = 1.0/(double(maxv+minv)/2);
  }

  unsigned operator()() {
    double v = exponential_distribution<double>(lambda)(rng);
    return v<minv?minv:(v>n?n:v);
  }
};

// perturbation according to Glover et al. (2010)
template<typename StepGenerator, typename Elite>
void perturbDiversity(Solution& S, StepGenerator s, const Elite& e, double beta) {
  const unsigned n = S.x.size();

  // (1) compute the score of the variables
  vector<double> score(n,0.0);
  unsigned max_freq = *max_element(S.freq.begin(),S.freq.end());
  for(unsigned i=0; i<n; i++)
  score[i]=double(e.freq[i]*(e.size()-e.freq[i]))/double(e.size()*e.size())+beta*(1-double(S.freq[i])/double(max_freq));

  // (2) rank them
  vector<unsigned> var(n);
  iota(var.begin(),var.end(),0);
  sort(var.begin(),var.end(),[&score](unsigned i, unsigned j) { return score[i]>score[j]; });
  // (3) select lambda different variables
  vector<bool> flipped(n,false);
  unsigned gamma = s();
  if (gamma >= (n / 2)) gamma = (n / 2); //If gamma value is near n value, algorithm turn very slow. I limit gamma to n/2.
  while (gamma>0) {
    unsigned j = rank_proportional(rng);
    if (!flipped[var[j]]) {
      flipped[var[j]]=true;
      S.flip(var[j]);
      gamma--;
    }
  }
}

// random perturbation
template<class StepGenerator>
void perturbRandom(Solution& S, StepGenerator s, unsigned b) {
  Solution R(S.I);
  S = R;
}

// perturbation according to Palubeckis (2006).
//   `r` times flip a random variable of the `b`
//   variables of least loss.
template<class StepGenerator>
void perturbLeastLoss(Solution& S, StepGenerator s, unsigned b) {
  vector<pair<unsigned,int>> cand; // candidates,  in order of increasing value
  cand.reserve(b);
  vector<bool> flipped(S.x.size(),false);

  unsigned r = s();
  if(r > S.x.size()) {
    r = S.x.size();
  }

  while (r-->0) {
    // (1) generate the candidate list
    cand.clear();
    for(unsigned i=0; i<S.x.size(); i++) {
      if(!flipped[i] && (cand.size()<b || S.flipvalue(i)<cand.back().second)) {
        // (1.1) insert i into the candidate list
        auto j = lower_bound(cand.begin(),cand.end(),S.flipvalue(i),[](const pair<unsigned,int>& a, int b) { return a.second<b; });
        cand.insert(j,make_pair(i,S.flipvalue(i)));
        if(cand.size()>b) {
          cand.pop_back();
        }
      }
    }
    
    // (2) select a candidate
    unsigned i = cand[randomInt(rng)%cand.size()].first;
    
    // (3) apply the perturbation
    S.flip(i);
    flipped[i] = true;
  }
}

// iterated local search
//   `improve` is a multi-step improvement method (usually some local search returning a local minimum)
//   `perturb` implements some kind of perturbation
template<typename Improvement, typename Perturbation>
unsigned iterated_localsearch(Solution& S, Improvement improve, Perturbation perturb, chrono::system_clock::time_point start, int target) {
  chrono::system_clock::time_point last_report;
  unsigned steps = 0;
  Solution B(S.I);
  B = S;

  do {
    // (1) report progress
    if (chrono::system_clock::now()-last_report>chrono::duration<int>(3)) {
      last_report = chrono::system_clock::now();
      if (verbose)
      cerr << "I" << setw(7) << steps << " " << setw(3) << chrono::duration_cast<chrono::seconds>(last_report-start).count() << " " << setw(7) << S.value << " " << setw(7) << B.value << " " << setw(4) << S.ones << endl;
    }
    // (2) check stopping conditions
    if (S.value <= target)
    break;
    if (chrono::system_clock::now()-start>chrono::duration<int>(timelimit))
    break;

    // (3) next iteration: perturb & improve
    steps++;
    perturb(S);
    improve(S);

    // (4) accept (unconditional acceptance for now)
    // TBD: implement a couple of acceptance rules (always,better,better_or_equal,metropolis)

    // (5) update incumbent
    if (S.value < B.value) {
      B = S;
      B.time = chrono::system_clock::now();
      if (record)
      cerr << " " << setw(7) << steps << " " << setw(6) << chrono::duration_cast<chrono::milliseconds>(B.time-start).count() << " " << setw(7) << S.value << " " << setw(7) << B.value << endl;
    }
  } while (true);
  steps--;
  S = B;
  return steps;
}

// iterated local search, with elite set: in each iteration a random
// solution from the elite set is the current solution
template<typename Improvement, typename Perturbation, typename Elite>
unsigned iterated_localsearch_elite(Solution& S, Improvement improve, Perturbation perturb, Elite e, chrono::system_clock::time_point start, int target) {
  chrono::system_clock::time_point last_report;
  unsigned steps = 0;

  // (1) construct elite set at start (NEW)
  for(unsigned i=0; i<e.maxsize; i++) {
    improve(S);
    e.add(S);
    if (termination(e[0].value,e.back().value,steps,target,start,last_report))
    goto done;
    S=Solution(S.I);
  }

  do {
    // (2) report progress
    if (chrono::system_clock::now()-last_report>chrono::duration<int>(3)) {
      last_report = chrono::system_clock::now();
      if (verbose)
      cerr << "I " << setw(7) << steps << " " << setw(3) << chrono::duration_cast<chrono::seconds>(last_report-start).count() << " " << setw(7) << e[0].value << " " << setw(7) << e.back().value << " " << setw(2) << e.size() << endl;
    }

    // (3) check stopping conditions
    if (termination(e[0].value,e.back().value,steps,target,start,last_report))
    break;

    // (4) next iteration: perturb & improve
    steps++;
    S = e.getRandom();
    perturb(S);
    improve(S);

    // (5) accept (unconditional acceptance for now)
    // TBD: implement a couple of acceptance rules (always,better,better_or_equal,metropolis)

    // (6) update incumbent
    if (S.value < e[0].value) {
      S.time = chrono::system_clock::now();
      if (record)
      cerr << " " << setw(7) << steps << " " << setw(6) << chrono::duration_cast<chrono::milliseconds>(S.time-start).count() << " " << setw(7) << S.value << " " << setw(7) << S.value << endl;
    }
    e.add(S);
  } while (true);
  done:
  S = e[0];
  return steps;
}

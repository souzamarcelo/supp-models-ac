#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <unordered_set>
#include <tuple>
#include <chrono>
#include <iterator>
#include <iomanip>
#include <boost/multi_array.hpp>
#include <boost/program_options.hpp>
#include <sys/ioctl.h>

namespace po = boost::program_options;
using namespace boost;
using namespace std;

#include "bqp-library/instance.hpp"
#include "bqp-library/solution.hpp"
#include "bqp-library/localsearch.hpp"
#include "bqp-library/rng.hpp"
#include "bqp-library/rng.cpp"
#include "bqp-library/elite.hpp"
#include "bqp-library/bqp.hpp"
#include "bqp-library/bqp.cpp"
#include "bqp-library/tabusearch.hpp"
#include "bqp-library/construct.hpp"
#include "bqp-library/recombine.hpp"
#include "instance_data.hpp"

Instance I;
chrono::system_clock::time_point start_time;
int target;
string instance_name;
string format;
bool maximize;

#include "algorithm.cpp"

unsigned setupRandom(unsigned seed) {
  if (seed == 0) {
    seed = time(0);
    ifstream f("/dev/urandom");
    if (f.good()) {
      f.read((char *)(&seed), sizeof(unsigned int));
    }
  }
  rng.seed(seed);
  srand48(seed);
  return seed;
}

int main(int argc, char *argv[]) {

  struct winsize wsize;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);

  po::options_description desc("General options",wsize.ws_col);
  desc.add_options()
    ("help", "show help")
    ("ins", po::value<string>(), "instance")
    ("seed", po::value<unsigned>()->default_value(0), "random seed")
    ("target", po::value<int>()->default_value(numeric_limits<int>::min()), "target value")
    ("timelimit", po::value<unsigned>(), "time limit (s)")
    ("verbose", po::value<bool>()->default_value(false), "verbose reporting")
    ("record", po::value<bool>()->default_value(false), "record improvements")
    ("format", po::value<string>()->default_value("sparse"), "instance format (sparse,dense,maxcut,pgen,lgen)")
    ("maximize", po::value<bool>()->default_value(false), "multiply weight by -1")
    ("wsolution", po::value<string>(), "Output file for best solution found")
    ("p", po::value<double>()->default_value(0.25), "probability for random move")
    ("tenure", po::value<string>()->default_value("10%"), "tabu tenure (# of variables or %), a=average deg, v=vertex degree, c=constant is n/cm")
    ("N", po::value<unsigned>()->default_value(100), "number of repetitions")
    ("alpha", po::value<double>()->default_value(0.5), "greediness alpha (0=random,1=greedy)")
    ("b", po::value<unsigned>()->default_value(5), "number of candidate variables for perturbation")
    ("d1", po::value<unsigned>()->default_value(10), "minimum number of perturbed variables")
    ("d2", po::value<unsigned>()->default_value(10), "d2^-1: maximum number of perturbed variables is max(d1,n/d2)")
    ("R", po::value<unsigned>()->default_value(8), "elite set size")
    ("ic", po::value<unsigned>()->default_value(20), "improvement cutoff of TS is ic*n")
    ("beta", po::value<double>()->default_value(0.3), "weight of frequency contribution to pertubation score")
    ("lambda", po::value<double>()->default_value(1.2), "perturbation selection importance factor")
    ("gammam", po::value<unsigned>()->default_value(4), "perturbation strength is n/gammam")
    ("bsize", po::value<unsigned>()->default_value(10), "reference set size")
    ("mum", po::value<unsigned>()->default_value(5), "improvement cutoff mu of TS is mum*n")
    ("gamma", po::value<double>()->default_value(0.333), "distance scale")
    ("maxsteps", po::value<string>()->default_value("i"), "max number of steps in tabu search (# of steps or option), a=according to instance size (20 * instance size), i=infinity")
    ("maxstagnate", po::value<string>()->default_value("i"), "max number of steps without improvements in tabu search (# of steps or option), a=according to instance size, i=infinity")
    ("param_tenure", po::value<unsigned>()->default_value(20), "TABU TENURE")
    ("param_e", po::value<unsigned>()->default_value(20), "ELITE SET SIZE")
    ("param_gamma", po::value<double>()->default_value(0.32), "DISTANCE FACTOR")
    ;

  po::positional_options_description pod;
  pod.add("ins", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
  po::notify(vm);

  if(vm.count("help") || !vm.count("ins")) {
    cout << desc << endl;
    return 0;
  }

  setupRandom(vm["seed"].as<unsigned>());
  instance_name = vm["ins"].as<string>();

  target = numeric_limits<int>::min();
  timelimit = 10;
  maximize = false;

  ifstream ins(instance_name);
  if (!ins.good()) {
    cout << "Can't open instance file " << instance_name << endl;
    return 1;
  }
  
  I.readInstance(ins);

  Solution S(I);

  start_time = chrono::system_clock::now();

  unsigned param_tenure = vm["param_tenure"].as<unsigned>();
  unsigned param_e = vm["param_e"].as<unsigned>();
  double param_gamma = vm["param_gamma"].as<double>();

  run(S, vm, start_time, param_tenure, param_e, param_gamma);

  S.value += (I.btb * I.P);
  cout << S.value << endl;

  /*
  for(int i = 0; i < S.x.size(); i++) {
    if(S.x[i]) cout << "1";
    else cout << "0";
  }
  cout << endl;
  */
}

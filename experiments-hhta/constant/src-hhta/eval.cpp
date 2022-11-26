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
#include "bqp-library/rng.hpp"
#include "bqp-library/rng.cpp"

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

string instance_name;
string format;
bool maximize = false;
string solution;

Instance I;

int main(int argc, char *argv[]) {

  struct winsize wsize;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
  
  po::options_description desc("General options",wsize.ws_col);
  desc.add_options()
    ("help", "show help")
    ("sol", po::value<string>(), "solution, format: 10110101011...")
    ("ins", po::value<string>(), "instance")
    ;
  
  po::positional_options_description pod;
  pod.add("ins", 1);
    
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
  po::notify(vm);
    
  if(vm.count("help") || !vm.count("sol") || !vm.count("ins")) {
    cout << desc << endl;
    return 0;
  }

  instance_name = vm["ins"].as<string>();
  solution = vm["sol"].as<string>();

  if(instance_name.find("sparse") != std::string::npos)
    format = "sparse";
  if(instance_name.find("def") != std::string::npos)
    format = "pgen";

  ifstream ins(vm["ins"].as<string>());
  if (!ins.good()) {
    cout << "Can't open instance file " << vm["ins"].as<string>() << endl;
    return 1;
  }

  if (format=="sparse")
    I.readSparse(ins,maximize);
  else if (format=="dense")
    I.readDense(ins,maximize);
  else if (format=="pgen")
    I.readPalubeckis(ins,maximize,false);
  else if (format=="lgen")
    I.readPalubeckis(ins,maximize,true);
  else
    I.readMaxcut(ins);

  Solution S(I,solution);

  //S.evaluate();
  cout << S.value << endl;

}

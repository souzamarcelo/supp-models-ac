/*
@author: Tadeu Knewitz Zubaran tkzubaran@gmail.com
*/

#include "BubbleSettings.hpp"
#include "Instance.hpp"
#include "State.hpp"
#include "RunData.hpp"

int main(int argc, char** argv) {
	unsigned milisecsMultiplier = 30;
	string instance;
	unsigned pos = 0, seed = 0, alpha = 50;
	for (int i = 0; i < argc; ++i) {
		if (i < argc - 1) {
			string arg = argv[i];
			string nextArg = argv[i + 1];
			if (arg == "--instance") instance = nextArg;
			if (arg == "--pos") pos = stoi(nextArg);
			if (arg == "--seed") seed = stoi(nextArg);
			if (arg == "--alpha") alpha = stoi(nextArg);
		}
	}

	State::runSimple(instance, pos, seed, alpha, milisecsMultiplier);
}

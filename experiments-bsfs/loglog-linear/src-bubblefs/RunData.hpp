/*
@author: Tadeu Knewitz Zubaran tkzubaran@gmail.com
*/

#ifndef RUN_DATA_HPP
#define RUN_DATA_HPP

#include "BubbleSettings.hpp"
#include "State.hpp"



class RunData {
public:
	inline RunData(unsigned seed, unsigned intialMakespan) {
		bestMakespan = INF_TIME;
		bestDispatchOrder.clear();
		steps = 0;
		improvs = 0;
		this->seed = seed;
		this->intialMakespan = intialMakespan;
		bestPerTime.clear();
	}
	inline ~RunData() {  }
	
	inline void tookStep() { steps++; }
	inline void newBest(unsigned bestMakespan, const vector<unsigned> & bestDispatchOrder, unsigned seconds) {
		improvs++;
		this->bestMakespan = bestMakespan;
		this->bestDispatchOrder = bestDispatchOrder;
		if(bestPerTime.size() < MAX_HISTORY)
			bestPerTime.push_back(pair<unsigned, unsigned>(bestMakespan ,seconds));
	}
	
	inline string toString() const {
		string str = "";
		
		str.append("seed: "+unsignedToString(seed));
		str.append(" --- steps: "+unsignedToString(steps));
		str.append(" ; improvs: "+unsignedToString(improvs));
		str.append(" --- intial Makespan: "+unsignedToString(intialMakespan));
		str.append(" ; best Makespan: "+unsignedToString(bestMakespan)+"\n\t");
		
		for(pair<unsigned, unsigned> p : bestPerTime) {
			str.append("<"+unsignedToString(p.first) + " | "+unsignedToString(p.second)+">  ");
		}
		
		return str;
	}
	unsigned bestMakespan;
private:
	unsigned intialMakespan;	
	vector<unsigned> bestDispatchOrder;
	unsigned steps;
	unsigned improvs;
	unsigned seed;
	vector<pair<unsigned, unsigned>> bestPerTime;
};


#endif //RUN_DATA_HPP

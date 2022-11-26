/*
@author: Tadeu Knewitz Zubaran tkzubaran@gmail.com
*/

#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "BubbleSettings.hpp"


#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <boost/multi_array.hpp>
#pragma GCC diagnostic warning "-Wunused-variable"

class Instance {
public:
	inline unsigned getTotalJobs() const { return totalJobs; }
	inline unsigned getTotalMachines() const { return totalMachines; }
	inline Time getProcTime(unsigned jobIndex, unsigned machineIndex) const {
		assert(jobIndex != 0  &&  machineIndex != 0);//indices are 1 based
		return procTimes[jobIndex][machineIndex];
	}
	inline void setProcTime(const Time & t, unsigned jobIndex, unsigned machineIndex) { procTimes[jobIndex][machineIndex] = t; }
	
	inline Time getLowerBound() const { return lowerBound; }
	inline Time getUpperBound() const { return upperBound; }
	inline void setLowerBound(const Time & lowerBound) { this->lowerBound = lowerBound; }
	inline void setUpperBound(const Time & upperBound) { this->upperBound = upperBound; }
	
	inline bool isInitialized() const { return totalJobs != 0   &&   totalMachines != 0; }
	
	inline Instance operator=(const Instance & rightFsi) {
		this->totalJobs = rightFsi.totalJobs;
		this->totalMachines = rightFsi.totalMachines;
		this->procTimes.resize(boost::extents[totalJobs+1][totalMachines+1]);
		this->procTimes = rightFsi.procTimes;
		this->upperBound = rightFsi.upperBound;
		this->lowerBound = rightFsi.lowerBound;
		this->C0 = rightFsi.C0;
		
		return *this;
	}

	inline Instance(const Instance & fsi) {
		this->totalJobs = fsi.totalJobs;
		this->totalMachines = fsi.totalMachines;
		this->procTimes.resize(boost::extents[totalJobs+1][totalMachines+1]);
		this->procTimes = fsi.procTimes;
		this->upperBound = fsi.upperBound;
		this->lowerBound = fsi.lowerBound;
		this->C0 = fsi.C0;
	}

	inline Instance(unsigned _totalJobs=0, unsigned _totalMachines=0) : totalJobs(_totalJobs), totalMachines(_totalMachines) {
		procTimes.resize(boost::extents[totalJobs+1][totalMachines+1]);//+1 cause indices are 1 based
	}
	
	inline Instance(istream & streamFromFile) {
		streamFromFile >> totalJobs;
		streamFromFile >> totalMachines;
		
		procTimes.resize(boost::extents[totalJobs+1][totalMachines+1]);//+1 cause indices are 1 based
		unsigned dummy;
		
		for(unsigned jobLine=0; jobLine<totalJobs; jobLine++) {
			for(unsigned machColumn=0; machColumn<totalMachines; machColumn++) {
				dummy = getTime(streamFromFile);
				if(dummy != machColumn) 
					throw errorText("Expected machine index, in order, before time. Should have: "+unsignedToString(machColumn)+" but was: "+ unsignedToString(dummy), "instance.hpp", "Instance(istream & streamFromFile)");
				procTimes[jobLine+1][machColumn+1] = getTime(streamFromFile);
			}
		}
		// setup C0
		C0 = 0.0;
		for(unsigned jobLine=0; jobLine<totalJobs; jobLine++) 
		  for(unsigned machColumn=0; machColumn<totalMachines; machColumn++) 
		    C0 += procTimes[jobLine+1][machColumn+1];
		C0 /= 10*totalJobs*totalMachines;
		cout << "Instance with " << totalJobs << " jobs and " << totalMachines << " machines has a C0 of " << C0 << endl;
		C0 *= 0.5;
	}
	
	//XXX if use more than once make it a field
	// indices are 1 based, 0 is dummy
	inline vector<Time> computePeriods() const {
		assert(isInitialized());
		vector<Time> periods(getTotalJobs()+1);
		
		for(unsigned jobIndex=0; jobIndex<getTotalJobs()+1; jobIndex++) {
			periods[jobIndex] = 0;
			for(unsigned machineIndex=0; machineIndex<getTotalMachines()+1; machineIndex++) {
				periods[jobIndex] += getProcTime(jobIndex, machineIndex);
			}
		}
		
		assert(periods[0] == 0);
		return periods;
	}
	
	inline static vector<Instance> parseTailard(const string & filePath) {
		
		vector<Instance> fsiVec;
		string bufferStr;
		ifstream streamFromFile;
		streamFromFile.open(filePath);
		if( ! streamFromFile.is_open())
			throw errorText("Could not open instance file.", "instance.hpp", "Instance::parseTailard()");
		
		while(true) {
			streamFromFile >> bufferStr;
			
			if(bufferStr.compare("number") != 0) {
				break;
			}
			
			// of jobs, number of machines, initial seed, upper bound and lower bound :
			for(unsigned u=0 ; u<13; u++)
				streamFromFile >> bufferStr;
			
			if(bufferStr.compare(":") != 0)
				throw errorText("Unexpected syntax, expected : but got <"+bufferStr+">", "instance.hpp", "Instance::parseTailard()");
			
			fsiVec.push_back(parseOneTailard(streamFromFile));
		}
		return fsiVec;
	}
	
	inline static Instance parseOneTailard(ifstream & streamFromFile) {
		Instance fsi;
		string bufferStr;
		
		streamFromFile >> fsi.totalJobs;
		streamFromFile >> fsi.totalMachines;
		streamFromFile >> bufferStr;
		streamFromFile >> fsi.lowerBound;
		streamFromFile >> fsi.upperBound;
		
		//processing times :
		streamFromFile >> bufferStr;
		streamFromFile >> bufferStr;
		streamFromFile >> bufferStr;
		
		fsi.procTimes.resize(boost::extents[fsi.totalJobs+1][fsi.totalMachines+1]);
		for(unsigned machineLine=1; machineLine<fsi.totalMachines+1; machineLine++) {
			for(unsigned jobColumn=1; jobColumn<fsi.totalJobs+1; jobColumn++) {
				fsi.setProcTime(getTime(streamFromFile), jobColumn, machineLine);
			}
		}
		
		fsi.C0 = 0.0;
		for(unsigned jobLine=0; jobLine<fsi.totalJobs; jobLine++) 
		  for(unsigned machColumn=0; machColumn<fsi.totalMachines; machColumn++) 
		    fsi.C0 += fsi.procTimes[jobLine+1][machColumn+1];
		fsi.C0 /= 10*fsi.totalJobs*fsi.totalMachines;
		fsi.C0 *= 0.5;
		//cout << "Instance with " << fsi.totalJobs << " jobs and " << fsi.totalMachines << " machines has a C0 of " << fsi.C0 << endl;
		return fsi;
	}
	
	inline string toString() const {
		string str = "";
		
		str.append("totalJobs: " + unsignedToString(getTotalJobs()));
		str.append(" ; totalMachines: " + unsignedToString(getTotalMachines()));
		str.append(" ; lowerBound: " + unsignedToString(getLowerBound()));
		str.append(" ; upperBound: " + unsignedToString(getUpperBound())+"\n");
		for(unsigned machIndex=1; machIndex<=getTotalMachines(); machIndex++) {
			for(unsigned jobIndex=1; jobIndex<=getTotalJobs() ; jobIndex++) {
				str.append(unsignedToString(getProcTime(jobIndex, machIndex))+"\t");
			}
			str.append("\n");
		}
		
		return str;
	}
public:
  double C0;


private:
	unsigned totalJobs;
	unsigned totalMachines;
	boost::multi_array<Time, 2> procTimes; //(job x machine)
  // indices are 1-based, element 0 is a dummy
  
	Time upperBound;
	Time lowerBound;
  
  inline static Time getTime(istream & streamFromFile) {
		string token;
		streamFromFile >> token;
		if (token[0]=='i' || token[0]=='I')
			return INF_TIME;
		stringstream num(token);
		Time t;
		num >> t;
		return t;
	}
};
#endif //INSTANCE_H

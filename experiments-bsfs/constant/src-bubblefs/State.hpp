/*
@author: Tadeu Knewitz Zubaran tkzubaran@gmail.com
*/

#ifndef STATE_HPP
#define STATE_HPP

#include "BubbleSettings.hpp"
#include "Instance.hpp"
#include "RunData.hpp"
#include <list>

class State {
public:
	 State() {}
	 State(const Instance & _fsi) { setInstance(_fsi); }
	 ~State() {  }
	
	static void runSimple(string instanceName, unsigned instancePos, unsigned seed, double alpha, double milisecsMultiplier) {
		Instance instance = Instance::parseTailard(instanceName)[instancePos];
		double timelimit = (instance.getTotalMachines())*(instance.getTotalJobs())*milisecsMultiplier;
		timelimit = timelimit / 60000.0;
		RunData data = bubbleSearch(instance, timelimit, alpha, false, seed, false);
		cout << data.bestMakespan << endl;
	}

	 static void batchHistory(const string & instDir, const string & logDir, const vector<string> & fileNameVec, const vector<unsigned> & probVec, unsigned seed, unsigned reps, bool useErrors) {
	
		string anInstFilePath;
		string anInstHistStr;
		string anInstHistPath;
		
		vector<unsigned> aHistory;
	
		ofstream myFile;
	
		vector<Instance> instVec;
		
		for(const string & anInstName : fileNameVec) {
			anInstFilePath = instDir+anInstName+".txt";
			
			vector<Instance> instVec = Instance::parseTailard(anInstFilePath);
			
			for(unsigned aProb : probVec) {
				
				myFile.open(logDir+"/"+anInstName+"-HIST-prob("+unsignedToString(aProb)+").txt");
				
				aHistory = makeHistory(instVec[0], aProb, seed, reps, useErrors);
				
				myFile << "TotalMeasures: " + unsignedToString(aHistory.size())+"\n";
				for(unsigned u : aHistory) {
					myFile << u << " ";
				}
				
				myFile.close();
			}
		}
	}
	
	
	 static vector<unsigned> makeHistory(const Instance & fsi, unsigned probPercent, unsigned seed, unsigned reps, bool useErrors) {
		
		assert(fsi.isInitialized());
		assert(probPercent>50 && probPercent<100);
		
		srand(seed);
		
		vector<unsigned> history;
		
		setInstance(fsi);
		
		//XXX XXX XXX XXX XXX XXX XXX
		State pivotState;
		if( ! useErrors)
			pivotState = nehkk1(false, false);
		else
			pivotState = nehErrorError();
		
		State bufferState = pivotState;
		
		for(unsigned currRep=0; currRep<reps; currRep++) {
			bufferState.bubble(probPercent, pivotState);
			
			history.push_back(bufferState.getMakespan());
		}
		
		return history;
	}
	
	 static void adjustedBatch(const string & instDir, const string & logDir, const vector<string> & fileNameVec, const vector<unsigned> & seedVec, double coreAlpha, unsigned milisecsMultiplier) {
	
		verboseBatch("Begining batch.\n");
	
		string aTailFilePath;
		string anInstLogStr;
		string anInstLogName;
		
		unsigned computedProb;
		unsigned computedMilisecs;
		
		vector<RunData> runDataVec;
		
		vector<Instance> instVec;
		
		ofstream logFile;
		
		unsigned tailIndex;
		
		for(const string & anInstName : fileNameVec) {
			aTailFilePath = instDir+anInstName+".txt";
			verboseBatch("\nOpening" + aTailFilePath);
			
			vector<Instance> instVec = Instance::parseTailard(aTailFilePath);
			
			//time = (mn/2)60ms, assumes all instances in a file have the same number of machines/jobs
			computedMilisecs = (instVec[0].getTotalMachines())*(instVec[0].getTotalJobs())*milisecsMultiplier;
			computedProb = (unsigned)(100.0*pow(coreAlpha, 1/(double)(instVec[0].getTotalJobs())));
			verboseBatch("\t; running them with prob: "+unsignedToString(computedProb)+" ;  and time: "+unsignedToString(computedMilisecs)+"ms.");
			tailIndex = 0;
			for(const Instance & aInst : instVec) {
				
				anInstLogName = "Result-"+anInstName+"-"+unsignedToString(tailIndex)+".txt";
				verboseBatch("\n\tPreparing: "+anInstLogName);
				runDataVec.clear();
				
				logFile.open(logDir+anInstLogName);
				anInstLogStr="\n"+anInstLogName+" ; milisecs: "+unsignedToString(computedMilisecs)+" ; removalProb: "+unsignedToString(computedProb)+"% ; adapt";
				for(unsigned aSeed : seedVec) {
					//(const Instance & fsi, double allowedMinutes, unsigned probPercent, bool adapt, unsigned seed, bool useErrors)
					runDataVec.push_back(bubbleSearch(aInst, computedMilisecs/60000.0, computedProb, true, aSeed, false));
				}				
				anInstLogStr += makeInstLogStr(runDataVec);
				logFile << anInstLogStr;
				logFile.close();
				tailIndex++;
				//break;
			}
			
		}
	}
	
	
	
	//wideBatchVarAlpha("./TailInstances/", "./WideLogVAR_ALPHA/", fileNameVec, 0.000798, seedVec, 5.0, false)
	 static void wideBatchVarAlpha(const string & instDir, const string & logDir, const vector<string> & fileNameVec, double coreAlpha, const vector<unsigned> & seedVec, double allowedMinutes) {
		string anInstFilePath;
		string anInstLogStr;
		string anInstLogPath;
		
		double readyProb;
		
		vector<RunData> runDataVec;
		
		vector<Instance> instVec;
		
		ofstream myFile;
		
		for(const string & anInstName : fileNameVec) {
			anInstFilePath = instDir+anInstName+".txt";
			
			vector<Instance> instVec = Instance::parseTailard(anInstFilePath);
			readyProb = (unsigned)(100.0*pow(coreAlpha, 1/(double)(instVec[0].getTotalJobs())));
#ifdef VERBOSE_BATCH
			cout << "\n\n\nInstance with machines: "+unsignedToString(instVec[0].getTotalJobs())+" ; jobs: "+unsignedToString(instVec[0].getTotalMachines());
			cout << " ; removal prob: "+doubleToString(readyProb) + " ; adapt\n";
			//sleep(1);
#endif //VERBOSE_BATCH	
			runDataVec.clear();
			myFile.open(logDir+"/"+anInstName+"-Adapt-prob("+unsignedToString(0.0)+").txt");
			anInstLogStr="\n"+anInstName+" ; mins: "+doubleToString(allowedMinutes)+" ; removalProb: "+unsignedToString(readyProb)+"% ; adapt";
			for(unsigned aSeed : seedVec) {
			//bubbleSearch(const Instance & fsi, double allowedMinutes, unsigned probPercent, bool adapt, unsigned seed)
				runDataVec.push_back(bubbleSearch(instVec[0], allowedMinutes, readyProb, true, aSeed, false));
			}
			anInstLogStr += makeInstLogStr(runDataVec);
#ifdef VERBOSE_BATCH
			cout << anInstLogStr << endl;
#endif //VERBOSE_BATCH				
			myFile << anInstLogStr;
			myFile.close();

		}
		
		
		for(const string & anInstName : fileNameVec) {
			anInstFilePath = instDir+anInstName+".txt";
			
			vector<Instance> instVec = Instance::parseTailard(anInstFilePath);
			readyProb = (unsigned)(100.0*pow(coreAlpha, 1/(double)(instVec[0].getTotalJobs())));
#ifdef VERBOSE_BATCH
			cout << "\n\n\nInstance with machines: "+unsignedToString(instVec[0].getTotalJobs())+" ; jobs: "+unsignedToString(instVec[0].getTotalMachines());
			cout << " ; removal prob: "+doubleToString(readyProb) + " ; nonAdapt\n";
			//sleep(1);
#endif //VERBOSE_BATCH	
			runDataVec.clear();
			myFile.open(logDir+"/"+anInstName+"-NonAdapt-prob("+unsignedToString(0.0)+").txt");
			anInstLogStr="\n"+anInstName+" ; mins: "+doubleToString(allowedMinutes)+" ; removalProb: "+unsignedToString(readyProb)+"% ; nonAdapt";
			for(unsigned aSeed : seedVec) {
			//bubbleSearch(const Instance & fsi, double allowedMinutes, unsigned probPercent, bool adapt, unsigned seed)
				runDataVec.push_back(bubbleSearch(instVec[0], allowedMinutes, readyProb, false, aSeed, false));
			}
			anInstLogStr += makeInstLogStr(runDataVec);
#ifdef VERBOSE_BATCH
			cout << anInstLogStr << endl;
#endif //VERBOSE_BATCH				
			myFile << anInstLogStr;
			myFile.close();

		}
	}
	
	
	
	 static void wideBatch(const string & instDir, const string & logDir, const vector<string> & fileNameVec, const vector<unsigned> & probVec, const vector<unsigned> & seedVec, double allowedMinutes, bool useErrors) {
	
		string anInstFilePath;
		string anInstLogStr;
		string anInstLogPath;
		
		vector<RunData> runDataVec;
		
		vector<Instance> instVec;
		
		ofstream myFile;
		
		for(const string & anInstName : fileNameVec) {
			anInstFilePath = instDir+anInstName+".txt";
			
			vector<Instance> instVec = Instance::parseTailard(anInstFilePath);
			
			for(unsigned aProb : probVec) {
				runDataVec.clear();
				myFile.open(logDir+"/"+anInstName+"-NonAdapt-prob("+unsignedToString(aProb)+").txt");
				anInstLogStr="\n"+anInstName+" ; mins: "+doubleToString(allowedMinutes)+" ; removalProb: "+unsignedToString(aProb)+"% ; nonAdapt";
				for(unsigned aSeed : seedVec) {
				//bubbleSearch(const Instance & fsi, double allowedMinutes, unsigned probPercent, bool adapt, unsigned seed)
					runDataVec.push_back(bubbleSearch(instVec[0], allowedMinutes, aProb, false, aSeed, useErrors));
				}
				anInstLogStr += makeInstLogStr(runDataVec);
#ifdef VERBOSE_BATCH
				cout << anInstLogStr << endl;
#endif //VERBOSE_BATCH				
				myFile << anInstLogStr;
				myFile.close();
			}
			
			
			for(unsigned aProb : probVec) {
				runDataVec.clear();
				myFile.open(logDir+"/"+anInstName+"-Adapt-prob("+unsignedToString(aProb)+").txt");
				anInstLogStr="\n"+anInstName+" ; mins: "+doubleToString(allowedMinutes)+" ; removalProb: "+unsignedToString(aProb)+"% ; adapt";
				for(unsigned aSeed : seedVec) {
				//bubbleSearch(const Instance & fsi, double allowedMinutes, unsigned probPercent, bool adapt, unsigned seed)
					runDataVec.push_back(bubbleSearch(instVec[0], allowedMinutes, aProb, true, aSeed, useErrors));
				}
				anInstLogStr += makeInstLogStr(runDataVec);
#ifdef VERBOSE_BATCH
				cout << anInstLogStr << endl;
#endif //VERBOSE_BATCH				
				myFile << anInstLogStr;
				myFile.close();
			}
		}
	}
	
	 static void batchInstance(const string & fileDir, const string & fileName, const string & logDir, unsigned instanceIndex, double allowedMinutes, unsigned probPercent, bool adapt, const vector<unsigned> & seedVec, bool useErrors) {
		string fullInstPath = fileDir+fileName+".txt";
		vector<Instance> fsiVec = Instance::parseTailard(fullInstPath);
		vector<RunData> runDataVec;
		string instLogStr;
		
		ofstream myFile;
		
		unsigned index = 0;
		unsigned currInstanceIndex = 0;
		
		for(const Instance & fsi : fsiVec) {
			
			if(currInstanceIndex == instanceIndex) {
				runDataVec.clear();
				index++;
				myFile.open(logDir+"/"+fileName+"LOG_"+unsignedToString(index)+".txt");
				instLogStr="\n"+fileName+" ; mins: "+doubleToString(allowedMinutes)+" ; removalProb: "+unsignedToString(probPercent)+"% ; "+(adapt ? "adapt" :"nonAdapt");
				for(unsigned aSeed : seedVec) {
					//bubbleSearch(const Instance & fsi, double allowedMinutes, unsigned probPercent, bool adapt, unsigned seed)
					runDataVec.push_back(bubbleSearch(fsi, allowedMinutes, probPercent, adapt, aSeed, useErrors));
				}
				instLogStr += makeInstLogStr(runDataVec);
#ifdef VERBOSE_BATCH
				cout << instLogStr << endl;
#endif //VERBOSE_BATCH
				myFile << instLogStr;
				myFile.close();
			}
			currInstanceIndex++;
		}
	}
	
	 static void batchFile(const string & fileDir, const string & fileName,const string & logDir, double allowedMinutes, unsigned probPercent, bool adapt, const vector<unsigned> & seedVec, bool useErrors) {
		string fullInstPath = fileDir+fileName+".txt";
		vector<Instance> fsiVec = Instance::parseTailard(fullInstPath);
		vector<RunData> runDataVec;
		string instLogStr;
		
		ofstream myFile;
		
		unsigned index = 0;
		
		for(const Instance & fsi : fsiVec) {
			runDataVec.clear();
			index++;
			myFile.open(logDir+"/"+fileName+"LOG_"+unsignedToString(index)+".txt");
			instLogStr="\n"+fileName+" ; mins: "+doubleToString(allowedMinutes)+" ; removalProb: "+unsignedToString(probPercent)+"% ; "+(adapt ? "adapt" :"nonAdapt");
			for(unsigned aSeed : seedVec) {
				//bubbleSearch(const Instance & fsi, double allowedMinutes, unsigned probPercent, bool adapt, unsigned seed)
				runDataVec.push_back(bubbleSearch(fsi, allowedMinutes, probPercent, adapt, aSeed, useErrors));
			}
			instLogStr += makeInstLogStr(runDataVec);
#ifdef VERBOSE_BATCH
			cout << instLogStr << endl;
#endif //VERBOSE_BATCH
			myFile << instLogStr;
			myFile.close();
		}
	}
	
	 static string makeInstLogStr(const vector<RunData> & runDataVec) {
		string str = "";
		
		for(RunData rd : runDataVec) {
			str += "\n" + rd.toString();
		}
		
		return str;
	}
	
	
	
	 static RunData bubbleSearch(const Instance & fsi, double allowedMinutes, unsigned probPercent, bool adapt, unsigned seed, bool useErrors) {
		srand(seed);
		assert(fsi.isInitialized());
		assert(probPercent>50 && probPercent<100);
		
		pair<unsigned, unsigned> miliSecs_bubbleXmakespan = pair<unsigned, unsigned>(0, 0);
		pair<unsigned, unsigned> bufferMiliSecs;
		
		setInstance(fsi);
		
		//XXX XXX XXX XXX XXX XXX XXX
		State pivotState;
		if( ! useErrors)
			pivotState = nehkk1(false, false);
		else
			pivotState = nehErrorError();
		State bestState = pivotState;
		State bufferState = bestState;
		
		RunData rd(seed, bestState.getMakespan());
		
		rd.newBest(bestState.getMakespan(), bestState.getDispatchOrder(), 0);
		
#ifdef VERBOSE_BUBBLE_SEARCH
		cout << "Start state: " << pivotState.toString() << endl;
#endif //VERBOSE_BUBBLE_SEARCH
		
		const unsigned totalSeconds = (unsigned)(allowedMinutes * 60.0);
		clock_t beginC = clock();
		struct timeval tvA;
		gettimeofday(&tvA, NULL);
		struct timeval tvB;
		
		while(secondsBetween(beginC, clock()) < totalSeconds) {
			//bufferState = pivotState;
			bufferMiliSecs = bufferState.bubble(probPercent, pivotState);
			//bufferState.verifyConsistency();
			//cout << bufferState.getMakespan() << endl;
			//sleep(1);
			miliSecs_bubbleXmakespan.first += bufferMiliSecs.first;
			miliSecs_bubbleXmakespan.second += bufferMiliSecs.second;
			
			rd.tookStep();
			
			if(bufferState.getMakespan() <= bestState.getMakespan()) {
				bestState = bufferState;
				
				gettimeofday(&tvB, NULL);
				rd.newBest(bestState.getMakespan(), bestState.getDispatchOrder(), miliSecondsBetween(tvA, tvB));
				//rd.newBest(bestState.getMakespan(), bestState.getDispatchOrder(), secondsBetween(beginC, clock()));
				
				if(adapt) {
					pivotState = bufferState; 
				}
			} 
#define     USE_METROPOLIS
#if defined(USE_METROPOLIS)
			else {
			  //cout << "Will I accept a solution that is " <<bufferState.getMakespan()-bestState.getMakespan() << " worse, when the temperature is " << fsi.C0 << "?" << endl;
			  //cout << "Yes, with probability " << exp(-double(bufferState.getMakespan()-bestState.getMakespan())/fsi.C0)<< endl;
			  if(drand48()<=exp(-double(bufferState.getMakespan()-bestState.getMakespan())/fsi.C0) && adapt) {
			    //cout << "Hey! I accepted a state that is " << bufferState.getMakespan()-bestState.getMakespan() << " worse!" << endl;
			    pivotState = bufferState;
			  }
			}
			
#endif
		}
#ifndef NDEBUG
		bestState.verifyConsistency();
#endif
		
#ifdef VERBOSE_BUBBLE_SEARCH
		cout << "Best  state: " << bestState.toString() << endl;
#endif //VERBOSE_BUBBLE_SEARCH
		//cout << "In Bubble: " << (miliSecs_bubbleXmakespan.first)/1000.0 << " in makespan: " << (miliSecs_bubbleXmakespan.second)/1000.0 << endl;
		return rd;
	}
	
	 pair<unsigned, unsigned> bubble(unsigned probPercent, const State & pivotState) {
		
    //const unsigned n = dispatchOrder.size();
    
    list<unsigned> l(pivotState.dispatchOrder.begin()+1,pivotState.dispatchOrder.end());

    unsigned c = 1;
    unsigned n = dispatchOrder.size()-1;
    while (n>0) {
      auto i = l.begin();
      while (true) {
	if((unsigned)(rand()%100) <= probPercent) {
	  dispatchOrder[c++]=*i;
	  l.erase(i);
	  n--;
	  break;
	}
	i++;
	if (i==l.end())
	  i = l.begin();
	  
      }
    }
    computeMakespan();
    
    return pair<unsigned, unsigned>(1,2);
  }
	
	 void verifyConsistency() const {
	
		const string msg = "\n\nTHE SHIT HIT THE FAN\n\n";
	
		if(dispatchOrder[0] != 0) {
			throw msg + " dummy is wrong\n";
		
		}
		for(unsigned base=0; base<dispatchOrder.size(); base++) {
			for(unsigned tail=base+1; tail<dispatchOrder.size(); tail++) {
				if(dispatchOrder[base] == dispatchOrder[tail]) {
					cout << toString();
					throw msg+ " repeat "+unsignedToString(base)+" ; "+unsignedToString(tail)+"\n";
				}
			}
		}
		
		vector<bool> v (dispatchOrder.size(), false);
		for(unsigned u : dispatchOrder) {
			v[u] = true;
		}
		
		for(bool b : v) {
			if(! b) {
				throw msg + "missing job\n";
			}
		}
	}
	
	/* pair<unsigned, unsigned> bubble(unsigned probPercent, const State & pivotState) {
		
		vector<bool> wasPicked(getTotalJobs()+1, false);
		unsigned baseIndex = 1;
		unsigned currIndex = 1;
		
		while(baseIndex < getTotalJobs()) {
			currIndex = 1;
			//cout << "base: " << pivotState << endl;
			//cout << "buffer: " << *this << endl << endl;
			//sleep(1);
			while(currIndex < getTotalJobs()+1) {
				
				//TODO sorteio
				if((unsigned)(rand()%100) <= probPercent) {
					if( ! wasPicked[currIndex]) {
						setOrder(baseIndex, pivotState.getJob(currIndex));
						wasPicked[currIndex] = true;
						baseIndex++;
						break;
					}
				}
				currIndex++;
			}
		}
		
		//inserting last element
		assert(baseIndex == getTotalJobs());
		while(currIndex < getTotalJobs()+1) {
			if( ! wasPicked[currIndex]) {
				setOrder(baseIndex, pivotState.getJob(currIndex));
				baseIndex++;
				break;
			}
			currIndex++;
		}
		
		computeMakespan();
		//cout << "\n\n\n!!!!!\n\n\n";
		return pair<unsigned, unsigned>(0,0);
	}*/
	
	
	 Time getMakespan() const { return makespan; }
	 const vector<unsigned> & getDispatchOrder() const { return dispatchOrder; }
	 unsigned getTotalJobs() const { return fsi.getTotalJobs(); }
	 unsigned getTotalMachines() const { return fsi.getTotalMachines(); }
	 unsigned getJob(unsigned orderIndex) const { return dispatchOrder[orderIndex]; }
	
	 void setOrder(unsigned orderIndex, unsigned jobIndex) {
		assert(jobIndex != 0  &&  jobIndex<=getTotalJobs());
		assert(orderIndex <= getTotalJobs());
		
		dispatchOrder[orderIndex] = jobIndex;
	}
	
	 static void setInstance(const Instance & _fsi) { fsi = _fsi; }
	 void setDispatchOrder(const vector<unsigned> & _dispatchOrder) {
		assert(_dispatchOrder.size() == fsi.getTotalMachines()+1);
		assert(_dispatchOrder[0] == 0);
		dispatchOrder = _dispatchOrder;
	}
	
	 void setRandom() {
		assert(fsi.isInitialized());
		
		for(unsigned i=0; i<fsi.getTotalJobs()+1; i++) {
			dispatchOrder.push_back(i);
		}
		
		random_shuffle(dispatchOrder.begin()+1, dispatchOrder.end());
	}
	
	 void pushInDispatch(unsigned jobIndex) { dispatchOrder.push_back(jobIndex); }

	
   unsigned computePartialMakespanOld(const vector<unsigned> & partialDispatch) {
		
		assert(fsi.isInitialized());
		assert(partialDispatch[0] == 0); //expects dummy job to always be first
		
		boost::multi_array<Time, 2> jobLeavesMachine; //jobFinsh[j][m] = t  <=>  job j leaves machine m in time t
		jobLeavesMachine.resize(boost::extents[partialDispatch.size()][fsi.getTotalMachines()+1]);
		
		// (1) completion times on dummy machine
    	for(unsigned job=0; job<partialDispatch.size(); job++)
      		jobLeavesMachine[job][0] = 0;
		
		// (2) completion times for dummy jobs
      	for(unsigned machine=0; machine<fsi.getTotalMachines()+1; machine++)
      		jobLeavesMachine[0][machine] = 0;
      		
      	// (3) determine completion times on actual remaining machines
      	for(unsigned machineIndex=1; machineIndex<fsi.getTotalMachines()+1; machineIndex++) {
			for(unsigned i=1; i<partialDispatch.size(); i++) {
				jobLeavesMachine[partialDispatch[i]][machineIndex] = max(jobLeavesMachine[partialDispatch[i]][machineIndex-1], jobLeavesMachine[partialDispatch[i-1]][machineIndex]) + fsi.getProcTime(partialDispatch[i], machineIndex);
			}
		}
		
		unsigned makespan = 0;
      	for(unsigned i=1; i<partialDispatch.size(); i++) {
      		makespan = max(makespan, jobLeavesMachine[partialDispatch[i]][fsi.getTotalMachines()]);
      	}
      	
      	return makespan;
	}

  unsigned computePartialMakespan(const vector<unsigned> & partialDispatch) {
    
    assert(fsi.isInitialized());
    assert(partialDispatch[0] == 0); //expects dummy job to always be first
	   
    // (1) completion times on dummy machine
    fill(&C[0],&C[501],0);
	   
    // (2) determine completion times on actual remaining machines
    for(unsigned machineIndex=1; machineIndex<=fsi.getTotalMachines(); machineIndex++)
      for(unsigned pj=0, j=1; j<partialDispatch.size(); pj++, j++)
	C[j]=max(C[j],C[pj])+fsi.getProcTime(partialDispatch[j], machineIndex);

    assert(C[partialDispatch.size()-1]==computePartialMakespanOld(partialDispatch));
    return C[partialDispatch.size()-1];
  }
	
	 void computeMakespan() {
		assert(fsi.isInitialized());
		assert(dispatchOrder[0]==0); //dummy job is always first
		assert(dispatchOrder.size() == fsi.getTotalJobs()+1);
		
		makespan = computePartialMakespan(dispatchOrder);
	}
	
	 static State nehErrorError() {
	
		vector<State> sV;
		sV.push_back(nehkk1(true,true));
		sV.push_back(nehkk1(false,false));
		sV.push_back(nehkk1(true,false));
		sV.push_back(nehkk1(false,true));

		unsigned bestIndex = 0;
		
		for(unsigned i=1; i<4; i++) {
			if(sV[i].getMakespan() < sV[bestIndex].getMakespan()) {
				bestIndex = i;
			}
		}
		
		return sV[bestIndex];
	}
	
	 static State nehkk1(bool useError1, bool useError2) {

		const unsigned c = (fsi.getTotalMachines()-1)*(fsi.getTotalMachines()-2)/2;
		const unsigned n = fsi.getTotalJobs();
		const unsigned m = fsi.getTotalMachines();
		// (1) compute a's and b's
	  	std::vector<Time> a(n+1), b(n+1);
	  	for(unsigned j=1; j<=n; j++) {
			for(unsigned i=1; i<=m; i++) {
				//fsi.getProcTime(jobIndex, machineIndex)
		  		a[j] += (c+m-i)*fsi.getProcTime(j, i);
		  		b[j] += (c+i-1)*fsi.getProcTime(j, i);
			}
		}
	  	// (2) compute the permutation of jobs in order of non-decreasing c
	  	std::vector<std::pair<Time,unsigned> > pi(n+1);
	  	for(unsigned j=1; j<=n; j++)
			pi[j]=std::make_pair(std::min(a[j],b[j]),j);
		if(useError1) {
		  	std::sort(pi.begin()+1, pi.end(), [](const std::pair<Time,unsigned>& p, const std::pair<Time,unsigned>& q) { return p.first < q.first; });
		} else {
			std::sort(pi.begin()+1, pi.end(), [](const std::pair<Time,unsigned>& p, const std::pair<Time,unsigned>& q) { return p.first > q.first; });
		}

	#if defined(NEH_DEBUG)
	  	std::cout << "Jobs in non-decreasing order of min(a_j,b_j): ";
	  	for(unsigned i=1; i<=n; i++)
			std::cout << "(" << pi[i].first << "," << pi[i].second << ") ";
	  	std::cout << std::endl;
	#endif
	  
	  	// (3) prepare heads, tails and relative heads
	  	Time h[m+1][n+1], t[m+2][n+2], rh[m+1][n+2];  // machine x job-index
	  	for(unsigned j=0; j<=n; j++)
			h[0][j] = t[m+1][j] = rh[0][j] = 0;   // (relative) heads on dummy machine 0, tails on dummy machine m+1
	  	t[m+1][n+1]=rh[0][n+1] = 0;
	  	for(unsigned i=0; i<=m; i++)
			h[i][0] = rh[i][0] = 0;               // (relative) heads of dummy job 0
		   
	  	// (3) compute the final permutation of the jobs by repeated insertion
	  	// TBD: partial update of heads and tails, depending on the last insertion position
	  	std::vector<unsigned> rho(1,0);
	  	for(unsigned k=1; k<=n; k++) { // insert k-th job
			const unsigned job = pi[k].second;

			// (3.1) compute heads, tails, and relative heads (Taillard, 1990)
			// (3.1a) initialize tails of (yet) dummy operation k
			for(unsigned i=0; i<=m; i++)
		  		t[i][k] = 0;
			// (3.1b) compute heads (earliest completion times)
			for(unsigned i=1; i<=m; i++)
		  		for(unsigned j=1; j<k; j++)
		  			h[i][j] = max(h[i-1][j],h[i][j-1])+fsi.getProcTime(rho[j], i);
		  		//fsi.getProcTime(jobIndex, machineIndex)
				//h[i][j] = std::max(h[i-1][j],h[i][j-1])+I.p[1][rho[j]][i];
			
		
			// (3.1b) compute tails (minimal remaining time)
			for(unsigned i=m; i>=1; i--)
		  		for(unsigned j=k-1; j>=1; j--)
					t[i][j] = max(t[i+1][j],t[i][j+1])+fsi.getProcTime(rho[j], i);
				//fsi.getProcTime(jobIndex, machineIndex)
				//t[i][j] = std::max(t[i+1][j],t[i][j+1])+I.p[1][rho[j]][i];
		
			// (3.1b) compute relative heads (earliest completion time after inserting job k into position j in machine i
			for(unsigned i=1; i<=m; i++)
		  		for(unsigned j=1; j<=k; j++)
					rh[i][j] = max(rh[i-1][j],h[i][j-1])+fsi.getProcTime(job, i);
				//fsi.getProcTime(jobIndex, machineIndex)
				//rh[i][j] = std::max(rh[i-1][j],h[i][j-1])+I.p[1][job][i];
		
			// insert job optimally into rho
			unsigned fp = 0, lp = 0; // first and last optimal position
			Time Cmax = INF_TIME;
			for(unsigned j=1; j<=k; j++) { // insert at j-th position
		  		// compute insertion cost
		  		Time Cmaxk = 0;
		  	for(unsigned i=1; i<=m; i++)
				Cmaxk = std::max(Cmaxk,rh[i][j]+t[i][j]);
		  
		  	// update best values found
		  	if (Cmaxk < Cmax) {
				Cmax = Cmaxk;
				fp = lp = j;
		  	} else if (Cmaxk == Cmax)
				lp = j;
			}
			assert(fp > 0 && lp >= fp);
		
			if(useError2) {
				if (a[job] <= b[job])
					fp = lp;
			} else {
				if (a[job] > b[job])
					fp = lp;
			}
			rho.insert(rho.begin()+fp,job);
	#if defined(NEH_DEBUG)
			std::cout << "Inserting job " << job << " at position " << fp << " gives minimal Cmax " << Cmax << std::endl;
			std::copy(rho.begin()+1,rho.end(),std::ostream_iterator<unsigned>(std::cout," "));
			std::cout << std::endl;
	#endif
		}	

		State state(fsi);
		state.pushInDispatch(0);
	   	for(unsigned j=1; j<=n; j++) {
			//multi_array<unsigned,2> pi; // permutation of jobs for each machine (machine x index)
			//r.pi[i][j]=rho[j];
			state.pushInDispatch(rho[j]);
		}
		state.computeMakespan();
		return state;
	/*
	  Solution r(I);
	  for(unsigned i=1; i<=m; i++)
		for(unsigned j=1; j<=n; j++)
		  r.pi[i][j]=rho[j];
	  return r;*/
	}
	
	 string toString() const {
		string str = "";
		
		str.append("Makespan<"+unsignedToString(makespan)+"> --- ");
		for(unsigned aJobIndex : dispatchOrder) {
			str.append(unsignedToString(aJobIndex)+" ");
		}
		
		return str;
	}
	
private:

	 static void verboseBatch(const string & str) {
#ifdef VERBOSE_BATCH
		cout << str;
#endif //VERBOSE_BATCH
	}
	
	 static string makeTailIndex(unsigned tailIndex) {
		string str = "";
		
		if(tailIndex < 100) {
			str.append("0");
		} else if(tailIndex < 10) {
			str.append("00");
		}
		str.append(unsignedToString(tailIndex));
		return str;	
	}

	static Instance fsi;
	//indices are 1-based
	vector<unsigned> dispatchOrder; //order jobs are dispatched
	//boost::multi_array<unsigned, 2> jobOrder; //jobOrder[x][y] = z <=> job z is the y'esim to be executed in machine x
	Time makespan;

  Time C[501];
};

Instance State::fsi;

#endif //STATE_HPP

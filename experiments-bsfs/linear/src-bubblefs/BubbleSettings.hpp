/*
@author: Tadeu Knewitz Zubaran tkzubaran@gmail.com
*/

#ifndef BUBBLE_SETTINGS_H
#define BUBBLE_SETTINGS_H

#include "GeneralSettings.hpp"
#include <limits>

using namespace std;

//#####################################################
//###############         UNITS         ###############
//#####################################################
typedef unsigned Time;



//#####################################################
//###############         CONSTS        ###############
//#####################################################
const Time INF_TIME = numeric_limits<Time>::max();
#define MAX_HISTORY 10000


//#####################################################
//###############      FLOW OPTIONS     ###############
//#####################################################




//#####################################################
//##############     VERBOSE OPTIONS     ##############
//#####################################################
//#define VERBOSE_COMPUTE_MAKESPAN
//#define VERBOSE_BUBBLE_SEARCH
#define VERBOSE_BATCH
//#define VERBOSE_TIME_USAGE



#endif //BUBBLE_SETTINGS_H

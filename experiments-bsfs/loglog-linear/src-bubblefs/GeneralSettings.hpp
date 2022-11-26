/*
@author: Tadeu Knewitz Zubaran tkzubaran@gmail.com
*/

#ifndef GENERAL_SETTINGS_H
#define GENERAL_SETTINGS_H

#define NDEBUG
#include <assert.h>

#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>


//#####################################################
//###########   GENERIC UTILITY FUNCTIONS   ###########
//#####################################################
inline std::string doubleToString(const long double aDouble) {
	std::ostringstream strs;
	strs << aDouble;
	std::string str = strs.str();
	return str;
}
inline std::string intToString(const long long aInt) {
    std::stringstream mySs;
    mySs << aInt;
    return mySs.str();
}
inline std::string unsignedToString(const unsigned long long aInt) {
    std::stringstream mySs;
    mySs << aInt;
    return mySs.str();
}
inline const std::string errorText(const std::string & messege, const std::string & fileName, const std::string & methodName) {
	return "\nERROR. "+messege+" in file: "+fileName+" ; method: "+methodName;
}
inline unsigned secondsBetween(clock_t beginC, clock_t endC) {
	return (unsigned)((endC-beginC)/CLOCKS_PER_SEC);
}
// gettimeofday(&aTimeVal, NULL);  
inline unsigned miliSecondsBetween(const struct timeval & start, const struct timeval & end) {
	long seconds, useconds;
	seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
	return  ((seconds) * 1000 + useconds/1000.0) + 0.5;
}

#endif //GENERAL_SETTINGS_H

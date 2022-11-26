/**
 * \file config.hpp
 *   \author Marcus Ritt <mrpritt@inf.ufrgs.br>
 *   \version $Id: emacs 4749 2013-08-16 11:47:04Z ritt $
 *   \date Time-stamp: <2016-05-19 16:13:20 ritt>

 */
#pragma once

#include <string>

#if !defined(DEBUGLEVEL)
// default debuglevel
#define DEBUGLEVEL 0
#endif
#define dstream(l) if (DEBUGLEVEL>=l) std::cout

const std::string version="@SVN_REVISION@";

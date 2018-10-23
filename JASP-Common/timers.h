#ifndef TIMERS_H
#define TIMERS_H

#ifdef PROFILE_JASP

#include <boost/timer/timer.hpp>
#include <string>
#include <map>
#include <iostream>

boost::timer::cpu_timer * _getTimer(std::string timerName);

#define JASPTIMER_START(  TIMERNAME ) _getTimer( #TIMERNAME )->start()
#define JASPTIMER_RESUME( TIMERNAME ) _getTimer( #TIMERNAME )->resume()
#define JASPTIMER_STOP(   TIMERNAME ) _getTimer( #TIMERNAME )->stop()
#define JASPTIMER_PRINT(  TIMERNAME ) std::cout << #TIMERNAME << " ran for " << _getTimer( #TIMERNAME )->format() << std::endl
#define JASPTIMER_FINISH( TIMERNAME ) JASPTIMER_STOP(TIMERNAME); JASPTIMER_PRINT(TIMERNAME)

#else
//No timers please!
#define JASPTIMER_START(  TIMERNAME ) /* TIMERNAME */
#define JASPTIMER_RESUME( TIMERNAME ) /* TIMERNAME */
#define JASPTIMER_STOP(   TIMERNAME ) /* TIMERNAME */
#define JASPTIMER_PRINT(  TIMERNAME ) /* TIMERNAME */
#define JASPTIMER_FINISH( TIMERNAME ) /* TIMERNAME */

#endif

#endif // TIMERS_H

#ifndef _sgp4tle_
#define _sgp4tle_

#include <stdio.h>
#include <math.h>

#include "sgp4ext.h"    // for several misc routines
#include "sgp4unit.h"   // for sgp4init and getgravconst

// ------------------------- function declarations -------------------------

void sgp4_init_from_tle
     (
      const char *TLE_LINE1, const char *TLE_LINE2, gravconsttype whichconst, elsetrec& satrec
     );

#endif


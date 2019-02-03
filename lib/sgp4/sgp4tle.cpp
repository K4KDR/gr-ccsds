#include "sgp4tle.h"

#include <cassert>
/*
 *  This is a stripped down version of the twoline2rv function from the sgp4io
 *  files last modified on 3. sep 2007.
 *  
 */
void sgp4_init_from_tle
     (
      const char *TLE_LINE1, const char *TLE_LINE2, gravconsttype whichconst, elsetrec& satrec
     )
     {
       char longstr1[75];
       char longstr2[75];
       
       assert(strlen(TLE_LINE1) < 75);
       assert(strlen(TLE_LINE2) < 75);
       strcpy(longstr1, TLE_LINE1);
       strcpy(longstr2, TLE_LINE2);
       
       /*
       printf("Using LINE1='%s'\n", longstr1);
       printf("Using LINE2='%s'\n", longstr2);
       */
       
       const double deg2rad  =   M_PI / 180.0;         //   0.0174532925199433
       const double xpdotp   =  1440.0 / (2.0 *M_PI);  // 229.1831180523293

       const char opsmode = 'a';  // 'i': improved mode, 'a': AFSPC mode
       
       double sec, mu, radiusearthkm, tumin, xke, j2, j3, j4, j3oj2;
       double startsec, stopsec, startdayofyr, stopdayofyr, jdstart, jdstop;
       int startyear, stopyear, startmon, stopmon, startday, stopday,
           starthr, stophr, startmin, stopmin;
       int cardnumb, numb, j;
       long revnum = 0, elnum = 0;
       char classification, intldesg[11];
       int year = 0;
       int mon, day, hr, minute, nexp, ibexp;

       getgravconst( whichconst, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );

       satrec.error = 0;

       // set the implied decimal points since doing a formated read
       // fixes for bad input data values (missing, ...)
       for (j = 10; j <= 15; j++)
           if (longstr1[j] == ' ')
               longstr1[j] = '_';

       if (longstr1[44] != ' ')
           longstr1[43] = longstr1[44];
       longstr1[44] = '.';
       if (longstr1[7] == ' ')
           longstr1[7] = 'U';
       if (longstr1[9] == ' ')
           longstr1[9] = '.';
       for (j = 45; j <= 49; j++)
           if (longstr1[j] == ' ')
               longstr1[j] = '0';
       if (longstr1[51] == ' ')
           longstr1[51] = '0';
       if (longstr1[53] != ' ')
           longstr1[52] = longstr1[53];
       longstr1[53] = '.';
       longstr2[25] = '.';
       for (j = 26; j <= 32; j++)
           if (longstr2[j] == ' ')
               longstr2[j] = '0';
       if (longstr1[62] == ' ')
           longstr1[62] = '0';
       if (longstr1[68] == ' ')
           longstr1[68] = '0';
       
       /*
       printf("Corr. LINE1='%s'\n", longstr1);
       printf("Corr. LINE2='%s'\n", longstr2);
       */
       
       sscanf(longstr1,"%2d %5ld %1c %10s %2d %12lf %11lf %7lf %2d %7lf %2d %2d %6ld ",
                       &cardnumb,&satrec.satnum,&classification, intldesg, &satrec.epochyr,
                       &satrec.epochdays,&satrec.ndot, &satrec.nddot, &nexp, &satrec.bstar,
                       &ibexp, &numb, &elnum );

       if (longstr2[52] == ' ') {
              sscanf(longstr2,"%2d %5ld %9lf %9lf %8lf %9lf %9lf %10lf %6ld \n",
                      &cardnumb,&satrec.satnum, &satrec.inclo,
                      &satrec.nodeo,&satrec.ecco, &satrec.argpo, &satrec.mo, &satrec.no,
                      &revnum );
       } else {
              sscanf(longstr2,"%2d %5ld %9lf %9lf %8lf %9lf %9lf %11lf %6ld \n",
                      &cardnumb,&satrec.satnum, &satrec.inclo,
                      &satrec.nodeo,&satrec.ecco, &satrec.argpo, &satrec.mo, &satrec.no,
                      &revnum );
       }
       
       // ---- find no, ndot, nddot ----
       satrec.no   = satrec.no / xpdotp; //* rad/min
       satrec.nddot= satrec.nddot * pow(10.0, nexp);
       satrec.bstar= satrec.bstar * pow(10.0, ibexp);

       // ---- convert to sgp4 units ----
       satrec.a    = pow( satrec.no*tumin , (-2.0/3.0) );
       satrec.ndot = satrec.ndot  / (xpdotp*1440.0);  //* ? * minperday
       satrec.nddot= satrec.nddot / (xpdotp*1440.0*1440);

       // ---- find standard orbital elements ----
       satrec.inclo = satrec.inclo  * deg2rad;
       satrec.nodeo = satrec.nodeo  * deg2rad;
       satrec.argpo = satrec.argpo  * deg2rad;
       satrec.mo    = satrec.mo     * deg2rad;

       satrec.alta = satrec.a*(1.0 + satrec.ecco) - 1.0;
       satrec.altp = satrec.a*(1.0 - satrec.ecco) - 1.0;

       // ----------------------------------------------------------------
       // find sgp4epoch time of element set
       // remember that sgp4 uses units of days from 0 jan 1950 (sgp4epoch)
       // and minutes from the epoch (time)
       // ----------------------------------------------------------------

       // ---------------- temp fix for years from 1957-2056 -------------------
       // --------- correct fix will occur when year is 4-digit in tle ---------
       if (satrec.epochyr < 57)
           year= satrec.epochyr + 2000;
         else
           year= satrec.epochyr + 1900;

         
       days2mdhms ( year,satrec.epochdays, mon,day,hr,minute,sec );
       jday( year,mon,day,hr,minute,sec, satrec.jdsatepoch );

         /*
       printf("Epoch year: %d\n", year);
       printf("Epoch days: %lf\n", satrec.epochdays);
       
       printf("n0: %lf\n", satrec.no);
       printf("ndot: %lf\n", satrec.ndot);
       printf("nddot: %lf\n", satrec.nddot);
       
       printf("bstar: %lf\n", satrec.bstar);
       
       printf("a: %lf\n", satrec.a);
       printf("Eccentricity: %lf\n", satrec.ecco);
       
       printf("incl: %lf\n", satrec.inclo);
       printf("nodeo: %lf\n", satrec.nodeo);
       printf("Argp0: %lf\n", satrec.argpo);
       printf("M0: %lf\n", satrec.mo);
       
       printf("alta: %lf\n", satrec.alta);
       printf("altp: %lf\n", satrec.altp);
       
       printf("Epoch (JD): %lf\n", satrec.jdsatepoch);
       */
       
         
       // ---------------- initialize the orbit at sgp4epoch -------------------
       sgp4init( whichconst, opsmode, satrec.satnum, satrec.jdsatepoch-2433281.5, satrec.bstar,
                 satrec.ecco, satrec.argpo, satrec.inclo, satrec.mo, satrec.no,
                 satrec.nodeo, satrec);
    } // end twoline2rv



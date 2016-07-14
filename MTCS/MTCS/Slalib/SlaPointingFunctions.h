#ifndef SLAPOINTINGFUNCTIONS_H
#define SLAPOINTINGFUNCTIONS_H

#include "TcRtInterfaces.h"
#include "SlalibServices.h"
#include "slalib/slalib.h"

// constants
#define PI                      3.141592653589793238462643
#define OFFSET_UNIX             946684800      // seconds between years 1970 and 2000 
#define JD_AT_UNIX_TIME_ORIGIN  2440587.5      // JD for 1970-01-01 00:00:00.000000
#define JD_TO_MJD               2400000.5      // JD - MJD
#define AZI_ENC_RESOLUTION      45000          // Heidenhain Azimuth encoder: 45000 INC/rev
#define EXE_INTERPOLATION       100            // Heidenhain EXE interpolation (100x)
#define QUAD_INTERPOLATION      4              // Quadrature encoder interpolation (4x)


union EncPosition {
    unsigned long uposition;
    unsigned int  uintarray[2];
};

// define a minimum elevation for the transformations.
// Below this elevation, the refraction effects may become too CPU-intensive to calculate
#define MINIMUM_ELEVATION  8.0 / 180.0 * PI

// inline function to check if a parameter is out-of-bounds
inline void checkAxesVariableBounds(double left, double value, double right, TmcAxesErrors& actualError, TmcAxesErrors errorToSetIfOutOfBounds)
{
    // only overwrite the error if the value is out of bounds *and* if the error was not already set
	if ( ((value < left) || (value > right)) && (actualError == TmcAxesErrors::AXES_NO_ERROR) )
		actualError = errorToSetIfOutOfBounds;
}

// inline function to check if a parameter is out-of-bounds
inline void checkTimingVariableBounds(double left, double value, double right, TmcTimingErrors& actualError, TmcTimingErrors errorToSetIfOutOfBounds)
{
    // only overwrite the error if the value is out of bounds *and* if the error was not already set
	if ( ((value < left) || (value > right)) && (actualError == TmcTimingErrors::TIMING_NO_ERROR) )
		actualError = errorToSetIfOutOfBounds;
}

// do the full coordinate transformations
void doTransformations(
    const TmcFromPlcAxes& fromAxes, 
    const TmcFromPlcTiming& fromTiming, 
    const TmcFromIoEL6688& fromEL6688, 
    const TmcFromIoEcatMaster& fromEcatMaster,
    ITcRTimePtr rTime, 
    TmcToPlcAxes& toAxes, 
    TmcToPlcTiming& toTiming);


HRESULT timeGetAbsoluteDcTime(TmcFromIoEL6688 fromEL6688, TmcFromIoEcatMaster fromEcatMaster, INT leapSecond, LONGLONG *dcTime);

void timeDcToUnixTime(LONGLONG dcTime, TmcUnixTime *unixTime);
void timeUnixToDcTime(TmcUnixTime unixTime, LONGLONG *dcTime);
DOUBLE timeGetMJD(const TmcUnixTime& unixTime);

void timeMJDToDateTime(DOUBLE mjd, TmcDateTime& dateTime);


#endif
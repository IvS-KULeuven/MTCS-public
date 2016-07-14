#include "SlaPointingFunctions.h"


void doTransformations(
    const TmcFromPlcAxes& fromAxes, 
    const TmcFromPlcTiming& fromTiming, 
    const TmcFromIoEL6688& fromEL6688, 
    const TmcFromIoEcatMaster& fromEcatMaster,
    ITcRTimePtr rTime, 
    TmcToPlcAxes& toAxes, 
    TmcToPlcTiming& toTiming)
{
    // first, set the errors to NO ERROR:
    toAxes.error = TmcAxesErrors::AXES_NO_ERROR;
    toTiming.error = TmcTimingErrors::TIMING_NO_ERROR;

    // Copy the "input target" to the "output target", so the PLC-side
    // knows when the new input has been processed
    toAxes.object.given = fromAxes.object;


    // ============================================================================================ //
    // COMPUTE TIME                                                                                 //
    // ============================================================================================ //


    // make a copy of the I/O inputs (because the ESO library does so, TBD if this can be removed)
    TmcFromIoEL6688 fromEL6688Copy; 
    TmcFromIoEcatMaster fromEcatMasterCopy;
    memcpy(&fromEL6688Copy, &fromEL6688, sizeof(TmcFromIoEL6688));
    memcpy(&fromEcatMasterCopy, &fromEcatMaster, sizeof(TmcFromIoEcatMaster));
    
    // get the current distributed clocks time (nanoseconds since January 1, 2000)
    HRESULT result = rTime->GetCurDcTime(GETCURDCTIME_ACTUAL, &toTiming.utc.dcTime );
    if (FAILED(result))
    {
        toTiming.error = TmcTimingErrors::TIMING_GETCURDCTIME_FAILED;
        return;
    }

    // correct the DC time if needed and if possible
    if (fromTiming.alwaysUseLocalClock)
    {
        toTiming.utc.mode = TmcTimingMode::LOCAL_CLOCK;
    }
    else
    {
	    if (timeGetAbsoluteDcTime(fromEL6688Copy, fromEcatMasterCopy, fromTiming.leapSeconds, &toTiming.utc.dcTime) == S_FALSE)
		    toTiming.utc.mode = TmcTimingMode::LOCAL_CLOCK;
	    else
		    toTiming.utc.mode = TmcTimingMode::PTP_IEEE_1588;
    }

    // compute the UTC unix time
	timeDcToUnixTime(toTiming.utc.dcTime, &toTiming.utc.unixTime);

    // compute the MJD from the UTC unix time
    toTiming.utc.mjd = timeGetMJD(toTiming.utc.unixTime);

    // update the UTC dateTime
    timeMJDToDateTime(toTiming.utc.mjd, toTiming.utc.dateTime);
    
	checkTimingVariableBounds(-1.0        , fromTiming.dut            , 1.0           , toTiming.error, TmcTimingErrors::TIMING_DUT_OUT_OF_RANGE);

    // calculate the "full" UT1 (date + fraction) in MJD
    toTiming.ut1.mjd = toTiming.utc.mjd + fromTiming.dut / (24.*60.*60.);
    

    // ============================================================================================ //
    // CHECK BOUNDS                                                                                 //
    // ============================================================================================ //

    // then, replace this value if needed:
	//checkAxesVariableBounds(0.0         , fromAxes.object.rc      , 2 * PI        , toAxes.error, TmcAxesErrors::AXES_OBJECT_RC_OUT_OF_RANGE);
	//checkAxesVariableBounds(-PI / 2     , fromAxes.object.dc      , PI / 2.       , toAxes.error, TmcAxesErrors::AXES_OBJECT_DC_OUT_OF_RANGE);
	checkAxesVariableBounds(-0.0001     , fromAxes.object.pr      , 0.0001        , toAxes.error, TmcAxesErrors::AXES_OBJECT_PR_OUT_OF_RANGE); // 1e-4 rad = 20000 mas >> > Barnards star
	checkAxesVariableBounds(-0.0001     , fromAxes.object.pd      , 0.0001        , toAxes.error, TmcAxesErrors::AXES_OBJECT_PD_OUT_OF_RANGE); // 1e-4 rad = 20000 mas >> > Barnards star
	checkAxesVariableBounds(-10000      , fromAxes.object.px      , 10000         , toAxes.error, TmcAxesErrors::AXES_OBJECT_PX_OUT_OF_RANGE); // 10000 mas >> > Barnards star
	checkAxesVariableBounds(1000        , fromAxes.epoch          , 3000          , toAxes.error, TmcAxesErrors::AXES_EQ_OUT_OF_RANGE);
	checkAxesVariableBounds(-0.0001     , fromAxes.location.xp    , 0.0001        , toAxes.error, TmcAxesErrors::AXES_LOCATION_XP_OUT_OF_RANGE);
	checkAxesVariableBounds(-0.0001     , fromAxes.location.yp    , 0.0001        , toAxes.error, TmcAxesErrors::AXES_LOCATION_YP_OUT_OF_RANGE);
	checkAxesVariableBounds(-0.00001    , fromAxes.location.dx    , 0.00001       , toAxes.error, TmcAxesErrors::AXES_LOCATION_DX_OUT_OF_RANGE);
	checkAxesVariableBounds(-0.00001    , fromAxes.location.dy    , 0.00001       , toAxes.error, TmcAxesErrors::AXES_LOCATION_DY_OUT_OF_RANGE);
	checkAxesVariableBounds(-PI         , fromAxes.location.elong , PI            , toAxes.error, TmcAxesErrors::AXES_LOCATION_ELONG_OUT_OF_RANGE);
	checkAxesVariableBounds(-PI / 2     , fromAxes.location.phi   , PI / 2        , toAxes.error, TmcAxesErrors::AXES_LOCATION_PHI_OUT_OF_RANGE);
	checkAxesVariableBounds(0.0         , fromAxes.location.hm    , 8000.0        , toAxes.error, TmcAxesErrors::AXES_LOCATION_HM_OUT_OF_RANGE);
	checkAxesVariableBounds(200.0       , fromAxes.location.tk    , 400.0         , toAxes.error, TmcAxesErrors::AXES_LOCATION_TK_OUT_OF_RANGE);
	checkAxesVariableBounds(200         , fromAxes.location.phpa  , 1500          , toAxes.error, TmcAxesErrors::AXES_LOCATION_PHPA_OUT_OF_RANGE);
	checkAxesVariableBounds(0.0         , fromAxes.location.rh    , 1.0           , toAxes.error, TmcAxesErrors::AXES_LOCATION_RH_OUT_OF_RANGE);
	checkAxesVariableBounds(0.1         , fromAxes.location.wlfq  , 200.          , toAxes.error, TmcAxesErrors::AXES_LOCATION_WLFQ_OUT_OF_RANGE);
	checkAxesVariableBounds(-10.0       , fromAxes.location.tlr   , 10.0          , toAxes.error, TmcAxesErrors::AXES_LOCATION_TLR_OUT_OF_RANGE);
	//checkAxesVariableBounds(0.0         , fromAxes.feedback.az    , 2.*PI         , toAxes.error, TmcAxesErrors::AXES_FEEDBACK_AZ_OUT_OF_RANGE);
	//checkAxesVariableBounds(-PI / 2     , fromAxes.feedback.el    , PI / 2.       , toAxes.error, TmcAxesErrors::AXES_FEEDBACK_EL_OUT_OF_RANGE);

    // only proceed if all inputs were OK
    if (toAxes.error != TmcAxesErrors::AXES_NO_ERROR)
        return;
    

    // ============================================================================================ //
    // PRECOMPUTE PARAMETERS                                                                        //
    // ============================================================================================ //

    // precompute apparent-to-mean parameters
    // Instead of the TDB, we use UTC (which is "adequate for all but the most critical applications", see manual)
    double amprms[21];
    slaMappa( fromAxes.epoch, toTiming.utc.mjd, amprms );

    // precompute apparent-to-observed parameters
    double aoprms[14];
    slaAoppa(   toTiming.utc.mjd,
                fromTiming.dut,
                fromAxes.location.elong,
                fromAxes.location.phi,
                fromAxes.location.hm,
                fromAxes.location.xp,
                fromAxes.location.yp,
                fromAxes.location.tk,
                fromAxes.location.phpa,
                fromAxes.location.rh,
                fromAxes.location.wlfq,
                fromAxes.location.tlr, 
                aoprms );

    // precompute polar motion of the Earth
    slaPolmo(   fromAxes.location.elong, 
                fromAxes.location.phi, 
                fromAxes.location.xp, 
                fromAxes.location.yp,
                &toAxes.polmo.elong,
                &toAxes.polmo.phi,
                &toAxes.polmo.daz);

    
    // ============================================================================================ //
    // OBJECT POSITION [alpha,delta] --> [azi,ele]                                                  //
    // ============================================================================================ //

    // convert the mean object [alpha,delta] to geocentric apparent [alpha,delta]
    // ra, da: apparent [alpha,delta] (radians)
    slaMapqk(   fromAxes.object.rc, 
                fromAxes.object.dc, 
                fromAxes.object.pr, 
                fromAxes.object.pd, 
                fromAxes.object.px, 
                fromAxes.object.rv, 
                amprms,
                &toAxes.object.apparent.ra,
                &toAxes.object.apparent.dec);
    
    // QUICK CHECK IF THE STAR HAS MINIMUM HEIGHT
    // Don't proceed if the star is too low above the horizon, 
    // because when the star is too low, then the time needed to
    // calculate the refraction effects becomes too big!
    
    toAxes.last = ( fmod_(slaGmst(toTiming.ut1.mjd), 2*PI) - (-fromAxes.location.elong));
    toAxes.ha   = fmod_(toAxes.last - toAxes.object.apparent.ra, 2*PI);

    slaAltaz(   toAxes.ha, 
                toAxes.object.apparent.dec, 
                fromAxes.location.phi,
                &toAxes.object.quick.az,
                &toAxes.object.quick.azd,
                &toAxes.object.quick.azdd,
                &toAxes.object.quick.el,
                &toAxes.object.quick.eld,
                &toAxes.object.quick.eldd,
                &toAxes.object.quick.pa,
                &toAxes.object.quick.pad,
                &toAxes.object.quick.padd);
    
    // Don't do the calculations below a minimum elevation.
    // On a higher software level (in the PLC or NC task), the elevation may be limited to more 
    // appropriate values, e.g. 12 degrees
    if (toAxes.object.quick.el < MINIMUM_ELEVATION)
    {
        // we cannot calculate the object's az/el in high-precision, because we risk that the calculation times 
        // become too long
        toAxes.object.isTooLow = true;
    }
    else
    {
        toAxes.object.isTooLow = false;
        // OK, at this point the object is > 8 degrees elevation,
        // so we can now calculate the high-precision position without risk of too long calculation times
        
        // convert apparent-to-observed, for sources distant from the solar system
        slaAopqk(   toAxes.object.apparent.ra, 
                    toAxes.object.apparent.dec,
                    aoprms,
                    &toAxes.object.observed.aob,
                    &toAxes.object.observed.zob,
                    &toAxes.object.observed.hob,
                    &toAxes.object.observed.dob,
                    &toAxes.object.observed.rob);

        // now calculate the observed altaz positions/velocities/accelerations
        slaAltaz(   toAxes.object.observed.hob,
                    toAxes.object.observed.dob,
                    fromAxes.location.phi,
                    &toAxes.object.observedFull.az,
                    &toAxes.object.observedFull.azd,
                    &toAxes.object.observedFull.azdd,
                    &toAxes.object.observedFull.el,
                    &toAxes.object.observedFull.eld,
                    &toAxes.object.observedFull.eldd,
                    &toAxes.object.observedFull.pa,
                    &toAxes.object.observedFull.pad,
                    &toAxes.object.observedFull.padd);

        // correct for the earths polar motion
        toAxes.object.observed.aob    += toAxes.polmo.daz;
        toAxes.object.observedFull.az += toAxes.polmo.daz;
    }
    

    // ============================================================================================ //
    // TELESCOPE POSITION [azi,ele] --> [alpha,delta]                                               //
    // ============================================================================================ //

    // Don't do the calculations below a minimum elevation.
    // On a higher software level (in the PLC or NC task), the elevation may be limited to more 
    // appropriate values, e.g. 12 degrees

    if (fromAxes.feedback.el < MINIMUM_ELEVATION)
    {
        // we cannot calculate the feedback's ra/dec in high-precision, because we risk that the 
        // calculation times become too long
        toAxes.feedback.isTooLow = true;
    }
    else
    {
        toAxes.feedback.isTooLow = false;
        // convert from observed to apparent
        char A = 'A';
        slaOapqk(   &A,                                      // 'A' means: next argument is azimuth
                    fromAxes.feedback.az - toAxes.polmo.daz,  // correct for the earths polar motion
                    PI / 2 - fromAxes.feedback.el,             // elevation to zenith distance
                    aoprms,
                    &toAxes.feedback.apparent.ra,
                    &toAxes.feedback.apparent.dec);
        
        // conver from apparent to mean
        slaAmpqk(   toAxes.feedback.apparent.ra,
                    toAxes.feedback.apparent.dec,
                    amprms,
                    &toAxes.feedback.mean.ra,
                    &toAxes.feedback.mean.dec);
    }

    toAxes.object.isValid = (!toAxes.object.isTooLow) && toAxes.error == TmcAxesErrors::AXES_NO_ERROR;
    toAxes.feedback.isValid = (!toAxes.feedback.isTooLow) && toAxes.error == TmcAxesErrors::AXES_NO_ERROR;

}




/*
 *****************************************************************************
 * timeGetAbsoluteDcTime: Adjust DC time based on the external time signal
 *
 *  IN   ptptime    : External time signal
 *  IN   leapSecond : Leap Second 
 *  OUT  dcTIME     : Adjusted TwinCAT DC time
 ***************************************************************************** 
 */
HRESULT timeGetAbsoluteDcTime(TmcFromIoEL6688 fromEL6688, TmcFromIoEcatMaster fromEcatMaster, INT leapSecond, LONGLONG *dcTime)
{
	HRESULT hr = S_OK; 
	LONGLONG val = 0;
	LONGLONG diff = 0;
	LONGLONG ONE_SEC = 1000000000;
	
	// Check if external time signal is connected
	if (fromEL6688.notConnected)
		return S_FALSE;

	// If both variables are null means most probably they are not mapped.
	if (fromEL6688.internalTimestamp == 0 || fromEL6688.externalTimestamp == 0)
		return S_FALSE;

	diff = (fromEL6688.internalTimestamp - fromEL6688.externalTimestamp) + (ONE_SEC * leapSecond) + fromEcatMaster.dcToTcTimeOffset;
	*dcTime -= diff;
	return hr;
}



/*
 *****************************************************************************
 * timeGetUTC: Get UTC time from TwinCAT DC time
 *
 *  IN   dcTime: TwinCAT DC time
 *  OUT  unixTime: UTC time 
 ***************************************************************************** 
 */
void timeDcToUnixTime(LONGLONG dcTime, TmcUnixTime *unixTime)
{
	// DC time is in nano seconds
	unixTime->sec = (LONGLONG)(dcTime/1000000000);
	unixTime->usec = (dcTime - (unixTime->sec * 1000000000))/1000;
	
	// Added offset between year 1970 and 2000 needed by the different references
	// used in UNIX and TWINCAT
	unixTime->sec += OFFSET_UNIX;
}

/*
 *****************************************************************************
 * timeGetDcTime: Get TwinCAT DC time from UTC time
 *
 *  IN   unixTime: Unix time 
 *  OUT  dcTIME: TwinCAT DC time
 ***************************************************************************** 
 */
void timeUnixToDcTime(TmcUnixTime unixTime, LONGLONG *dcTime)
{
	LONGLONG val = 0;
	val = unixTime.sec * 1000000000;
	val -= OFFSET_UNIX;
	*dcTime = val + unixTime.usec * 1000;
}



/*
 *****************************************************************************
 * timeUTCToJD: convert struct ccsTIMEVAL into Modified Julian date
 *
 *  IN   unixTime: time in UT
 *  OUT    : Julian date  
 ***************************************************************************** 
 */
DOUBLE timeGetMJD(const TmcUnixTime& unixTime)
{
    DOUBLE jd  = JD_AT_UNIX_TIME_ORIGIN + (unixTime.sec + unixTime.usec/1000000.)/86400.;
    DOUBLE mjd = jd - JD_TO_MJD;
    return mjd;
}


void timeMJDToDateTime(DOUBLE mjd, TmcDateTime& dateTime)
{ 
    // convert mjd into years/months/days/fractionOfDays
    int years, months, days, status;
    double fod;
    slaDjcl(mjd, &years, &months, &days, &fod, &status);
    
    // update the first part of the dateTime
    dateTime.years = years;
    dateTime.months = months;
    dateTime.days = days;
    dateTime.fractionOfDay = fod;

    // convert fractionOfDays into hours/minutes/microseconds
    int ihmsf[4];
    int decimalPlacesOfMicroseconds = 6;
    char sign;
    slaDd2tf(decimalPlacesOfMicroseconds, fod, &sign, ihmsf);
    
    // update the second part of the dateTime
    dateTime.hours        = ihmsf[0];
    dateTime.minutes      = ihmsf[1];
    dateTime.seconds      = ihmsf[2];
    dateTime.microseconds = ihmsf[3];
}




    /*
    // define a status int which will be used by the SLALIB functions below
    int status;
    
    //calculate the UTC date in MJD
    slaCldj(inputs.utc.years, inputs.utc.months, inputs.utc.days, &outputs.utcDate, &status);
    if (status != 0)
    {
        outputs.error = TmcAxesErrors::AXES_SLA_CLDJ_ERROR;
        return;
    }

    // calculate the UTC fraction of day
    outputs.utcFod = inputs.utc.hours/24. + inputs.utc.minutes/(24.*60.) + inputs.utc.seconds/(24.*60.*60.);


    // calculate the "full" UTC (date + fraction) in MJD
    outputs.utcMjd = outputs.utcDate + outputs.utcFod;
    */
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
	#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
//#include <afx.h>

#include "math.h"


#include "xearth.h"
#include "sgp.h"

#define and &&
#define or ||
	BOOL		m_bLatLonAlt;
	// here are some constants
	double	ae;
	double	tothrd;
	double	xkmper;	// 6378.135 {Earth equatorial radius - kilometers (WGS '72)}
	double	f;		// 1/298.26 {Earth flattening (WGS '72)}
	double	ge;		// 398600.8 {Earth gravitational constant (WGS '72)}
	double	J2;		// 1.0826158E-3 {J2 harmonic (WGS '72)}
	double	J3;		// -2.53881E-6  {J3 harmonic (WGS '72)}
	double	J4;		// -1.65597E-6  {J4 harmonic (WGS '72)}
	double	ck2;	// J2/2;
	double	ck4;	// -3*J4/8;
	double	xj3;	// J3;
	double	qo;		// ae + 120/xkmper;
	double	s;		// ae + 78/xkmper;	
	double	e6a;	// 1E-6;
	double	dpinit;	// 1        {Deep-space initialization code}	
	double	dpsec;	// 2        {Deep-space secular code}
	double	dpper;	// 3        {Deep-space periodic code}
	long	secday;	// seconds per day
	double	omega_E;// earth rotation per sideral day

	int		iflag,ideep;
	double	xke,xmnpda;

	double	eqsq,siniq,cosiq,rteqsq,ao,cosq2,sinomo,cosomo;
	double	bsq,xlldot,omgdt,xnodot,xnodp;
	double	xll,omgasm,xnodes,_em,xinc,xn,t;
	double	qoms2t;
	double		m_fTime;
SATELLITE	m_Sat;
VECTOR		m_vPOS;
VECTOR		m_vVEL;
VECTOR		m_vLLA;
VECTOR		m_vUPos;
VECTOR		m_vUVel;
VECTOR		m_vObs;
VECTOR		geodetic;
VECTOR pos;
VECTOR vel;

/*
long GetInt   (int iStart, int iEnd, char *cLine);
double GetFloat (int iStart, int iEnd, char *cLine) ;
char *GetString(int iStart, int iEnd, char *cLine) ;

double JulianDate (CTime ct);

WINAPI TestAgSgp()
{
	char		m_cLine0[23], m_cLine1[70], m_cLine2[70];
	double	dEpochDay;
	int		iTemp;
	char	*pChar;
	double time;
	SYSTEMTIME st;
	
	memcpy(m_cLine0, "SO-35                  ", 22);
	memcpy(m_cLine1, "1 25636U 99008C   01013.83490286  .00000915  00000-0  25386-3 0  3629", 69);
	memcpy(m_cLine2, "2 25636  96.4524 172.8522 0150725 235.3242 123.3704 14.41659147 99439", 69);

	memcpy(m_Sat.cSatelliteName, m_cLine0, 22);

	m_Sat.iSatelliteNumber		= GetInt	(3 , 7,m_cLine1);	// = CString >catnr<
	m_Sat.iLaunchYear			= GetInt	(10,11,m_cLine1);
	m_Sat.iLaunchNumber			= GetInt	(12,14,m_cLine1);
	pChar						= GetString	(15,17,m_cLine1);
	memcpy(m_Sat.cLaunchPiece, pChar, 3);
	m_Sat.iEpochYear			= GetInt	(19,20,m_cLine1);	// = epoch 19, 14 lang
	m_Sat.fEpochDay				= GetFloat	(21,32,m_cLine1);	// = epoch 19, 14 lang
	m_Sat.fBalisticCoefficient	= GetFloat	(34,43,m_cLine1);
//	m_Sat.fRadiationCoefficient = GetFloat	(54,61,m_cLine1)*1e-5;	// = bstar 54, 6 lang
	// the input format is 12345-x convert to 0.12345^x
	iTemp	= GetInt	(60,61,m_cLine1);	// get the exponent, and make 0.xxxxx of it
	m_Sat.fRadiationCoefficient	= GetFloat	(54,59,m_cLine1)*pow(10,-5+iTemp);

	pChar						= GetString	(63,63,m_cLine1);
	memcpy(&m_Sat.cEmphemeristType, pChar, 2);
	m_Sat.iElementNumber		= GetInt	(65,68,m_cLine1);

	m_Sat.fInclination			= GetFloat	(9 ,16,m_cLine2);	// = xincl
	m_Sat.fRightAscending		= GetFloat	(18,25,m_cLine2);	// = xnodeo
	m_Sat.fEccentricity			= GetFloat	(27,33,m_cLine2)*1e-7;	// = eo
	m_Sat.fPeregee				= GetFloat	(35,42,m_cLine2);	// = omegao
	m_Sat.fMeanAnomaly			= GetFloat	(44,51,m_cLine2);	// = xmo
	m_Sat.fMeanMotion			= GetFloat	(53,63,m_cLine2);	// = xno
	m_Sat.iRevAtEpoch			= GetInt	(64,68,m_cLine2);

	m_Sat.fJulianEpoch = JulianDate (m_Sat.iEpochYear *1000.0+ m_Sat.fEpochDay);
	m_Sat.fEpochFraction = modf(m_Sat.fEpochDay, &dEpochDay);
	m_Sat.iEpochDay = (int) dEpochDay;
//	if ( m_Sat.iEpochYear < 60 )
//		{
//			m_Sat.iEpochYear += 2000;
//			m_Sat.iLaunchYear += 2000;
//		} else {
			m_Sat.iEpochYear += 1900;
			m_Sat.iLaunchYear += 1900;
//		}
	geodetic.x = 52;
	geodetic.y = 0;
	
//	CTime theTime = CTime::GetCurrentTime();

//	time = JulianDate( theTime );

//	sgp(&m_Sat,theTime, geodetic, pos, vel, m_vObs, m_vLLA);
	}


/*double GetFloat (int iStart, int iEnd, char *cLine) 
{
	CString cs;
	CString csLine (cLine);
	double ret;
	iStart --;
	cs = csLine.Mid(iStart, iEnd-iStart);
	ret = atof(cs);
	return ret;
}

long GetInt   (int iStart, int iEnd, char *cLine) 
{
	CString cs;
	CString csLine(cLine);
	long ret;
	iStart --;
	cs = csLine.Mid(iStart, iEnd-iStart);
	ret = atoi(cs);
	return ret;
}

char *GetString(int iStart, int iEnd, char *cLine) 
{
	iStart --;
	CString csLine(cLine);
	return (char *)((const char *)csLine.Mid(iStart, iEnd-iStart));
}
*/

double JulianDate (SYSTEMTIME st)
{
    int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int i;
	double arg, arg2, fFraction;
	int iYear = (int)st.wYear-1;
	int iDay  = (int)st.wDay;
	double A,B;

	for (i=0;i < st.wMonth-1;i++) iDay += days[i];	// addiere die Tage des Jahres ...
	if	(((st.wYear %4) == 0) &&
		(((st.wYear %100) != 0) || ((st.wYear %400) == 0)) &&
		(st.wMonth > 2)) iDay += 1;		// Schaltjahr ???

	modf(iYear/100.0,&A);
	modf(A/4.0,&B);
	B += 2.0-A;

	modf(365.25 * iYear, &arg);
//	modf(30.6001 * 14, &arg2);
	arg2 = 428;
	arg += arg2 + 1720994.5 + B;
	fFraction = (double) (st.wHour*60.0*60.0	+ 
						  st.wMinute*60.0		+
						  st.wSecond         )/(24.0*60.0*60.0);
//						  st.wMilliseconds/100)/(24*60*60);Not precise enough for milliseconds
	return arg+iDay+fFraction;	// The JulianDate of the CTime Date
}


//WINAPI AGCalcSGP(SATELLITE& pSatellite, SYSTEMTIME& TheTime, VECTOR& geodetic, VECTOR& pos, VECTOR& vel, VECTOR& m_vObs, VECTOR& m_vLLA)
WINAPI AGCalcSGP()
{	
	double tsince;
	double time;
	BOOL ret;
	double	dEpochDay;
//	VECTOR pos,vel;

	SATELLITE pSatellite;
	SYSTEMTIME TheTime;
	VECTOR geodetic;
	VECTOR pos;
	VECTOR vel;
	VECTOR m_vObs;
	VECTOR m_vLLA;


	pSatellite.fJulianEpoch = JulianDate (m_Sat.iEpochYear *1000.0+ m_Sat.fEpochDay);
	pSatellite.fEpochFraction = modf(m_Sat.fEpochDay, &dEpochDay);
	pSatellite.iEpochDay = (int) dEpochDay;

	time = JulianDate( TheTime );

	SetSatellite(pSatellite);
	m_bLatLonAlt = FALSE;
	m_fTime = time;
	tsince = (time - m_Sat.fJulianEpoch) * xmnpda;
	if (ideep == 0)
		ret = CalcSGP4(tsince,&iflag,&pos,&vel);	// Calculation for near earth
	else
		ret = CalcSDP4(tsince,&iflag,&pos,&vel);	// Calculation for the deep space
	if (ret == FALSE) return FALSE;	// Ooops, there was a mistake within the calculation !!!
	ConvertSatState(&pos, &vel);	// here the pos and vel vector are 
									// to be copied to m_vPOS and m_vVEL !!!
	CalculateLatLonAlt( time );

	CalculateObs(pos, vel, geodetic, time);
	return TRUE;

}

void CalculateLatLonAlt(double time)	//,VECTOR pos)
{
// Reference:  The 1992 Astronomical Almanac, page K12. 
	double lat,lon,alt;
	double theta,r,e2,phi,c;
	theta = AcTan(m_vPOS.y,m_vPOS.x);
	lon = Modulus(theta - ThetaG(time),2.0*PI);
	r = sqrt(sqr(m_vPOS.x) + sqr(m_vPOS.y));
	e2 = f*(2.0 - f);
	lat = AcTan(m_vPOS.z,r);
	do	{
		phi = lat;
		c = 1.0/sqrt(1.0 - e2*sqr(sin(phi)));
		lat = AcTan(m_vPOS.z + xkmper*c*e2*sin(phi),r);
	}	while (fabs(lat - phi) > 1E-7);	//1E-10); For speeding up calculation 7 digit
										//is exact enough (123.45 
	alt = r/cos(lat) - xkmper*c;

	m_bLatLonAlt = TRUE;
	m_vLLA.x = lat*180.0/PI;   // radians
	m_vLLA.y = lon*180.0/PI;   // radians
	m_vLLA.z = alt;			// kilometers
	m_vLLA.w = theta*180.0/PI; // radians
 }


void CalculateUserPosVel(VECTOR *geodetic, double time)
{
//Procedure Calculate_User_PosVel(var geodetic : vector;
//                                        time : double;
//                         var obs_pos,obs_vel : vector);
// Reference:  The 1992 Astronomical Almanac, page K11
	double mfactor = 2.0*PI*omega_E/secday;
	double	lat,lon,alt,theta,c,s,achcp;
	lat = geodetic->x;
	lon = geodetic->y;
	alt = geodetic->z;
	theta = Modulus(ThetaG(time) + lon,2.0*PI);
	geodetic->w = theta;						// {LMST}
	c = 1.0/sqrt(1.0 + f*(f - 2.0)*sqr(sin(lat)));
	s = sqr(1.0 - f)*c;
	achcp = (xkmper*c + alt)*cos(lat);
	m_vUPos.x = achcp*cos(theta);			// {kilometers}
	m_vUPos.y = achcp*sin(theta);
	m_vUPos.z = (xkmper*s + alt)*sin(lat);
	m_vUPos.w = sqrt(sqr(m_vUPos.x) + sqr(m_vUPos.y) + sqr(m_vUPos.z));
	m_vUVel.x = -mfactor*m_vUPos.y;		// {kilometers/second}
	m_vUVel.y =  mfactor*m_vUPos.x;
	m_vUVel.z =  0.0;
	m_vUVel.w = sqrt(sqr(m_vUVel.x) + sqr(m_vUVel.y));
}
// Calculates the Observer data for a given Satellite and returns m_vObs
BOOL CalculateObs(VECTOR pos, VECTOR vel, VECTOR geodetic, double time)
{
// Procedure Calculate_Obs(pos,vel,geodetic : vector;
//                                    time : double;
//                             var obs_set : vector);
	BOOL	visible;
	double	lat,lon,alt,theta,sin_lat,cos_lat,sin_theta,cos_theta;
	double	el,azim,top_s,top_e,top_z;
	VECTOR	range,rgvel;
	CalculateUserPosVel(&geodetic,time);	//,obs_pos,obs_vel);
	rgvel.x = vel.x - m_vUVel.x;
	rgvel.y = vel.y - m_vUVel.y;
	rgvel.z = vel.z - m_vUVel.z;
	range.x = pos.x - m_vUPos.x;
	range.y = pos.y - m_vUPos.y;
	range.z = pos.z - m_vUPos.z;
	range.w = sqrt(sqr(range.x) + sqr(range.y) + sqr(range.z));
	lat = geodetic.x;
	lon = geodetic.y;
	alt = geodetic.z;
	theta = geodetic.w;
	sin_lat = sin(lat);
	cos_lat = cos(lat);
	sin_theta = sin(theta);
	cos_theta = cos(theta);
	top_s = sin_lat*cos_theta*range.x + sin_lat*sin_theta*range.y - cos_lat*range.z;
	top_e = -sin_theta*range.x + cos_theta*range.y;
	top_z = cos_lat*cos_theta*range.x + cos_lat*sin_theta*range.y + sin_lat*range.z;
	azim = atan(-top_e/top_s);	//{Azimuth}
	if (top_s > 0.0)	azim += PI;
	if (azim < 0.0)	    azim += 2.0*PI;
	el = asin(top_z/range.w);
	m_vObs.x = azim;            //{Azimuth (radians)}
	m_vObs.y = el;				//{Elevation (radians)}
	m_vObs.z = range.w;			//{Range (kilometers)}
	m_vObs.w = range.x*rgvel.x + range.y*rgvel.y + range.z*rgvel.z;
	m_vObs.w /= range.w;	    //{Range Rate (kilometers/second)}
//{ Corrections for atmospheric refraction }
//{ Reference:  Astronomical Algorithms by Jean Meeus, pp. 101-104 }
//{ Note:  Correction is meaningless when apparent elevation is below horizon }
	m_vObs.y += DegToRad((1.02/tan(DegToRad(RadToDeg(el)+10.3/(RadToDeg(el)+5.11))))/60.0);
	if (m_vObs.y >= 0)	visible = TRUE;
	else	{
		m_vObs.y = el;	//{Reset to true elevation}
		visible = FALSE;
	}
	return visible;
}


void ConvertSatState(VECTOR *pos,VECTOR *vel)
{
	pos->x *= xkmper;
	pos->y *= xkmper;
	pos->z *= xkmper;
	pos->w = sqrt(sqr(pos->x)+sqr(pos->y)+sqr(pos->z));
	vel->x *= xkmper/60.0;	// kilometers/second
	vel->y *= xkmper/60.0;	// kilometers/second
	vel->z *= xkmper/60.0;	// kilometers/second
	vel->w = sqrt(sqr(vel->x)+sqr(vel->y)+sqr(vel->z));
	// write to the public member ...
	m_vPOS.x = pos->x;
	m_vPOS.y = pos->y;
	m_vPOS.z = pos->z;
	m_vPOS.w = pos->w;
	m_vVEL.x = vel->x;
	m_vVEL.y = vel->y;
	m_vVEL.z = vel->z;
	m_vVEL.w = vel->w;
} //Procedure Convert_Sat_State

void SetSatellite(SATELLITE pSatellite)
{
	int iEpochYear;
//	SATELLITE *pSat = (SATELLITE *)pSatellite;
	SATELLITE pSat = pSatellite;
	memcpy(m_Sat.cSatelliteName, pSat.cSatelliteName, 22);
// Now everything is in butter !
//	CString		csHelp;
	m_Sat.iSatelliteNumber		= pSat.iSatelliteNumber;
	m_Sat.iLaunchYear			= pSat.iLaunchYear;
	m_Sat.iLaunchNumber			= pSat.iLaunchNumber;
	memcpy(m_Sat.cLaunchPiece, pSat.cLaunchPiece, 3);
	m_Sat.iEpochYear			= pSat.iEpochYear;
	m_Sat.iEpochDay				= pSat.iEpochDay;
	m_Sat.fEpochDay				= pSat.fEpochDay;
	m_Sat.fEpochFraction		= pSat.fEpochFraction;
	m_Sat.fBalisticCoefficient	= pSat.fBalisticCoefficient;
	m_Sat.fRadiationCoefficient = pSat.fRadiationCoefficient;
	memcpy(m_Sat.cEmphemeristType, pSat.cEmphemeristType,2);
	m_Sat.iElementNumber		= pSat.iElementNumber;

	m_Sat.fInclination			= pSat.fInclination;
	m_Sat.fRightAscending		= pSat.fRightAscending;
	m_Sat.fEccentricity			= pSat.fEccentricity;
	m_Sat.fPeregee				= pSat.fPeregee;
	m_Sat.fMeanAnomaly			= pSat.fMeanAnomaly;
	m_Sat.fMeanMotion			= pSat.fMeanMotion;
	m_Sat.iRevAtEpoch			= pSat.iRevAtEpoch;

	iEpochYear = (int)(m_Sat.iEpochYear/100.0)*100;	// convert 1996 to 1900
	iEpochYear = m_Sat.iEpochYear - iEpochYear;		// but i need 96
	m_Sat.fJulianEpoch = JulianDate (iEpochYear*1000.0+m_Sat.iEpochDay+m_Sat.fEpochFraction);
	Init();
	return;
}

double JulianDate (double st)
{
	double dYear;
	int iYear = (int)(st/1000.0);
	st -= iYear * 1000;
	if (iYear < 50) iYear +=1999;
	else iYear +=1899;

	long A,B;
	A = (long)(iYear/100.0);
	B = 2 - A + (long)(A/4.0);
	dYear = (long)(365.25 * iYear)
          + 428.0 + 1720994.5 + B;
	return dYear+st;
}

void Init()
{
// Normal variables for the 72'model !!!
	ae       = 1.0;
	tothrd   = 2.0/3.0;
	xkmper   = 6378.135;	// Earth equatorial radius - kilometers (WGS '72)
	f        = 1.0/298.26;	// Earth flattening (WGS '72)
	ge       = 398600.8;	// Earth gravitational constant (WGS '72)
	J2       = 1.0826158E-3;// J2 harmonic (WGS '72)
	J3       = -2.53881E-6;	// J3 harmonic (WGS '72)
	J4       = -1.65597E-6;	// J4 harmonic (WGS '72)
	ck2      = (double)J2/2.0;
	ck4      = -3.0*J4/8.0;
	xj3      = (double)J3;
	qo       = ae + 120.0/xkmper;
	s        = ae + 78.0/xkmper;
	e6a      = 1E-6;
	dpinit   = 1.0;			// Deep-space initialization code
	dpsec    = 2.0;			// Deep-space secular code
	dpper    = 3.0;			// Deep-space periodic code
	xmnpda	 = 1440.0;		// Minutes per day
	secday	 = 86400;		// seconds per day
	omega_E	 = 1.00273790934;	// earth rotation per sideral day
	xke    = sqrt(3600.0*ge/(xkmper*xkmper*xkmper));  //sqrt(ge) ER^3/min^2
	qoms2t = sqr(sqr(qo-s));              //(qo-s)^4 ER^4
// Convert sat datas to proper units ...
//	int ibexp;												// Already
//	ibexp = GetInt (60,61, m_cLine1);						// done 
//	m_Sat.fRadiationCoefficient *= pow(10.0, ibexp)/ae;		// earlier !!!
	m_Sat.fRightAscending = DegToRad(m_Sat.fRightAscending);
	m_Sat.fPeregee = DegToRad(m_Sat.fPeregee);
	m_Sat.fMeanAnomaly = DegToRad(m_Sat.fMeanAnomaly);
	m_Sat.fInclination = DegToRad(m_Sat.fInclination);
	m_Sat.fMeanMotion *= 2.0*PI/xmnpda;
// determination if it is deep space or not ...
	double a1,a0,del1, del0,temp;
	a1 = pow (xke/m_Sat.fMeanMotion, tothrd);
	temp = (double)(1.5*ck2*(3.0*sqr(cos(m_Sat.fInclination))-1.0)/pow(1.0-m_Sat.fEccentricity*m_Sat.fEccentricity,1.5));
	del1 = temp /(a1*a1);
	a0 = a1*(1-del1*(0.5*tothrd+del1*(1.0+134.0/81.0*del1)));
	del0 = temp/(a0*a0);
	xnodp = m_Sat.fMeanMotion/(1.0+del0);
	if (2.0*PI/xnodp >= 225.0) ideep = 1;
	else ideep = 0;
	iflag = 1;
}

double sqr (double arg)
{
	return arg*arg;
}

double DegToRad (double arg)
{
	return (double)(arg/360.0*(2.0*PI));
}

double RadToDeg (double arg)
{
	return (double)(arg/(2.0*PI)*360.0);
}
double Fmod2p (double arg)
{
	double modu, ret;
	double twopi = 2.0*PI;
	modu = arg - (int)(arg/twopi) * twopi;
	if (modu >= 0.0)
		ret = modu;
	else
		ret = modu + twopi;
	return ret;
}

double AcTan(double sinx, double cosx)
{
	double ret;
	if (cosx == 0.0)	{
		if (sinx > 0.0)
			ret = PI/2.0;
		else
			ret = 3.0*PI/2.0;
	}
	else	{
		if (cosx > 0.0)
			ret = atan(sinx/cosx);
		else
			ret = PI + atan(sinx/cosx);
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////////////////////
BOOL CalcSGP4(double tsince,int *iflag, VECTOR *pos, VECTOR *vel)
{

//  label
//    9,10,90,100,110,130,140;
//  const
	double a1		= 0.0;double a3ovk2	= 0.0;double ao		= 0.0;
	double aodp		= 0.0;double aycof	= 0.0;double betao	= 0.0;
	double betao2	= 0.0;double c1		= 0.0;double c1sq	= 0.0;
	double c2		= 0.0;double c3		= 0.0;double c4		= 0.0;
	double c5		= 0.0;double coef	= 0.0;double coef1	= 0.0;
	double cosio	= 0.0;double d2		= 0.0;double d3		= 0.0;
	double d4		= 0.0;double del1	= 0.0;double delmo	= 0.0;
	double delo		= 0.0;double eeta	= 0.0;double eosq	= 0.0;
	double eta		= 0.0;double etasq	= 0.0;int    isimp	= 0;
	double omgcof	= 0.0;double omgdot	= 0.0;double perige	= 0.0;
	double pinvsq	= 0.0;double psisq	= 0.0;double qoms24	= 0.0;
	double s4		= 0.0;double sinio	= 0.0;double sinmo	= 0.0;
	double t2cof	= 0.0;double t3cof	= 0.0;double t4cof	= 0.0;
	double t5cof	= 0.0;double temp	= 0.0;double temp1	= 0.0;
	double temp2	= 0.0;double temp3	= 0.0;double theta2	= 0.0;
	double theta4	= 0.0;double tsi	= 0.0;double x1m5th	= 0.0;
	double x1mth2	= 0.0;double x3thm1	= 0.0;double x7thm1	= 0.0;
	double xhdot1	= 0.0;double xlcof	= 0.0;double xmcof	= 0.0;
	double xmdot	= 0.0;double xnodcf	= 0.0;double xnodot	= 0.0;
	double xnodp	= 0.0;
// var
    int i;
    double cosuk,sinuk,rfdotk,vx,vy,vz,ux,uy,uz,xmy,xmx;
    double cosnok,sinnok,cosik,sinik,rdotk,xinck,xnodek,uk,rk;
    double cos2u,sin2u,u,sinu,cosu,betal,rfdot,rdot,r,pl,elsq;
    double esine,ecose,epw,temp6,temp5,temp4,cosepw,sinepw;
    double capu,ayn,xlt,aynl,xll,axn,xn,beta,xl,e,a,tfour;
    double tcube,delm,delomg,templ,tempe,tempa,xnode,tsq,xmp;
    double omega,xnoddf,omgadf,xmdf,x,y,z,xdot,ydot,zdot;
	double ee;
// Recover original mean motion (xnodp) and semimajor axis (aodp) 
// from input elements. 
  if (iflag == 0)
    goto SGP100;
  a1 = pow(xke/m_Sat.fMeanMotion,tothrd);
  cosio = cos(m_Sat.fInclination);
  theta2 = cosio*cosio;
  x3thm1 = 3.0*theta2 - 1.0;
  eosq = m_Sat.fEccentricity*m_Sat.fEccentricity;
  betao2 = 1.0 - eosq;
  betao = sqrt(betao2);
  del1 = 1.5*ck2*x3thm1/(a1*a1*betao*betao2);
  ao = a1*(1.0 - del1*(0.5*tothrd + del1*(1.0 + 134.0/81.0*del1)));
  delo = 1.5*ck2*x3thm1/(ao*ao*betao*betao2);
  xnodp = m_Sat.fMeanMotion/(1.0 + delo);
  aodp = ao/(1.0 - delo);
// Initialization 
// For perigee less than 220 kilometers, the isimp flag is set and
//  the equations are truncated to linear variation in sqrt a and
//  quadratic variation in mean anomaly.  Also, the c3 term, the
//  delta omega term, and the delta m term are dropped.
  isimp = 0;
  if ((aodp*(1.0 - m_Sat.fEccentricity)/ae) < (220.0/xkmper + ae))
    isimp = 1;
// For perigee below 156 km, the values of s and qoms2t are altered. 
  s4 = s;
  qoms24 = qoms2t;
  perige = xkmper*(aodp*(1.0 - m_Sat.fEccentricity) - ae);
  if (perige >= 156.0)
    goto SGP10;
  s4 = perige - 78.0;
  if (perige > 98.0)
    goto SGP9;
  s4 = 20.0;
SGP9:
  qoms24 = pow((120.0 - s4)*ae/xkmper,4.0);
  s4 = s4/xkmper + ae;
SGP10:
  pinvsq = 1.0/(aodp*aodp*betao2*betao2);
  tsi = 1.0/(aodp - s4);
  eta = aodp*m_Sat.fEccentricity*tsi;
  etasq = eta*eta;
  eeta = m_Sat.fEccentricity*eta;
  psisq = fabs(1.0 - etasq);
  coef = qoms24*pow(tsi,4.0);
  coef1 = coef/pow(psisq,3.5);
  c2 = coef1*xnodp*(aodp*(1.0 + 1.5*etasq + eeta*(4.0 + etasq))
      + 0.75*ck2*tsi/psisq*x3thm1*(8.0 + 3.0*etasq*(8.0 + etasq)));
  c1 = m_Sat.fRadiationCoefficient*c2;
  sinio = sin(m_Sat.fInclination);
  a3ovk2 = -xj3/ck2*pow(ae,3.0);
  c3 = coef*tsi*a3ovk2*xnodp*ae*sinio/m_Sat.fEccentricity;
  x1mth2 = 1.0 - theta2;
  c4 = 2.0*xnodp*coef1*aodp*betao2*(eta*(2.0 + 0.5*etasq)
      + m_Sat.fEccentricity*(0.5 + 2.0*etasq) - 2.0*ck2*tsi/(aodp*psisq)
      *(-3.0*x3thm1*(1.0 - 2.0*eeta + etasq*(1.5 - 0.5*eeta))
      + 0.75*x1mth2*(2.0*etasq - eeta*(1.0 + etasq))*cos(2.0*m_Sat.fPeregee)));
  c5 = 2.0*coef1*aodp*betao2*(1.0 + 2.75*(etasq + eeta) + eeta*etasq);
  theta4 = theta2*theta2;
  temp1 = 3.0*ck2*pinvsq*xnodp;
  temp2 = temp1*ck2*pinvsq;
  temp3 = 1.25*ck4*pinvsq*pinvsq*xnodp;
  xmdot = xnodp + 0.5*temp1*betao*x3thm1
         + 0.0625*temp2*betao*(13.0 - 78.0*theta2 + 137.0*theta4);
  x1m5th = 1.0 - 5.0*theta2;
  omgdot = -0.5*temp1*x1m5th + 0.0625*temp2*(7.0 - 114.0*theta2 +395.0*theta4)
          + temp3*(3.0 - 36.0*theta2 + 49.0*theta4);
  xhdot1 = -temp1*cosio;
  xnodot = xhdot1 + (0.5*temp2*(4.0 - 19.0*theta2)
          + 2.0*temp3*(3.0 - 7.0*theta2))*cosio;
  omgcof = m_Sat.fRadiationCoefficient*c3*cos(m_Sat.fPeregee);
  xmcof = -tothrd*coef*m_Sat.fRadiationCoefficient*ae/eeta;
  xnodcf = 3.5*betao2*xhdot1*c1;
  t2cof = 1.5*c1;
  xlcof = 0.125*a3ovk2*sinio*(3.0 + 5.0*cosio)/(1.0 + cosio);
  aycof = 0.25*a3ovk2*sinio;
  delmo = pow(1.0 + eta*cos(m_Sat.fMeanAnomaly),3.0);
  sinmo = sin(m_Sat.fMeanAnomaly);
  x7thm1 = 7.0*theta2 - 1.0;
  if (isimp == 1)
    goto SGP90;
  c1sq = c1*c1;
  d2 = 4.0*aodp*tsi*c1sq;
  temp = d2*tsi*c1/3.0;
  d3 = (17.0*aodp + s4)*temp;
  d4 = 0.5*temp*aodp*tsi*(221.0*aodp + 31.0*s4)*c1;
  t3cof = d2 + 2.0*c1sq;
  t4cof = 0.25*(3.0*d3 + c1*(12.0*d2 + 10.0*c1sq));
  t5cof = 0.2*(3.0*d4 + 12.0*c1*d3 + 6.0*d2*d2 + 15.0*c1sq*(2.0*d2 + c1sq));
SGP90:
  iflag = 0;
// Update for secular gravity and atmospheric drag. 
SGP100:
  xmdf = m_Sat.fMeanAnomaly + xmdot*tsince;
  omgadf = m_Sat.fPeregee + omgdot*tsince;
  xnoddf = m_Sat.fRightAscending + xnodot*tsince;
  omega = omgadf;
  xmp = xmdf;
  tsq = tsince*tsince;
  xnode = xnoddf + xnodcf*tsq;
  tempa = 1.0 - c1*tsince;
  tempe = m_Sat.fRadiationCoefficient*c4*tsince;
  templ = t2cof*tsq;
  if (isimp == 1)
    goto SGP110;
  delomg = omgcof*tsince;
  delm = xmcof*(pow(1.0 + eta*cos(xmdf),3.0) - delmo);
  temp = delomg + delm;
  xmp = xmdf + temp;
  omega = omgadf - temp;
  tcube = tsq*tsince;
  tfour = tsince*tcube;
  tempa = tempa - d2*tsq - d3*tcube - d4*tfour;
  tempe = tempe + m_Sat.fRadiationCoefficient*c5*(sin(xmp) - sinmo);
  templ = templ + t3cof*tcube + tfour*(t4cof + tsince*t5cof);
SGP110:
  a = aodp*sqr(tempa);
  e = m_Sat.fEccentricity - tempe;
  ee = e*e;
  if ( ee > 1.0) return FALSE;	// error, no good satellite datas ...
  xl = xmp + omega + xnode + xnodp*templ;
  beta = sqrt(1.0 - ee);
  xn = xke/pow(a,1.5);
// Long period periodics 
  axn = e*cos(omega);
  temp = 1.0/(a*beta*beta);
  xll = temp*xlcof*axn;
  aynl = temp*aycof;
  xlt = xl + xll;
  ayn = e*sin(omega) + aynl;
// Solve Kepler's Equation 
  capu = Fmod2p(xlt - xnode);
  temp2 = capu;
  for (i = 1;i <= 10;i++)
    {
    sinepw = sin(temp2);
    cosepw = cos(temp2);
    temp3 = axn*sinepw;
    temp4 = ayn*cosepw;
    temp5 = axn*cosepw;
    temp6 = ayn*sinepw;
    epw = (capu - temp4 + temp3 - temp2)/(1.0 - temp5 - temp6) + temp2;
    if (fabs(epw - temp2) <= e6a)
      goto SGP140;
// SGP130: Wird nicht angesprochen ...??? ...
    temp2 = epw;
    }; //for i
// Short period preliminary quantities 
SGP140:
  ecose = temp5 + temp6;
  esine = temp3 - temp4;
  elsq = axn*axn + ayn*ayn;
  temp = 1.0 - elsq;
  pl = a*temp;
  r = a*(1.0 - ecose);
  temp1 = 1.0/r;
  rdot = xke*sqrt(a)*esine*temp1;
  rfdot = xke*sqrt(pl)*temp1;
  temp2 = a*temp1;
  betal = sqrt(temp);
  temp3 = 1.0/(1.0 + betal);
  cosu = temp2*(cosepw - axn + ayn*esine*temp3);
  sinu = temp2*(sinepw - ayn - axn*esine*temp3);
  
  u = AcTan(sinu,cosu);
  
  sin2u = 2*sinu*cosu;
  cos2u = 2*cosu*cosu - 1.0;
  temp = 1.0/pl;
  temp1 = ck2*temp;
  temp2 = temp1*temp;
// Update for short periodics 
  rk = r*(1.0 - 1.5*temp2*betal*x3thm1) + 0.5*temp1*x1mth2*cos2u;
  uk = u - 0.25*temp2*x7thm1*sin2u;
  xnodek = xnode + 1.5*temp2*cosio*sin2u;
  xinck = m_Sat.fInclination + 1.5*temp2*cosio*sinio*cos2u;
  rdotk = rdot - xn*temp1*x1mth2*sin2u;
  rfdotk = rfdot + xn*temp1*(x1mth2*cos2u + 1.5*x3thm1);
// Orientation vectors 
  sinuk = sin(uk);
  cosuk = cos(uk);
  sinik = sin(xinck);
  cosik = cos(xinck);
  sinnok = sin(xnodek);
  cosnok = cos(xnodek);
  xmx = -sinnok*cosik;
  xmy = cosnok*cosik;
  ux = xmx*sinuk + cosnok*cosuk;
  uy = xmy*sinuk + sinnok*cosuk;
  uz = sinik*sinuk;
  vx = xmx*cosuk - cosnok*sinuk;
  vy = xmy*cosuk - sinnok*sinuk;
  vz = sinik*cosuk;
// Position and velocity 
  x = rk*ux;  pos->x = x;
  y = rk*uy;  pos->y = y;
  z = rk*uz;  pos->z = z;
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
  xdot = rdotk*ux + rfdotk*vx;  vel->x = xdot;
  ydot = rdotk*uy + rfdotk*vy;  vel->y = ydot;
  zdot = rdotk*uz + rfdotk*vz;  vel->z = zdot;
  return TRUE;
}; //Procedure SGP4


BOOL CalcSDP4(double tsince,int *iflag,VECTOR *pos, VECTOR *vel)
{
//  label
//    9,10,90,100,130,140;
//  const
    double a1       = 0.0;double   a3ovk2   = 0.0;double   ao       = 0.0;
    double aodp     = 0.0;double   aycof    = 0.0;double   betao    = 0.0;
    double betao2   = 0.0;double   c1       = 0.0;double   c2       = 0.0;
    double c4       = 0.0;double   coef     = 0.0;double   coef1    = 0.0;
    double cosg     = 0.0;double   cosio    = 0.0;double   del1     = 0.0;
    double delo     = 0.0;double   eeta     = 0.0;double   eosq     = 0.0;
    double eta      = 0.0;double   etasq    = 0.0;double   omgdot   = 0.0;
    double perige   = 0.0;double   pinvsq   = 0.0;double   psisq    = 0.0;
    double qoms24   = 0.0;double   s4       = 0.0;double   sing     = 0.0;
    double sinio    = 0.0;double   t2cof    = 0.0;double   temp1    = 0.0;
    double temp2    = 0.0;double   temp3    = 0.0;double   theta2   = 0.0;
    double theta4   = 0.0;double   tsi      = 0.0;double   x1m5th   = 0.0;
    double x1mth2   = 0.0;double   x3thm1   = 0.0;double   x7thm1   = 0.0;
    double xhdot1   = 0.0;double   xlcof    = 0.0;double   xmdot    = 0.0;
    double xnodcf   = 0.0;double   xnodot   = 0.0;double   xnodp    = 0.0;
//  var
    int i;
    double a,axn,ayn,aynl,beta,betal,capu,cos2u,cosepw,cosik;
    double cosnok,cosu,cosuk,e,ecose,elsq,em,epw,esine,omgadf;
    double pl,r,rdot,rdotk,rfdot,rfdotk,rk,sin2u,sinepw,sinik;
    double sinnok,sinu,sinuk,temp,temp4,temp5,temp6,tempa;
    double tempe,templ,tsq,u,uk,ux,uy,uz,vx,vy,vz,xinc,xinck;
    double xl,xll,xlt,xmam,xmdf,xmx,xmy,xn,xnoddf,xnode,xnodek;
    double x,y,z,xdot,ydot,zdot;
	double ee;
  if (iflag == 0)
    goto SDP100;
// Recover original mean motion (xnodp) and semimajor axis (aodp) 
// from input elements. 
  a1 = pow(xke/m_Sat.fMeanMotion,tothrd);
  cosio = cos(m_Sat.fInclination);
  theta2 = cosio*cosio;
  x3thm1 = 3.0*theta2 - 1.0;
  eosq = m_Sat.fEccentricity*m_Sat.fEccentricity;
  betao2 = 1.0 - eosq;
  betao = sqrt(betao2);
  del1 = 1.5*ck2*x3thm1/(a1*a1*betao*betao2);
  ao = a1*(1.0 - del1*(0.5*tothrd + del1*(1.0 + 134.0/81.0*del1)));
  delo = 1.5*ck2*x3thm1/(ao*ao*betao*betao2);
  xnodp = m_Sat.fMeanMotion/(1.0 + delo);
  aodp = ao/(1.0 - delo);
// Initialization 
// For perigee below 156 km, the values of s and qoms2t are altered. 
  s4 = s;
  qoms24 = qoms2t;
  perige = (double)(aodp*(1.0 - m_Sat.fEccentricity) - ae)*xkmper;
  if (perige >= 156.0) goto SDP10;
  s4 = perige - 78.0;
  if (perige > 98.0) goto SDP9;
  s4 = 20.0;
SDP9:
  qoms24 = pow((120.0 - s4)*ae/xkmper,4.0);
  s4 = s4/xkmper + ae;
SDP10:
  pinvsq = 1.0/(aodp*aodp*betao2*betao2);
  sing = sin(m_Sat.fPeregee);
  cosg = cos(m_Sat.fPeregee);
  tsi = 1.0/(aodp - s4);
  eta = aodp*m_Sat.fEccentricity*tsi;
  etasq = eta*eta;
  eeta = m_Sat.fEccentricity*eta;
  psisq = fabs(1.0 - etasq);
  coef = qoms24*pow(tsi,4.0);
  coef1 = coef/pow(psisq,3.5);
  c2 = coef1*xnodp*(aodp*(1.0 + 1.5*etasq + eeta*(4.0 + etasq))
      + 0.75*ck2*tsi/psisq*x3thm1*(8.0 + 3.0*etasq*(8.0 + etasq)));
  c1 = m_Sat.fRadiationCoefficient*c2;
  sinio = sin(m_Sat.fInclination);
  a3ovk2 = -xj3/ck2*pow(ae,3.0);
  x1mth2 = 1.0 - theta2;
  c4 = 2.0*xnodp*coef1*aodp*betao2*(eta*(2.0 + 0.5*etasq)
      + m_Sat.fEccentricity*(0.5 + 2.0*etasq) - 2.0*ck2*tsi/(aodp*psisq)
      *(-3.0*x3thm1*(1.0 - 2.0*eeta + etasq*(1.5 - 0.5*eeta))
      + 0.75*x1mth2*(2.0*etasq - eeta*(1.0 + etasq))*cos(2.0*m_Sat.fPeregee)));
  theta4 = theta2*theta2;
  temp1 = 3.0*ck2*pinvsq*xnodp;
  temp2 = temp1*ck2*pinvsq;
  temp3 = 1.25*ck4*pinvsq*pinvsq*xnodp;
  xmdot = xnodp + 0.5*temp1*betao*x3thm1
         + 0.0625*temp2*betao*(13.0 - 78.0*theta2 + 137.0*theta4);
  x1m5th = 1.0 - 5.0*theta2;
  omgdot = -0.5*temp1*x1m5th + 0.0625*temp2*(7.0 - 114.0*theta2 + 395.0*theta4)
          + temp3*(3.0 - 36.0*theta2 + 49.0*theta4);
  xhdot1 = -temp1*cosio;
  xnodot = xhdot1 + (0.5*temp2*(4.0 - 19.0*theta2)
          + 2.0*temp3*(3.0 - 7.0*theta2))*cosio;
  xnodcf = 3.5*betao2*xhdot1*c1;
  t2cof = 1.5*c1;
  xlcof = 0.125*a3ovk2*sinio*(3.0 + 5.0*cosio)/(1.0 + cosio);
  aycof = 0.25*a3ovk2*sinio;
  x7thm1 = 7.0*theta2 - 1.0;
// SDP90:Never used
  iflag = 0;
  Call_dpinit(&eosq,&sinio,&cosio,&betao,&aodp,&theta2,&sing,&cosg,
              &betao2,&xmdot,&omgdot,&xnodot,&xnodp);
// Update for secular gravity and atmospheric drag 
SDP100:
  xmdf = m_Sat.fMeanAnomaly + xmdot*tsince;
  omgadf = m_Sat.fPeregee + omgdot*tsince;
  xnoddf = m_Sat.fRightAscending + xnodot*tsince;
  tsq = tsince*tsince;
  xnode = xnoddf + xnodcf*tsq;
  tempa = 1.0 - c1*tsince;
  tempe = m_Sat.fRadiationCoefficient*c4*tsince;
  templ = t2cof*tsq;
  xn = xnodp;
  Call_dpsec(&xmdf,&omgadf,&xnode,&em,&xinc,&xn,&tsince);
  a = pow(xke/xn,tothrd)*sqr(tempa);
  e = em - tempe;
  ee = e*e;
  if ( ee > 1.0) return FALSE;	// wrong satellite datas
  xmam = xmdf + xnodp*templ;
  Call_dpper(&e,&xinc,&omgadf,&xnode,&xmam);
  xl = xmam + omgadf + xnode;
  beta = sqrt(1.0 - ee);
  xn = xke/pow(a,1.5);
// Long period periodics 
  axn = e*cos(omgadf);
  temp = 1.0/(a*beta*beta);
  xll = temp*xlcof*axn;
  aynl = temp*aycof;
  xlt = xl + xll;
  ayn = e*sin(omgadf) + aynl;
// Solve Kepler's Equation 
  capu = Fmod2p(xlt - xnode);
  temp2 = capu;
  for (i = 1;i < 10;i++)
    {
    sinepw = sin(temp2);
    cosepw = cos(temp2);
    temp3 = axn*sinepw;
    temp4 = ayn*cosepw;
    temp5 = axn*cosepw;
    temp6 = ayn*sinepw;
    epw = (capu - temp4 + temp3 - temp2)/(1.0 - temp5 - temp6) + temp2;
    if (fabs(epw - temp2) <= e6a) goto SDP140;
//SDP130: Never used
    temp2 = epw;
    }; //for i
// Short period preliminary quantities 
SDP140:
  ecose = temp5 + temp6;
  esine = temp3 - temp4;
  elsq = axn*axn + ayn*ayn;
  temp = 1.0 - elsq;
  pl = a*temp;
  r = a*(1.0 - ecose);
  temp1 = 1.0/r;
  rdot = xke*sqrt(a)*esine*temp1;
  rfdot = xke*sqrt(pl)*temp1;
  temp2 = a*temp1;
  betal = sqrt(temp);
  temp3 = 1.0/(1.0 + betal);
  cosu = temp2*(cosepw - axn + ayn*esine*temp3);
  sinu = temp2*(sinepw - ayn - axn*esine*temp3);
  u = AcTan(sinu,cosu);
  sin2u = 2.0*sinu*cosu;
  cos2u = 2.0*cosu*cosu - 1.0;
  temp = 1.0/pl;
  temp1 = ck2*temp;
  temp2 = temp1*temp;
// Update for short periodics 
  rk = r*(1.0 - 1.5*temp2*betal*x3thm1) + 0.5*temp1*x1mth2*cos2u;
  uk = u - 0.25*temp2*x7thm1*sin2u;
  xnodek = xnode + 1.5*temp2*cosio*sin2u;
  xinck = xinc + 1.5*temp2*cosio*sinio*cos2u;
  rdotk = rdot - xn*temp1*x1mth2*sin2u;
  rfdotk = rfdot + xn*temp1*(x1mth2*cos2u + 1.5*x3thm1);
// Orientation vectors 
  sinuk = sin(uk);
  cosuk = cos(uk);
  sinik = sin(xinck);
  cosik = cos(xinck);
  sinnok = sin(xnodek);
  cosnok = cos(xnodek);
  xmx = -sinnok*cosik;
  xmy = cosnok*cosik;
  ux = xmx*sinuk + cosnok*cosuk;
  uy = xmy*sinuk + sinnok*cosuk;
  uz = sinik*sinuk;
  vx = xmx*cosuk - cosnok*sinuk;
  vy = xmy*cosuk - sinnok*sinuk;
  vz = sinik*cosuk;
// Position and velocity 
  x = rk*ux;  pos->x = x;
  y = rk*uy;  pos->y = y;
  z = rk*uz;  pos->z = z;
  xdot = rdotk*ux + rfdotk*vx;  vel->x = xdot;
  ydot = rdotk*uy + rfdotk*vy;  vel->y = ydot;
  zdot = rdotk*uz + rfdotk*vz;  vel->z = zdot;
  return TRUE;
};	//Procedure SDP4

void Call_dpinit(double *eosq,double *sinio,double *cosio,double *betao,double *aodp,
							double *theta2,double *sing,double *cosg,double *betao2,double *xmdot,
							double *omgdot,double *xnodott,double *xnodpp)
  {
  eqsq   = *eosq;    siniq  = *sinio;   cosiq  = *cosio;   rteqsq = *betao;
  ao     = *aodp;    cosq2  = *theta2;  sinomo = *sing;    cosomo = *cosg;
  bsq    = *betao2;  xlldot = *xmdot;   omgdt  = *omgdot;  xnodot = *xnodott;
  xnodp  = *xnodpp;
  Deep(1);
  *eosq   = eqsq;    *sinio  = siniq;   *cosio  = cosiq;   *betao  = rteqsq;
  *aodp   = ao;      *theta2 = cosq2;   *sing   = sinomo;  *cosg   = cosomo;
  *betao2 = bsq;     *xmdot  = xlldot;  *omgdot = omgdt;   *xnodott = xnodot;
  *xnodpp  = xnodp;
}; //Procedure Call_dpinit

void Call_dpsec( double *xmdf,double *omgadf,double *xnode,double *emm,double *xincc,
							 double *xnn,double *tsince)
{

  xll    = *xmdf;    omgasm = *omgadf;  xnodes = *xnode;   /*_em     = emm;
  *xinc   = *xincc;*/   xn     = *xnn;  t      = *tsince;
  Deep(2);
  *xmdf   = xll;     *omgadf = omgasm;  *xnode  = xnodes;  *emm    = _em;
  *xincc  = xinc;    *xnn    = xn;      *tsince = t;
}; // Procedure Call_dpsec

void Call_dpper(double *e,double *xincc,double *omgadf,double *xnode,double *xmam)
{
  _em     = *e;       xinc   = *xincc;   omgasm = *omgadf;  xnodes = *xnode;
  xll    = *xmam;
  Deep(3);
  *e      = _em;      *xincc  = xinc;    *omgadf = omgasm;  *xnode  = xnodes;
  *xmam   = xll;
}; //Procedure Call_dpper

BOOL Deep(int ideep)
{
//  const
    double zns    =  1.19459E-5;     double c1ss   =  2.9864797E-6;   double zes    =  0.01675;
    double znl    =  1.5835218E-4;   double c1l    =  4.7968065E-7;   double zel    =  0.05490;
    double zcosis =  0.91744867;     double zsinis =  0.39785416;     double zsings = -0.98088458;
    double zcosgs =  0.1945905;      double zcoshs =  1;              double zsinhs =  0;
    double q22    =  1.7891679E-6;   double q31    =  2.1460748E-6;   double q33    =  2.2123015E-7;
    double g22    =  5.7686396;      double g32    =  0.95240898;     double g44    =  1.8014998;
    double g52    =  1.0508330;      double g54    =  4.4108898;      double root22 =  1.7891679E-6;
    double root32 =  3.7393792E-7;   double root44 =  7.3636953E-9;   double root52 =  1.1428639E-7;
    double root54 =  2.1765803E-9;   double thdt   =  4.3752691E-3;
//  label
    //dpinit
//    5,10,20,30,40,45,50,55,60,65,70,80,
    //dpsec
//    90,100,105,110,120,125,130,140,150,152,154,160,165,170,175,180,
    //dpper
//    205,210,218,220,230;
//const  Typed constants to retain values between ENTRY calls 
    int iresfl	= 0;int isynfl	= 0;
    int iret	= 0;int iretn	= 0;
    int ls		= 0;
    double a1      = 0.0;double    a2      = 0.0;double    a3      = 0.0;
    double a4      = 0.0;double    a5      = 0.0;double    a6      = 0.0;
    double a7      = 0.0;double    a8      = 0.0;double    a9      = 0.0;
    double a10     = 0.0;double    ainv2   = 0.0;double    alfdp   = 0.0;
    double aqnv    = 0.0;double    atime   = 0.0;double    betdp   = 0.0;
    double bfact   = 0.0;double    c       = 0.0;double    cc      = 0.0;
    double cosis   = 0.0;double    cosok   = 0.0;double    cosq    = 0.0;
    double ctem    = 0.0;double    d2201   = 0.0;double    d2211   = 0.0;
    double d3210   = 0.0;double    d3222   = 0.0;double    d4410   = 0.0;
    double d4422   = 0.0;double    d5220   = 0.0;double    d5232   = 0.0;
    double d5421   = 0.0;double    d5433   = 0.0;double    dalf    = 0.0;
    double day     = 0.0;double    dbet    = 0.0;double    del1    = 0.0;
    double del2    = 0.0;double    del3    = 0.0;double    delt    = 0.0;
    double dls     = 0.0;double    e3      = 0.0;double    ee2     = 0.0;
    double eoc     = 0.0;double    eq      = 0.0;double    f2      = 0.0;
    double f220    = 0.0;double    f221    = 0.0;double    f3      = 0.0;
    double f311    = 0.0;double    f321    = 0.0;double    f322    = 0.0;
    double f330    = 0.0;double    f441    = 0.0;double    f442    = 0.0;
    double f522    = 0.0;double    f523    = 0.0;double    f542    = 0.0;
    double f543    = 0.0;double    fasx2   = 0.0;double    fasx4   = 0.0;
    double fasx6   = 0.0;double    ft      = 0.0;double    g200    = 0.0;
    double g201    = 0.0;double    g211    = 0.0;double    g300    = 0.0;
    double g310    = 0.0;double    g322    = 0.0;double    g410    = 0.0;
    double g422    = 0.0;double    g520    = 0.0;double    g521    = 0.0;
    double g532    = 0.0;double    g533    = 0.0;double    gam     = 0.0;
    double omegaq  = 0.0;double    pe      = 0.0;double    pgh     = 0.0;
    double ph      = 0.0;double    pinc    = 0.0;double    pl      = 0.0;
    double preep   = 0.0;double    s1      = 0.0;double    s2      = 0.0;
    double s3      = 0.0;double    s4      = 0.0;double    s5      = 0.0;
    double s6      = 0.0;double    s7      = 0.0;double    savtsn  = 0.0;
    double se      = 0.0;double    se2     = 0.0;double    se3     = 0.0;
    double sel     = 0.0;double    ses     = 0.0;double    sgh     = 0.0;
    double sgh2    = 0.0;double    sgh3    = 0.0;double    sgh4    = 0.0;
    double sghl    = 0.0;double    sghs    = 0.0;double    sh      = 0.0;
    double sh2     = 0.0;double    sh3     = 0.0;double    sh1     = 0.0;
    double shs     = 0.0;double    si      = 0.0;double    si2     = 0.0;
    double si3     = 0.0;double    sil     = 0.0;double    sini2   = 0.0;
    double sinis   = 0.0;double    sinok   = 0.0;double    sinq    = 0.0;
    double sinzf   = 0.0;double    sis     = 0.0;double    sl      = 0.0;
    double sl2     = 0.0;double    sl3     = 0.0;double    sl4     = 0.0;
    double sll     = 0.0;double    sls     = 0.0;double    sse     = 0.0;
    double ssg     = 0.0;double    ssh     = 0.0;double    ssi     = 0.0;
    double ssl     = 0.0;double    stem    = 0.0;double    step2   = 0.0;
    double stepn   = 0.0;double    stepp   = 0.0;double    temp    = 0.0;
    double temp1   = 0.0;double    thgr    = 0.0;double    x1      = 0.0;
    double x2      = 0.0;double    x2li    = 0.0;double    x2omi   = 0.0;
    double x3      = 0.0;double    x4      = 0.0;double    x5      = 0.0;
    double x6      = 0.0;double    x7      = 0.0;double    x8      = 0.0;
    double xfact   = 0.0;double    xgh2    = 0.0;double    xgh3    = 0.0;
    double xgh4    = 0.0;double    xh2     = 0.0;double    xh3     = 0.0;
    double xi2     = 0.0;double    xi3     = 0.0;double    xl      = 0.0;
    double xl2     = 0.0;double    xl3     = 0.0;double    xl4     = 0.0;
    double xlamo   = 0.0;double    xldot   = 0.0;double    xli     = 0.0;
    double xls     = 0.0;double    xmao    = 0.0;double    xnddt   = 0.0;
    double xndot   = 0.0;double    xni     = 0.0;double    xno2    = 0.0;
    double xnodce  = 0.0;double    xnoi    = 0.0;double    xnq     = 0.0;
    double xomi    = 0.0;double    xpidot  = 0.0;double    xqncl   = 0.0;
    double z1      = 0.0;double    z11     = 0.0;double    z12     = 0.0;
    double z13     = 0.0;double    z2      = 0.0;double    z21     = 0.0;
    double z22     = 0.0;double    z23     = 0.0;double    z3      = 0.0;
    double z31     = 0.0;double    z32     = 0.0;double    z33     = 0.0;
    double zcosg   = 0.0;double    zcosgl  = 0.0;double    zcosh   = 0.0;
    double zcoshl  = 0.0;double    zcosi   = 0.0;double    zcosil  = 0.0;
    double ze      = 0.0;double    zf      = 0.0;double    zm      = 0.0;
    double zmo     = 0.0;double    zmol    = 0.0;double    zmos    = 0.0;
    double zn      = 0.0;double    zsing   = 0.0;double    zsingl  = 0.0;
    double zsinh   = 0.0;double    zsinhl  = 0.0;double    zsini   = 0.0;
    double zsinil  = 0.0;double    zx      = 0.0;double    zy      = 0.0;

//	switch (ideep)	{
    if (ideep == dpinit) { // Entrance for deep space initialization 
             thgr = ThetaG(m_Sat.fJulianEpoch);
			 eq = m_Sat.fEccentricity;
             xnq = xnodp;
             aqnv = 1.0/ao;
             xqncl = m_Sat.fInclination;
             xmao = m_Sat.fMeanAnomaly;
             xpidot = omgdt + xnodot;
             sinq = sin(m_Sat.fRightAscending);
             cosq = cos(m_Sat.fRightAscending);
             omegaq = m_Sat.fPeregee;
    // Initialize lunar solar terms 
//DEEP5: Unused ...
			 day = m_Sat.fJulianEpoch - 2433281.5 + 18261.5;  //Days since 1900 Jan 0.5
             if (day == preep)
               goto DEEP10;
             preep = day;
             xnodce = 4.5236020 - 9.2422029E-4*day;
             stem = sin(xnodce);
             ctem = cos(xnodce);
             zcosil = 0.91375164 - 0.03568096*ctem;
             zsinil = sqrt(1.0 - zcosil*zcosil);
             zsinhl = 0.089683511*stem/zsinil;
             zcoshl = sqrt(1.0 - zsinhl*zsinhl);
             c = 4.7199672 + 0.22997150*day;
             gam = 5.8351514 + 0.0019443680*day;
             zmol = Fmod2p(c - gam);
             zx = 0.39785416*stem/zsinil;
             zy = zcoshl*ctem + 0.91744867*zsinhl*stem;
             zx = AcTan(zx,zy);
             zx = gam + zx - xnodce;
             zcosgl = cos(zx);
             zsingl = sin(zx);
             zmos = 6.2565837 + 0.017201977*day;
             zmos = Fmod2p(zmos);
    // Do solar terms 
DEEP10:		 savtsn = 1E20;
             zcosg = zcosgs;
             zsing = zsings;
             zcosi = zcosis;
             zsini = zsinis;
             zcosh = cosq;
             zsinh = sinq;
             cc = c1ss;
             zn = zns;
             ze = zes;
             zmo = zmos;
             xnoi = 1.0/xnq;
             ls = 30; //assign 30 to ls
DEEP20:		 a1 = zcosg*zcosh + zsing*zcosi*zsinh;
             a3 = -zsing*zcosh + zcosg*zcosi*zsinh;
             a7 = -zcosg*zsinh + zsing*zcosi*zcosh;
             a8 = zsing*zsini;
             a9 = zsing*zsinh + zcosg*zcosi*zcosh;
             a10 = zcosg*zsini;
             a2 = cosiq*a7 +  siniq*a8;
             a4 = cosiq*a9 +  siniq*a10;
             a5 = -siniq*a7 +  cosiq*a8;
             a6 = -siniq*a9 +  cosiq*a10;
             x1 = a1*cosomo + a2*sinomo;
             x2 = a3*cosomo + a4*sinomo;
             x3 = -a1*sinomo + a2*cosomo;
             x4 = -a3*sinomo + a4*cosomo;
             x5 = a5*sinomo;
             x6 = a6*sinomo;
             x7 = a5*cosomo;
             x8 = a6*cosomo;
             z31 = 12.0*x1*x1 - 3.0*x3*x3;
             z32 = 24.0*x1*x2 - 6.0*x3*x4;
             z33 = 12.0*x2*x2 - 3.0*x4*x4;
             z1 = 3.0*(a1*a1 + a2*a2) + z31*eqsq;
             z2 = 6.0*(a1*a3 + a2*a4) + z32*eqsq;
             z3 = 3.0*(a3*a3 + a4*a4) + z33*eqsq;
             z11 = -6.0*a1*a5 + eqsq*(-24.0*x1*x7 - 6.0*x3*x5);
             z12 = -6.0*(a1*a6 + a3*a5)
                  + eqsq*(-24.0*(x2*x7 + x1*x8) - 6.0*(x3*x6 + x4*x5));
             z13 = -6.0*a3*a6 + eqsq*(-24.0*x2*x8 - 6.0*x4*x6);
             z21 = 6.0*a2*a5 + eqsq*(24.0*x1*x5 - 6.0*x3*x7);
             z22 = 6.0*(a4*a5 + a2*a6)
                  + eqsq*(24.0*(x2*x5 + x1*x6) - 6.0*(x4*x7 + x3*x8));
             z23 = 6.0*a4*a6 + eqsq*(24.0*x2*x6 - 6.0*x4*x8);
             z1 = z1 + z1 + bsq*z31;
             z2 = z2 + z2 + bsq*z32;
             z3 = z3 + z3 + bsq*z33;
             s3 = cc*xnoi;
             s2 = -0.5*s3/rteqsq;
             s4 = s3*rteqsq;
             s1 = -15.0*eq*s4;
             s5 = x1*x3 + x2*x4;
             s6 = x2*x3 + x1*x4;
             s7 = x2*x4 - x1*x3;
             se = s1*zn*s5;
             si = s2*zn*(z11 + z13);
             sl = -zn*s3*(z1 + z3 - 14.0 - 6.0*eqsq);
             sgh = s4*zn*(z31 + z33 - 6.0);
             sh = -zn*s2*(z21 + z23);
             if (xqncl < 5.2359877E-2)
               sh = 0.0;
             ee2 = 2.0*s1*s6;
             e3 = 2.0*s1*s7;
             xi2 = 2.0*s2*z12;
             xi3 = 2.0*s2*(z13 - z11);
             xl2 = -2.0*s3*z2;
             xl3 = -2.0*s3*(z3 - z1);
             xl4 = -2.0*s3*(-21.0 - 9.0*eqsq)*ze;
             xgh2 = 2.0*s4*z32;
             xgh3 = 2.0*s4*(z33 - z31);
             xgh4 = -18.0*s4*ze;
             xh2 = -2.0*s2*z22;
             xh3 = -2.0*s2*(z23 - z21);
             if (ls == 30) goto DEEP30;
             else if (ls == 40) goto DEEP40;
             else	return TRUE;
    // Do lunar terms 
DEEP30:		 sse = se;
             ssi = si;
             ssl = sl;
             ssh = sh/siniq;
             ssg = sgh - cosiq*ssh;
             se2 = ee2;
             si2 = xi2;
             sl2 = xl2;
             sgh2 = xgh2;
             sh2 = xh2;
             se3 = e3;
             si3 = xi3;
             sl3 = xl3;
             sgh3 = xgh3;
             sh3 = xh3;
             sl4 = xl4;
             sgh4 = xgh4;
             zcosg = zcosgl;
             zsing = zsingl;
             zcosi = zcosil;
             zsini = zsinil;
             zcosh = zcoshl*cosq + zsinhl*sinq;
             zsinh = sinq*zcoshl - cosq*zsinhl;
             zn = znl;
             cc = c1l;
             ze = zel;
             zmo = zmol;
             ls = 40; //assign 40 to ls
             goto DEEP20;
DEEP40:		 sse = sse + se;
             ssi = ssi + si;
             ssl = ssl + sl;
             ssg = ssg + sgh - cosiq/siniq*sh;
             ssh = ssh + sh/siniq;
    // Geopotential resonance initialization for 12 hour orbits 
             iresfl = 0;
             isynfl = 0;
             if ((xnq < 0.0052359877) and (xnq > 0.0034906585))
               goto DEEP70;
             if ((xnq < 8.26E-3) or (xnq > 9.24E-3))
               return TRUE;
             if (eq < 0.5)
               return TRUE;
             iresfl = 1;
             eoc = eq*eqsq;
             g201 = -0.306 - (eq - 0.64)*0.440;
             if (eq > 0.65)
               goto DEEP45;
             g211 = 3.616 - 13.247*eq + 16.290*eqsq;
             g310 = -19.302 + 117.390*eq - 228.419*eqsq + 156.591*eoc;
             g322 = -18.9068 + 109.7927*eq - 214.6334*eqsq + 146.5816*eoc;
             g410 = -41.122 + 242.694*eq - 471.094*eqsq + 313.953*eoc;
             g422 = -146.407 + 841.880*eq - 1629.014*eqsq + 1083.435*eoc;
             g520 = -532.114 + 3017.977*eq - 5740.0*eqsq + 3708.276*eoc;
             goto DEEP55;
DEEP45:		 g211 = -72.099 + 331.819*eq - 508.738*eqsq + 266.724*eoc;
             g310 = -346.844 + 1582.851*eq - 2415.925*eqsq + 1246.113*eoc;
             g322 = -342.585 + 1554.908*eq - 2366.899*eqsq + 1215.972*eoc;
             g410 = -1052.797 + 4758.686*eq - 7193.992*eqsq + 3651.957*eoc;
             g422 = -3581.69 + 16178.11*eq - 24462.77*eqsq + 12422.52*eoc;
             if (eq > 0.715)
               goto DEEP50;
             g520 = 1464.74 - 4664.75*eq + 3763.64*eqsq;
             goto DEEP55;
DEEP50:		 g520 = -5149.66 + 29936.92*eq - 54087.36*eqsq + 31324.56*eoc;
DEEP55:		 if (eq >= (0.7))
               goto DEEP60;
             g533 = -919.2277 + 4988.61*eq - 9064.77*eqsq + 5542.21*eoc;
             g521 = -822.71072 + 4568.6173*eq - 8491.4146*eqsq + 5337.524*eoc;
             g532 = -853.666 + 4690.25*eq - 8624.77*eqsq + 5341.4*eoc;
               goto DEEP65;
DEEP60:		 g533 = -37995.78 + 161616.52*eq - 229838.2*eqsq + 109377.94*eoc;
             g521 = -51752.104 + 218913.95*eq - 309468.16*eqsq + 146349.42*eoc;
             g532 = -40023.88 + 170470.89*eq - 242699.48*eqsq + 115605.82*eoc;
DEEP65:		 sini2 = siniq*siniq;
             f220 = 0.75*(1.0 + 2.0*cosiq + cosq2);
             f221 = 1.5*sini2;
             f321 = 1.875*siniq*(1.0 - 2.0*cosiq - 3.0*cosq2);
             f322 = -1.875*siniq*(1.0 + 2.0*cosiq - 3.0*cosq2);
             f441 = 35.0*sini2*f220;
             f442 = 39.3750*sini2*sini2;
             f522 = 9.84375*siniq*(sini2*(1.0 - 2.0*cosiq - 5.0*cosq2)
                   + 0.33333333*(-2.0 + 4.0*cosiq + 6.0*cosq2));
             f523 = siniq*(4.92187512*sini2*(-2.0 - 4.0*cosiq + 10.0*cosq2)
                   + 6.56250012*(1.0 + 2.0*cosiq - 3.0*cosq2));
             f542 = 29.53125*siniq*(2.0 - 8.0*cosiq + cosq2*(-12.0 + 8.0*cosiq + 10.0*cosq2));
             f543 = 29.53125*siniq*(-2.0 - 8.0*cosiq + cosq2*(12.0 + 8.0*cosiq - 10.0*cosq2));
             xno2 = xnq*xnq;
             ainv2 = aqnv*aqnv;
             temp1 = 3.0*xno2*ainv2;
             temp = temp1*root22;
             d2201 = temp*f220*g201;
             d2211 = temp*f221*g211;
             temp1 = temp1*aqnv;
             temp = temp1*root32;
             d3210 = temp*f321*g310;
             d3222 = temp*f322*g322;
             temp1 = temp1*aqnv;
             temp = 2.0*temp1*root44;
             d4410 = temp*f441*g410;
             d4422 = temp*f442*g422;
             temp1 = temp1*aqnv;
             temp = temp1*root52;
             d5220 = temp*f522*g520;
             d5232 = temp*f523*g532;
             temp = 2.0*temp1*root54;
             d5421 = temp*f542*g521;
             d5433 = temp*f543*g533;
             xlamo = xmao + m_Sat.fRightAscending + m_Sat.fRightAscending - thgr - thgr;
             bfact = xlldot + xnodot + xnodot - thdt - thdt;
             bfact = bfact + ssl + ssh + ssh;
               goto DEEP80;
    // Synchronous resonance terms initialization 
DEEP70:		 iresfl = 1;
             isynfl = 1;
             g200 = 1.0 + eqsq*(-2.5 + 0.8125*eqsq);
             g310 = 1.0 + 2.0*eqsq;
             g300 = 1.0 + eqsq*(-6.0 + 6.60937*eqsq);
             f220 = 0.75*(1.0 + cosiq)*(1.0 + cosiq);
             f311 = 0.9375*siniq*siniq*(1.0 + 3*cosiq) - 0.75*(1.0 + cosiq);
             f330 = 1.0 + cosiq;
             f330 = 1.875*f330*f330*f330;
             del1 = 3.0*xnq*xnq*aqnv*aqnv;
             del2 = 2.0*del1*f220*g200*q22;
             del3 = 3.0*del1*f330*g300*q33*aqnv;
             del1 = del1*f311*g310*q31*aqnv;
             fasx2 = 0.13130908;
             fasx4 = 2.8843198;
             fasx6 = 0.37448087;
             xlamo = xmao + m_Sat.fRightAscending + m_Sat.fPeregee - thgr;
             bfact = xlldot + xpidot - thdt;
             bfact = bfact + ssl + ssg + ssh;
DEEP80:		 xfact = bfact - xnq;
    // Initialize integrator 
             xli = xlamo;
             xni = xnq;
             atime = 0.0;
             stepp = 720.0;
             stepn = -720.0;
             step2 = 259200.0;
             } //dpinit
	else if (ideep == dpsec) { // Entrance for deep space secular effects 
             xll = xll + ssl*t;
             omgasm = omgasm + ssg*t;
             xnodes = xnodes + ssh*t;
             _em = m_Sat.fEccentricity + sse*t;
             xinc = m_Sat.fInclination + ssi*t;
             if (xinc >= 0.0)
               goto DEEP90;
             xinc = -xinc;
             xnodes = xnodes  +  PI;
             omgasm = omgasm - PI;
DEEP90:		 if (iresfl == 0) 
               return TRUE;
DEEP100:	 if (atime == 0.0)
               goto DEEP170;
             if ((t >= 0.0) and (atime < 0.0))
               goto DEEP170;
             if ((t < 0.0) and (atime >= 0.0))
               goto DEEP170;
//DEEP105:	 Unused ...
			 if (fabs(t) >= fabs(atime))
               goto DEEP120;
             delt = stepp;
             if (t >= 0.0)
               delt = stepn;
//DEEP110:	 Unused ...
			 iret = 100; //assign 100 to iret
             goto DEEP160;
DEEP120:	 delt = stepn;
             if (t > 0.0)
               delt = stepp;
DEEP125:	 if (fabs(t - atime) < stepp)
               goto DEEP130;
             iret = 125; //assign 125 to iret
             goto DEEP160;
DEEP130:	 ft = t - atime;
             iretn = 140; //assign 140 to iretn
             goto DEEP150;
DEEP140:	 xn = xni + xndot*ft + xnddt*ft*ft*0.5;
             xl = xli + xldot*ft + xndot*ft*ft*0.5;
             temp = -xnodes + thgr + t*thdt;
             xll = xl - omgasm + temp;
             if (isynfl == 0)
               xll = xl + temp + temp;
             return TRUE;
    // Dot terms calculated 
DEEP150:	 if (isynfl == 0)
               goto DEEP152;
             xndot = del1*sin(xli - fasx2) + del2*sin(2.0*(xli - fasx4))
                    + del3*sin(3.0*(xli - fasx6));
             xnddt = del1*cos(xli - fasx2)
                    + 2.0*del2*cos(2.0*(xli - fasx4))
                    + 3.0*del3*cos(3.0*(xli - fasx6));
             goto DEEP154;
DEEP152:	 xomi = omegaq + omgdt*atime;
             x2omi = xomi + xomi;
             x2li = xli + xli;
             xndot = d2201*sin(x2omi + xli - g22)
                    + d2211*sin(xli - g22)
                    + d3210*sin(xomi + xli - g32)
                    + d3222*sin(-xomi + xli - g32)
                    + d4410*sin(x2omi + x2li - g44)
                    + d4422*sin(x2li - g44)
                    + d5220*sin(xomi + xli - g52)
                    + d5232*sin(-xomi + xli - g52)
                    + d5421*sin(xomi + x2li - g54)
                    + d5433*sin(-xomi + x2li - g54);
             xnddt = d2201*cos(x2omi + xli - g22)
                    + d2211*cos(xli - g22)
                    + d3210*cos(xomi + xli - g32)
                    + d3222*cos(-xomi + xli - g32)
                    + d5220*cos(xomi + xli - g52)
                    + d5232*cos(-xomi + xli - g52)
                    + 2.0*(d4410*cos(x2omi + x2li - g44)
                    + d4422*cos(x2li - g44)
                    + d5421*cos(xomi + x2li - g54)
                    + d5433*cos(-xomi + x2li - g54));
DEEP154:	 xldot = xni + xfact;
             xnddt = xnddt*xldot;
               if (iretn == 140) goto DEEP140;
               else if (iretn == 165) goto DEEP165;
			   else return TRUE;
//             } //case
    // Integrator 
DEEP160:	 iretn = 165; //assign 165 to iretn
             goto DEEP150;
DEEP165:	 xli = xli + xldot*delt + xndot*step2;
             xni = xni + xndot*delt + xnddt*step2;
             atime = atime + delt;
             if(iret == 100) goto DEEP100;
             else if(iret == 125) goto DEEP125;
             else return TRUE;
//             }; //case
    // Epoch restart 
DEEP170:	 if (t >= 0)
               goto DEEP175;
             delt = stepn;
             goto DEEP180;
DEEP175:	 delt = stepp;
DEEP180:	 atime = 0.0;
             xni = xnq;
             xli = xlamo;
             goto DEEP125;
             } //dpsec
    else if(ideep ==  dpper) { // Entrance for lunar-solar periodics 
             sinis = sin(xinc);
             cosis = cos(xinc);
             if (fabs(savtsn - t) < 30.0)
               goto DEEP210;
             savtsn = t;
             zm = zmos + zns*t;
//DEEP205:	 Unused ...
			 zf = zm + 2.0*zes*sin(zm);
             sinzf = sin(zf);
             f2 = 0.5*sinzf*sinzf - 0.25;
             f3 = -0.5*sinzf*cos(zf);
             ses = se2*f2 + se3*f3;
             sis = si2*f2 + si3*f3;
             sls = sl2*f2 + sl3*f3 + sl4*sinzf;
             sghs = sgh2*f2 + sgh3*f3 + sgh4*sinzf;
             shs = sh2*f2 + sh3*f3;
             zm = zmol + znl*t;
             zf = zm + 2.0*zel*sin(zm);
             sinzf = sin(zf);
             f2 = 0.5*sinzf*sinzf - 0.25;
             f3 = -0.5*sinzf*cos(zf);
             sel = ee2*f2 + e3*f3;
             sil = xi2*f2 + xi3*f3;
             sll = xl2*f2 + xl3*f3 + xl4*sinzf;
             sghl = xgh2*f2 + xgh3*f3 + xgh4*sinzf;
             sh1 = xh2*f2 + xh3*f3;
             pe = ses + sel;
             pinc = sis + sil;
             pl = sls + sll;
DEEP210:	 pgh = sghs + sghl;
             ph = shs + sh1;
             xinc = xinc + pinc;
             _em = _em + pe;
             if (xqncl < 0.2)
               goto DEEP220;
             goto DEEP218;
    // Apply periodics directly 
DEEP218:	 ph = ph/siniq;
             pgh = pgh - cosiq*ph;
             omgasm = omgasm + pgh;
             xnodes = xnodes + ph;
             xll = xll + pl;
             goto DEEP230;
    // Apply periodics with Lyddane modification 
DEEP220:	 sinok = sin(xnodes);
             cosok = cos(xnodes);
             alfdp = sinis*sinok;
             betdp = sinis*cosok;
             dalf = ph*cosok + pinc*cosis*sinok;
             dbet = -ph*sinok + pinc*cosis*cosok;
             alfdp = alfdp + dalf;
             betdp = betdp + dbet;
             xls = xll + omgasm + cosis*xnodes;
             dls = pl + pgh - pinc*xnodes*sinis;
             xls = xls + dls;
             xnodes = AcTan(alfdp,betdp);
             xll = xll + pl;
             omgasm = xls - xll - cos(xinc)*xnodes;
DEEP230: ;	 }; //dpper
//    } //case
 return TRUE;
}; //Procedure Deep

double ThetaG(double jd)
{
// Reference:  The 1992 Astronomical Almanac, page B6. 
	double	UT,TU,GMST;
  UT	= modf(jd + 0.5, &jd);
  TU	= (jd - 2451545.0)/36525.0;
  GMST	= 24110.54841 + TU * (8640184.812866 + TU * (0.093104 - TU * 6.2E-6));
  GMST	= Modulus(GMST + secday*omega_E*UT,secday);
  return (2.0*PI*GMST/secday);
}

double Modulus(double arg1, double arg2)
{
	double modu;
	modu = arg1 - (long)(arg1/arg2) * arg2;
	if (modu >= 0.0)	return modu;
	else			return modu + arg2;
}
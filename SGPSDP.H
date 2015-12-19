
#define DLLEXPORT __declspec(dllexport) 
// for the inclusion in other files write :
//#define DLLEXPORT __declspec(dllimport)

//* Already defined in  >stdafx.h<
typedef struct	tagVECTOR	{ 
	double x,y,z,w;
} VECTOR;

typedef struct tagSATELLITE	{
	char	cSatelliteName[23];
	int		iSecondMeanMotion;
	int		iSatelliteNumber;
	int		iLaunchYear;
	int		iLaunchNumber;
	char	cLaunchPiece[3];
	int		iEpochYear;
	double	fEpochDay;
	int		iEpochDay;
	double	fEpochFraction;
	double	fBalisticCoefficient;
	double	fSecondMeanMotion;
	double	fRadiationCoefficient;
	char	cEmphemeristType[2];
	int		iElementNumber;

	double	fInclination;
	double	fRightAscending;
	double	fEccentricity;
	double	fPeregee;
	double	fMeanAnomaly;
	double	fMeanMotion;
	int		iRevAtEpoch;
	double  fJulianEpoch;
}	SATELLITE;

#ifndef PI
#define PI	3.141592654
#endif

class DLLEXPORT CSGP4_SDP4 //: public CObject
{                      
protected:	// the public accesible data members ...
	char		m_cLine0[23], m_cLine1[70], m_cLine2[70];

	SATELLITE	m_Sat;
	VECTOR		m_vLLA;
	VECTOR		m_vPOS;
	VECTOR		m_vVEL;
	VECTOR		m_vUPos;
	VECTOR		m_vUVel;
	VECTOR		m_vObs;
	VECTOR		m_vRad;
	double		m_fTime;
protected:
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

	double		Modulus(double a1,double a2);	// returns the remainder 
	void		ConvertData();
	void		Init();
	long		round (double arg);
	double		Fmod2p (double arg);
	double		AcTan(double sinx, double cosx);
	double		sqr (double arg);

protected:
	// the two routines to calculate the satellite position ...
	BOOL SGP4(double tsince,int *iflag,VECTOR *pos, VECTOR *vel);
	BOOL SDP4(double tsince, int *iflag,VECTOR *pos,VECTOR *vel);
	// some subroutines used by SDP4 / SGP4
	void Call_dpper(double *e,double *xincc,double *omgadf,double *xnode,double *xmam);
	void Call_dpsec(double *xmdf,double *omgadf,double *xnode,double *emm,double *xincc,
					double *xnn,double *tsince);
	void Call_dpinit(double *eosq,double *sinio,double *cosio,double *betao,double *aodp,
					 double *theta2,double *sing,double *cosg,double *betao2,double *xmdot,
					 double *omgdot,double *xnodott,double *xnodpp);
	BOOL Deep(int ideep);
public:
	// the first constructor is for direct use with the Norad2Line elements
//	CSGP4_SDP4 (CString m_csLine0, CString m_csLine1, CString m_csLine2);
	CSGP4_SDP4 (char *m_cLine0, char *m_cLine1, char *m_cLine2);
	// this will be the preffered constructor, because there is no calculation for the Satellite needed
	CSGP4_SDP4 (SATELLITE *pSat);
	// this constructor is usefull for accessing the time/math - routines, without any sat data
	CSGP4_SDP4 ();      // the default constructor ...
	// and the destructor of course !!!
	~CSGP4_SDP4 ();
	// This both routines set a new satellite as datas
//	void SetSatellite (CString m_csLine0, CString m_csLine1, CString m_csLine2);
	void SetSatellite (char *m_cLine0, char *m_cLine1, char *m_cLine2);
	void SetSatellite (SATELLITE *pSat);

	// This routine will automatically determine, which routine to use (SDP4 / SGP4)
	// It is the main function of this DLL ...
	BOOL SGP(double time);
	// This routine will convert the pos and vel vector to units (km and km/sec)
	void ConvertSatState(VECTOR *pos,VECTOR *vel);
	// And this one calculate the Lattitude / Longitude and altitude of pos at time in ->pLLA
	void CalculateLatLonAlt(double time);
	// Calculates the User pos and vel, and return m_vUPos and m_vUVel
	void CalculateUserPosVel(VECTOR *geodetic, double time);
	// Calculates the Observer data for a given Satellite and returns m_vObs
	BOOL CalculateObs(VECTOR pos, VECTOR vel, VECTOR geodetic, double time);
	// Calculates the Observer data in topocentric data and return m_vRad
	void CalculateRADec(VECTOR pos, VECTOR vel, VECTOR geodetic, double time);

	// Here ar the accesible routines ...
	double		GetFloat (int iStart, int iEnd, char *cLine);	// extracts a float form a string
	long		GetInt   (int iStart, int iEnd, char *cLine);	// extracts a int from a string
	char	   *GetString(int iStart, int iEnd, char *cLine);	// extracts a string from the string
	double		RadToDeg (double arg);			// converts radians to degree
	double		DegToRad (double arg);			// converts degree to radians
	double		JulianDate (SYSTEMTIME st);		// returns the julian date from SYSTEMTIME
	double		JulianDate (CTime ct);			// returns the julian date from CTime
	double		JulianDate (double st);			// returns the julian date from the Norad2Line epoch style
	double		ThetaG(double jd);				// returns the sideral GMT
	SYSTEMTIME	CalendarDate (double dJulian);	// returns the date from a julian date
	double		SideralTime(double jd);			// returns the sideral time at jd
	VECTOR		GetPos();		// returns the calculated Position vector
	VECTOR		GetVel();		// returns the calculated Velocity vector
	double		GetLat();		// returns the calculated Lattitude 
	double		GetLon();		// returns the calculated Longitude
	double		GetAlt();		// returns the calculated Altitude
	double		GetTime();		// returns the actual time
	VECTOR		GetUserPos();	// returns the calculated Position vector
	VECTOR		GetUserVel();	// returns the calculated Velocity vector
	VECTOR		GetObserver();	// returns the calculated Position vector
	VECTOR		GetRADec();		// returns the calculated Velocity vector
	char	   *GetVersion();	// returns a pointer to the version of this dll
	void	   *GetSatellite();	// returns a pointer to th actual satellite
};
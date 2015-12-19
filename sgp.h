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

typedef struct	tagVECTOR	{ 
	double x,y,z,w;
} VECTOR;

#ifndef PI
#define PI	3.141592654
#endif


void SetSatellite(SATELLITE pSatellite);
void Init();
double JulianDate (double st);
double sqr (double arg);
double DegToRad (double arg);
void ConvertSatState(VECTOR *pos,VECTOR *vel);
BOOL CalcSGP4(double tsince,int *iflag, VECTOR *pos, VECTOR *vel);
BOOL CalcSDP4(double tsince,int *iflag,VECTOR *pos, VECTOR *vel);
double AcTan(double sinx, double cosx);
double Fmod2p (double arg);
void Call_dpinit(double *eosq,double *sinio,double *cosio,double *betao,double *aodp,
							double *theta2,double *sing,double *cosg,double *betao2,double *xmdot,
							double *omgdot,double *xnodott,double *xnodpp);
void Call_dpsec( double *xmdf,double *omgadf,double *xnode,double *emm,double *xincc,
							 double *xnn,double *tsince);
void Call_dpper(double *e,double *xincc,double *omgadf,double *xnode,double *xmam);
BOOL Deep(int ideep);
double ThetaG(double jd);
double Modulus(double arg1, double arg2);
void CalculateLatLonAlt(double time);
BOOL CalculateObs(VECTOR pos, VECTOR vel, VECTOR geodetic, double time);
void CalculateUserPosVel(VECTOR *geodetic, double time);
double RadToDeg (double arg);
//WINAPI sgp(SATELLITE *pSatellite, SYSTEMTIME time, VECTOR geodetic, VECTOR pos, VECTOR vel, VECTOR m_vObs, VECTOR m_Vlla);
WINAPI AGCalcSGP(void);
//SYSTEMTIME time, VECTOR geodetic, VECTOR pos, VECTOR vel, VECTOR m_vObs, VECTOR m_Vlla);



WINAPI TestAgSgp(void);



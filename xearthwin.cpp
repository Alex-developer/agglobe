/*
 * xearthwin.cpp
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * Main xearth for Windows module.
 */

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <setjmp.h>
#pragma hdrstop

#include "xearth.h"
#include "settings.h"
#include "sgp.h"

//const char *DefaultRegistryKey = "Software\\Software Gems\\xearth for Windows";

HANDLE TerminateEvent;
HANDLE RefreshEvent;
DWORD LastRefresh;

HINSTANCE g_hInstance;
HWND MainWindow;

bool InProperties = false;

extern int main(int argc, char **argv);
extern jmp_buf fataljmp;

char **tokenize(char *s, int *argc_ret)
{
  *argc_ret = 0;
  char **r = NULL;
  char *p = strtok(s, " ");
  while (p) {
    int newargc = *argc_ret + 1;
    r = (char **)realloc(r, newargc*sizeof(char *));
    r[*argc_ret] = p;
    *argc_ret = newargc;
    p = strtok(NULL, " ");
  }
  return r;
}

bool DoRefresh()
{
    char cmdline[1024];
    sprintf(cmdline, "xearth -bmp");
    strcat(cmdline, Settings.proj ? " -proj merc" : " -proj orth");
    if (Settings.pos.type != Position::_default) {
      strcat(cmdline, " -pos");
      switch (Settings.pos.type) {
        case Position::fixed:
          sprintf(&cmdline[strlen(cmdline)], " fixed,%f,%f", Settings.pos.latitude, Settings.pos.longitude);
          break;
        case Position::sunrel:
          sprintf(&cmdline[strlen(cmdline)], " sunrel,%f,%f", Settings.pos.rlatitude, Settings.pos.rlongitude);
          break;
        case Position::orbit:
          sprintf(&cmdline[strlen(cmdline)], " orbit,%f,%f", Settings.pos.period, Settings.pos.inclination);
          break;
        case Position::random:
          strcat(cmdline, " random");
          break;
      }
    }
    if (Settings.rot != 0) {
      sprintf(&cmdline[strlen(cmdline)], " -rot %f", Settings.rot);
    }
    if (Settings.sunpos.type != Position::_default) {
      strcat(cmdline, " -sunpos");
      switch (Settings.sunpos.type) {
        case Position::fixed:
          sprintf(&cmdline[strlen(cmdline)], " %f,%f", Settings.sunpos.latitude, Settings.sunpos.longitude);
          break;
      }
    }
    if (Settings.mag != 1) {
      sprintf(&cmdline[strlen(cmdline)], " -mag %f", Settings.mag);
    }
    strcat(cmdline, " -size");
    if (Settings.size.cx == 0 || Settings.size.cy == 0) {
      sprintf(&cmdline[strlen(cmdline)], " %d,%d", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    } else {
      sprintf(&cmdline[strlen(cmdline)], " %d,%d", Settings.size.cx, Settings.size.cy);
    }
    if (Settings.shift.x != 0 || Settings.shift.y != 0) {
      sprintf(&cmdline[strlen(cmdline)], " -shift %d,%d", Settings.shift.x, Settings.shift.y);
    }
    strcat(cmdline, Settings.shade ? " -shade" : " -noshade");
    strcat(cmdline, Settings.label ? " -label" : " -nolabel");
    if (Settings.label) {
      sprintf(&cmdline[strlen(cmdline)], " -labelpos %d", Settings.labelpos);
    }
    strcat(cmdline, Settings.markers ? " -markers" : " -nomarkers");
    // markerfile
    strcat(cmdline, Settings.stars ? " -stars" : " -nostars");
    if (Settings.stars) {
      sprintf(&cmdline[strlen(cmdline)], " -starfreq %f", Settings.starfreq);
      if (Settings.bigstars > 0) {
        sprintf(&cmdline[strlen(cmdline)], " -bigstars %d", Settings.bigstars);
      }
    }
    strcat(cmdline, Settings.grid ? " -grid" : " -nogrid");
    if (Settings.grid) {
      sprintf(&cmdline[strlen(cmdline)], " -grid1 %d", Settings.grid1);
      sprintf(&cmdline[strlen(cmdline)], " -grid2 %d", Settings.grid2);
    }
    if (Settings.shade) {
      sprintf(&cmdline[strlen(cmdline)], " -day %d", Settings.day);
      sprintf(&cmdline[strlen(cmdline)], " -night %d", Settings.night);
      sprintf(&cmdline[strlen(cmdline)], " -term %d", Settings.term);
    }
    sprintf(&cmdline[strlen(cmdline)], " -gamma %f", Settings.gamma);
    if (Settings.timewarp != 1) {
      sprintf(&cmdline[strlen(cmdline)], " -timewarp %f", Settings.timewarp);
    }
    if (Settings.time) {
      sprintf(&cmdline[strlen(cmdline)], " -time %d", Settings.time);
    }
    int argc;
    char **argv = tokenize(cmdline, &argc);
    int err = setjmp(fataljmp);
    if (err == 0) {
      main(argc, argv);
    } else {
      const char *msg = reinterpret_cast<const char *>(err);
		return false;
	}
    free(argv);
  return true;
}

BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,  // handle to DLL module
  DWORD fdwReason,     // reason for calling function
  LPVOID lpvReserved   // reserved
  )
{
  return 1;
}
 
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	char szLabel[100];
	int i;

	Settings.pos.type = Position::fixed;
	Settings.pos.latitude = 52;
	Settings.pos.longitude = 0;
//	Settings.size.cx = nWidth;
//	Settings.size.cy = nHeight;
	Settings.label = true;
	Settings.labelpos = 1;
	Settings.markers = true;
	Settings.stars = true;
	Settings.grid = true;

	Settings.shade = true;
//	Settings.day = dDay;
//	Settings.night = dNight;
//	Settings.term = dTerm;
//	Settings.time = lTime;

	Settings.satpos.lat = 54;
	Settings.satpos.lon = 12;
	strcpy(szLabel,"AO-40");
	strcpy(Settings.szMarkerPath,"built-in");
	Settings.satpos.label = (char *)szLabel;
	Settings.lElements = 500;
	for (i=0;i<500;i++)
	{
		Settings.satTrack[i].lat = i;
		Settings.satTrack[i].lon = 1;
		Settings.satTrack[i].lColour = i;
	}
	Settings.LoadMarkers(false);
	DoRefresh();

//	TestAgSgp();

	return 0;
} 


WINAPI  GenerateGlobe( double dLat , double dLon, long nWidth, long nHeight, bool bLabel, bool bLocations, LPSTR szLocsFile,long dDay, long dNight, long dTerm, bool bShade, bool bStars, bool bGrid, long lTime, long lSatLat, long lSatLon, LPSTR szSatName, SAFEARRAY **psaLat,SAFEARRAY **psaLon, SAFEARRAY **psaColour)
{
	long i;
	long lElements;

    double *pTrackLats;
	double *pTrackLons;
	COLORREF *pTrackColour;
      
	Settings.pos.type = Position::fixed;
	Settings.pos.latitude = dLat;
	Settings.pos.longitude = dLon;
	Settings.size.cx = nWidth;
	Settings.size.cy = nHeight;
	Settings.label = bLabel;
	Settings.labelpos = 1;
	Settings.markers = bLocations;
	strcpy(Settings.szMarkerPath,szLocsFile);
	Settings.stars = bStars;
	Settings.grid = bGrid;

	Settings.shade = bShade;
	Settings.day = dDay;
	Settings.night = dNight;
	Settings.term = dTerm;
	Settings.time = lTime;

	Settings.satpos.lat = lSatLat;
	Settings.satpos.lon = lSatLon;
	Settings.satpos.label = (char *)szSatName;

//	Settings.bDisplayondesktop = bDeskTop;

	// how many elements are there in the array
	lElements=(*psaLat)->rgsabound[0].cElements;
	if (lElements>500)
		lElements = 500;

	Settings.lElements = lElements;

	pTrackLats=(double*) (*psaLat)->pvData;
	pTrackLons=(double*) (*psaLon)->pvData;
	pTrackColour = (COLORREF *) (*psaColour)->pvData;
	
	for (i=0;i<lElements;i++)
	{
		Settings.satTrack[i].lat = pTrackLats[i];
		Settings.satTrack[i].lon = pTrackLons[i];
		Settings.satTrack[i].lColour = pTrackColour[i];
	}
	Settings.LoadMarkers(false);
	
	
	return DoRefresh();
}

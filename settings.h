/*
 * settings.h
 *
 * Copyright (C) 1996 Greg Hewgill
 *
 * Persistent settings support
 *
 */

#ifndef __SETTINGS_H
#define __SETTINGS_H

#include <list>

// Image: proj, rot, mag, size, shift
// Viewpoint: pos
// Sun: sunpos
// Labels: label, labelpos, markers, markerfile
// Quakes: quakes, qdelay, qupdated
// Dots: stars, starfreq, bigstars, grid, grid1, grid2
// Shading: shade, day, night, term
// Time: timewarp, time
// Display: wait, gamma

struct Position {
  enum Type {_default, fixed, sunrel, orbit, random} type;
  float latitude, longitude;
  float rlatitude, rlongitude;
  float period, inclination;
};

struct marker {
  std::string name;
  float latitude;
  float longitude;
};

class TSettings {
public:
  TSettings();
  void Save();
  void LoadMarkers(bool reset);
  void SaveMarkers();

  POINT properties;
  DWORD proj; // 0=orthographic, 1=mercator
  Position pos;
  float rot;
  Position sunpos;
  float mag;
  SIZE size;
  POINT shift;
  BOOL shade;
  BOOL label;
  DWORD labelpos;
  BOOL markers;
  //char markerfile[MAX_PATH];
  std::list<marker> markerlist;
  BOOL stars;
  float starfreq;
  DWORD bigstars;
  BOOL grid;
  DWORD grid1;
  DWORD grid2;
  DWORD day;
  DWORD night;
  DWORD term;
  float gamma;
  DWORD wait;
  float timewarp;
  DWORD time;
  BOOL quakes;
  DWORD qdelay;
  DWORD qupdated;
  MarkerInfo satpos;
  MarkerInfo satTrack[1000];
  long lElements;
  char szMarkerPath[200];
  bool bDisplayondesktop;
//  char font_name[MAX_PATH];
//  DWORD font_size;
};

extern TSettings Settings;

#endif

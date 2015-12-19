/*
 * settings.cpp
 *
 * Copyright (C) 1996 Greg Hewgill
 *
 * Persistent settings support
 *
 */

#include <windows.h>
#pragma hdrstop

#include "markers.h"
#include "settings.h"

#include "registry.h"

#include "port.h"
#include "extarr.h"

TSettings Settings;

TSettings::TSettings()
{
  properties.x = 0;
  properties.y = 0;
  proj = 0;
  pos.type = Position::fixed;
  pos.latitude = 0;
  pos.longitude = 0;
  pos.rlatitude = 0;
  pos.rlongitude = 0;
  pos.period = 1;
  pos.inclination = 0;
  rot = 0;
  sunpos.type = Position::_default;
  sunpos.latitude = 0;
  sunpos.longitude = 0;
  mag = 1;
  size.cx = 0;
  size.cy = 0;
  shift.x = 0;
  shift.y = 0;
  shade = TRUE;
  label = FALSE;
  labelpos = 0;
  markers = TRUE;
//  strcpy(markerfile, "built-in");
  stars = TRUE;
  starfreq = (float)0.002;
  bigstars = 0;
  grid = FALSE;
  grid1 = 6;
  grid2 = 15;
  day = 100;
  night = 5;
  term = 1;
  gamma = 1.0;
  wait = 5;
  timewarp = 1;
  time = 0;
  quakes = 1;
  qdelay = 3;
  ZeroMemory(&qupdated, sizeof(qupdated));
//	LoadMarkers(false);
  }

void TSettings::Save()
{

}

void TSettings::LoadMarkers(bool reset)
{
  markerlist.clear();
 //   load_marker_info("built-in");
    load_marker_info(Settings.szMarkerPath);
    for (MarkerInfo *mi = marker_info; mi->label; mi++)
	{
      marker m;
      m.name = mi->label;
      m.latitude = mi->lat;
      m.longitude = mi->lon;
      markerlist.push_back(m);
    }
    return;
}

void TSettings::SaveMarkers()
{

}

void LoadMarkers()
{
  MarkerInfo *newm;
  static ExtArr info = NULL;

  if (info == NULL)
  {
    /* first time through, allocate a new extarr
     */
    info = extarr_alloc(sizeof(MarkerInfo));
  }
  else
  {
    /* on subsequent passes, just clean it up for reuse
     */
    for (int i=0; i<(info->count-1); i++)
      free(((MarkerInfo *) info->body)[i].label);
    info->count = 0;
  }

  for (std::list<marker>::const_iterator i = Settings.markerlist.begin(); i != Settings.markerlist.end(); i++) {
    newm = (MarkerInfo *)extarr_next(info);
    newm->lat = i->latitude;
    newm->lon = i->longitude;
    newm->label = strdup(i->name.c_str());
    newm->align = 0; //MarkerAlignDefault;
  }

  newm = (MarkerInfo *) extarr_next(info);
  newm->lat   = 0;
  newm->lon   = 0;
  newm->label = NULL;

  marker_info = (MarkerInfo *) info->body;
}

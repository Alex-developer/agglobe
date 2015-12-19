/*
 * quake.h
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * Get earthquake information from:
 *   finger quake@gldfs.cr.usgs.gov
 *
 */

#pragma once

#include <list>
#include <string>

struct quake {
  SYSTEMTIME time;
  float lat, lon, dep, mag;
  std::string location;
};

std::list<quake> GetQuakes();
void UpdateQuakes();
bool QuakesUpdated();
void StartQuakeThread();

/*
 * markers.h
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * Marker structure
 */

typedef struct
{
  float lat;
  float lon;
  char *label;
  int   align;
  COLORREF lColour;
} MarkerInfo;

extern MarkerInfo *marker_info;
extern void load_marker_info(char *);

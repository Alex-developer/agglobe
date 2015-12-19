/*
 * markerdlg.h
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * Marker dialog box
 */

#include "resource.h"

#include "dialog.h"
#include "settings.h"

class MarkerDialog: public Dialog {
public:
  MarkerDialog(HWND parent): Dialog(parent, MAKEINTRESOURCE(IDD_MARKER)) {}
  marker marker;
  virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

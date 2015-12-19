/*
 * markerdlg.cpp
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * Marker dialog box
 */

#include <windows.h>

#include "markerdlg.h"

BOOL MarkerDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
    case WM_COMMAND:
      if (HIWORD(wParam) == BN_CLICKED) {
        if (LOWORD(wParam) == IDOK) {
          char buf[40];
          GetDlgItemText(hwnd, IDC_NAME, buf, sizeof(buf));
          marker.name = buf;
          marker.latitude = GetDlgItemFloat(hwnd, IDC_LATITUDE, 0);
          marker.longitude = GetDlgItemFloat(hwnd, IDC_LONGITUDE, 0);
          if (marker.latitude > 90 || marker.latitude < -90) {
            MessageBox(hwnd, "Latitude must be between -90 and 90.", "xearth", MB_OK|MB_ICONSTOP);
            SetFocus(GetDlgItem(hwnd, IDC_LATITUDE));
            break;
          }
          if (marker.longitude > 180 || marker.longitude < -180) {
            MessageBox(hwnd, "Longitude must be between -180 and 180.", "xearth", MB_OK|MB_ICONSTOP);
            SetFocus(GetDlgItem(hwnd, IDC_LONGITUDE));
            break;
          }
        }
        EndDialog(hwnd, LOWORD(wParam));
      }
      break;
    case WM_INITDIALOG:
      if (!marker.name.empty()) {
        SetDlgItemText(hwnd, IDC_NAME, marker.name.c_str());
        SetDlgItemFloat(hwnd, IDC_LATITUDE, marker.latitude);
        SetDlgItemFloat(hwnd, IDC_LONGITUDE, marker.longitude);
      }
      return TRUE;
    default:
      return FALSE;
  }
  return TRUE;
}

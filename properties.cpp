/*
 * properties.cpp
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * Property sheet for xearth
 */

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <time.h>
#pragma hdrstop

#include "markerdlg.h"
#include "markers.h"
#include "settings.h"
#include "subclasswnd.h"

#include "resource.h"

extern HWND MainWindow;
extern void Refresh();

void GetDateTimeFormat(LCID lcid, DWORD dflags, DWORD tflags, const SYSTEMTIME *st, LPTSTR result, int cresult)
{
  GetDateFormat(lcid, dflags, st, NULL, result, cresult);
  strcat(result, " ");
  GetTimeFormat(lcid, tflags, st, NULL, &result[strlen(result)], cresult-strlen(result));
}

class XearthPropertySheet: public SubclassedWindow {
public:
  XearthPropertySheet(HWND hwnd);
  virtual ~XearthPropertySheet();
  void OnPaint(HWND hwnd);
private:
  HFONT font;
  virtual LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

XearthPropertySheet::XearthPropertySheet(HWND hwnd)
 : SubclassedWindow(hwnd)
{
  LOGFONT lf;
  ZeroMemory(&lf, sizeof(lf));
  lf.lfHeight = 36;
  lf.lfWeight = FW_BOLD;
  lf.lfItalic = TRUE;
  strcpy(lf.lfFaceName, "Times New Roman");
  font = CreateFontIndirect(&lf);
}

XearthPropertySheet::~XearthPropertySheet()
{
  if (font) {
    DeleteObject(font);
  }
}

void XearthPropertySheet::OnPaint(HWND hwnd)
{
  CallWindowProc(OldWndProc, hwnd, WM_PAINT, 0, 0);
  HDC dc = GetDC(hwnd);
  HGDIOBJ of = SelectObject(dc, font);
  SetBkMode(dc, TRANSPARENT);
  SetTextColor(dc, RGB(0, 0, 0));
  TextOut(dc, 20, 2, "Xearth for Windows", 18);
  SetTextColor(dc, RGB(255, 255, 255));
  TextOut(dc, 17, -1, "Xearth for Windows", 18);
  SetTextColor(dc, RGB(0, 0, 255));
  TextOut(dc, 18, 0, "Xearth for Windows", 18);
  SelectObject(dc, of);
  ReleaseDC(hwnd, dc);
}

LRESULT XearthPropertySheet::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_PAINT:
      return HANDLE_WM_PAINT(hwnd, wparam, lparam, OnPaint);
    default:
      return CallWindowProc(OldWndProc, hwnd, msg, wparam, lparam);
  }
  return 0;
}

int CALLBACK PropSheetProc(HWND hwnd, UINT msg, LPARAM lparam)
{
  if (msg == PSCB_PRECREATE) {
    SetWindowPos(MainWindow, NULL, Settings.properties.x, Settings.properties.y, 0, 0, 0);
    DLGTEMPLATE *dlg = reinterpret_cast<DLGTEMPLATE *>(lparam);
    wchar_t *p = reinterpret_cast<wchar_t *>(dlg+1);
    wchar_t *menu = p;
    if (*p == 0xFFFF) {
      p += 2;
    } else {
      p += wcslen(p) + 1;
    }
    wchar_t *wclass = p;
    if (*p == 0xFFFF) {
      p += 2;
    } else {
      p += wcslen(p) + 1;
    }
    wchar_t *title = p;
    p += wcslen(p) + 1;
    int size = 0;
    wchar_t *font = NULL;
    if (dlg->style & DS_SETFONT) {
      size = *p++;
      font = p;
      p += wcslen(p) + 1;
    }
    for (int i = 0; i < dlg->cdit; i++) {
      if ((int)p & 3) {
        *reinterpret_cast<int *>(&p) += 4 - ((int)p & 3);
      }
      DLGITEMTEMPLATE *dlgitem = reinterpret_cast<DLGITEMTEMPLATE *>(p);
      p = reinterpret_cast<wchar_t *>(dlgitem+1);
      wchar_t *wclass = p;
      if (*p == 0xFFFF) {
        p += 2;
      } else {
        p += wcslen(p) + 1;
      }
      wchar_t *title = p;
      if (*p == 0xFFFF) {
        p += 2;
      } else {
        p += wcslen(p) + 1;
      }
      if ((int)p & 3) {
        *reinterpret_cast<int *>(&p) += 4 - ((int)p & 3);
      }
      if (*p) {
        p += (*p+1) / 2;
      } else {
        p += 2;
      }
      dlgitem->y += 20;
    }
    dlg->cy += 20;
  } else if (msg == PSCB_INITIALIZED) {
    new XearthPropertySheet(hwnd);
  }
  return 0;
}

void DoApply(HWND hwnd)
{
//  Refresh();
  RECT r;
  GetWindowRect(GetParent(hwnd), &r);
  Settings.properties.x = r.left;
  Settings.properties.y = r.top;
  Settings.Save();
  PropSheet_UnChanged(GetParent(hwnd), hwnd);
}

bool GetSettingsImage(HWND hwnd, bool save)
{
  DWORD proj = SendDlgItemMessage(hwnd, IDC_PROJ_MERCATOR, BM_GETCHECK, 0, 0) == BST_CHECKED;
  float rot = GetDlgItemFloat(hwnd, IDC_ROT, 0);
  float mag = GetDlgItemFloat(hwnd, IDC_MAG, 1);
  POINT shift;
  shift.x = GetDlgItemInt(hwnd, IDC_SHIFT_X, NULL, TRUE);
  shift.y = GetDlgItemInt(hwnd, IDC_SHIFT_Y, NULL, TRUE);
  SIZE size;
  if (SendDlgItemMessage(hwnd, IDC_SIZE_DEFAULT, BM_GETCHECK, 0, 0) == BST_CHECKED) {
    size.cx = 0;
    size.cy = 0;
  } else {
    size.cx = GetDlgItemInt(hwnd, IDC_SIZE_CX, NULL, FALSE);
    size.cy = GetDlgItemInt(hwnd, IDC_SIZE_CY, NULL, FALSE);
  }
  if (rot < -180 || rot > 360) {
	  MessageBox(hwnd, "Viewing rotation must be between -180 and 360.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_ROT));
    return false;
  }
  if (mag <= 0) {
    MessageBox(hwnd, "Viewing magnification must be positive.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_MAG));
    return false;
  }
  if (size.cx > 0 || size.cy > 0) {
    if (size.cx <= 0) {
      MessageBox(hwnd, "Width must be positive.", "xearth", MB_OK|MB_ICONSTOP);
      SetFocus(GetDlgItem(hwnd, IDC_SHIFT_X));
      return false;
    }
    if (size.cy <= 0) {
      MessageBox(hwnd, "Height must be positive.", "xearth", MB_OK|MB_ICONSTOP);
      SetFocus(GetDlgItem(hwnd, IDC_SHIFT_Y));
      return false;
    }
  }
  if (save) {
    Settings.proj = proj;
    Settings.rot = rot;
    Settings.mag = mag;
    Settings.shift = shift;
    Settings.size = size;
  }
  return true;
}

BOOL CALLBACK PropertiesImageProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (HIWORD(wparam)) {
        case BN_CLICKED:
          if (LOWORD(wparam) == IDC_SIZE_DEFAULT) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_SIZE_DEFAULT, BM_GETCHECK, 0, 0) != BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_SIZE_CX), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_SIZE_CY), enable);
          }
          // FALL THROUGH //
        case EN_CHANGE:
          PropSheet_Changed(GetParent(hwnd), hwnd);
          break;
      }
      break;
    case WM_INITDIALOG: {
      SendDlgItemMessage(hwnd, IDC_PROJ_ORTHOGRAPHIC, BM_SETCHECK, Settings.proj == 0, 0);
      SendDlgItemMessage(hwnd, IDC_PROJ_MERCATOR, BM_SETCHECK, Settings.proj == 1, 0);
      SetDlgItemFloat(hwnd, IDC_ROT, Settings.rot);
      SetDlgItemFloat(hwnd, IDC_MAG, Settings.mag);
      SetDlgItemInt(hwnd, IDC_SHIFT_X, Settings.shift.x, TRUE);
      SetDlgItemInt(hwnd, IDC_SHIFT_Y, Settings.shift.y, TRUE);
      if (Settings.size.cx == 0 || Settings.size.cy == 0) {
        SendDlgItemMessage(hwnd, IDC_SIZE_DEFAULT, BM_SETCHECK, TRUE, 0);
        SetDlgItemInt(hwnd, IDC_SIZE_CX, GetSystemMetrics(SM_CXSCREEN), FALSE);
        SetDlgItemInt(hwnd, IDC_SIZE_CY, GetSystemMetrics(SM_CYSCREEN), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_SIZE_CX), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_SIZE_CY), FALSE);
      } else {
        SetDlgItemInt(hwnd, IDC_SIZE_CX, Settings.size.cx, FALSE);
        SetDlgItemInt(hwnd, IDC_SIZE_CY, Settings.size.cy, FALSE);
      }
      break;
    }
    case WM_NOTIFY:
      switch (((NMHDR *)lparam)->code) {
        case PSN_APPLY: {
          GetSettingsImage(hwnd, true);
          DoApply(hwnd);
          break;
        }
        case PSN_KILLACTIVE: {
          if (!GetSettingsImage(hwnd, false)) {
            SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
          }
          break;
        }
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

bool GetSettingsViewpoint(HWND hwnd, bool save)
{
  Position pos;
       if (SendDlgItemMessage(hwnd, IDC_POS_DEFAULT, BM_GETCHECK, 0, 0) == BST_CHECKED) pos.type = Position::_default;
  else if (SendDlgItemMessage(hwnd, IDC_POS_FIXED  , BM_GETCHECK, 0, 0) == BST_CHECKED) pos.type = Position::fixed;
  else if (SendDlgItemMessage(hwnd, IDC_POS_SUNREL , BM_GETCHECK, 0, 0) == BST_CHECKED) pos.type = Position::sunrel;
  else if (SendDlgItemMessage(hwnd, IDC_POS_ORBIT  , BM_GETCHECK, 0, 0) == BST_CHECKED) pos.type = Position::orbit;
  else if (SendDlgItemMessage(hwnd, IDC_POS_RANDOM , BM_GETCHECK, 0, 0) == BST_CHECKED) pos.type = Position::random;
  pos.latitude = GetDlgItemFloat(hwnd, IDC_POS_LATITUDE, 0);
  pos.longitude = GetDlgItemFloat(hwnd, IDC_POS_LONGITUDE, 0);
  pos.rlatitude = GetDlgItemFloat(hwnd, IDC_POS_RLATITUDE, 0);
  pos.rlongitude = GetDlgItemFloat(hwnd, IDC_POS_RLONGITUDE, 0);
  pos.period = GetDlgItemFloat(hwnd, IDC_POS_PERIOD, 0);
  pos.inclination = GetDlgItemFloat(hwnd, IDC_POS_INCLINATION, 0);
  switch (pos.type) {
    case Position::fixed:
      if (pos.latitude > 90 || pos.latitude < -90) {
        MessageBox(hwnd, "Viewing latitude must be between -90 and 90.", "xearth", MB_OK|MB_ICONSTOP);
        SetFocus(GetDlgItem(hwnd, IDC_POS_LATITUDE));
        return false;
      }
      if (pos.longitude > 180 || pos.longitude < -180) {
        MessageBox(hwnd, "Viewing longitude must be between -180 and 180.", "xearth", MB_OK|MB_ICONSTOP);
        SetFocus(GetDlgItem(hwnd, IDC_POS_LONGITUDE));
        return false;
      }
      break;
    case Position::sunrel:
      if (pos.rlatitude > 90 || pos.rlatitude < -90) {
        MessageBox(hwnd, "Viewing latitude must be between -90 and 90.", "xearth", MB_OK|MB_ICONSTOP);
        SetFocus(GetDlgItem(hwnd, IDC_POS_RLATITUDE));
        return false;
      }
      if (pos.rlongitude > 180 || pos.rlongitude < -180) {
        MessageBox(hwnd, "Viewing longitude must be between -180 and 180.", "xearth", MB_OK|MB_ICONSTOP);
        SetFocus(GetDlgItem(hwnd, IDC_POS_RLONGITUDE));
        return false;
      }
      break;
    case Position::orbit:
      if (pos.period <= 0) {
        MessageBox(hwnd, "Orbital period must be positive.", "xearth", MB_OK|MB_ICONSTOP);
        SetFocus(GetDlgItem(hwnd, IDC_POS_PERIOD));
        return false;
      }
      if (pos.inclination > 90 || pos.inclination < -90) {
        MessageBox(hwnd, "Orbital inclination must be between -90 and 90.", "xearth", MB_OK|MB_ICONSTOP);
        SetFocus(GetDlgItem(hwnd, IDC_POS_INCLINATION));
        return false;
      }
      break;
  }
  if (save) {
    Settings.pos = pos;
  }
  return true;
}

BOOL CALLBACK PropertiesViewpointProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (HIWORD(wparam)) {
        case BN_CLICKED:
          if (LOWORD(wparam) == IDC_POS_DEFAULT
           || LOWORD(wparam) == IDC_POS_FIXED
           || LOWORD(wparam) == IDC_POS_SUNREL
           || LOWORD(wparam) == IDC_POS_ORBIT
           || LOWORD(wparam) == IDC_POS_RANDOM) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_POS_FIXED, BM_GETCHECK, 0, 0) == BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_POS_LATITUDE), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_POS_LONGITUDE), enable);
            enable = SendDlgItemMessage(hwnd, IDC_POS_SUNREL, BM_GETCHECK, 0, 0) == BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_POS_RLATITUDE), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_POS_RLONGITUDE), enable);
            enable = SendDlgItemMessage(hwnd, IDC_POS_ORBIT, BM_GETCHECK, 0, 0) == BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_POS_PERIOD), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_POS_INCLINATION), enable);
          }
          // FALL THROUGH //
        case EN_CHANGE:
          PropSheet_Changed(GetParent(hwnd), hwnd);
          break;
      }
      break;
    case WM_INITDIALOG: {
      SendDlgItemMessage(hwnd, IDC_POS_DEFAULT, BM_SETCHECK, Settings.pos.type == Position::_default, 0);
      SendDlgItemMessage(hwnd, IDC_POS_FIXED, BM_SETCHECK, Settings.pos.type == Position::fixed, 0);
      SendDlgItemMessage(hwnd, IDC_POS_SUNREL, BM_SETCHECK, Settings.pos.type == Position::sunrel, 0);
      SendDlgItemMessage(hwnd, IDC_POS_ORBIT, BM_SETCHECK, Settings.pos.type == Position::orbit, 0);
      SendDlgItemMessage(hwnd, IDC_POS_RANDOM, BM_SETCHECK, Settings.pos.type == Position::random, 0);
      SetDlgItemFloat(hwnd, IDC_POS_LATITUDE, Settings.pos.latitude);
      SetDlgItemFloat(hwnd, IDC_POS_LONGITUDE, Settings.pos.longitude);
      SetDlgItemFloat(hwnd, IDC_POS_RLATITUDE, Settings.pos.rlatitude);
      SetDlgItemFloat(hwnd, IDC_POS_RLONGITUDE, Settings.pos.rlongitude);
      SetDlgItemFloat(hwnd, IDC_POS_PERIOD, Settings.pos.period);
      SetDlgItemFloat(hwnd, IDC_POS_INCLINATION, Settings.pos.inclination);
      if (Settings.pos.type != Position::fixed) {
        EnableWindow(GetDlgItem(hwnd, IDC_POS_LATITUDE), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_POS_LONGITUDE), FALSE);
      }
      if (Settings.pos.type != Position::sunrel) {
        EnableWindow(GetDlgItem(hwnd, IDC_POS_RLATITUDE), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_POS_RLONGITUDE), FALSE);
      }
      if (Settings.pos.type != Position::orbit) {
        EnableWindow(GetDlgItem(hwnd, IDC_POS_PERIOD), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_POS_INCLINATION), FALSE);
      }
      break;
    }
    case WM_NOTIFY:
      switch (((NMHDR *)lparam)->code) {
        case PSN_APPLY: {
          GetSettingsViewpoint(hwnd, true);
          DoApply(hwnd);
          break;
        }
        case PSN_KILLACTIVE: {
          if (!GetSettingsViewpoint(hwnd, false)) {
            SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
          }
          break;
        }
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

bool GetSettingsSun(HWND hwnd, bool save)
{
  Position sunpos;
  sunpos.type = SendDlgItemMessage(hwnd, IDC_SUNPOS_DEFAULT, BM_GETCHECK, 0, 0) == BST_CHECKED ? Position::_default : Position::fixed;
  sunpos.latitude = GetDlgItemFloat(hwnd, IDC_SUNPOS_LATITUDE, 0);
  sunpos.longitude = GetDlgItemFloat(hwnd, IDC_SUNPOS_LONGITUDE, 0);
  if (sunpos.type == Position::fixed) {
    if (sunpos.latitude > 90 || sunpos.latitude < -90) {
      MessageBox(hwnd, "Sun latitude must be between -90 and 90.", "xearth", MB_OK|MB_ICONSTOP);
      SetFocus(GetDlgItem(hwnd, IDC_SUNPOS_LATITUDE));
      return false;
    }
    if (sunpos.longitude > 180 || sunpos.longitude < -180) {
      MessageBox(hwnd, "Sun longitude must be between -180 and 180.", "xearth", MB_OK|MB_ICONSTOP);
      SetFocus(GetDlgItem(hwnd, IDC_SUNPOS_LONGITUDE));
      return false;
    }
  }
  if (save) {
    Settings.sunpos = sunpos;
  }
  return true;
}

BOOL CALLBACK PropertiesSunProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (HIWORD(wparam)) {
        case BN_CLICKED:
          if (LOWORD(wparam) == IDC_SUNPOS_DEFAULT) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_SUNPOS_DEFAULT, BM_GETCHECK, 0, 0) != BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_SUNPOS_LATITUDE), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_SUNPOS_LONGITUDE), enable);
          }
          // FALL THROUGH //
        case EN_CHANGE:
          PropSheet_Changed(GetParent(hwnd), hwnd);
          break;
      }
      break;
    case WM_INITDIALOG: {
      SendDlgItemMessage(hwnd, IDC_SUNPOS_DEFAULT, BM_SETCHECK, Settings.sunpos.type == Position::_default, 0);
      SetDlgItemFloat(hwnd, IDC_SUNPOS_LATITUDE, Settings.sunpos.latitude);
      SetDlgItemFloat(hwnd, IDC_SUNPOS_LONGITUDE, Settings.sunpos.longitude);
      if (Settings.sunpos.type == Position::_default) {
        EnableWindow(GetDlgItem(hwnd, IDC_SUNPOS_LATITUDE), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_SUNPOS_LONGITUDE), FALSE);
      }
      break;
    }
    case WM_NOTIFY:
      switch (((NMHDR *)lparam)->code) {
        case PSN_APPLY: {
          GetSettingsSun(hwnd, true);
          DoApply(hwnd);
          break;
        }
        case PSN_KILLACTIVE: {
          if (!GetSettingsSun(hwnd, false)) {
            SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
          }
          break;
        }
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

void SetMarkerItems(HWND list, int index, const marker &m)
{
  ListView_SetItemText(list, index, 0, const_cast<char *>(m.name.c_str()));
  char buf[40];
  sprintf(buf, "%.2f", m.latitude);
  ListView_SetItemText(list, index, 1, buf);
  sprintf(buf, "%.2f", m.longitude);
  ListView_SetItemText(list, index, 2, buf);
}

void AddMarker(HWND list, const marker &m)
{
  LV_ITEM lvi;
  lvi.mask = LVIF_TEXT | LVIF_PARAM;
  lvi.iItem = 0;
  lvi.iSubItem = 0;
  lvi.pszText = const_cast<char *>(m.name.c_str());
  lvi.lParam = (LPARAM)&m;
  int index = ListView_InsertItem(list, &lvi);
  SetMarkerItems(list, index, m);
}

void FillMarkerList(HWND list)
{
  ListView_DeleteAllItems(list);
  for (std::list<marker>::const_iterator i = Settings.markerlist.begin(); i != Settings.markerlist.end(); i++) {
    AddMarker(list, *i);
  }
}

int CALLBACK CompareMarkerName(LPARAM p1, LPARAM p2, LPARAM p)
{
  marker *m1 = reinterpret_cast<marker *>(p1);
  marker *m2 = reinterpret_cast<marker *>(p2);
  return m1->name.compare(m2->name);
}

int CALLBACK CompareMarkerLatitude(LPARAM p1, LPARAM p2, LPARAM p)
{
  marker *m1 = reinterpret_cast<marker *>(p1);
  marker *m2 = reinterpret_cast<marker *>(p2);
  if (m1->latitude < m2->latitude) {
    return -1;
  } else if (m1->latitude > m2->latitude) {
    return 1;
  }
  return 0;
}

int CALLBACK CompareMarkerLongitude(LPARAM p1, LPARAM p2, LPARAM p)
{
  marker *m1 = reinterpret_cast<marker *>(p1);
  marker *m2 = reinterpret_cast<marker *>(p2);
  if (m1->longitude < m2->longitude) {
    return -1;
  } else if (m1->longitude > m2->longitude) {
    return 1;
  }
  return 0;
}

bool GetSettingsLabels(HWND hwnd, bool save)
{
  BOOL label = SendDlgItemMessage(hwnd, IDC_LABEL, BM_GETCHECK, 0, 0) == BST_CHECKED;
  DWORD labelpos;
       if (SendDlgItemMessage(hwnd, IDC_LABELPOS_TOPLEFT    , BM_GETCHECK, 0, 0) == BST_CHECKED) labelpos = 0;
  else if (SendDlgItemMessage(hwnd, IDC_LABELPOS_TOPRIGHT   , BM_GETCHECK, 0, 0) == BST_CHECKED) labelpos = 1;
  else if (SendDlgItemMessage(hwnd, IDC_LABELPOS_BOTTOMLEFT , BM_GETCHECK, 0, 0) == BST_CHECKED) labelpos = 2;
  else if (SendDlgItemMessage(hwnd, IDC_LABELPOS_BOTTOMRIGHT, BM_GETCHECK, 0, 0) == BST_CHECKED) labelpos = 3;
  BOOL markers = SendDlgItemMessage(hwnd, IDC_MARKERS, BM_GETCHECK, 0, 0) == BST_CHECKED;
  if (save) {
    Settings.label = label;
    Settings.labelpos = labelpos;
    Settings.markers = markers;
  }
  return true;
}

BOOL CALLBACK PropertiesLabelsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (HIWORD(wparam)) {
        case BN_CLICKED:
          if (LOWORD(wparam) == IDC_ADD) {
            MarkerDialog dlg(hwnd);
            if (dlg.DoModal() != IDOK) {
              break;
            }
            Settings.markerlist.push_back(dlg.marker);
            AddMarker(GetDlgItem(hwnd, IDC_MARKERLIST), dlg.marker);
            PropSheet_CancelToClose(GetParent(hwnd));
          } else if (LOWORD(wparam) == IDC_REMOVE) {
            if (ListView_GetNextItem(GetDlgItem(hwnd, IDC_MARKERLIST), -1, LVNI_SELECTED) == -1) {
              MessageBox(hwnd, "No items are currently selected.", "xearth", MB_OK|MB_ICONSTOP);
              break;
            }
            if (MessageBox(hwnd, "Remove all selected items?", "xearth", MB_YESNO|MB_ICONQUESTION) != IDYES) {
              break;
            }
            int index = -1;
            while (true) {
              index = ListView_GetNextItem(GetDlgItem(hwnd, IDC_MARKERLIST), index, LVNI_SELECTED);
              if (index < 0) {
                break;
              }
              LV_ITEM lvi;
              lvi.mask = LVIF_PARAM;
              lvi.iItem = index;
              ListView_GetItem(GetDlgItem(hwnd, IDC_MARKERLIST), &lvi);
              for (std::list<marker>::iterator i = Settings.markerlist.begin(); i != Settings.markerlist.end(); i++) {
                if ((LPARAM)&(*i) == lvi.lParam) {
                  Settings.markerlist.erase(i);
                  ListView_DeleteItem(GetDlgItem(hwnd, IDC_MARKERLIST), index);
                  break;
                }
              }
              index--;
            }
            PropSheet_CancelToClose(GetParent(hwnd));
          } else if (LOWORD(wparam) == IDC_IMPORT) {
            char fn[MAX_PATH];
            fn[0] = 0;
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
            ofn.lpstrFile = fn;
            ofn.nMaxFile = sizeof(fn);
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
            ofn.lpstrDefExt = "txt";
            if (GetOpenFileName(&ofn)) {
              load_marker_info(fn);
              if (marker_info->label == NULL) {
                MessageBox(hwnd, "There were no valid place locations in the selected file.", "xearth", MB_OK|MB_ICONSTOP);
                break;
              }
              bool cancel = false;
              for (MarkerInfo *mi = marker_info; mi->label; mi++) {
                bool add = true;
                for (std::list<marker>::const_iterator i = Settings.markerlist.begin(); i != Settings.markerlist.end(); i++) {
                  if (i->name.compare(mi->label) == 0) {
                    if (i->latitude == mi->lat && i->longitude == mi->lon) {
                      add = false;
                    } else {
                      char buf[256];
                      sprintf(buf, "A marker with name \"%s\" already exists at the coordinates (%g,%g). The new marker is being added at coordinates (%g,%g). Add the new marker anyway?", i->name.c_str(), i->latitude, i->longitude, mi->lat, mi->lon);
                      switch (MessageBox(hwnd, buf, "xearth", MB_YESNOCANCEL|MB_ICONQUESTION)) {
                        case IDYES:
                          add = true;
                          break;
                        case IDCANCEL:
                          cancel = true;
                          break;
                      }
                    }
                    break;
                  } else if (i->latitude == mi->lat && i->longitude == mi->lon) {
                    char buf[256];
                    sprintf(buf, "A marker with name \"%s\" already exists at the same location as a new marker \"%s\". Add the new marker anyway?", i->name.c_str(), mi->label);
                    switch (MessageBox(hwnd, buf, "xearth", MB_YESNOCANCEL|MB_ICONQUESTION)) {
                      case IDYES:
                        add = true;
                        break;
                      case IDCANCEL:
                        cancel = true;
                        break;
                    }
                    break;
                  }
                }
                if (cancel) {
                  break;
                }
                if (add) {
                  marker m;
                  m.name = mi->label;
                  m.latitude = mi->lat;
                  m.longitude = mi->lon;
                  Settings.markerlist.push_back(m);
                }
              }
              FillMarkerList(GetDlgItem(hwnd, IDC_MARKERLIST));
              PropSheet_CancelToClose(GetParent(hwnd));
            }
            break;
          } else if (LOWORD(wparam) == IDC_EXPORT) {
            char fn[MAX_PATH];
            fn[0] = 0;
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
            ofn.lpstrFile = fn;
            ofn.nMaxFile = sizeof(fn);
            ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
            ofn.lpstrDefExt = "txt";
            if (GetSaveFileName(&ofn)) {
              HANDLE f = CreateFile(fn, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
              if (f == INVALID_HANDLE_VALUE && GetLastError() == ERROR_FILE_EXISTS) {
                char buf[MAX_PATH];
                sprintf(buf, "Overwrite %s?", fn);
                if (MessageBox(hwnd, buf, "xearth", MB_YESNO|MB_ICONQUESTION) != IDYES) {
                  break;
                }
                f = CreateFile(fn, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
              }
              if (f == INVALID_HANDLE_VALUE) {
                DWORD error = GetLastError();
                char buf[MAX_PATH];
                if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, buf, sizeof(buf), NULL) == 0) {
                  sprintf(buf, "Unknown error %d.", error);
                }
                MessageBox(hwnd, buf, "xearth", MB_OK|MB_ICONSTOP);
                break;
              }
              for (std::list<marker>::const_iterator i = Settings.markerlist.begin(); i != Settings.markerlist.end(); i++) {
                char buf[80];
                sprintf(buf, "%7.2f %7.2f \"%s\"\n", i->latitude, i->longitude, i->name.c_str());
                DWORD n;
                WriteFile(f, buf, strlen(buf), &n, NULL);
              }
              CloseHandle(f);
            }
            break;
          } else if (LOWORD(wparam) == IDC_RESET) {
            if (MessageBox(hwnd, "Reset entire marker list to default list?", "xearth", MB_YESNO|MB_ICONQUESTION) != IDYES) {
              break;
            }
            Settings.LoadMarkers(true);
            FillMarkerList(GetDlgItem(hwnd, IDC_MARKERLIST));
            PropSheet_CancelToClose(GetParent(hwnd));
          } else if (LOWORD(wparam) == IDC_LABEL) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_LABEL, BM_GETCHECK, 0, 0) == BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_LABELPOS_TOPLEFT), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_LABELPOS_TOPRIGHT), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_LABELPOS_BOTTOMLEFT), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_LABELPOS_BOTTOMRIGHT), enable);
          } else if (LOWORD(wparam) == IDC_MARKERS) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_MARKERS, BM_GETCHECK, 0, 0) == BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_MARKERLIST), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_ADD), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_REMOVE), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_IMPORT), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_EXPORT), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_RESET), enable);
          }
          // FALL THROUGH //
        case EN_CHANGE:
          PropSheet_Changed(GetParent(hwnd), hwnd);
          break;
      }
      break;
    case WM_INITDIALOG: {
      SendDlgItemMessage(hwnd, IDC_LABEL, BM_SETCHECK, Settings.label, 0);
      SendDlgItemMessage(hwnd, IDC_LABELPOS_TOPLEFT, BM_SETCHECK, Settings.labelpos == 0, 0);
      SendDlgItemMessage(hwnd, IDC_LABELPOS_TOPRIGHT, BM_SETCHECK, Settings.labelpos == 1, 0);
      SendDlgItemMessage(hwnd, IDC_LABELPOS_BOTTOMLEFT, BM_SETCHECK, Settings.labelpos == 2, 0);
      SendDlgItemMessage(hwnd, IDC_LABELPOS_BOTTOMRIGHT, BM_SETCHECK, Settings.labelpos == 3, 0);
      SendDlgItemMessage(hwnd, IDC_MARKERS, BM_SETCHECK, Settings.markers, 0);
      LV_COLUMN lvc;
      lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
      lvc.fmt = LVCFMT_LEFT;
      lvc.cx = 91;
      lvc.pszText = "Name";
      ListView_InsertColumn(GetDlgItem(hwnd, IDC_MARKERLIST), 0, &lvc);
      lvc.fmt = LVCFMT_RIGHT;
      lvc.cx = 48;
      lvc.pszText = "Lat";
      ListView_InsertColumn(GetDlgItem(hwnd, IDC_MARKERLIST), 1, &lvc);
      lvc.pszText = "Lon";
      ListView_InsertColumn(GetDlgItem(hwnd, IDC_MARKERLIST), 2, &lvc);
      FillMarkerList(GetDlgItem(hwnd, IDC_MARKERLIST));
      if (!Settings.label) {
        EnableWindow(GetDlgItem(hwnd, IDC_LABELPOS_TOPLEFT), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_LABELPOS_TOPRIGHT), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_LABELPOS_BOTTOMLEFT), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_LABELPOS_BOTTOMRIGHT), FALSE);
      }
      if (!Settings.markers) {
        EnableWindow(GetDlgItem(hwnd, IDC_MARKERLIST), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_ADD), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_REMOVE), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_IMPORT), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_EXPORT), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_RESET), FALSE);
      }
      break;
    }
    case WM_NOTIFY:
      switch (((NMHDR *)lparam)->code) {
        case LVN_COLUMNCLICK: {
          NM_LISTVIEW *nlv = reinterpret_cast<NM_LISTVIEW *>(lparam);
          switch (nlv->iSubItem) {
            case 0: ListView_SortItems(GetDlgItem(hwnd, IDC_MARKERLIST), CompareMarkerName, 0); break;
            case 1: ListView_SortItems(GetDlgItem(hwnd, IDC_MARKERLIST), CompareMarkerLatitude, 0); break;
            case 2: ListView_SortItems(GetDlgItem(hwnd, IDC_MARKERLIST), CompareMarkerLongitude, 0); break;
          }
          break;
        }
        case NM_DBLCLK: {
          int index = ListView_GetNextItem(GetDlgItem(hwnd, IDC_MARKERLIST), -1, LVNI_FOCUSED);
          if (index < 0) {
            break;
          }
          LV_ITEM lvi;
          lvi.mask = LVIF_PARAM;
          lvi.iItem = index;
          ListView_GetItem(GetDlgItem(hwnd, IDC_MARKERLIST), &lvi);
          marker &m = *reinterpret_cast<marker *>(lvi.lParam);
          MarkerDialog dlg(hwnd);
          dlg.marker = m;
          if (dlg.DoModal() != IDOK) {
            break;
          }
          m = dlg.marker;
          SetMarkerItems(GetDlgItem(hwnd, IDC_MARKERLIST), index, dlg.marker);
          PropSheet_CancelToClose(GetParent(hwnd));
          break;
        }
        case PSN_APPLY: {
          GetSettingsLabels(hwnd, true);
          DoApply(hwnd);
          break;
        }
        case PSN_KILLACTIVE: {
          if (!GetSettingsLabels(hwnd, false)) {
            SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
          }
          break;
        }
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}


bool GetSettingsDots(HWND hwnd, bool save)
{
  BOOL stars = SendDlgItemMessage(hwnd, IDC_STARS, BM_GETCHECK, 0, 0) == BST_CHECKED;
  float starfreq = GetDlgItemFloat(hwnd, IDC_STARFREQ, 0.002f);
  DWORD bigstars = GetDlgItemInt(hwnd, IDC_BIGSTARS, NULL, FALSE);
  BOOL grid = SendDlgItemMessage(hwnd, IDC_GRID, BM_GETCHECK, 0, 0) == BST_CHECKED;
  DWORD grid1 = GetDlgItemInt(hwnd, IDC_GRID1, NULL, FALSE);
  DWORD grid2 = GetDlgItemInt(hwnd, IDC_GRID2, NULL, FALSE);
  if (starfreq < 0 || starfreq > 1) {
	  MessageBox(hwnd, "Star frequency must be between 0 and 1.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_STARFREQ));
    return false;
  }
  if (bigstars < 0 || bigstars > 100) {
	  MessageBox(hwnd, "Percent big stars must be between 0 and 100.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_BIGSTARS));
    return false;
  }
  if (grid1 <= 0) {
	  MessageBox(hwnd, "Grid lines per quadrant must be positive.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_GRID1));
    return false;
  }
  if (grid2 <= 0) {
	  MessageBox(hwnd, "Dots per grid line must be positive.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_GRID2));
    return false;
  }
  if (save) {
    Settings.stars = stars;
    Settings.starfreq = starfreq;
    Settings.bigstars = bigstars;
    Settings.grid = grid;
    Settings.grid1 = grid1;
    Settings.grid2 = grid2;
  }
  return true;
}

BOOL CALLBACK PropertiesDotsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (HIWORD(wparam)) {
        case BN_CLICKED:
          if (LOWORD(wparam) == IDC_STARS) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_STARS, BM_GETCHECK, 0, 0) == BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_STARFREQ), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_BIGSTARS), enable);
          } else if (LOWORD(wparam) == IDC_GRID) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_GRID, BM_GETCHECK, 0, 0) == BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_GRID1), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_GRID2), enable);
          }
          // FALL THROUGH //
        case EN_CHANGE:
          PropSheet_Changed(GetParent(hwnd), hwnd);
          break;
      }
      break;
    case WM_INITDIALOG: {
      SendDlgItemMessage(hwnd, IDC_STARS, BM_SETCHECK, Settings.stars, 0);
      SetDlgItemFloat(hwnd, IDC_STARFREQ, Settings.starfreq);
      SetDlgItemInt(hwnd, IDC_BIGSTARS, Settings.bigstars, FALSE);
      if (!Settings.stars) {
        EnableWindow(GetDlgItem(hwnd, IDC_STARFREQ), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_BIGSTARS), FALSE);
      }
      SendDlgItemMessage(hwnd, IDC_GRID, BM_SETCHECK, Settings.grid, 0);
      SetDlgItemInt(hwnd, IDC_GRID1, Settings.grid1, FALSE);
      SetDlgItemInt(hwnd, IDC_GRID2, Settings.grid2, FALSE);
      if (!Settings.grid) {
        EnableWindow(GetDlgItem(hwnd, IDC_GRID1), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_GRID2), FALSE);
      }
      break;
    }
    case WM_NOTIFY:
      switch (((NMHDR *)lparam)->code) {
        case PSN_APPLY: {
          GetSettingsDots(hwnd, true);
          DoApply(hwnd);
          break;
        }
        case PSN_KILLACTIVE: {
          if (!GetSettingsDots(hwnd, false)) {
            SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
          }
          break;
        }
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

bool GetSettingsShading(HWND hwnd, bool save)
{
  BOOL shade = SendDlgItemMessage(hwnd, IDC_SHADE, BM_GETCHECK, 0, 0) == BST_CHECKED;
  DWORD day = GetDlgItemInt(hwnd, IDC_DAY, NULL, FALSE);
  DWORD night = GetDlgItemInt(hwnd, IDC_NIGHT, NULL, FALSE);
  DWORD term = GetDlgItemInt(hwnd, IDC_TERM, NULL, FALSE);
  if (day > 100 || day < 0) {
    MessageBox(hwnd, "Daylight intensity must be between 0 and 100.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_DAY));
    return false;
  }
  if (night > 100 || night < 0) {
    MessageBox(hwnd, "Night intensity must be between 0 and 100.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_NIGHT));
    return false;
  }
  if (term > 100 || term < 0) {
    MessageBox(hwnd, "Terminator falloff must be between 0 and 100.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_TERM));
    return false;
  }
  if (save) {
    Settings.shade = shade;
    Settings.day = day;
    Settings.night = night;
    Settings.term = term;
  }
  return true;
}

BOOL CALLBACK PropertiesShadingProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (HIWORD(wparam)) {
        case BN_CLICKED:
          if (LOWORD(wparam) == IDC_SHADE) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_SHADE, BM_GETCHECK, 0, 0) == BST_CHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_DAY), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_NIGHT), enable);
            EnableWindow(GetDlgItem(hwnd, IDC_TERM), enable);
          }
          // FALL THROUGH //
        case EN_CHANGE:
          PropSheet_Changed(GetParent(hwnd), hwnd);
          break;
      }
      break;
    case WM_INITDIALOG: {
      SendDlgItemMessage(hwnd, IDC_SHADE, BM_SETCHECK, Settings.shade, 0);
      SetDlgItemInt(hwnd, IDC_DAY, Settings.day, FALSE);
      SetDlgItemInt(hwnd, IDC_NIGHT, Settings.night, FALSE);
      SetDlgItemInt(hwnd, IDC_TERM, Settings.term, FALSE);
      if (!Settings.shade) {
        EnableWindow(GetDlgItem(hwnd, IDC_DAY), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_NIGHT), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_TERM), FALSE);
      }
      break;
    }
    case WM_NOTIFY:
      switch (((NMHDR *)lparam)->code) {
        case PSN_APPLY: {
          GetSettingsShading(hwnd, true);
          DoApply(hwnd);
          break;
        }
        case PSN_KILLACTIVE: {
          if (!GetSettingsShading(hwnd, false)) {
            SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
          }
          break;
        }
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

bool GetSettingsTime(HWND hwnd, bool save)
{
  DWORD time;
  if (SendDlgItemMessage(hwnd, IDC_TIME_CURRENT, BM_GETCHECK, 0, 0) == BST_CHECKED) {
    time = 0;
  } else {
    char buf[80];
    GetDlgItemText(hwnd, IDC_TIME, buf, sizeof(buf));
    wchar_t wbuf[80];
    MultiByteToWideChar(CP_ACP, 0, buf, -1, wbuf, sizeof(wbuf)/sizeof(wchar_t));
    VARIANT var;
    VariantInit(&var);
    var.vt = VT_BSTR;
    var.bstrVal = SysAllocString(wbuf);
    VariantChangeType(&var, &var, 0, VT_DATE);
    SYSTEMTIME st;
    VariantTimeToSystemTime(var.date, &st);
    VariantClear(&var);
    FILETIME ftl;
    SystemTimeToFileTime(&st, &ftl);
    FILETIME ft;
    LocalFileTimeToFileTime(&ftl, &ft);
    st.wYear = 1970;
    st.wMonth = 1;
    st.wDay = 1;
    st.wHour = 0;
    st.wMinute = 0;
    st.wSecond = 0;
    st.wMilliseconds = 0;
    FILETIME epoch;
    SystemTimeToFileTime(&st, &epoch);
    time = (*(__int64 *)&ft - *(__int64 *)&epoch) / 10000000;
  }
  float timewarp = GetDlgItemFloat(hwnd, IDC_TIMEWARP, 1);
  if (timewarp <= 0) {
	  MessageBox(hwnd, "Timewarp factor must be positive.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_TIMEWARP));
    return false;
  }
  if (save) {
    Settings.time = time;
    Settings.timewarp = timewarp;
  }
  return true;
}

BOOL CALLBACK PropertiesTimeProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (HIWORD(wparam)) {
        case BN_CLICKED:
          if (LOWORD(wparam) == IDC_TIME_CURRENT) {
            BOOL enable = SendDlgItemMessage(hwnd, IDC_TIME_CURRENT, BM_GETCHECK, 0, 0) == BST_UNCHECKED;
            EnableWindow(GetDlgItem(hwnd, IDC_TIME), enable);
          }
          // FALL THROUGH //
        case EN_CHANGE:
          PropSheet_Changed(GetParent(hwnd), hwnd);
          break;
      }
      break;
    case WM_INITDIALOG: {
      SendDlgItemMessage(hwnd, IDC_TIME_CURRENT, BM_SETCHECK, Settings.time == 0, 0);
      SetDlgItemFloat(hwnd, IDC_TIMEWARP, Settings.timewarp);
      SYSTEMTIME st;
      if (Settings.time == 0) {
        GetLocalTime(&st);
        EnableWindow(GetDlgItem(hwnd, IDC_TIME), FALSE);
      } else {
        st.wYear = 1970;
        st.wMonth = 1;
        st.wDay = 1;
        st.wHour = 0;
        st.wMinute = 0;
        st.wSecond = 0;
        st.wMilliseconds = 0;
        FILETIME ft;
        SystemTimeToFileTime(&st, &ft);
        FILETIME ftl;
        FileTimeToLocalFileTime(&ft, &ftl);
        *(__int64 *)&ftl += __int64(Settings.time)*10000000;
        FileTimeToSystemTime(&ftl, &st);
      }
      double time;
      SystemTimeToVariantTime(&st, &time);
      VARIANT var;
      VariantInit(&var);
      var.vt = VT_DATE;
      var.date = time;
      VariantChangeType(&var, &var, 0, VT_BSTR);
      char buf[80];
      WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, buf, sizeof(buf), NULL, NULL);
      SetDlgItemText(hwnd, IDC_TIME, buf);
      VariantClear(&var);
      break;
    }
    case WM_NOTIFY:
      switch (((NMHDR *)lparam)->code) {
        case PSN_APPLY: {
          GetSettingsTime(hwnd, true);
          DoApply(hwnd);
          break;
        }
        case PSN_KILLACTIVE: {
          if (!GetSettingsTime(hwnd, false)) {
            SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
          }
          break;
        }
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

bool GetSettingsDisplay(HWND hwnd, bool save)
{
  DWORD wait = GetDlgItemInt(hwnd, IDC_WAITTIME, NULL, FALSE);
  float gamma = GetDlgItemFloat(hwnd, IDC_GAMMA, 1);
  if (gamma <= 0) {
    MessageBox(hwnd, "Gamma correction must be positive.", "xearth", MB_OK|MB_ICONSTOP);
    SetFocus(GetDlgItem(hwnd, IDC_GAMMA));
    return false;
  }
  if (save) {
    Settings.wait = wait;
    Settings.gamma = gamma;
  }
  return true;
}

BOOL CALLBACK PropertiesDisplayProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (HIWORD(wparam)) {
        case BN_CLICKED:
        case EN_CHANGE:
          PropSheet_Changed(GetParent(hwnd), hwnd);
          break;
      }
      break;
    case WM_INITDIALOG: {
      SetDlgItemInt(hwnd, IDC_WAITTIME, Settings.wait, FALSE);
      SetDlgItemFloat(hwnd, IDC_GAMMA, Settings.gamma);
      break;
    }
    case WM_NOTIFY:
      switch (((NMHDR *)lparam)->code) {
        case PSN_APPLY: {
          GetSettingsDisplay(hwnd, true);
          DoApply(hwnd);
          break;
        }
        case PSN_KILLACTIVE: {
          if (!GetSettingsDisplay(hwnd, false)) {
            SetWindowLong(hwnd, DWL_MSGRESULT, TRUE);
          }
          break;
        }
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

void Properties(HWND parent)
{
  PROPSHEETPAGE psp[9];
  ZeroMemory(&psp, sizeof(psp));
  psp[0].dwSize = sizeof(PROPSHEETPAGE);
  psp[0].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[0].hInstance = GetModuleHandle(NULL);
  psp[0].pszIcon = MAKEINTRESOURCE(IDI_EARTH);
  psp[0].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_IMAGE);
  psp[0].pfnDlgProc = PropertiesImageProc;
  psp[1].dwSize = sizeof(PROPSHEETPAGE);
  psp[1].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[1].hInstance = GetModuleHandle(NULL);
  psp[1].pszIcon = MAKEINTRESOURCE(IDI_VIEWPOINT);
  psp[1].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_VIEWPOINT);
  psp[1].pfnDlgProc = PropertiesViewpointProc;
  psp[2].dwSize = sizeof(PROPSHEETPAGE);
  psp[2].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[2].hInstance = GetModuleHandle(NULL);
  psp[2].pszIcon = MAKEINTRESOURCE(IDI_SUN);
  psp[2].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_SUN);
  psp[2].pfnDlgProc = PropertiesSunProc;
  psp[3].dwSize = sizeof(PROPSHEETPAGE);
  psp[3].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[3].hInstance = GetModuleHandle(NULL);
  psp[3].pszIcon = MAKEINTRESOURCE(IDI_LABELS);
  psp[3].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_LABELS);
  psp[3].pfnDlgProc = PropertiesLabelsProc;
  psp[4].dwSize = sizeof(PROPSHEETPAGE);
  psp[4].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[4].hInstance = GetModuleHandle(NULL);
  psp[4].pszIcon = MAKEINTRESOURCE(IDI_QUAKES);
//  psp[4].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_QUAKES);
  psp[4].pfnDlgProc = PropertiesQuakesProc;
  psp[5].dwSize = sizeof(PROPSHEETPAGE);
  psp[5].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[5].hInstance = GetModuleHandle(NULL);
  psp[5].pszIcon = MAKEINTRESOURCE(IDI_DOTS);
  psp[5].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_DOTS);
  psp[5].pfnDlgProc = PropertiesDotsProc;
  psp[6].dwSize = sizeof(PROPSHEETPAGE);
  psp[6].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[6].hInstance = GetModuleHandle(NULL);
  psp[6].pszIcon = MAKEINTRESOURCE(IDI_SHADING);
  psp[6].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_SHADING);
  psp[6].pfnDlgProc = PropertiesShadingProc;
  psp[7].dwSize = sizeof(PROPSHEETPAGE);
  psp[7].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[7].hInstance = GetModuleHandle(NULL);
  psp[7].pszIcon = MAKEINTRESOURCE(IDI_TIME);
  psp[7].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_TIME);
  psp[7].pfnDlgProc = PropertiesTimeProc;
  psp[8].dwSize = sizeof(PROPSHEETPAGE);
  psp[8].dwFlags = PSP_DEFAULT | PSP_USEICONID;
  psp[8].hInstance = GetModuleHandle(NULL);
  psp[8].pszIcon = MAKEINTRESOURCE(IDI_DISPLAY);
  psp[8].pszTemplate = MAKEINTRESOURCE(IDD_PROPERTIES_DISPLAY);
  psp[8].pfnDlgProc = PropertiesDisplayProc;
  PROPSHEETHEADER psh;
  ZeroMemory(&psh, sizeof(psh));
  psh.dwSize = sizeof(psh);
  psh.dwFlags = PSH_PROPSHEETPAGE | /*PSH_PROPTITLE |*/ PSH_USECALLBACK | PSH_USEICONID;
  psh.hwndParent = parent;
  psh.hInstance = GetModuleHandle(NULL);
  psh.pszIcon = MAKEINTRESOURCE(IDI_EARTH);
  psh.pszCaption = "xearth for Windows";
  psh.nPages = sizeof(psp)/sizeof(PROPSHEETPAGE);
  psh.ppsp = &psp[0];
  psh.pfnCallback = PropSheetProc;
  PropertySheet(&psh);
}

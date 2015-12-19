/*
 * dialog.cpp
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * Generic dialog box support.
 */

#include <windows.h>
#include <stdio.h>

#include "dialog.h"

Dialog::Dialog(HWND parent, LPCTSTR resid)
 : Parent(parent), Resid(resid)
{
}

int Dialog::DoModal()
{
  return DialogBoxParam(GetModuleHandle(NULL), Resid, Parent, &DlgProc, (LPARAM)this);
}

void CenterWindow(HWND hwnd)
{
        //ASSERT(::IsWindow(hwnd));

        // determine owner window to center against
        DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
        HWND hWndCenter = NULL;
        if (dwStyle & WS_CHILD)
                hWndCenter = ::GetParent(hwnd);
        else
                hWndCenter = ::GetWindow(hwnd, GW_OWNER);
        //if (hWndCenter != NULL)
        //{
        //        // let parent determine alternate center window
        //        HWND hWndTemp =
        //                (HWND)::SendMessage(hWndCenter, WM_QUERYCENTERWND, 0, 0);
        //        if (hWndTemp != NULL)
        //                hWndCenter = hWndTemp;
        //}

        // get coordinates of the window relative to its parent
        RECT rcDlg;
        GetWindowRect(hwnd, &rcDlg);
        RECT rcArea;
        RECT rcCenter;
        HWND hWndParent;
        if (!(dwStyle & WS_CHILD))
        {
                // don't center against invisible or minimized windows
                if (hWndCenter != NULL)
                {
                        DWORD dwStyle = ::GetWindowLong(hWndCenter, GWL_STYLE);
                        if (!(dwStyle & WS_VISIBLE) || (dwStyle & WS_MINIMIZE))
                                hWndCenter = NULL;
                }
                // center within screen coordinates
                SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);

                if (hWndCenter == NULL)
                        rcCenter = rcArea;
                else
                {
                        ::GetWindowRect(hWndCenter, &rcCenter);
                }
        }
        else
        {
                // center within parent client coordinates
                hWndParent = ::GetParent(hwnd);
                //ASSERT(::IsWindow(hWndParent));

                ::GetClientRect(hWndParent, &rcArea);
                //ASSERT(::IsWindow(hWndCenter));
                ::GetClientRect(hWndCenter, &rcCenter);
                ::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
        }

        // find dialog's upper left based on rcCenter
        int xLeft = (rcCenter.left + rcCenter.right) / 2 - (rcDlg.right-rcDlg.left) / 2;
        int yTop = (rcCenter.top + rcCenter.bottom) / 2 - (rcDlg.bottom-rcDlg.top) / 2;

        // if the dialog is outside the screen, move it inside
        if (xLeft < rcArea.left)
                xLeft = rcArea.left;
        else if (xLeft + (rcDlg.right-rcDlg.left) > rcArea.right)
                xLeft = rcArea.right - (rcDlg.right-rcDlg.left);

        if (yTop < rcArea.top)
                yTop = rcArea.top;
        else if (yTop + (rcDlg.bottom-rcDlg.top) > rcArea.bottom)
                yTop = rcArea.bottom - (rcDlg.bottom-rcDlg.top);

        // map screen coordinates to child coordinates
        SetWindowPos(hwnd, NULL, xLeft, yTop, -1, -1,
                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CALLBACK Dialog::DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_INITDIALOG) {
    SetWindowLong(hwndDlg, GWL_USERDATA, lParam);
    CenterWindow(hwndDlg);
  }
  Dialog *This = reinterpret_cast<Dialog *>(GetWindowLong(hwndDlg, GWL_USERDATA));
  if (uMsg == WM_INITDIALOG) {
    This->hwnd = hwndDlg;
  }
  if (This == NULL) {
    return FALSE;
  }
  return This->DialogProc(uMsg, wParam, lParam);
}

BOOL SetDlgItemFloat(HWND hwnd, int id, float value)
{
  char buf[40];
  sprintf(buf, "%g", value);
  return SetDlgItemText(hwnd, id, buf);
}

float GetDlgItemFloat(HWND hwnd, int id, float def)
{
  char buf[40];
  GetDlgItemText(hwnd, id, buf, sizeof(buf));
  float r;
  if (sscanf(buf, "%f", &r) <= 0) {
    return def;
  }
  return r;
}

/*
 * subclasswnd.cpp
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * C++ class to subclass a Windows window
 */

#include <windows.h>
#pragma hdrstop

#include "subclasswnd.h"

SubclassedWindow::SubclassedWindow(HWND hwnd)
 : Hwnd(hwnd)
{
  OldWndProc = (WNDPROC)GetWindowLong(Hwnd, GWL_WNDPROC);
  SetWindowLong(Hwnd, GWL_WNDPROC, (long)StaticWndProc);
  SetWindowLong(Hwnd, GWL_USERDATA, (long)this);
}

SubclassedWindow::~SubclassedWindow()
{
  SetWindowLong(Hwnd, GWL_WNDPROC, (long)OldWndProc);
}

LRESULT CALLBACK SubclassedWindow::StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  SubclassedWindow *This = reinterpret_cast<SubclassedWindow *>(GetWindowLong(hwnd, GWL_USERDATA));
  LRESULT r = This->WndProc(hwnd, msg, wparam, lparam);
  if (msg == WM_NCDESTROY) { // last message this window will get
    SetWindowLong(This->Hwnd, GWL_WNDPROC, (long)This->OldWndProc);
    delete This;
  }
  return r;
}

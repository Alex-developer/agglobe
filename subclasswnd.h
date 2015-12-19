/*
 * subclasswnd.h
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * C++ class to subclass a Windows window
 */

#pragma once

class SubclassedWindow {
public:
  SubclassedWindow(HWND hwnd);
  virtual ~SubclassedWindow();
protected:
  const HWND Hwnd;
  WNDPROC OldWndProc;
  static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
  virtual LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
};

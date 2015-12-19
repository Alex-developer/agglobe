/*
 * dialog.h
 *
 * Copyright (C) 1998 Greg Hewgill
 *
 * Generic dialog box support.
 */

class Dialog {
public:
  Dialog(HWND parent, LPCTSTR resid);
  int DoModal();
  virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
  const HWND Parent;
  const LPCTSTR Resid;
  HWND hwnd;
  static BOOL CALLBACK Dialog::DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

BOOL SetDlgItemFloat(HWND hwnd, int id, float value);
float GetDlgItemFloat(HWND hwnd, int id, float def);

/*
 * registry.cpp
 *
 * Copyright (C) 1996 Greg Hewgill
 *
 * Window registry support
 *
 */

#include <windows.h>
#include <stdio.h>
#pragma hdrstop

#include "registry.h"

void SetRegistry(const char *key, const char *name, DWORD type, const void *data, DWORD datasize)
{
  HKEY k;
  DWORD d;
  if (RegCreateKeyEx(HKEY_CURRENT_USER, key, 0, NULL, 0, KEY_WRITE, NULL, &k, &d) == ERROR_SUCCESS) {
    RegSetValueEx(k, name, 0, type, (BYTE *)data, datasize);
    RegCloseKey(k);
  }
}

BOOL GetRegistry(const char *key, const char *name, DWORD type, void *data, DWORD datasize)
{
  BOOL ok = FALSE;
  HKEY k;
  if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &k) == ERROR_SUCCESS) {
    DWORD t;
    if (RegQueryValueEx(k, (char *)name, NULL, &t, (BYTE *)data, &datasize) == ERROR_SUCCESS) {
      ok = t == type;
    }
    RegCloseKey(k);
  }
  return ok;
}

void SetRegistryString(const char *name, const char *data, const char *key)
{
  SetRegistry(key, name, REG_SZ, data, strlen(data)+1);
}

BOOL GetRegistryString(const char *name, char *data, DWORD datasize, const char *key)
{
  return GetRegistry(key, name, REG_SZ, data, datasize);
}

void SetRegistryDword(const char *name, DWORD data, const char *key)
{
  SetRegistry(key, name, REG_DWORD, &data, sizeof(data));
}

BOOL GetRegistryDword(const char *name, DWORD &data, const char *key)
{
  return GetRegistry(key, name, REG_DWORD, &data, sizeof(data));
}

void SetRegistryBool(const char *name, BOOL data, const char *key)
{
  SetRegistry(key, name, REG_DWORD, &data, sizeof(data));
}

BOOL GetRegistryBool(const char *name, BOOL &data, const char *key)
{
  return GetRegistry(key, name, REG_DWORD, &data, sizeof(data));
}

void SetRegistryBinary(const char *name, const void *data, DWORD datasize, const char *key)
{
  SetRegistry(key, name, REG_BINARY, data, datasize);
}

BOOL GetRegistryBinary(const char *name, void *data, DWORD datasize, const char *key)
{
  return GetRegistry(key, name, REG_BINARY, data, datasize);
}

void SetRegistryFloat(const char *name, float data, const char *key)
{
  char buf[40];
  sprintf(buf, "%g", data);
  SetRegistryString(name, buf, key);
}

BOOL GetRegistryFloat(const char *name, float &data, const char *key)
{
  char buf[40];
  if (!GetRegistryString(name, buf, sizeof(buf))) {
    return FALSE;
  }
  return sscanf(buf, "%f", &data) == 1;
}

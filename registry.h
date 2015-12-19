/*
 * registry.h
 *
 * Copyright (C) 1996 Greg Hewgill
 *
 * Window registry support
 *
 */

#ifndef __REGISTRY_H
#define __REGISTRY_H

extern const char *DefaultRegistryKey;

void SetRegistryString(const char *name, const char *data, const char *key = DefaultRegistryKey);
BOOL GetRegistryString(const char *name, char *data, DWORD datasize, const char *key = DefaultRegistryKey);

void SetRegistryDword(const char *name, DWORD data, const char *key = DefaultRegistryKey);
BOOL GetRegistryDword(const char *name, DWORD &data, const char *key = DefaultRegistryKey);

void SetRegistryBool(const char *name, BOOL data, const char *key = DefaultRegistryKey);
BOOL GetRegistryBool(const char *name, BOOL &data, const char *key = DefaultRegistryKey);

void SetRegistryBinary(const char *name, const void *data, DWORD datasize, const char *key = DefaultRegistryKey);
BOOL GetRegistryBinary(const char *name, void *data, DWORD datasize, const char *key = DefaultRegistryKey);

void SetRegistryFloat(const char *name, float data, const char *key = DefaultRegistryKey);
BOOL GetRegistryFloat(const char *name, float &data, const char *key = DefaultRegistryKey);

#endif

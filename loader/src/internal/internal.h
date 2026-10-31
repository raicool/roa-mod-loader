#pragma once

#ifndef LOADER_DLL
#define LOADER_DLL __declspec(dllexport)
#endif

#define INCREMENT_POINTER(x, idx) ((char*)x + idx)
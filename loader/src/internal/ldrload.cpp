#include "internal/ldrload.h"

#include <SubAuth.h>
#include <loader/log.h>

typedef NTSTATUS(__stdcall* pLdrLoadDll) (
	PWCHAR PathToFile,
	ULONG Flags,
	PUNICODE_STRING ModuleFileName,
	PHANDLE ModuleHandle
	);

typedef void(__stdcall* pRtlInitUnicodeString) (
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString
	);

HMODULE internal_load_library(LPCWSTR lpFileName)
{
	UNICODE_STRING ustrModule;
	HANDLE hModule = NULL;

	HMODULE hNtdll = GetModuleHandle("ntdll.dll");
	pRtlInitUnicodeString RtlInitUnicodeString = (pRtlInitUnicodeString)GetProcAddress(hNtdll, "RtlInitUnicodeString");

	RtlInitUnicodeString(&ustrModule, lpFileName);

	pLdrLoadDll LdrLoadDll = (pLdrLoadDll)GetProcAddress(GetModuleHandle("ntdll.dll"), "LdrLoadDll");
	if (!LdrLoadDll)
	{
		return NULL;
	}

	NTSTATUS status = LdrLoadDll(NULL, 0, &ustrModule, &hModule);
	loader_log_info("LdrLoadDll returned 0x{:08x}", (unsigned)status);
	return (HMODULE)hModule;
}
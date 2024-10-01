// McGee Power Plan.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
//#include <stdio.h>
//#include <strsafe.h>

using namespace std;

GUID McGeeScheme;

//*************************************************************
//
//  RegDelnodeRecurse()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************
/*
BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
	LPTSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	TCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;

	// First, see if we can delete the key without having
	// to recurse.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;

	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND) {
			printf("Deleting Key. Key not found.\n");
			return TRUE;
		}
		else if (lResult == ERROR_ACCESS_DENIED) {
			printf("Deleting Key. Access Denied.\n");
			return TRUE;
		}
		else {
			printf("Deleting Key. Error opening key.\n");
			return FALSE;
		}
	}

	// Check for an ending slash and add one if it is missing.

	lpEnd = lpSubKey + lstrlen(lpSubKey);

	if (*(lpEnd - 1) != TEXT('\\'))
	{
		*lpEnd = TEXT('\\');
		lpEnd++;
		*lpEnd = TEXT('\0');
	}

	// Enumerate the keys

	dwSize = MAX_PATH;
	lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
		NULL, NULL, &ftWrite);

	if (lResult == ERROR_SUCCESS)
	{
		do {

			*lpEnd = TEXT('\0');
			StringCchCat(lpSubKey, MAX_PATH * 2, szName);

			if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
				break;
			}

			dwSize = MAX_PATH;

			lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
				NULL, NULL, &ftWrite);

		} while (lResult == ERROR_SUCCESS);
	}

	lpEnd--;
	*lpEnd = TEXT('\0');

	RegCloseKey(hKey);

	// Try again to delete the key.

	lResult = RegDeleteKey(hKeyRoot, lpSubKey);

	if (lResult == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}
*/
//*************************************************************
//
//  RegDelnode()
//
//  Purpose:    Deletes a registry key and all its subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************
/*
BOOL RegDelnode(HKEY hKeyRoot, LPCTSTR lpSubKey)
{
	TCHAR szDelKey[MAX_PATH * 2];

	StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
	return RegDelnodeRecurse(hKeyRoot, szDelKey);

}
*/

/*
void ResetDefaultActivePowerScheme() {

	// Only works on Windows 10. On Windows 7, only the SYSTEM user account has security perms to modify this key.
	// Reg key update in a GPO should do it.

	LONG myResult;
	HKEY myKey;
	LPCTSTR Balanced = TEXT("381b4222-f694-41f0-9685-ff5bb260df2e");

	myResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\Power\\User\\Default\\PowerSchemes"), 0, KEY_SET_VALUE, &myKey);

	if (myResult != ERROR_SUCCESS)
	{
		if (myResult == ERROR_FILE_NOT_FOUND) {
			printf("PowerSchemes key not found.\n");
			
		}
		else if (myResult == ERROR_ACCESS_DENIED) {
			printf("PowerSchemes key access denied.\n");

		}
		else {
			printf("Error opening PowerSchemes key.\n");
			
		}
	}

	RegSetValueEx(myKey, TEXT("ActivePowerScheme"), 0, REG_SZ, (LPBYTE)Balanced, _tcslen(Balanced) * sizeof(TCHAR));

	RegCloseKey(myKey);
}
*/

void DeleteOldMcGeeScheme() {

	typedef DWORD(WINAPI *PowerDeleteScheme)(
		HKEY       RootPowerKey,
		const GUID *SchemeGuid
	);

	PowerDeleteScheme PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerDeleteScheme)GetProcAddress(dll, "PowerDeleteScheme");
		if (NULL != PowerFunc) {
			GUID oldMcGeePlan;
			CLSIDFromString(L"{811ae50a-73cb-4477-930f-d42e8ab8a6bd}", &oldMcGeePlan);
			PowerFunc(NULL, &oldMcGeePlan);
		}
		else {
			cout << "Error: Failed to load 'PowerDeleteScheme' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}
}

void ReplaceDefaultSchemes() {

	typedef DWORD(WINAPI *PowerReplaceDefaultPowerSchemes)();

	PowerReplaceDefaultPowerSchemes PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerReplaceDefaultPowerSchemes)GetProcAddress(dll, "PowerReplaceDefaultPowerSchemes");
		if (NULL != PowerFunc) {
			PowerFunc();
		}
		else {
			cout << "Error: Failed to load 'PowerReplaceDefaultPowerSchemes' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}
}

void RestoreDefaultSchemes() {

	typedef DWORD(WINAPI *PowerRestoreDefaultPowerSchemes)();

	PowerRestoreDefaultPowerSchemes PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerRestoreDefaultPowerSchemes)GetProcAddress(dll, "PowerRestoreDefaultPowerSchemes");
		if (NULL != PowerFunc) {
			PowerFunc();
		}
		else {
			cout << "Error: Failed to load 'PowerRestoreDefaultPowerSchemes' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}
}

GUID GetActivePowerScheme() {

	// Create blank Guid
	GUID var = GUID();
	// a pointer to var, a blank Guid
	GUID *ptr2;
	// a pointer which points to ptr2
	GUID **ptr1;
	// setting ptr2 to the address of var (point at var)
	ptr2 = &var;
	// setting ptr1 to point at ptr2, which in turn points at var
	ptr1 = &ptr2;

	typedef DWORD(WINAPI *PowerGetActiveScheme)(
		HKEY UserRootPowerKey,
		GUID **ActivePolicyGuid
		);

	PowerGetActiveScheme PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerGetActiveScheme)GetProcAddress(dll, "PowerGetActiveScheme");
		if (NULL != PowerFunc) {
			PowerFunc(NULL, ptr1);
		}
		else {
			cout << "Error: Failed to load 'PowerGetActiveScheme' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}

	return *ptr2;
}

wchar_t* GetCurrentSchemeName() {
	typedef DWORD(WINAPI *PowerReadFriendlyName)(
		HKEY       RootPowerKey,
		const GUID *SchemeGuid,
		const GUID *SubGroupOfPowerSettingsGuid,
		const GUID *PowerSettingGuid,
		PUCHAR     Buffer,
		LPDWORD    BufferSize
		);

	PowerReadFriendlyName PowerFunc;

	wchar_t* wName = new wchar_t[50]; //50 wide unicode characters is max size.
	DWORD NameSize = sizeof(wchar_t[50]);
	DWORD result;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerReadFriendlyName)GetProcAddress(dll, "PowerReadFriendlyName");
		if (NULL != PowerFunc) {
			const GUID activeScheme = GetActivePowerScheme();
			result = PowerFunc(NULL, &activeScheme, NULL, NULL, (UCHAR*)wName, &NameSize);

			//cout << result << endl;
			//wcout << wName << endl;
			return wName;
		}
		else {
			cout << "Error: Failed to load 'PowerReadFriendlyName' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}
	return NULL;
}

GUID DuplicateScheme(GUID SourceSchemeGuid) {

	// Create blank Guid
	GUID var = GUID();
	// a pointer to var, a blank Guid that later gets generated by by CoCreateGuid() 
	GUID *ptr2;
	// a pointer which points to ptr2
	GUID **ptr1;
	// setting ptr2 to the address of var (point at var)
	ptr2 = &var;
	// create GUID where ptr2 points to (i.e. var)
	CoCreateGuid(ptr2);
	// setting ptr1 to point at ptr2, which in turn points at var
	ptr1 = &ptr2;

	typedef DWORD(WINAPI *PowerDuplicateScheme)(
		HKEY       RootPowerKey,
		const GUID *SourceSchemeGuid,
		GUID       **DestinationSchemeGuid
		);

	PowerDuplicateScheme PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerDuplicateScheme)GetProcAddress(dll, "PowerDuplicateScheme");
		if (NULL != PowerFunc) {
			PowerFunc(NULL, &SourceSchemeGuid, ptr1);
		}
		else {
			cout << "Error: Failed to load 'PowerDuplicateScheme' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}

	return *ptr2;
}

void SetActivePowerScheme(GUID scheme) {

	typedef DWORD(WINAPI *PowerSetActiveScheme)(
		HKEY       UserRootPowerKey,
		const GUID *SchemeGuid
		);

	PowerSetActiveScheme PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerSetActiveScheme)GetProcAddress(dll, "PowerSetActiveScheme");
		if (NULL != PowerFunc) {
			const GUID myScheme = scheme;
			PowerFunc(NULL, &myScheme);
		}
		else {
			cout << "Error: Failed to load 'PowerSetActiveScheme' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}
}

void WriteSchemeName() {

	typedef DWORD(WINAPI *PowerWriteFriendlyName)(
		HKEY       RootPowerKey,
		const GUID *SchemeGuid,
		const GUID *SubGroupOfPowerSettingsGuid,
		const GUID *PowerSettingGuid,
		UCHAR      *Buffer,
		DWORD      BufferSize
		);

	PowerWriteFriendlyName PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerWriteFriendlyName)GetProcAddress(dll, "PowerWriteFriendlyName");
		if (NULL != PowerFunc) {
			const GUID myScheme = McGeeScheme;
			PowerFunc(NULL, &myScheme, NULL, NULL, (UCHAR*)L"McGee Power Scheme", 18*2);
		}
		else {
			cout << "Error: Failed to load 'PowerWriteFriendlyName' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}
}

void WriteSchemeDescription() {

	typedef DWORD(WINAPI *PowerWriteDescription)(
		HKEY       RootPowerKey,
		const GUID *SchemeGuid,
		const GUID *SubGroupOfPowerSettingsGuid,
		const GUID *PowerSettingGuid,
		UCHAR      *Buffer,
		DWORD      BufferSize
		);

	PowerWriteDescription PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerWriteDescription)GetProcAddress(dll, "PowerWriteDescription");
		if (NULL != PowerFunc) {
			const GUID myScheme = McGeeScheme;
			PowerFunc(NULL, &myScheme, NULL, NULL, (UCHAR*)L"Prevents sleep on closing the lid.", 34*2);
		}
		else {
			cout << "Error: Failed to load 'PowerWriteDescription' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}
};

void WriteAcPwrLidCloseAction() {

	typedef DWORD(WINAPI *PowerWriteACDefaultIndex)(
		HKEY       RootSystemPowerKey,
		const GUID *SchemePersonalityGuid,
		const GUID *SubGroupOfPowerSettingsGuid,
		const GUID *PowerSettingGuid,
		DWORD      DefaultAcIndex
		);

	PowerWriteACDefaultIndex PowerFunc;

	HMODULE dll = LoadLibraryA("PowrProf.dll");
	if (NULL != dll) {
		PowerFunc = (PowerWriteACDefaultIndex)GetProcAddress(dll, "PowerWriteACDefaultIndex");
		if (NULL != PowerFunc) {
			const GUID McGScheme = McGeeScheme;
			PowerFunc(NULL, &GUID_TYPICAL_POWER_SAVINGS/*Balanced*/, &GUID_SYSTEM_BUTTON_SUBGROUP, &GUID_LIDCLOSE_ACTION, 0);
			PowerFunc(NULL, &GUID_MIN_POWER_SAVINGS/*High Performance*/, &GUID_SYSTEM_BUTTON_SUBGROUP, &GUID_LIDCLOSE_ACTION, 0);
			PowerFunc(NULL, &GUID_MAX_POWER_SAVINGS/*Power Saver*/, &GUID_SYSTEM_BUTTON_SUBGROUP, &GUID_LIDCLOSE_ACTION, 0);
			PowerFunc(NULL, &McGScheme/*McGee Power Scheme*/, &GUID_SYSTEM_BUTTON_SUBGROUP, &GUID_LIDCLOSE_ACTION, 0);
		}
		else {
			cout << "Error: Failed to load 'PowerWriteACDefaultIndex' function." << endl;
		}
	}
	else {
		cout << "Error: Failed to load 'PowrProf.dll' library." << endl;
	}
};

int main()
{
	// Hides the console window as soon as it's created. Not needed - compiler options specify not to create a console window for the application.
		//ShowWindow(GetConsoleWindow(), SW_HIDE);	

	// These functions change the power plans by modifying the registry. On Win7 these return ERROR_ACCESS_DENIED as NTAuthority\SYSTEM is the only user capable of modifying these keys.
		//ResetDefaultActivePowerScheme();
		//RegDelnode(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\Power\\User\\Default\\PowerSchemes\\811ae50a-73cb-4477-930f-d42e8ab8a6bd")); //Delete old plan (was set as a default)
		//RegDelnode(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\Power\\User\\PowerSchemes\\811ae50a-73cb-4477-930f-d42e8ab8a6bd")); //Delete old plan
		//SetActivePowerScheme(GUID_TYPICAL_POWER_SAVINGS/*Balanced*/);
	
	// 1. Restore the default power schemes. This restores the McGee Power Scheme as the current scheme if it exists.
	RestoreDefaultSchemes();

	// 2. If the McGee Power Scheme is the current scheme, do nothing. If not, continue to step 3.
	wchar_t *CurrentSchemeName = GetCurrentSchemeName();
	if (wcscmp(CurrentSchemeName, L"McGee Power Scheme") == 0) {
		cout << "McGee Power Scheme is already the default active scheme. Nothing to run." << endl;
		// Prevent console window closing
			//cin.get();

		return 0;
	}
	else {
		// 3.Duplicate the High Performance power scheme and return it's GUID
		McGeeScheme = DuplicateScheme(GUID_MIN_POWER_SAVINGS/*High Performance*/);
		// 4. Set the new scheme as the active power scheme
		SetActivePowerScheme(McGeeScheme);
		// 5. Name the new scheme 'McGee Power Scheme'
		WriteSchemeName();
		// 6. Set a description for the McGee Power Scheme 
		WriteSchemeDescription();
		// 7. Set the 'Balanced', 'High Performance', 'Power Saver' and 'McGee Power Scheme' schemes to 'Do Nothing' on lid close
		WriteAcPwrLidCloseAction();
		// 8. Delete the old 'McGee Power Plan' from user schemes. This scheme was created by and old logon script.
		DeleteOldMcGeeScheme();
		// 9. Replace the default power schemes with the current user power schemes. This should then remove the old 'McGee Power Plan' and add the new 'McGee Power Scheme' as a defualt scheme. 
		ReplaceDefaultSchemes();
	}
	
	cout << "Process complete. Press 'Enter' to exit...";
	cin.get(); // Prevent console window closing

	return 0;
}
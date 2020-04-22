#ifndef _LRD_REGISTER_NOTIFICATION_H_
#define _LRD_REGISTER_NOTIFICATION_H_

#define STATUS_SUCCESS 0
#define LDR_DLL_NOTIFICATION_REASON_LOADED 1
#define LDR_DLL_NOTIFICATION_REASON_UNLOADED 0

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
	ULONG Flags;                    //Reserved.
	PUNICODE_STRING FullDllName;   //The full path name of the DLL module.
	PUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
	PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
	ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_UNLOADED_NOTIFICATION_DATA, *PLDR_DLL_UNLOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
	ULONG Flags;                    //Reserved.
	PUNICODE_STRING FullDllName;   //The full path name of the DLL module.
	PUNICODE_STRING BaseDllName;   //The base file name of the DLL module.
	PVOID DllBase;                  //A pointer to the base address for the DLL in memory.
	ULONG SizeOfImage;              //The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA {
	LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
	LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

typedef VOID (*PLDR_DLL_NOTIFICATION_FUNCTION)(
	_In_     ULONG                       NotificationReason,
	_In_     PLDR_DLL_NOTIFICATION_DATA NotificationData,
	_In_opt_ PVOID                       Context
);

typedef NTSTATUS (*PLDR_REGISTER_NOTIFICATION)(
	_In_     ULONG                          Flags,
	_In_     PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
	_In_opt_ PVOID                          Context,
	_Out_    PVOID                          *Cookie
);

#endif // !_LRD_REGISTER_NOTIFICATION_H_

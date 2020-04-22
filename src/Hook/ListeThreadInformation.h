#ifndef _TLISTETHREADINFORMATION_H_
#define _TLISTETHREADINFORMATION_H_

/**
 * Used internally
 */
typedef struct ListeThreadInformation_s {
	struct LIST_BASIC_s  *Flink;
	struct LIST_BASIC_s  *Blink;
	void * threadId;
	void * retAddress;
	void * callAddress;
	void * precallback;
	void * postcallback;

	BOOL onlyPrecallback;
	CONTEXT context;

	QWORD stack[32];

	DWORD cpt;
}ListeThreadInformation, *PListeThreadInformation;

/**
 * Saved the called address.
 */
HOOK_LIB void saveCallAddress(_In_ PVOID callAddress_);

/**
 * Get the only pre callback address.
 */
HOOK_LIB PVOID getCallAddress(void);

/**
 * Saved the return address.
 */
HOOK_LIB void saveRetAddress(_In_ PVOID retAddress_);

/**
 * Get the only pre callback address.
 */
HOOK_LIB PVOID getRetAddress(void);

/**
 * Saved the pre callback address.
 */
HOOK_LIB void savePrecallback(_In_ PVOID precallback_);

/**
 * Get the only pre callback address.
 */
HOOK_LIB PVOID getPrecallback(void);

/**
 * Saved the post callback address.
 */
HOOK_LIB void savePostcallback(_In_ PVOID postcallback_);

/**
 * Get the only pre callback address.
 */
HOOK_LIB PVOID getPostcallback(void);

/**
 * Saved the only pre callback address.
 */
HOOK_LIB void saveOnlyPrecallback(_In_ BOOL onlyPrecallback_);

/**
 * Get the only pre callback address.
 */
HOOK_LIB BOOL getOnlyPrecallback(void);

/**
 * Get the register.
 */
HOOK_LIB CONTEXT* getRegister(void);

/**
 * Used internally.
 */
HOOK_LIB void saveStack(_In_ QWORD stackValue, _In_ BYTE stackIndex);

/**
 * Used internally.
 */
HOOK_LIB QWORD* getStack(void);


#endif // !_TLISTETHREADINFORMATION_H_

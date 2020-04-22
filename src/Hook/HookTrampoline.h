#ifndef _HOOKTRAMPOLINE_H_
#define _HOOKTRAMPOLINE_H_



/**
 * Used internally
 */
HOOK_LIB BOOL initHook();


/**
 * Place a hook at functionToReplace which could goes to functionWhichReplace or execute a optional precallback or execute a optional postcallback.
 *
 *
 * @param[in] functionToReplace is the address targetted by the hook.
 * @param[in] functionWhichReplace which is executed instead of the functionToReplace if not NULL.
 * @param[in] precallback is a callback executed before the execution of functionToReplace if not NULL.
 * @param[out] postcallback is a callback executed after the execution of functionToReplace if not NULL.
 * @return TRUE if the function succeed, FALSE otherwise.
 */
HOOK_LIB BOOL placeHookTrampoline(_In_ PVOID functionToReplace, _In_opt_ PVOID functionWhichReplace, _In_opt_ PVOID precallback, _In_opt_ PVOID postcallback);

/**
 * Used internally
 */
UINT64 __fonctionHookGeneric(UINT64 somethingsYouDontCare, ...);

#endif // !_HOOKTRAMPOLINE_H_

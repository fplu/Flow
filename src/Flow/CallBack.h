#ifndef _callback_H_
#define _callback_H_

/**
 * Used internally
 */
void CallbackRun(DOCKER * docker);

/**
 * Used internally
 */
void CallBackExecute(_In_opt_ DOCKER* docker, _In_opt_ INSTRUCTION_READ* instruction);

/**
 * Used internally
 */
void CallBackGet(_In_opt_ DOCKER* docker, _In_opt_ INSTRUCTION_READ* instruction, _Out_writes_bytes_(*pCallbackTabLength * 8) void** callbackTab, _Inout_ DWORD* pCallbackTabLength);


#endif // !_callback_H_

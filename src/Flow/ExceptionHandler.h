#ifndef _EXCEPTION_HANDLER_H_
#define _EXCEPTION_HANDLER_H_

/**
 * Used internally
 */
_LIB_INC_RIP_API_ void ExceptionHandlerInit();

/**
 * Used internally
 */
typedef struct EXCEPTION_PARAMETER_ {
	union {
		CONTEXT context;
		struct {
			BYTE reserved1[0x4F0];
			EXCEPTION_RECORD exceptionRecord;
		};
	};
}EXCEPTION_PARAMETER, *PEXCEPTION_PARAMETER;

#endif // !_EXCEPTION_HANDLER_H_

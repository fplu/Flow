#ifndef _LISTHOOK_H_
#define _LISTHOOK_H_


/**
 * Used internally
 */
typedef struct ListHook_s {
	struct ListHook_s  *Flink;
	struct ListHook_s  *Blink;
	void *data;
	unsigned int indexStartHook;
	size_t sizeHook;
} ListHook, *PListHook;

/**
 * Used internally
 */
BOOL pushListeHook(_Out_ PListHook * head, _In_ size_t sizeHook);
#endif

#ifndef _LIST_BASIC_H_
#define _LIST_BASIC_H_

#include <sal.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>


typedef struct LIST_BASIC_s {
	struct LIST_BASIC_s  *Flink;//Suivant
	struct LIST_BASIC_s  *Blink;//Avant
	void * Data;
}LIST_BASIC, *PLIST_BASIC;


BOOL createNodeLIST_BASIC(_In_ void* data, _In_ size_t nodeSize, _Out_ PLIST_BASIC * node);
void freeLIST_BASIC(_In_ PLIST_BASIC * head);

BOOL pushNodeLIST_BASIC(_In_ void* data, _In_ size_t nodeSize, _Inout_ PLIST_BASIC * head);
BOOL AddNodeListBasic(_In_ void* data, _In_ size_t nodeSize, _Inout_ PLIST_BASIC * head);
BOOL movNodeToEndLIST_BASIC(_In_ PLIST_BASIC * head, _In_ PLIST_BASIC nodeToMov);


_Success_(return) BOOL PopNodeListBasic(_In_ PLIST_BASIC * head, _Outptr_opt_ PLIST_BASIC * nodePoped, _Out_opt_ void ** data);

//BOOL parcoursBASICLIST(_In_ PLIST_BASIC head);
BOOL IsDataInListBasic(_In_ void* Data, _In_opt_ PLIST_BASIC head);

BOOL AddListBasic(_In_ PLIST_BASIC list, _Inout_ PLIST_BASIC * head);

BOOL inverseLIST_BASIC(_Inout_ PLIST_BASIC * node1, _Inout_ PLIST_BASIC * node2);

#endif // !_LIST_BASIC_H_

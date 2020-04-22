#ifndef _GRAPH_BASIC_H_
#define _GRAPH_BASIC_H_

#include <sal.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "MACRO.h"
#include "LIST_BASIC.h"

#ifdef _LIB_C_GRAPH_
#define LIB_API_GRAPH __declspec(dllexport)
#else
#define LIB_API_GRAPH __declspec(dllimport)
#endif

typedef struct GRAPH_BASIC_s GRAPH_BASIC, *PGRAPH_BASIC;
typedef struct TreeBASIC_s TreeBASIC, *PTreeBASIC;
typedef struct ARCBASIC_s ARCBASIC, *PARCBASIC;

struct GRAPH_BASIC_s {
	PARCBASIC ARC;
	BOOL visited;
	void * Data;
};

struct TreeBASIC_s {
	PARCBASIC ARC;
	BOOL visited;
	void * Data;

	PTreeBASIC father;
};


struct ARCBASIC_s {
	struct ARCBASIC_s  *Flink;//Suivant
	struct ARCBASIC_s  *Blink;//Avant
	PGRAPH_BASIC Sommet;
	QWORD poid;
};

typedef struct LIST_BASICDJIKSTRA_s {
	struct LIST_BASICDJIKSTRA_s  *Flink;//Suivant
	struct LIST_BASICDJIKSTRA_s  *Blink;//Avant
	PGRAPH_BASIC Sommet;
	QWORD poid;
	PTreeBASIC noeudTree;
}LIST_BASICDJISKTRA, *PLIST_BASICDJISKTRA;


typedef struct ARGDJISKSTRA_s {
	PGRAPH_BASIC start;
	PGRAPH_BASIC end;
	DWORD visitedFlag;
	PLIST_BASIC * chemin;
}ARGDJISKSTRA, *PARGDJISKSTRA;

BOOL createNodeGRAPH_BASIC(_In_ void * data, _In_ size_t nodeSize, _Out_ PGRAPH_BASIC * node);
BOOL createSonNodeGRAPH_BASIC(_In_ void * data, _In_ size_t nodeSize, _In_ size_t arcSize, _Inout_ PGRAPH_BASIC node);
void freeGRAPH_BASIC(_In_ BOOL freeData, _Inout_ PGRAPH_BASIC * node);

BOOL joinOneNodeGRAPH_BASIC(_In_ QWORD poid, _In_ size_t arcSize, _Inout_ PGRAPH_BASIC node1, _Inout_ PGRAPH_BASIC node2);
BOOL joinBothNodeGRAPH_BASIC(_In_ QWORD poid1, _In_ QWORD poid2, _In_ size_t arcSize, _Inout_ PGRAPH_BASIC node1, _Inout_ PGRAPH_BASIC node2);

BOOL parcoursGRAPH_BASIC(_In_ PGRAPH_BASIC start, _In_ BOOL parcoursProfondeur);
BOOL parcoursProfondeurListGRAPH_BASIC(_In_ PGRAPH_BASIC start, _Out_ PLIST_BASIC * listeRes);
BOOL parcoursLargeurListGRAPH_BASIC(_In_ PGRAPH_BASIC start, _Out_ PLIST_BASIC * listeRes);
BOOL clearVisitedListGRAPH_BASIC(_In_ PGRAPH_BASIC start);



#endif // !_GRAPH_BASIC_H_

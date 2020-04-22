
#include "GRAPH_BASIC.h"

BOOL createNodeGRAPH_BASIC(_In_ void * data, _In_ size_t nodeSize, _Out_ PGRAPH_BASIC * node) {
	BOOL success = TRUE;

	__try {
		if (!node) {
			success = FALSE;
			__leave;
		}

		*node = NULL;
		*node = (PGRAPH_BASIC)calloc(1, nodeSize);
		if (!*node) {
			success = FALSE;
			__leave;
		}

		(*node)->ARC = NULL;
		(*node)->visited = FALSE;
		(*node)->Data = data;
	}
	__finally {

	}

	return success;
}

BOOL createSonNodeGRAPH_BASIC(_In_ void * data, _In_ size_t nodeSize, _In_ size_t arcSize, _Inout_ PGRAPH_BASIC node) {
	BOOL success = TRUE;
	PGRAPH_BASIC newNode;
	__try {
		if (!node) {
			success = FALSE;
			__leave;
		}
		if (!createNodeGRAPH_BASIC(data, nodeSize, &newNode)) {
			success = FALSE;
			__leave;
		}
		if (!pushNodeLIST_BASIC(newNode, arcSize, (PLIST_BASIC *)&(node->ARC))) {
			success = FALSE;
			__leave;
		}
	}
	__finally {

	}

	return success;
}

void freeGRAPH_BASIC(_In_ BOOL freeData, _Inout_ PGRAPH_BASIC * node) {
	PLIST_BASIC currentNode = NULL, listeGraph = NULL;

	if (!node || !*node) {
		return;
	}
	if (!clearVisitedListGRAPH_BASIC(*node)) {
		*node = NULL;
		return;
	}
	if (!parcoursLargeurListGRAPH_BASIC(*node, &listeGraph)) {
		*node = NULL;
		return;
	}
	currentNode = listeGraph;
	do {
		if (currentNode->Data) {
			if (((PGRAPH_BASIC)currentNode->Data)->ARC) {
				freeLIST_BASIC((PLIST_BASIC *)&((PGRAPH_BASIC)currentNode->Data)->ARC);
			}
			if (freeData && ((PGRAPH_BASIC)currentNode->Data)->Data) {
				free(&((PGRAPH_BASIC)currentNode->Data)->Data);
			}
			free(currentNode->Data);
		}
		currentNode = currentNode->Flink;
	} while (currentNode != listeGraph);

	freeLIST_BASIC(&listeGraph);

	*node = NULL;
}


BOOL joinOneNodeGRAPH_BASIC(_In_ QWORD poid, _In_ size_t arcSize, _Inout_ PGRAPH_BASIC node1, _Inout_ PGRAPH_BASIC node2) {
	BOOL success = TRUE;
	__try {
		if (!node1 || !node2) {
			success = FALSE;
			__leave;
		}
		if (!pushNodeLIST_BASIC(node2, arcSize, (PLIST_BASIC*)&(node1->ARC))) {
			success = FALSE;
			__leave;
		}
		node1->ARC->poid = poid;
	}
	__finally {

	}
	return success;
}

BOOL joinBothNodeGRAPH_BASIC(_In_ QWORD poid1, _In_ QWORD poid2, _In_ size_t arcSize, _Inout_ PGRAPH_BASIC node1, _Inout_ PGRAPH_BASIC node2) {
	BOOL success = TRUE;
	__try {
		if (!node1 || !node2) {
			success = FALSE;
			__leave;
		}
		if (!pushNodeLIST_BASIC(node2, arcSize, (PLIST_BASIC *)&(node1->ARC))) {
			success = FALSE;
			__leave;
		}
		node1->ARC->poid = poid1;
		if (!pushNodeLIST_BASIC(node1, arcSize, (PLIST_BASIC *)&(node2->ARC))) {
			success = FALSE;
			__leave;
		}
		node2->ARC->poid = poid2;
	}
	__finally {

	}
	return success;
}

//parcoursProfondeur = true => parcoursProfondeur, parcoursProfondeur = false => parcoursLargeur
BOOL parcoursGRAPH_BASIC(_In_ PGRAPH_BASIC start, _In_ BOOL parcoursProfondeur) {
	BOOL success = TRUE;
	PLIST_BASIC listeGraph = NULL, currentNode;

	__try {
		if (!start) {
			success = FALSE;
			__leave;
		}

		//Choix en fonction du mode en parcours
		if (parcoursProfondeur) {
			if (!parcoursProfondeurListGRAPH_BASIC(start, &listeGraph)) {
				success = FALSE;
				__leave;
			}
		}
		else {
			if (!parcoursLargeurListGRAPH_BASIC(start, &listeGraph)) {
				success = FALSE;
				__leave;
			}
		}
		currentNode = listeGraph;
		do {
			_tprintf(_T("%p\t"), ((PGRAPH_BASIC)currentNode->Data)->Data);
			currentNode = currentNode->Flink;
		} while (currentNode != listeGraph);
		_tprintf(_T("\n"));
	}
	__finally {
		if (listeGraph) {
			freeLIST_BASIC(&listeGraph);
		}
	}

	return success;
}

BOOL parcoursProfondeurListGRAPH_BASIC(_In_ PGRAPH_BASIC start, _Out_ PLIST_BASIC * listeRes) {
	BOOL success = TRUE;
	PLIST_BASIC currentStack = NULL, currentNode;
	PARCBASIC currentArc = NULL, firstArc;

	__try {
		if (!start) {
			success = FALSE;
			__leave;
		}
		if (!pushNodeLIST_BASIC(start, sizeof(LIST_BASIC), &currentStack)) {
			success = FALSE;
			__leave;
		}
		if (!AddNodeListBasic(start, sizeof(LIST_BASIC), listeRes)) {
			success = FALSE;
			__leave;
		}
		start->visited = TRUE;

		while (currentStack != NULL) {
			firstArc = ((PGRAPH_BASIC)currentStack->Data)->ARC;
			currentArc = firstArc;
			do {
				if (!currentArc) {
					break;
				}

				if (!currentArc->Sommet->visited) {
					if (!pushNodeLIST_BASIC(currentArc->Sommet, sizeof(LIST_BASIC), &currentStack)) {
						success = FALSE;
						__leave;
					}
					currentArc->Sommet->visited = TRUE;
					if (!AddNodeListBasic(currentArc->Sommet, sizeof(LIST_BASIC), listeRes)) {
						success = FALSE;
						__leave;
					}
					firstArc = ((PGRAPH_BASIC)currentStack->Data)->ARC;
					currentArc = firstArc;
				}
				else {
					currentArc = currentArc->Flink;
					if (currentArc == firstArc) {
						break;
					}
				}
			} while (1);
			PopNodeListBasic(&currentStack, NULL, NULL);
		}
	}
	__finally {
		if (listeRes && *listeRes) {
			currentNode = *listeRes;
			do {
				((PGRAPH_BASIC)currentNode->Data)->visited = FALSE;
				currentNode = currentNode->Flink;
			} while (currentNode != *listeRes);
		}
	}

	return success;
}



BOOL parcoursLargeurListGRAPH_BASIC(_In_ PGRAPH_BASIC start, _Out_ PLIST_BASIC * listeRes) {
	BOOL success = TRUE;
	PLIST_BASIC currentStack = NULL, currentNode;
	PARCBASIC currentArc = NULL, firstArc;

	__try {
		if (!start || !listeRes) {
			success = FALSE;
			__leave;
		}
		*listeRes = NULL;
		if (!pushNodeLIST_BASIC(start, sizeof(LIST_BASIC), &currentStack)) {
			success = FALSE;
			__leave;
		}
		if (!AddNodeListBasic(start, sizeof(LIST_BASIC), listeRes)) {
			success = FALSE;
			__leave;
		}
		start->visited = TRUE;

		while (currentStack != NULL) {
			firstArc = ((PGRAPH_BASIC)currentStack->Data)->ARC;
			currentArc = firstArc;
			do {
				if (!currentArc) {
					break;
				}
				if (!currentArc->Sommet->visited) {
					if (!AddNodeListBasic(currentArc->Sommet, sizeof(LIST_BASIC), &currentStack)) {
						success = FALSE;
						__leave;
					}
					currentArc->Sommet->visited = TRUE;
					if (!AddNodeListBasic(currentArc->Sommet, sizeof(LIST_BASIC), listeRes)) {
						success = FALSE;
						__leave;
					}
				}
				currentArc = currentArc->Flink;
			} while (currentArc != firstArc);


			PopNodeListBasic(&currentStack, NULL, NULL);
		}
	}
	__finally {
		if (listeRes && *listeRes) {
			currentNode = *listeRes;
			do {
				((PGRAPH_BASIC)currentNode->Data)->visited = FALSE;
				currentNode = currentNode->Flink;
			} while (currentNode != *listeRes);
		}
	}

	return success;
}

BOOL clearVisitedListGRAPH_BASIC(_In_ PGRAPH_BASIC start) {
	BOOL success = TRUE;
	PLIST_BASIC currentStack = NULL, currentNode;
	PLIST_BASIC clearList = NULL;
	PARCBASIC currentArc = NULL, firstArc;

	__try {
		if (!start) {
			success = FALSE;
			__leave;
		}
		if (!pushNodeLIST_BASIC(start, sizeof(LIST_BASIC), &currentStack)) {
			success = FALSE;
			__leave;
		}
		if (!AddNodeListBasic(start, sizeof(LIST_BASIC), &clearList)) {
			success = FALSE;
			__leave;
		}
		start->visited = -1;

		while (currentStack != NULL) {
			firstArc = ((PGRAPH_BASIC)currentStack->Data)->ARC;
			currentArc = firstArc;
			do {
				if (!currentArc) {
					break;
				}
				if (currentArc->Sommet->visited != -1) {
					if (!AddNodeListBasic(currentArc->Sommet, sizeof(LIST_BASIC), &currentStack)) {
						success = FALSE;
						__leave;
					}
					currentArc->Sommet->visited = -1;
					if (!AddNodeListBasic(currentArc->Sommet, sizeof(LIST_BASIC), &clearList)) {
						success = FALSE;
						__leave;
					}
				}
				currentArc = currentArc->Flink;
			} while (currentArc != firstArc);


			PopNodeListBasic(&currentStack, NULL, NULL);
		}
	}
	__finally {
		if (clearList) {
			currentNode = clearList;
			do {
				((PGRAPH_BASIC)currentNode->Data)->visited = FALSE;
				currentNode = currentNode->Flink;
			} while (currentNode != clearList);

			freeLIST_BASIC(&clearList);
		}
	}

	return success;
}


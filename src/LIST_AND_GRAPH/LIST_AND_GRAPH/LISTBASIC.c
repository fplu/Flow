
#include "LIST_BASIC.h"


BOOL createNodeLIST_BASIC(_In_ void* data, _In_ size_t nodeSize, _Out_ PLIST_BASIC * node) {
	BOOL success = TRUE;

	__try {
		if (!node) {
			success = FALSE;
			__leave;
		}

		*node = NULL;
		*node = (PLIST_BASIC)calloc(1, nodeSize);
		if (!*node) {
			success = FALSE;
			__leave;
		}


		(*node)->Data = data;
	}
	__finally {

	}
	return success;
}

void freeLIST_BASIC(_In_ PLIST_BASIC * head) {
	PLIST_BASIC headArg = NULL, tmpMachinChose;

	__try {
		if (!head || !*head) {
			__leave;
		}
		headArg = *head;
		do {
			/*	if (!headArg->Flink) {
			free(headArg);
			break;
			}*/
			tmpMachinChose = headArg->Flink;
			free(headArg);
			headArg = tmpMachinChose;
		} while (*head != headArg);
		*head = NULL;
	}
	__finally {

	}

}

BOOL pushNodeLIST_BASIC(_In_ void* data, _In_ size_t nodeSize, _Inout_ PLIST_BASIC * head) {
	BOOL success = TRUE;
	PLIST_BASIC oldHead = NULL;

	__try {
		if (!head) {
			success = FALSE;
			__leave;
		}

		if (!*head) {
			if (!createNodeLIST_BASIC(data, nodeSize, head)) {
				success = FALSE;
				__leave;
			}
			(*head)->Blink = (*head);
			(*head)->Flink = (*head);
			__leave;
		}

		oldHead = *head;
		if (!createNodeLIST_BASIC(data, nodeSize, head)) {
			success = FALSE;
			__leave;
		}

		(*head)->Blink = oldHead->Blink;
		(*head)->Flink = oldHead;
		oldHead->Blink->Flink = (*head);
		oldHead->Blink = (*head);
	}
	__finally {

	}
	return success;
}

BOOL AddNodeListBasic(_In_ void* data, _In_ size_t nodeSize, _Inout_ PLIST_BASIC * head) {
	BOOL success = TRUE;

	__try {
		if (!head) {
			success = FALSE;
			__leave;
		}
		if (!pushNodeLIST_BASIC(data, nodeSize, head)) {
			success = FALSE;
			__leave;
		}
		(*head) = (*head)->Flink;
	}
	__finally {

	}

	return success;
}

BOOL movNodeToEndLIST_BASIC(_In_ PLIST_BASIC * head, _In_ PLIST_BASIC nodeToMov) {
	BOOL success = TRUE;
	PLIST_BASIC nodeMoving;

	__try {
		if (!head || !*head || !nodeToMov) {
			success = FALSE;
			__leave;
		}
		if (*head == nodeToMov) {
			PopNodeListBasic(head, &nodeMoving, NULL);
		}
		else {
			PopNodeListBasic(&nodeToMov, &nodeMoving, NULL);
		}

		AddListBasic(nodeMoving, head);
	}
	__finally {

	}

	return success;
}

//Si nodePoped vaut null libère le premier noeud au lieu de le renvoyer
_Success_(return) BOOL PopNodeListBasic(_In_ PLIST_BASIC * head, _Outptr_opt_ PLIST_BASIC * nodePoped, _Out_opt_ void ** data) {
	BOOL success = TRUE;
	PLIST_BASIC temporaryNode = NULL;
	__try {
		if (!head || !*head) {
			success = FALSE;
			__leave;
		}

		if (data) {
			(*data) = (*head)->Data;
		}
		//Renvoie le noeud poped dans nodePoped
		if (nodePoped) {
			if (!*nodePoped) {
				success = FALSE;
				__leave;
			}

			*nodePoped = *head;

			(*head) = (*head)->Flink;
			if (*head == (*head)->Flink) {
				(*head) = NULL;
			}
			else {
				(*head)->Blink->Blink->Flink = (*head);
				(*head)->Blink = (*head)->Blink->Blink;
			}

			(*nodePoped)->Blink = *nodePoped;
			(*nodePoped)->Flink = *nodePoped;
		}
		//libère le noeud poped
		else {
			temporaryNode = *head;
			(*head) = (*head)->Flink;
			if (*head == (*head)->Flink) {
				(*head) = NULL;
			}
			else {
				(*head)->Blink->Blink->Flink = (*head);
				(*head)->Blink = (*head)->Blink->Blink;
			}
			if (temporaryNode) {
				free(temporaryNode);
			}
		}
	}
	__finally {

	}

	return success;
}


/*BOOL parcoursBASICLIST(_In_ PLIST_BASIC head) {
	BOOL success = TRUE;
	PLIST_BASIC firstLIST_BASIC, currentLIST_BASIC;

	__try {
		if (!head) {
			success = FALSE;
			__leave;
		}
		firstLIST_BASIC = head;
		currentLIST_BASIC = head;
		do {
			_tprintf(_T("%p\t"), currentLIST_BASIC->Data);
			currentLIST_BASIC = currentLIST_BASIC->Flink;
		} while (currentLIST_BASIC != firstLIST_BASIC);
		_tprintf(_T("\n"));
	}
	__finally {

	}


	return success;
}*/

BOOL IsDataInListBasic(_In_ void* Data, _In_opt_ PLIST_BASIC head) {
	BOOL res = FALSE;
	PLIST_BASIC firstLIST_BASIC, currentLIST_BASIC;

	__try {
		if (!head) {
			__leave;
		}
		firstLIST_BASIC = head;
		currentLIST_BASIC = head;
		do {
			if (Data == currentLIST_BASIC->Data) {
				res = TRUE;
				__leave;
			}
			currentLIST_BASIC = currentLIST_BASIC->Flink;
		} while (currentLIST_BASIC != firstLIST_BASIC);
	}
	__finally {

	}
	return res;
}

BOOL AddListBasic(_Inout_ PLIST_BASIC list, _Inout_ PLIST_BASIC * head) {
	BOOL success = TRUE;
	PLIST_BASIC lastElementOfhead;

	__try {
		if (!head || !list) {
			success = FALSE;
			__leave;
		}

		if (!*head) {
			*head = list;
			__leave;
		}

		lastElementOfhead = (*head)->Blink;
		(*head)->Blink = list->Blink;
		list->Blink->Flink = (*head);
		lastElementOfhead->Flink = list;
		list->Blink = lastElementOfhead;
	}
	__finally {

	}

	return success;
}



BOOL inverseLIST_BASIC(_Inout_ PLIST_BASIC * node1, _Inout_ PLIST_BASIC * node2) {
	BOOL success = TRUE;
	PLIST_BASIC tmpLIST_BASIC = NULL;
	__try {
		if (!node1 || !node2 || !*node1 || !*node2) {
			success = FALSE;
			__leave;
		}

		(*node1)->Blink->Flink = *node2;
		(*node1)->Flink->Blink = *node2;
		(*node2)->Blink->Flink = *node1;
		(*node2)->Flink->Blink = *node1;

		tmpLIST_BASIC = (*node1)->Blink;
		(*node1)->Blink = (*node2)->Blink;
		(*node2)->Blink = tmpLIST_BASIC;

		tmpLIST_BASIC = (*node1)->Flink;
		(*node1)->Flink = (*node2)->Flink;
		(*node2)->Flink = tmpLIST_BASIC;

		tmpLIST_BASIC = *node1;
		*node1 = *node2;
		*node2 = tmpLIST_BASIC;
	}
	__finally {

	}

	return success;
}
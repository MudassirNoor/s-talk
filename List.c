//
// Created by Mudassir Noor on 2020-01-24.
//
#include <stdio.h>
#include "List.h"

#define HEADSCOUNT 50
#define NODESCOUNT 500
#define BEFOREHEAD -1
#define BEYONDTAIL 1
List HEADS[HEADSCOUNT];
Node NODES[NODESCOUNT];
List *freeHead;
Node *freeNode;
int IsFirstCall = 1;

void AddToFreeNode(Node *removedNode) {
    removedNode->previous = NULL;
    removedNode->item = NULL;

    // Reattach the removed node to the head of the freeNode list
    // Point freeNode to the head
    // Ensures freeNode is always pointing to the head of the freeNodes
    if (freeNode == NULL) {
        freeNode = removedNode;
        freeNode->next = NULL;
    }
    else {
        removedNode->next = freeNode;
        freeNode->previous = removedNode;
        freeNode = freeNode->previous;
    }
}

Node *GetFreeNode() {
    Node * newNode = freeNode;

    // Set freeNode to the next free node
    // Disconnect any link to the newNode
    // Ensures that freeNode is always pointing to the head of the free nodes
    freeNode = freeNode->next;
    if (freeNode != NULL) {
        freeNode->previous = NULL;
    }

    newNode->previous = NULL;
    newNode->next = NULL;
    return newNode;
}

void AddToFreeHead(List *removedList) {
    removedList->head = NULL;
    removedList->tail = NULL;
    removedList->current = NULL;
    removedList->previousFreeList = NULL;
    removedList->count = 0;
    removedList->currentOutOfBounds = 0;

    // Reattach the removed list to the head of the freeHead list
    // Point freeHead to the head
    // Ensures freeHead is always pointing to the head of the freeHead list
    if (freeHead == NULL) {
        freeHead = removedList;
        freeHead->nextFreeList = NULL;
    }
    else {
        freeHead->previousFreeList = removedList;
        removedList->nextFreeList = freeHead;
        freeHead = freeHead->previousFreeList;
    }
}

/*
 * Function called when ListCreate() is first called.
 * This interconnects the lists and nodes available in the HEADS and NODES array
 * The freeHead and freeNode are set to point to the first item in the HEADS and NODES array
 */
void InstantiateMemorySpaces() {
    freeHead = HEADS;
    freeNode = NODES;
    for (int i = 0; i < HEADSCOUNT - 1; i++) {
        HEADS[i].nextFreeList = &HEADS[i + 1];
        HEADS[i + 1].previousFreeList = &HEADS[i];
    }

    for (int i = 0; i < NODESCOUNT - 1; i++) {
        NODES[i].next = &NODES[i + 1];
        NODES[i + 1].previous = &NODES[i];
    }

    IsFirstCall = 0;
}

List *ListCreate() {
    if (IsFirstCall == 1) {
        InstantiateMemorySpaces();
    }

    if (freeHead == NULL) {
        return NULL;
    }

    List *emptyList = freeHead;

    // Points freeHead to the next free list
    // Disconnect any links to the emptyList
    freeHead = freeHead->nextFreeList;
    if (freeHead != NULL) {
        freeHead->previousFreeList = NULL;
    }

    emptyList->count = 0;
    emptyList->nextFreeList = NULL;
    emptyList->previousFreeList = NULL;

    return emptyList;
}

int ListCount(List *list) {
    if (list == NULL) {
        fprintf(stderr, "No List exist\n");
        return 0;
    }
    return list->count;
}

void *ListFirst(List *list) {
    if (list == NULL) {
        return NULL;
    }
    list->currentOutOfBounds = 0;
    list->current = list->head;
    return list->current;
}

void *ListLast(List *list) {
    if (list == NULL) {
        return NULL;
    }list->currentOutOfBounds = 0;
    list->current = list->tail;
    return list->current;
}

void *ListNext(List *list) {
    if (list == NULL) {
        return NULL;
    }if (list->current == NULL)
        return NULL;
    if (list->current == list->tail) {
        list->currentOutOfBounds = BEYONDTAIL;
    }
    list->current = list->current->next;
    return list->current;
}

void *ListPrev(List *list) {
    if (list == NULL) {
        return NULL;
    }if (list->current == NULL)
        return NULL;
    if (list->current == list->head){
        list->currentOutOfBounds = BEFOREHEAD;
    }
    list->current = list->current->previous;
    return list->current;
}

void *ListCurr(List *list) {
    if (list == NULL || list->current == NULL) {
        return NULL;
    }
    return list->current->item;
}

int ListAdd(List *list, void *item) {
    if (list == NULL) {
        return -1;
    }
    if (freeNode == NULL)
        return -1;

    // Implementation is the same as if it were at the tail
    if (list->currentOutOfBounds == BEYONDTAIL) {
        list->current = list->tail;
    }

    Node * newNode = GetFreeNode();

    newNode->item = item;
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    }
    else if (list->currentOutOfBounds == BEFOREHEAD) {
        // If current pointer was before the head, makes the new node the head
        list->current = list->head;
        list->head = newNode;
        newNode->next = list->current;
        list->current->previous = newNode;
    }
    else {
        if (list->current == list->tail) {
            list->tail = newNode;
        }
        else {
            // Attach the next node to the new node
            newNode->next = list->current->next;
            newNode->next->previous = newNode;
        }
        // Make current node the previous node of the new node
        newNode->previous = list->current;
        list->current->next = newNode;
    }

    list->current = newNode;
    list->count++;

    list->currentOutOfBounds = 0;
    return 0;
}

int ListInsert(List *list, void *item) {
    if (list == NULL) {
        return -1;
    }
    if (freeNode == NULL)
        return -1;

    // Implementation is the same as if it were at the head
    if (list->currentOutOfBounds == BEFOREHEAD) {
        list->current = list->head;
    }

    Node * newNode = GetFreeNode();

    newNode->item = item;
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    }
    else if (list->currentOutOfBounds == BEYONDTAIL) {
        // Make the new node the tail of the list
        list->current = list->tail;
        list->tail = newNode;
        list->current->next = newNode;
        newNode->previous = list->current;
    }
    else {
        if (list->current == list->head){
            list->head = newNode;
        }
        else {
            // Attach the previous node to the new node
            newNode->previous = list->current->previous;
            newNode->previous->next = newNode;
        }
        // Make current node the next node after new node
        newNode->next = list->current;
        list->current->previous = newNode;
    }

    list->current = newNode;
    list->count++;

    list->currentOutOfBounds = 0;
    return 0;
}

int ListAppend(List *list, void *item) {
    if (list == NULL) {
        return -1;
    }
    if (freeNode == NULL)
        return -1;

    Node * newTailNode = GetFreeNode();

    newTailNode->item = item;
    if (list->head == NULL) {
        list->head = newTailNode;
    }
    else {
        newTailNode->previous = list->tail;
        list->tail->next = newTailNode;
    }

    list->tail = newTailNode;
    list->current = newTailNode;
    list->count++;

    list->currentOutOfBounds = 0;
    return 0;
}

int ListPrepend(List *list, void *item) {
    if (freeNode == NULL || list == NULL)
        return -1;

    Node * newHeadNode = GetFreeNode();

    newHeadNode->item = item;
    if (list->head == NULL) {
        list->head = newHeadNode;
        list->tail = newHeadNode;
    }
    else {
        newHeadNode->next = list->head;
        list->head->previous = newHeadNode;
    }

    list->head = newHeadNode;
    list->current = newHeadNode;
    list->count++;

    list->currentOutOfBounds = 0;
    return 0;
}

void *ListRemove(List *list) {
    Node *removedNode = list->current;

    if (list->currentOutOfBounds == BEFOREHEAD || list->currentOutOfBounds == BEYONDTAIL || list->head == NULL) {
        return NULL;
    }

    if (list->head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
        list->current = NULL;
    }
    else if (list->tail == list->current) {
        list->tail = list->current->previous;
        list->tail->next = NULL;
        list->current = list->tail->next;
        list->currentOutOfBounds = BEYONDTAIL;
    }
    else {
        if (list->head == list->current) {
            list->head = list->current->next;
            list->head->previous = NULL;
            list->current = list->head;
        }
        else {
            // Attach the previous node to the next node and thus cut off connection to the current node
            list->current->previous->next = list->current->next;
            list->current->next->previous = list->current->previous;
            list->current = list->current->next;
        }
    }
    list->count--;

    void *dataRemoved = removedNode->item;
    AddToFreeNode(removedNode);
    return dataRemoved;
}

void ListConcat(List *list1, List *list2) {
    if (list1->head == NULL && list2->head == NULL) {
        AddToFreeHead(list2);
    }
    else if (list2->head == NULL) {
        AddToFreeHead(list2);
    }
    else if (list1->head == NULL) {
        // If list 1 is empty, point list1 elements to list2 and free list 2
        list1->head = list2->head;
        list1->tail = list2->tail;
        ListFirst(list1);
        list1->count = list2->count;
        AddToFreeHead(list2);
    }
    else {
        list2->head->previous = list1->tail;
        list1->tail->next = list2->head;
        list1->tail = list2->tail;
        list1->count += list2->count;

        AddToFreeHead(list2);
    }
}

void ListFree(List *list, void (*itemFree)(void *)) {
    if (list == NULL) {
        return;
    }
    if (list->head != NULL) {
        list->current = list->head;
        while (list->current != list->tail->next) {
            (*itemFree)(list->current->item);
            list->current = list->current->next;
        }

        // As the nodes in the list are already linked to each other, connect the tail to the freeNodes head
        // Point freeNodes to start (list->head)
        // The entire list nodes are thus reattached to the freeNodes list
        if (freeNode != NULL) {
            freeNode->previous = list->tail;
            list->tail->next = freeNode;
        }
        freeNode = list->head;
    }

    AddToFreeHead(list);
}

void *ListTrim(List *list) {
    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    Node *removedNode = list->tail;
    if (list->head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
        list->current = NULL;
    }
    else {
        list->tail = list->tail->previous;
        list->tail->next = NULL;
        list->current = list->tail;
    }

    void *dataRemoved = removedNode->item;
    AddToFreeNode(removedNode);

    list->count--;
    list->currentOutOfBounds = 0;
    return dataRemoved;
}

void *ListSearch(List *list, int (*comparator)(void *, void *), void *comparisonArg) {
    ListFirst(list);
    if (list->head == NULL)
        return NULL;

    // The while loop continues until the match is found or the current pointer is beyond the tail
    int matchFound = 0;
    while (list->current != list->tail->next) {
        matchFound = (*comparator)(list->current->item, comparisonArg);
        if (matchFound == 1) {
            return list->current->item;
        }
        list->current = list->current->next;
    }

    list->currentOutOfBounds = BEYONDTAIL;
    return list->current;
}
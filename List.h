//
// Created by Mudassir Noor on 2020-01-24.
//
#ifndef LIST_H
#define LIST_H
typedef struct List List;
typedef struct Node Node;

struct Node {
    void *item;
    Node *next;
    Node *previous;
};

struct List {
    Node *head;
    Node *tail;
    Node *current;
    int count;

    int currentOutOfBounds;

    List *previousFreeList;
    List *nextFreeList;
};

List *ListCreate();
int ListCount(List *list);
void *ListFirst(List *list);
void *ListLast(List *list);
void *ListNext(List *list);
void *ListPrev(List *list);
void *ListCurr(List *list);
int ListAdd(List *list, void * item);
int ListInsert(List *list, void * item);
int ListAppend(List *list, void * item);
int ListPrepend(List *list, void * item);
void *ListRemove(List *list);
void ListConcat(List *list1, List *list2);
void ListFree(List *list, void (*itemFree)(void *));
void *ListTrim(List *list);
void *ListSearch(List *list, int (*comparator)(void *, void *), void *comparisonArg);

#endif
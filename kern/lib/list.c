#include <list.h>
#include <types.h>
#include <kern/errno.h>
#include <lib.h>

struct listnode {
    void* val;
    struct listnode* next;
};

struct list {
    struct listnode* head;
    struct listnode* tail;
    unsigned int size;
};

/* Allocates and returns a list node object containing given element */
struct listnode* listnode_create(void* newval);

struct listnode*
listnode_create(void* newval)
{
    struct listnode* newnode;
    
    newnode = (struct listnode*)kmalloc(sizeof(struct listnode));
    if (newnode == NULL) {
        return NULL;
    }
    newnode->val = newval;
    newnode->next = NULL;

    return newnode;
}

struct list*
list_create(void)
{
    struct list* newlist;

    newlist = (struct list*)kmalloc(sizeof(struct list));
    if (newlist == NULL) {
        return NULL;
    }

    newlist->head = NULL;
    newlist->tail = NULL;
    newlist->size = 0;

    return newlist;
}

int
list_push_back(struct list* lst, void* newval)
{
    KASSERT(lst != NULL);

    struct listnode* newnode = listnode_create(newval);
    if (newnode == NULL) {
        return ENOMEM;
    }

    if (lst->size == 0) {
        lst->head = newnode;
    } else {
        lst->tail->next = newnode;
    }
    lst->tail = newnode;

    ++lst->size;
    
    return 0;
}

void
list_pop_front(struct list* lst)
{
    KASSERT(lst != NULL);

    if (lst->size == 0) {
        return;
    }
    
    struct listnode* old_head = lst->head;
    lst->head = lst->head->next;
    --lst->size;

    if (lst->size == 0) {
        lst->tail = NULL;
    }

    kfree(old_head);
}

void*
list_front(struct list* lst)
{
    KASSERT(lst != NULL);

    if (lst->size == 0) {
        return NULL;
    }

    return lst->head->val;
}

void*
list_find(struct list* lst, void* query_val, int(*comparator)(void* left, void* right))
{
    KASSERT(lst != NULL);

    struct listnode* p = lst->head;
    while (p) {
        if (comparator(p->val, query_val) == 0) {
            return p->val;
        }
        p = p->next;
    }
    return NULL;
}

void*
list_remove(struct list* lst, void* query_val, int(*comparator)(void* left, void* right))
{
    KASSERT(lst != NULL);
    void* res = NULL;
    struct listnode* p;
    struct listnode* q = NULL;
    for (p = lst->head; p != NULL; q = p, p = p->next) {
        if (comparator(p->val, query_val) == 0) {
            if (q == NULL) {
                /* Removing from head */
                lst->head = p->next;
            } else {
                /* Removing after head. */
                q->next = p->next;
            }
            res = p->val;
            kfree(p);
            --lst->size;
            break;
        }
    }
    return res;
}

int
list_isempty(struct list* lst)
{
    KASSERT(lst != NULL);

    return (lst->size == 0);
}

unsigned int
list_getsize(struct list* lst)
{
    KASSERT(lst != NULL);

    return (lst->size);
}

void
list_destroy(struct list* lst)
{
    if (lst != NULL) {
        struct listnode* p = lst->head;
        struct listnode* q;
        /* frees every listnode object in the list */
        while (p != NULL) {
            q = p->next;
            kfree(p);
            p = q;
        }
    }
    /* frees the list object itself */
    kfree(lst);
}


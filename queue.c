#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    /* Got a null pointer */
    if (!q)
        return NULL;
    q->head = NULL;
    /* initialize queue tail */
    q->tail = NULL;
    /* initialize queue size */
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    list_ele_t *cur = q->head;
    while (cur) {
        list_ele_t *tmp = cur;
        cur = cur->next;
        free(tmp->value);
        free(tmp);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    /* Got a null pointer */
    if (!q)
        return false;
    /* allocate spcae */
    newh = malloc(sizeof(list_ele_t));
    /* Got a null pointer */
    if (!newh)
        return false;
    newh->value = malloc(sizeof(char) * (strlen(s) + 1));
    /* Got a null pointer */
    if (!newh->value) {
        free(newh);
        return false;
    }
    /* Copy string and check if the string size is too long */
    int buf_len = strlcpy(newh->value, s, strlen(newh->value) + 1);
    if (buf_len >= strlen(newh->value) + 1 || buf_len < 0) {
        free(newh->value);
        free(newh);
        return false;
    }
    // memcpy(newh->value, s, strlen(s) + 1);
    newh->next = q->head;
    q->head = newh;
    /* initialize queue tail when queue size is 0 */
    if (q->size == 0)
        q->tail = q->head;
    /* update queue size */
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newt;
    /* Got a null pointer */
    if (!q)
        return false;
    /* allocate space */
    newt = malloc(sizeof(list_ele_t));
    /* Got a null pointer */
    if (!newt)
        return false;
    newt->value = malloc(sizeof(char) * (strlen(s) + 1));
    /* Got a null pointer */
    if (!newt->value) {
        free(newt);
        return false;
    }
    /* Copy string and check if the string size is too long */
    int buf_len = strlcpy(newt->value, s, strlen(newt->value) + 1);
    if (buf_len >= strlen(newt->value) + 1 || buf_len < 0) {
        free(newt->value);
        free(newt);
        return false;
    }
    newt->next = NULL;
    if (q->size == 0) {
        q->head = newt;
        q->tail = q->head;
    } else {
        q->tail->next = newt;
        q->tail = newt;
    }
    /* update queue size */
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    list_ele_t *tmp;
    /* Check if queue is NULL or empty */
    if (!q || q->size == 0)
        return false;
    /* Copy string */
    if (sp) {
        size_t len = strlen(q->head->value) + 1;
        len = len > bufsize ? bufsize : len;
        strncpy(sp, q->head->value, len);
        sp[len - 1] = '\0';
    }
    tmp = q->head;
    q->head = q->head->next;
    /* update queue size */
    q->size--;
    /* Check tail */
    if (q->size == 0)
        q->tail = NULL;
    /* Free space */
    free(tmp->value);
    free(tmp);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* forget !q  */
    if (!q)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* Check if q is NULL */
    if (q) {
        list_ele_t *cur = q->head;
        list_ele_t *prev = NULL;
        list_ele_t *tmp = NULL;
        /* traversal queue */
        while (cur) {
            tmp = cur;
            cur = cur->next;
            tmp->next = prev;
            prev = tmp;
        }
        /* Swap tail and head */
        tmp = q->head;
        q->head = q->tail;
        q->tail = tmp;
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    /* forget !q->head */
    if (!q || !q->head)
        return;
    merge_sort(&q->head);

    /* more efficiency way to find tail? */
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}

void merge_sort(list_ele_t **head_ref)
{
    list_ele_t *head = *head_ref;
    list_ele_t *a, *b;

    if ((!head) || (!head->next)) {
        return;
    }

    front_back_split(head, &a, &b);

    merge_sort(&a);
    merge_sort(&b);

    *head_ref = sorted_merge(a, b);
}

void move_node(list_ele_t **dst, list_ele_t **src)
{
    list_ele_t *new = *src;
    assert(new != NULL);

    *src = new->next;

    new->next = *dst;

    *dst = new;
}

list_ele_t *sorted_merge(list_ele_t *a, list_ele_t *b)
{
    list_ele_t dummy;
    list_ele_t *tail = &dummy;
    dummy.next = NULL;
    while (1) {
        if (!a) {
            tail->next = b;
            break;
        } else if (!b) {
            tail->next = a;
            break;
        }
        if (strncmp(a->value, b->value, strlen(b->value)) < 0) {
            move_node(&(tail->next), &a);
        } else {
            move_node(&(tail->next), &b);
        }
        tail = tail->next;
    }
    return dummy.next;
}

void front_back_split(list_ele_t *src,
                      list_ele_t **front_ref,
                      list_ele_t **back_ref)
{
    list_ele_t *fast, *slow;
    slow = src;
    fast = src->next;

    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front_ref = src;
    *back_ref = slow->next;
    slow->next = NULL;
}

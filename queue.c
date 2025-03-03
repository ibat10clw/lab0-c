#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

static inline int min(int a, int b)
{
    return b ^ ((a ^ b) & -(a < b));
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = (struct list_head *) malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *it, *tmp = NULL;
    list_for_each_entry_safe (it, tmp, head, list) {
        free(it->value);
        free(it);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node)
        return false;

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node)
        return false;

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *lh = head->next;
    element_t *ele = list_entry(lh, element_t, list);
    list_del_init(lh);
    if (sp) {
        size_t sz = min(strlen(ele->value), bufsize - 1);
        memcpy(sp, ele->value, sz);
        sp[sz] = '\0';
    }
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *lt = head->prev;
    element_t *ele = list_entry(lt, element_t, list);
    list_del_init(lt);
    if (sp) {
        size_t sz = min(strlen(ele->value), bufsize - 1);
        memcpy(sp, ele->value, sz);
        sp[sz] = '\0';
    }
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return -1;
    int sz = 0;
    struct list_head *it;
    list_for_each (it, head)
        sz++;
    return sz;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}

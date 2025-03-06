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
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next, *fast = head->next->next;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    element_t *tmp = list_entry(slow, element_t, list);
    list_del(slow);
    free(tmp->value);
    free(tmp);
    return true;
}

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_is_singular(head) || list_empty(head))
        return false;
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    struct list_head *cur = head->next;
    while (cur != head && cur->next != head) {
        element_t *e1 = list_entry(cur, element_t, list);
        element_t *e2 = list_next_entry(e1, list);
        bool flag = 0;
        while (cur->next != head && !strcmp(e1->value, e2->value)) {
            list_del(cur->next);
            free(e2->value);
            free(e2);
            flag = 1;
            e2 = list_next_entry(e1, list);
        }
        struct list_head *tmp = cur->next;
        if (flag) {
            list_del(cur);
            free(e1->value);
            free(e1);
        }
        cur = tmp;
    }
    return true;
}
/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_head *old, struct list_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}
/**
 * list_swap - replace entry1 with entry2 and re-add entry1 at entry2's position
 * @entry1: the location to place entry2
 * @entry2: the location to place entry1
 */
static inline void list_swap(struct list_head *entry1, struct list_head *entry2)
{
    struct list_head *pos = entry2->prev;

    list_del(entry2);
    list_replace(entry1, entry2);
    if (__glibc_unlikely(pos == entry1))
        pos = entry2;
    list_add(entry1, pos);
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *cur = head->next;
    while (cur != head && cur->next != head) {
        struct list_head *tmp = cur->next->next;
        list_swap(cur->next, cur);
        cur = tmp;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *cur = head->next;
    struct list_head *tmp;
    while (cur != head) {
        tmp = cur->next;
        cur->next = cur->prev;
        cur->prev = tmp;
        cur = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}
void printlist(struct list_head *head)
{
    element_t *e;
    list_for_each_entry (e, head, list) {
        printf("val: %s\n", e->value);
    }
}
/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (k <= 1 || !head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *cur, *tmp;
    struct list_head *sub_head = head;
    unsigned int count = 0;

    list_for_each_safe (cur, tmp, head) {
        count++;
        if (count == k) {
            struct list_head sublist;
            INIT_LIST_HEAD(&sublist);
            // cur range: [sub_head->next, ..., cur]
            list_cut_position(&sublist, sub_head, cur);

            q_reverse(&sublist);
            /*
             * list_splice() expects the 'head' parameter to be a sentinel node
             * with no data. This places inserted elements between 'head' and
             * the first real node. If a data-bearing node is passed instead,
             * its 'prev' must be used to ensure the insertion happens before
             * that node.
             */
            list_splice(&sublist, tmp->prev);
            count = 0;
            sub_head = tmp->prev;
        }
    }
}
static inline bool str_cmp_asc(const char *a, const char *b)
{
    return strcmp(a, b) <= 0;
}
static inline bool str_cmp_dsc(const char *a, const char *b)
{
    return strcmp(a, b) >= 0;
}

struct list_head *merge(struct list_head *l1,
                        struct list_head *l2,
                        bool (*cmp)(const char *a, const char *b))
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; l1 && l2; *node = (*node)->next) {
        node = cmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value)
                   ? &l1
                   : &l2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = l1 ? l1 : l2;
    return head;
}

struct list_head *mergesort(struct list_head *head,
                            bool (*cmp)(const char *a, const char *b))
{
    if (!head->next) {
        return head;
    }
    struct list_head *fast = head;
    struct list_head *slow = head;
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    struct list_head *mid = slow->next;
    slow->next = NULL;
    struct list_head *left = mergesort(head, cmp);
    struct list_head *right = mergesort(mid, cmp);
    return merge(left, right, cmp);
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    /*
     * First unlink list's head and destory circular structure
     * for detect NULL in merge stage
     */
    struct list_head *tmp = head->next;
    bool (*cmp)(const char *a, const char *b) =
        descend ? str_cmp_dsc : str_cmp_asc;
    head->prev->next = NULL;
    head->next = NULL;

    // Sort the list and ignore prev link in this stage
    struct list_head *sorted = mergesort(tmp, cmp);
    // Recover previous link and stop at last node
    struct list_head *current = sorted;
    while (current->next) {
        current->next->prev = current;
        current = current->next;
    }

    // Recover circular structure
    sorted->prev = head;
    head->next = sorted;
    head->prev = current;
    current->next = head;
}

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal
 * of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; n != (head); pos = n, n = pos->prev)

#define list_for_each_entry_prev_safe(entry, safe, head, member)       \
    for (entry = list_entry((head)->prev, typeof(*entry), member),     \
        safe = list_entry(entry->member.prev, typeof(*entry), member); \
         &entry->member != (head); entry = safe,                       \
        safe = list_entry(safe->member.prev, typeof(*entry), member))
/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *cur, *tmp;
    const element_t *min_node = list_last_entry(head, element_t, list);
    list_for_each_entry_prev_safe(cur, tmp, head, list)
    {
        if (strcmp(cur->value, min_node->value) > 0) {
            list_del(&cur->list);
            free(cur->value);
            free(cur);
        } else {
            min_node = cur;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *cur, *tmp;
    const element_t *max_node = list_last_entry(head, element_t, list);
    list_for_each_entry_prev_safe(cur, tmp, head, list)
    {
        if (strcmp(cur->value, max_node->value) < 0) {
            list_del(&cur->list);
            free(cur->value);
            free(cur);
        } else {
            max_node = cur;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    // This function's head is refer to queue_chain_t *chain in qtest.c
    // look the function call q_merge in do_merge
    queue_contex_t *cur;
    bool (*cmp)(const char *a, const char *b) =
        descend ? str_cmp_dsc : str_cmp_asc;
    struct list_head *lh = NULL;
    list_for_each_entry (cur, head, chain) {
        if (!lh) {
            lh = cur->q;
            lh->prev->next = NULL;
        } else {
            cur->q->prev->next = NULL;
            lh->next = merge(lh->next, cur->q->next, cmp);
            // In do_merge (in q_test.c), after q_merge completes, the program
            // frees every queue except for the first one in q_chain_t.
            // Therefore, after merging, we reinitialize the queue head to
            // prevent the element_t nodes from being accidentally freed.
            INIT_LIST_HEAD(cur->q);
        }
    }
    struct list_head *tmp = lh->next, *prev = lh;
    while (tmp) {
        tmp->prev = prev;
        prev = tmp;
        tmp = tmp->next;
    }
    lh->prev = prev;
    prev->next = lh;
    return q_size(lh);
}

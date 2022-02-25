#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    // allocate a space of list_head
    struct list_head *q = (struct list_head *) malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    // if the queue is null do nothing
    if (!l)
        return;
    // free each node which in list
    element_t *previous, *current;
    list_for_each_entry_safe (current, previous, l, list) {
        free(current->value);
        free(current);
    }
    // free list_head
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    // if the queue is NULL, return false
    if (!head)
        return false;
    // allocate space for new node, if fail return false
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node)
        return false;
    // allocate space for copy string, if fail free the node and return false
    unsigned int len = strlen(s) + 1;
    node->value = (char *) malloc(sizeof(char) * len);
    if (!node->value) {
        free(node);
        return false;
    }
    memcpy(node->value, s, len);
    list_add(&node->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node)
        return false;
    unsigned int len = strlen(s) + 1;
    node->value = (char *) malloc(sizeof(char) * len);
    if (!node->value) {
        free(node);
        return false;
    }
    memcpy(node->value, s, len);
    list_add_tail(&node->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // if queue is NULL or empty return NULL
    if (!head || head->next == head)
        return NULL;
    element_t *node = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);
    return node;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head)
        return NULL;
    element_t *node = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return node;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head || head == head->next)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || head->next == head)
        return false;
    // using fastptr and slowptr to find middle node in list
    struct list_head *fast = head;
    struct list_head *slow = head;
    do {
        fast = fast->next->next;
        slow = slow->next;
    } while (fast->next != head && fast != head);
    // get the target address
    element_t *target = list_entry(slow, element_t, list);
    // remove from list
    list_del(slow);
    // delete node
    free(target->value);
    free(target);
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    // three pointers for help change link
    struct list_head *current = head->next, *previous = head, *tmp = NULL;
    while (current != head && current->next != head) {
        tmp = previous;
        previous = current;
        current = current->next;
        // change link
        previous->next = current->next;
        current->next = previous;
        current->prev = previous->prev;
        previous->prev = current;
        tmp->next = current;
        // move current to next
        current = previous->next;
    }
    // when # nodes is even, change last node's link
    if (current->next != head) {
        previous->next = head;
        head->prev = previous;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || head == head->next || list_is_singular(head))
        return;
    struct list_head *tmp;
    struct list_head *current = head->next;
    /*
     * tmp means previous node depand current
     * then change node's prev and next
     * since prev and next be changed so use prev to move current
     */
    while (current != head) {
        tmp = current->prev;
        current->prev = current->next;
        current->next = tmp;
        current = current->prev;
    }
    // swap head's prev and next
    tmp = head->prev;
    head->prev = head->next;
    head->next = tmp;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head) {}

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define XORLIST_SZ 1000
#define SORT_TIME_OUTPUT_FILE "sort_time_optimized_s.txt"
#define OPTIMIZED_S 100

int opt_s = 0;

typedef struct __list list;
struct __list {
    int data;
    struct __list *addr;
};

#define XOR(a, b) ((list *) ((uintptr_t) (a) ^ (uintptr_t) (b)))

static long timespec_diff(struct timespec *a, struct timespec *b) {
  return ((b->tv_sec - a->tv_sec) * 1000000000) + (b->tv_nsec - a->tv_nsec);
}

void insert_node(list **l, int d) {
    list *tmp = malloc(sizeof(list));
    tmp->data = d;

    if (!(*l)) {
      tmp->addr = NULL;
    } else {
      (*l)->addr = XOR(tmp, (*l)->addr);
      tmp->addr = *l;
    }
    *l = tmp;
}

void delete_list(list *l) {
    while (l) {
        list *next = l->addr;
        if (next)
            next->addr = XOR(next->addr, l);
        free(l);
        l = next;
    }
}

void print_list(list *l) {
  list *curr = l;
  list *prev = NULL;
  list *next = NULL;

  printf("XOR list: ");
  while (curr != NULL) {
    printf("[%d]", curr->data);
    next = XOR(prev, curr->addr);
    prev = curr;
    curr = next;
    if (curr)
      printf("<->");
  }
  printf("\n");
}

list *bubble_sort(list *start, int cnt) {
  for (int i = 0; i < (cnt - 1); i++) {
    list *curr = start;
    list *prev = NULL;

    while (curr && XOR(prev, curr->addr)) {
      list *next = XOR(prev, curr->addr);

      /* Swap data */
      if (next->data < curr->data) {
        int tmp = curr->data;
        curr->data = next->data;
        next->data = tmp;
      }
      prev = curr;
      curr = next;
    }
  }
  return start;
}

list *sort(list *start)
{
    if (!start || !start->addr)
        return start;

        /* Original: Divide XOR list one node by one node. */
#if 0
    list *left = start, *right = start->addr;
    left->addr = NULL;
    right->addr = XOR(right->addr, left);
#endif

    /* New: Divide XOR list into two equivalent length list */
    list *l_prev = NULL;
    list *r_prev = NULL;
    list *left = start;
    list *right = XOR(r_prev, start->addr);
    int cnt = 1;
    r_prev = start;

    while (right && XOR(r_prev, right->addr)) {
      list *tmp;

      /* Move forward one step */
      tmp = left;
      left = XOR(l_prev, left->addr);
      l_prev = tmp;

      /* Move forward two stpes */
      tmp = right;
      right = XOR(r_prev, right->addr);
      r_prev = tmp;
      tmp = right;
      right = XOR(r_prev, right->addr);
      r_prev = tmp;

      /* Counting data numbers of sub linked list */
      cnt++;
    }

    /* We could get length of sub linked list after divide. (cnt * 2)
     * Bubble sorting it if its length less or equal than optimaied S. */
    if ((cnt * 2) <= opt_s) {
      return bubble_sort(start, (cnt * 2));
    }

    /* Assign right as right part of list (Next node of current left) */
    right = XOR(l_prev, left->addr);
    /* Cut from left part off */
    right->addr = XOR(left, right->addr);
    /* Cut from riht part off */
    left->addr = XOR(right, left->addr);

    left = sort(start);
    right = sort(right);

    for (list *merge = NULL; left || right;) {
        if (!right || (left && left->data < right->data)) {
            list *next = left->addr;
            if (next)
                next->addr = XOR(left, next->addr);

            if (!merge) {
                start = merge = left;
                merge->addr = NULL;
            } else {
                merge->addr = XOR(merge->addr, left); /* LL1 */
                left->addr = merge; /* LL2 */
                merge = left;
            }
            left = next;
        } else {
            list *next = right->addr;
            if (next)
                next->addr = XOR(right, next->addr);

            if (!merge) {
                start = merge = right;
                merge->addr = NULL;
            } else {
                merge->addr = XOR(merge->addr, right); /* RR1 */
                right->addr = merge; /* RR2 */
                merge = right;
            }
            right = next;
        }
    }

    return start;
}

int main(int argc, char *argv[]) {
  list *xorlist = NULL;
  struct timespec t1, t2;
  FILE *fptr = NULL;

  fptr = fopen(SORT_TIME_OUTPUT_FILE, "w+");

  srand(time(NULL));
  for (int i = 0; i < XORLIST_SZ; i++) {
    /* Generate random data within 0~10000 */
    insert_node(&xorlist, (rand() & 10000));
  }

  for (int i = 0; i <= OPTIMIZED_S; i++) {
    opt_s = i;
    clock_gettime(CLOCK_REALTIME, &t1);
    xorlist = sort(xorlist);
    clock_gettime(CLOCK_REALTIME, &t2);
    fprintf(fptr, "%d %ld\n", i, timespec_diff(&t1, &t2));
  }

  fclose(fptr);

  return 0;
}

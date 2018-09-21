#ifndef __LIST_H__
#define __LIST_H__


typedef struct
{
	char **obj;
	int delta;
	int count;
	int index;
} LLIST;

int ll_init(LLIST *ll);
int ll_destroy(LLIST *ll);

int ll_get_count(LLIST *ll);

int ll_add(LLIST *ll, void *lo);
void *ll_delete_index(LLIST *l, int index);
int ll_delete_obj(LLIST *l, void *lo);
int ll_find(LLIST *l, void *lo);

void *ll_get(LLIST *ll, int index);

#endif // __LIST_H__

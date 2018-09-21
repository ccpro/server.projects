#include <stdlib.h>
#include <memory.h>

#include <llist.h>

static int extend(LLIST *ll);

int ll_init(LLIST *ll)
{
	if (ll == NULL)
		return -1;

	memset((void*)ll, 0, sizeof(LLIST));
	ll->delta = 10;

	return 0;
}

int ll_destroy(LLIST *ll)
{
	int i=0;

	if (ll == NULL)
		return -1;

	free (ll->obj);

	return 0;
}

int ll_get_count(LLIST *ll)
{
	if (ll == NULL)
		return 0;

	return ll->index;
}

void *ll_get(LLIST *ll, int index)
{
	if (ll == NULL)
		return NULL;

	if (index >= ll->count)
		return NULL;

	return (void*)ll->obj[index];
}

int ll_add(LLIST *ll, void *lo)
{
	if (ll == NULL || lo == NULL)
		return -1;

	if (ll->index >= ll->count)
		extend(ll);

	ll->obj[ll->index] = (char*)lo;

	return ll->index++;
}

void *ll_delete_index(LLIST *l, int index)
{
	return NULL;
}

int ll_delete_obj(LLIST *l, void *lo)
{
	return 0;
}

/*
int ll_insert(LLIST *l, LL_OBJ *lo)
{
}
*/
int extend(LLIST *ll)
{
	char *new_obj;

	if (ll == NULL)
		return -1;

	new_obj = malloc(sizeof(char*) * (ll->count + ll->delta));

	if (ll->count > 0)
	{
		memcpy(new_obj, ll->obj, sizeof(char*) * ll->count);
		free (ll->obj);
	}

	ll->obj = (char **)new_obj;
	ll->count += ll->delta;

	return 0;
}

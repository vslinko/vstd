/* Copyright (c) 2014 Vyacheslav Slinko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "list.h"
#include "object_pool.h"

#include <assert.h>
#include <stdlib.h>

struct vstd_object_pool* list_pool;
struct vstd_object_pool* list_item_pool;

static void vstd_list_reset(struct vstd_list* list) {
    list->first = NULL;
    list->last = NULL;
    list->length = 0;
}

static void vstd_list_item_reset(struct vstd_list_item* item) {
    item->value = NULL;
    item->next = NULL;
}

struct vstd_list* vstd_list_alloc() {
    if (!list_pool) {
        list_pool = vstd_object_pool_alloc(
            8,
            sizeof(struct vstd_list),
            (vstd_object_pool_reset_fn*) &vstd_list_reset
        );
    }

    return vstd_object_pool_get(list_pool);
}

static struct vstd_list_item* vstd_list_item_alloc() {
    if (!list_item_pool) {
        list_item_pool = vstd_object_pool_alloc(
            8,
            sizeof(struct vstd_list_item),
            (vstd_object_pool_reset_fn*) &vstd_list_item_reset
        );
    }

    return vstd_object_pool_get(list_pool);
}

void vstd_list_push(struct vstd_list* list, void* value) {
    struct vstd_list_item* item = vstd_list_item_alloc();
    item->value = value;

    if (list->length == 0) {
        list->first = item;
        list->last = item;
    } else {
        list->last->next = item;
        list->last = item;
    }

    list->length++;
}

void* vstd_list_unshift(struct vstd_list* list) {
    if (list->first == NULL) {
        return NULL;
    }

    struct vstd_list_item* item = list->first;
    void* value = item->value;

    if (list->length == 1) {
        list->first = NULL;
        list->last = NULL;
    } else {
        list->first = item->next;
    }
    list->length--;

    vstd_object_pool_return(list_item_pool, item);

    return value;
}

void vstd_list_free(struct vstd_list* list) {
    struct vstd_list_item* item = list->first;
    struct vstd_list_item* next = NULL;
    while (item) {
        next = item->next;
        vstd_object_pool_return(list_item_pool, item);
        item = next;
    }

    vstd_object_pool_return(list_pool, list);
}

void vstd_list_free_object_pool() {
    if (list_pool) {
        vstd_object_pool_free(list_pool);
        list_pool = NULL;
    }

    if (list_item_pool) {
        vstd_object_pool_free(list_item_pool);
        list_item_pool = NULL;
    }
}

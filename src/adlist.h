/* adlist.h - A generic doubly linked list implementation
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ADLIST_H__
#define __ADLIST_H__

/* Node, List, and Iterator are the only data structures used currently. */
/* 以下是双向列表实现：定义接口 */
/* 节点定义 */
typedef struct listNode {
    struct listNode *prev; // 当前节点的前节点
    struct listNode *next; // 当前节点的下节点
    void *value;   // 当前节点的值
} listNode;

/* 迭代器 */
typedef struct listIter {
    listNode *next;  /* 下个节点 */
    int direction;   /* 方向，可以向前、可以向后 */
} listIter;

/* 列表实现：包括头指针，尾指针，接口实现 */
typedef struct list {
    listNode *head;  /* 头指针 */
    listNode *tail;  /* 尾指针 */
    void *(*dup)(void *ptr);  /* 复制 */
    void (*free)(void *ptr);  /* 释放 */
    int (*match)(void *ptr, void *key);  /* 匹配，指针、key */
    unsigned long len;  /* 长度 */
} list;  /* 列表 */

/* Functions implemented as macros */

#define listLength(l) ((l)->len) /* 宏: 列表长度 */
#define listFirst(l) ((l)->head) /* 宏: 头列表节点 */
#define listLast(l) ((l)->tail)  /* 宏: 列表尾节点 */
#define listPrevNode(n) ((n)->prev) /* 宏: 当前节点前一个节点 */
#define listNextNode(n) ((n)->next) /* 宏: 当前节点下一个节点*/
#define listNodeValue(n) ((n)->value) /* 宏: 当前节点值 */

#define listSetDupMethod(l,m) ((l)->dup = (m))  /* 宏: 设置复制函数 */
#define listSetFreeMethod(l,m) ((l)->free = (m)) /* 宏：设置释放函数 */
#define listSetMatchMethod(l,m) ((l)->match = (m)) /* 宏: 设置匹配函数 */

#define listGetDupMethod(l) ((l)->dup)  /* 宏: 获取列表复制函数 */
#define listGetFree(l) ((l)->free) /* 宏 获取列表释放函数: */
#define listGetMatchMethod(l) ((l)->match) /* 宏: 获取列表匹配函数 */

/* Prototypes */
list *listCreate(void);    /* 创建列表 */
void listRelease(list *list); /* 释放列表 */
void listEmpty(list *list); /* 清空列表 */
list *listAddNodeHead(list *list, void *value); /* 在列表头部添加节点 */
list *listAddNodeTail(list *list, void *value); /* 在列表尾部添加节点 */
list *listInsertNode(list *list, listNode *old_node, void *value, int after); /* 插入节点 */
void listDelNode(list *list, listNode *node); /* 删除节点 */
listIter *listGetIterator(list *list, int direction); /* 迭代 */
listNode *listNext(listIter *iter); /* 下个节点 */
void listReleaseIterator(listIter *iter); /* 释放迭代器 */
list *listDup(list *orig); /* 复制 */
listNode *listSearchKey(list *list, void *key); /* 按key搜索节点 */
listNode *listIndex(list *list, long index); /* 按index获取节点  */
void listRewind(list *list, listIter *li); /* 回转列表 */
void listRewindTail(list *list, listIter *li);
void listRotate(list *list);
void listJoin(list *l, list *o); /* 列表合并 */

/* Directions for iterators */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif /* __ADLIST_H__ */

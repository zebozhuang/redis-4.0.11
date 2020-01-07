/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
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

// 事件结构和接口

#ifndef __AE_H__
#define __AE_H__

#include <time.h>

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0       /* No events registered. */
#define AE_READABLE 1   /* Fire when descriptor is readable. */
#define AE_WRITABLE 2   /* Fire when descriptor is writable. */
#define AE_BARRIER 4    /* With WRITABLE, never fire the event if the
                           READABLE event already fired in the same event
                           loop iteration. Useful when you want to persist
                           things to disk before sending replies, and want
                           to do that in a group fashion. */

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4
#define AE_CALL_AFTER_SLEEP 8

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

struct aeEventLoop;

/* Types and data structures */
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

/* 文件事件结构 */
/* File event structure */
typedef struct aeFileEvent {
    int mask; /* one of AE_(READABLE|WRITABLE|BARRIER) */
    aeFileProc *rfileProc;
    aeFileProc *wfileProc;
    void *clientData;
} aeFileEvent;

/* 时间事件结构 */
/* Time event structure */
typedef struct aeTimeEvent {
    long long id; /* time event identifier. */ /* id */
    long when_sec; /* seconds */              /* 秒 */
    long when_ms; /* milliseconds */          /* 毫秒 */
    aeTimeProc *timeProc;
    aeEventFinalizerProc *finalizerProc;
    void *clientData;
    struct aeTimeEvent *prev; /* 前一个时间事件 */
    struct aeTimeEvent *next; /* 下一个时间事件 */
} aeTimeEvent;

/* 事件 */
/* A fired event */
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;

/* State of an event based program */
typedef struct aeEventLoop {
    int maxfd;   /* highest file descriptor currently registered */ /* 当前注册的最大文件描述符 */
    int setsize; /* max number of file descriptors tracked */ /* 最大文件描述符数量 */
    long long timeEventNextId;  // 下个时间事件id
    time_t lastTime;     /* Used to detect system clock skew */
    aeFileEvent *events; /* Registered events */ /* */
    aeFiredEvent *fired; /* Fired events */
    aeTimeEvent *timeEventHead;
    int stop;
    void *apidata; /* This is used for polling API specific data */
    aeBeforeSleepProc *beforesleep;
    aeBeforeSleepProc *aftersleep;
} aeEventLoop;

/* Prototypes */
// 接口原型
aeEventLoop *aeCreateEventLoop(int setsize);  // 创建事件循环
void aeDeleteEventLoop(aeEventLoop *eventLoop);  // 删除事件循环
void aeStop(aeEventLoop *eventLoop); // 事件停止
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);  // 创建文件事件
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);  // 删除文件事件
int aeGetFileEvents(aeEventLoop *eventLoop, int fd); // 获取文件事件
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc); // 创建事件事件
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id); // 删除事件事件
int aeProcessEvents(aeEventLoop *eventLoop, int flags);  // 处理事件
int aeWait(int fd, int mask, long long milliseconds);  // 等待
void aeMain(aeEventLoop *eventLoop);  // 事件入口
char *aeGetApiName(void);  // api?
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);  // 设置处理函数
void aeSetAfterSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *aftersleep);   // 设置处理函数
int aeGetSetSize(aeEventLoop *eventLoop);  // 获取集合大小
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);  // 重置集合大小

#endif

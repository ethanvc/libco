/*
* Tencent is pleased to support the open source community by making Libco available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, 
* software distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License.
*/


#ifndef __CO_ROUTINE_INNER_H__

#include "co_routine.h"
#include "coctx.h"
struct stCoRoutineEnv_t;
struct stCoSpec_t
{
	void *value;
};

struct stStackMem_t
{
	stCoRoutine_t* occupy_co; // 当前在该栈上跑的协程
	int stack_size; // stack_bp所指向的栈空间的大小
	char* stack_bp; //stack_buffer + stack_size 栈底
	char* stack_buffer; // 栈顶

};

struct stShareStack_t
{
	unsigned int alloc_idx;
	int stack_size;
	int count;
	stStackMem_t** stack_array;
};



struct stCoRoutine_t
{
	stCoRoutineEnv_t *env;
	pfn_co_routine_t pfn;
	void *arg;
	coctx_t ctx;

	char cStart; // 协程是否已经开始执行
	char cEnd;	// 协程是否已经结束
	char cIsMain;	// 是否是主协程，主协程比较特殊，它没有执行函数
	char cEnableSysHook; // 当前协程执行时，是否hook系统函数
	char cIsShareStack; // 是否使用共享栈

	void *pvEnv;

	//char sRunStack[ 1024 * 128 ];
	stStackMem_t* stack_mem; // 协程运行栈的地址、大小


	//save satck buffer while confilct on same stack_buffer;
	char* stack_sp; // 需要保存的栈的起始地址，栈空间从高地址往低地址走，因此这个值是最后一个函数内的一个局部变量的地址，参考co_swap的实现
	unsigned int save_size; // save_buffer的长度
	char* save_buffer; // 保存协程栈内容的缓冲区

	stCoSpec_t aSpec[1024]; // 支持协程私有变量

};



//1.env 创建线程执行环境
void 				co_init_curr_thread_env();
// 获取当前线程的协程执行环境，由于线程ID的最大值可控，因此采用分配大数组的形式实现类似线程局部变量的功能，能够大大提高效率
stCoRoutineEnv_t *	co_get_curr_thread_env();

//2.coroutine
// 释放协程所占用内存
void    co_free( stCoRoutine_t * co );
// 让出执行流，给当前环境下的其它协程执行
void    co_yield_env(  stCoRoutineEnv_t *env );

//3.func



//-----------------------------------------------------------------------------------------------

struct stTimeout_t;
struct stTimeoutItem_t ;

stTimeout_t *AllocTimeout( int iSize );
void 	FreeTimeout( stTimeout_t *apTimeout );
// 添加一个定时器
int  	AddTimeout( stTimeout_t *apTimeout,stTimeoutItem_t *apItem ,uint64_t allNow );

struct stCoEpoll_t;
stCoEpoll_t * AllocEpoll();
void 		FreeEpoll( stCoEpoll_t *ctx );

stCoRoutine_t *		GetCurrThreadCo();
void 				SetEpoll( stCoRoutineEnv_t *env,stCoEpoll_t *ev );

typedef void (*pfnCoRoutineFunc_t)();

#endif

#define __CO_ROUTINE_INNER_H__

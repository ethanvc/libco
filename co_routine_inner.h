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
	stCoRoutine_t* occupy_co; // ��ǰ�ڸ�ջ���ܵ�Э��
	int stack_size; // stack_bp��ָ���ջ�ռ�Ĵ�С
	char* stack_bp; //stack_buffer + stack_size ջ��
	char* stack_buffer; // ջ��

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

	char cStart; // Э���Ƿ��Ѿ���ʼִ��
	char cEnd;	// Э���Ƿ��Ѿ�����
	char cIsMain;	// �Ƿ�����Э�̣���Э�̱Ƚ����⣬��û��ִ�к���
	char cEnableSysHook; // ��ǰЭ��ִ��ʱ���Ƿ�hookϵͳ����
	char cIsShareStack; // �Ƿ�ʹ�ù���ջ

	void *pvEnv;

	//char sRunStack[ 1024 * 128 ];
	stStackMem_t* stack_mem; // Э������ջ�ĵ�ַ����С


	//save satck buffer while confilct on same stack_buffer;
	char* stack_sp; // ��Ҫ�����ջ����ʼ��ַ��ջ�ռ�Ӹߵ�ַ���͵�ַ�ߣ�������ֵ�����һ�������ڵ�һ���ֲ������ĵ�ַ���ο�co_swap��ʵ��
	unsigned int save_size; // save_buffer�ĳ���
	char* save_buffer; // ����Э��ջ���ݵĻ�����

	stCoSpec_t aSpec[1024]; // ֧��Э��˽�б���

};



//1.env �����߳�ִ�л���
void 				co_init_curr_thread_env();
// ��ȡ��ǰ�̵߳�Э��ִ�л����������߳�ID�����ֵ�ɿأ���˲��÷�����������ʽʵ�������ֲ߳̾������Ĺ��ܣ��ܹ�������Ч��
stCoRoutineEnv_t *	co_get_curr_thread_env();

//2.coroutine
// �ͷ�Э����ռ���ڴ�
void    co_free( stCoRoutine_t * co );
// �ó�ִ����������ǰ�����µ�����Э��ִ��
void    co_yield_env(  stCoRoutineEnv_t *env );

//3.func



//-----------------------------------------------------------------------------------------------

struct stTimeout_t;
struct stTimeoutItem_t ;

stTimeout_t *AllocTimeout( int iSize );
void 	FreeTimeout( stTimeout_t *apTimeout );
// ���һ����ʱ��
int  	AddTimeout( stTimeout_t *apTimeout,stTimeoutItem_t *apItem ,uint64_t allNow );

struct stCoEpoll_t;
stCoEpoll_t * AllocEpoll();
void 		FreeEpoll( stCoEpoll_t *ctx );

stCoRoutine_t *		GetCurrThreadCo();
void 				SetEpoll( stCoRoutineEnv_t *env,stCoEpoll_t *ev );

typedef void (*pfnCoRoutineFunc_t)();

#endif

#define __CO_ROUTINE_INNER_H__

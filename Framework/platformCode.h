#ifndef PLATFORMCODE_H
#define PLATFORMCODE_H
/*
 * Program Name: CnC Common Headers
 * File Name: platformCode.h
 * Date Created: January 21, 2024
 * Date Updated: November 9, 2024
 * Version: 0.4
 * Purpose: This file contains all functions that interact with platform-specific functionality
 */
#ifdef __MINGW32__
// MINGW (at least on debian) expects _aligned_malloc, while gcc and clang expect aligned_alloc
#define aligned_alloc(sz, aln) _aligned_malloc(sz, aln)
#include <windows.h>
#include <pthread.h>

#elif __unix__
#define _GNU_SOURCE // This is required for `CPU_ZERO`, and `CPU_SET`
#include <pthread.h>
#include <unistd.h>

#endif

int getThreadCount();
int getAffinity(pthread_t thread);
int setAffinity(pthread_t thread, int proc);
int createThread(pthread_t *handle, void *(*routine)(void *), void *argument);
int joinThread(pthread_t handle, void **retval);


#endif // PLATFORMCODE_H

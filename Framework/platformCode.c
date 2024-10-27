#include "platformCode.h"
/*
 * Program Name: CnC Common Headers
 * File Name: platformCode.c
 * Date Created: October 27, 2024
 * Date Updated: October 27, 2024
 * Version: 0.3
 * Purpose: This file contains all functions that interact with platform-specific functionality
 */

//SEE HEADER FILE FOR FUNCTION DOCUMENTATION

#ifdef __MINGW32__

int getThreadCount()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

int getAffinity(pthread_t thread)
{

}

int setAffinity(pthread_t thread, int proc)
{
    HANDLE native_handle = pthread_gethandle(thread);
    // This isn't guaranteed to work on systems with more than 64 processors
    // To fix this we need to look into process groups
    DWORD mask = 1 << proc;

    return !SetProcessAffinityMask(
        native_handle,
        (DWORD_PTR)&mask
        );
}
#elif __unix__

int getThreadCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

int getAffinity(pthread_t thread)
{

}

int setAffinity(pthread_t thread, int proc)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(proc, &cpuset);

    return pthread_setaffinity_np(
        thread,
        sizeof(cpu_set_t),
        &cpuset
        );
}
#endif

int createThread(pthread_t *handle, void *(*routine)(void *), void *argument)
{
    return pthread_create(handle, NULL, routine, argument);
}

int joinThread(pthread_t handle, void **retval) {
    return pthread_join(handle, retval);
}

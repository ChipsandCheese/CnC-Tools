/*
 * Program Name: CoherencyLatencyTest
 * File Name: main.c
 * Date Created: November 11, 2024
 * Date Updated: November 11, 2024
 * Version: 0.1
 * Purpose: Test Core-to-Core Latency of Multi-Core CPU's using Coherency checks.
 */

#include <platformCode.h>
#include <storage.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ITERATIONS 10000000;

typedef struct LatencyThreadData {
    uint64_t start;
    uint64_t iters;
    volatile uint64_t *target;
    uint32_t processorIndex;
} LatencyThreadData;

typedef struct LatencyPairRunData {
    uint32_t proc1;
    uint32_t proc2;
    uint64_t iter;
    double result;
    uint64_t *target;
} LatencyPairRunData;

/*
 * Tests the latency using a non-locking algorithm.
 * @Param param: Pointer to the LatencyThreadData structure to operate on.
 * @return: Will always return NULL.
 */
void *NoLockLatencyTestThread(void *param) {
    LatencyThreadData *latencyData = (LatencyThreadData *)param;
    uint64_t current = latencyData->start;

    setAffinity(pthread_self(), latencyData->processorIndex);

    while (current <= 2 * latencyData->iters) {
        if (*(latencyData->target) == current - 1) {
            *(latencyData->target) = current;
            current += 2;
        } 
    }

    return NULL;
} 

/*
 * Run all the tests and gather latency data.
 * @Param proc1: Processor index for the first processor in the test.
 * @Param proc2: Processor index for the second processor in the test.
 * @Param iter: Number of times to iterate over the tests, higher is more accurate.
 * @Param lat1: Pointer to LatencyThreadData for the first processor.
 * @Param lat2: Pointer to LatencyThreadData for the second processor.
 * @Param threadfunc: Function pointer to test across both processors.
 * @return: Latency between both processors.
 */
double TimeThreads(uint32_t proc1,
                  uint32_t proc2,
                  uint64_t iter,
                  LatencyThreadData *lat1,
                  LatencyThreadData *lat2,
                  void *(*threadFunc)(void *)) {
    struct timeval startTv, endTv;
    struct timezone startTz, endTz;
    pthread_t testThreads[2];
    int t1rc, t2rc;
    void *res1, *res2;

    gettimeofday(&startTv, &startTz);
    t1rc = pthread_create(&testThreads[0], NULL, threadFunc, (void *)lat1);
    t2rc = pthread_create(&testThreads[1], NULL, threadFunc, (void *)lat2);
    if (t1rc != 0 || t2rc != 0) {
      fprintf(stderr, "Could not create threads\n");
      return 0;
    }

    pthread_join(testThreads[0], &res1);
    pthread_join(testThreads[1], &res2);
    gettimeofday(&endTv, &endTz);

    uint64_t time_diff_ms = 1000 * (endTv.tv_sec - startTv.tv_sec) + ((endTv.tv_usec - startTv.tv_usec) / 1000);
    double latency = 1e6 * (double)time_diff_ms / (double)iter;
    return latency;
}


/*
 * Test the latency across two logical processors using the TimeThreads function.
 * @Param param: Pointer to the LatencyThreadData structure to clone for both.
 * @return: Will always return NULL directly, but returns a double in the provided LatencyThreadData.
 */
void *RunTest(void *param) {
  LatencyPairRunData *pairRunData = (LatencyPairRunData *)param;
  uint32_t processor1 = pairRunData->proc1;
  uint32_t processor2 = pairRunData->proc2;
  uint64_t iter = pairRunData->iter;
  LatencyThreadData lat1, lat2;
  double latency;

  *(pairRunData->target) = 0;
  lat1.iters = iter;
  lat1.start = 1;
  lat1.target = pairRunData->target;
  lat1.processorIndex = processor1;
  lat2.iters = iter;
  lat2.start = 2;
  lat2.target = pairRunData->target;
  lat2.processorIndex = processor2;
  latency = TimeThreads(processor1, processor2, iter, &lat1, &lat2, NoLockLatencyTestThread);
  fprintf(stderr, "%d to %d: %f ns\n", processor1, processor2, latency);
  pairRunData->result = latency;
  return NULL;
}


/*
 * Tests the latency using a locking algorithm.
 * @Param param: Pointer to the LatencyThreadData structure to operate on.
 * @return: Will always return NULL.
 */
void *LatencyTestThread(void *param) {
    LatencyThreadData *latencyData = (LatencyThreadData *)param;
    uint64_t current = latencyData->start;

    setAffinity(pthread_self(), latencyData->processorIndex);
    //fprintf(stderr, "thread %ld set affinity %d\n", gettid(), latencyData->processorIndex);

    while (current <= 2 * latencyData->iters) {
        if (__sync_bool_compare_and_swap(latencyData->target, current - 1, current)) current += 2;
    }

    return NULL;
}

void *(*testFunc)(void *) = LatencyTestThread;

/*
 * Runs latency tests across all present processors, and then outputs the results.
 * @Param iterations: Number of iterations to use in the latency tests, higher is more accurate.
 * @Param nolock: Tells the benchmark to use the non-locking test algorithm.
 * @Param offsets: TODO.
 * @Param parallel: How many processors to test in parallel.
 * @Param outfile: File path for output data.
 * @return: Status code, zero is successful.
 */
int main(int argc, char *argv[]) {
    double **latencies;
    int *parallelTestState;
    int numProcs, offsets = 1, parallelismFactor = 1;
    char *outFilePath;
    uint64_t iter = ITERATIONS;
    uint64_t *bouncyArr;

    numProcs = getThreadCount();
    fprintf(stderr, "Number of CPUs: %u\n", numProcs);

    for (int argIdx = 1; argIdx < argc; argIdx++) {
        if (*(argv[argIdx]) == '-') {
            char* arg = argv[argIdx] + 1;
            if (strncmp(arg, "iterations", 10) == 0) {
                argIdx++;
                iter = atoi(argv[argIdx]);
                fprintf(stderr, "%lu iterations requested\n", iter);
            }
            else if (strncmp(arg, "nolock", 6) == 0) {
                fprintf(stderr, "No locks, plain loads and stores\n");
                testFunc = NoLockLatencyTestThread;
            }
            else if (strncmp(arg, "offset", 6) == 0) {
                argIdx++;
                offsets = atoi(argv[argIdx]);
                fprintf(stderr, "Offsets: %d\n", offsets);
            }
            else if (strncmp(arg, "parallel", 8) == 0) {
                argIdx++;
                parallelismFactor = atoi(argv[argIdx]);
                fprintf(stderr, "Will go for %d runs in parallel\n", parallelismFactor);
            }
            else if (strncmp(arg, "outfile", 7) == 0) {
                argIdx++;
                outFilePath = argv[argIdx];
                fprintf(stderr, "Outputting data to %s\n", outFilePath);
            }
        }
    }

    latencies = (double **)malloc(sizeof(double *) * offsets);
    parallelTestState = (int *)malloc(sizeof(int) * numProcs * numProcs);
    memset(latencies, 0, sizeof(double) * offsets);
    bouncyArr = aligned_alloc(4096 * parallelismFactor, 4096);
    if (bouncyArr == NULL) {
        fprintf(stderr, "Could not allocate aligned mem\n");
        return 0;
    } 

    LatencyPairRunData *pairRunData = (LatencyPairRunData *)malloc(sizeof(LatencyPairRunData) * parallelismFactor);

    for (int offsetIdx = 0; offsetIdx < offsets; offsetIdx++) {
        latencies[offsetIdx] = (double *)malloc(sizeof(double) * numProcs * numProcs);
        memset(parallelTestState, 0, sizeof(int) * numProcs * numProcs);
        double *latenciesPtr = latencies[offsetIdx];

        while (1) {
            // select parallelismFactor threads
            int selectedParallelTestCount = 0;
            memset(pairRunData, 0, sizeof(LatencyPairRunData) * parallelismFactor);
            for (int i = 0;i < numProcs && selectedParallelTestCount < parallelismFactor; i++) {
                for (int j = 0;j < numProcs && selectedParallelTestCount < parallelismFactor; j++) {
                    if (j == i) { latenciesPtr[j + i * numProcs] = 0; continue; }
                    if (parallelTestState[j + i * numProcs] == 1) {
                        fprintf(stderr, "Thread unexpectedly did not complete\n");
                        exit(0);
                    }
                    if (parallelTestState[j + i * numProcs] == 0) {
                        // neither thread can already have a pending run
                        int validPair = 1;
                        for (int c = 0; c < numProcs; c++) {
                            if (parallelTestState[j + c * numProcs] == 1 || 
                                parallelTestState[c + i * numProcs] == 1 ||
                                parallelTestState[i + c * numProcs] == 1 ||
                                parallelTestState[c + j * numProcs] == 1) {
                                validPair = 0;
                                break;
                            }
                        }

                        if (!validPair) continue;

                        // for SMT enabled CPUs, check sibling threads. will do later
                        parallelTestState[j + i * numProcs] = 1;
                        pairRunData[selectedParallelTestCount].proc1 = i;
                        pairRunData[selectedParallelTestCount].proc2 = j;
                        pairRunData[selectedParallelTestCount].iter = iter;
                        pairRunData[selectedParallelTestCount].result = 0.0f;
                        pairRunData[selectedParallelTestCount].target = bouncyArr + (512 * selectedParallelTestCount + 8 * offsetIdx);
                        fprintf(stderr, "Selected %d -> %d\n", i, j);
                        selectedParallelTestCount++;
                    }
                }
            }
            
            if (selectedParallelTestCount == 0) break;

            // Launch threads to test with
            fprintf(stderr, "Selected %d pairs for parallel testing\n", selectedParallelTestCount);
            pthread_t *testThreads = (pthread_t *)malloc(selectedParallelTestCount * sizeof(pthread_t));
            memset(testThreads, 0, selectedParallelTestCount * sizeof(pthread_t));
            for (int parallelIdx = 0; parallelIdx < selectedParallelTestCount; parallelIdx++) {
                if (pairRunData[parallelIdx].proc1 == 0 && pairRunData[parallelIdx].proc2 == 0) break;
                pthread_create(testThreads + parallelIdx, NULL, RunTest, (void *)(pairRunData + parallelIdx));
            }

            // Wait for threads to end
            for (int parallelIdx = 0; parallelIdx < selectedParallelTestCount; parallelIdx++) {
                pthread_join(testThreads[parallelIdx], NULL);
                int i = pairRunData[parallelIdx].proc1;
                int j = pairRunData[parallelIdx].proc2;
                latenciesPtr[j + i * numProcs] = pairRunData[parallelIdx].result;
                parallelTestState[j + i * numProcs] = 2;
            }

            free(testThreads);
        }
    }

    // Allocate a place for all the column names to be placed, then fill it with names
    char (*names)[256] = malloc(numProcs * (256 * sizeof(char)));
    for (int i = 0; i < numProcs; i++)
        snprintf(&names[i][0], 256, "Proc%u\0", i);

    if (write_CNC(
        outFilePath,
        latencies[0],
        numProcs * numProcs,
        numProcs,
        names
    ) != 0)
        return -1;

    // Print out data to the terminal
    for (int offsetIdx = 0; offsetIdx < offsets; offsetIdx++) {
        double *latenciesPtr = latencies[offsetIdx];
        printf("Cache line offset: %d\n", offsetIdx);
        
        // Iterate over all possible processor combinations
        for (int i = 0;i < numProcs; i++) {
            for (int j = 0;j < numProcs; j++) {
                // Print out a delimiter before we print the data, unless this is the first entry
                if (j != 0) printf(",");

                // If j and i are the same processors the just output an x.
                if (j == i) printf("x");
                // Print out the latency between processors j and i
                // to maintain consistency, divide by 2 (see justification in windows version)
                else printf("%f", latenciesPtr[j + i * numProcs] / 2);
            }
            printf("\n");
        }

        free(latenciesPtr);
    }

    free(parallelTestState);
    free(pairRunData);
    free(latencies);
    free(bouncyArr);
    return 0;
}

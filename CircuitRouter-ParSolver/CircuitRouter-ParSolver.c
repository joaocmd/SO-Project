/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This code is an adaptation of the Lee algorithm's implementation originally included in the STAMP Benchmark
 * by Stanford University.
 *
 * The original copyright notice is included below.
 *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) Stanford University, 2006.  All Rights Reserved.
 * Author: Chi Cao Minh
 *
 * =============================================================================
 *
 * Unless otherwise noted, the following license applies to STAMP files:
 *
 * Copyright (c) 2007, Stanford University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Stanford University nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 *
 * CircuitRouter-SeqSolver.c
 *
 * =============================================================================
 */


#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "lib/list.h"
#include "maze.h"
#include "router.h"
#include "locksgrid.h"
#include "mutexutils.h"
#include "lib/timer.h"
#include "lib/types.h"

enum param_types {
    PARAM_NUMTHREADS = (unsigned char)'t',
    PARAM_BENDCOST   = (unsigned char)'b',
    PARAM_XCOST      = (unsigned char)'x',
    PARAM_YCOST      = (unsigned char)'y',
    PARAM_ZCOST      = (unsigned char)'z',
};

enum param_defaults {
    PARAM_DEFAULT_BENDCOST = 1,
    PARAM_DEFAULT_XCOST    = 1,
    PARAM_DEFAULT_YCOST    = 1,
    PARAM_DEFAULT_ZCOST    = 2,
};

bool_t global_doPrint = TRUE;
char* global_inputFile = NULL;
long global_params[256]; /* 256 = ascii limit */


/* =============================================================================
 * displayUsage
 * =============================================================================
 */
static void displayUsage (const char* appName){
    printf("Usage: %s <inputfile> [options]\n", appName);
    puts("\nOptions:                            (defaults)\n");
    printf("    t <UINT+>  [t] num threads      MANDATORY\n");
    printf("    b <INT>    [b]end cost          (%i)\n", PARAM_DEFAULT_BENDCOST);
    printf("    x <UINT>   [x] movement cost    (%i)\n", PARAM_DEFAULT_XCOST);
    printf("    y <UINT>   [y] movement cost    (%i)\n", PARAM_DEFAULT_YCOST);
    printf("    z <UINT>   [z] movement cost    (%i)\n", PARAM_DEFAULT_ZCOST);
    printf("    h          [h]elp message       (false)\n");
    exit(1);
}


/* =============================================================================
 * setDefaultParams
 * =============================================================================
 */
static void setDefaultParams (){
    global_params[PARAM_BENDCOST] = PARAM_DEFAULT_BENDCOST;
    global_params[PARAM_XCOST]    = PARAM_DEFAULT_XCOST;
    global_params[PARAM_YCOST]    = PARAM_DEFAULT_YCOST;
    global_params[PARAM_ZCOST]    = PARAM_DEFAULT_ZCOST;
}


/* =============================================================================
 * parseArgs
 * =============================================================================
 */
static void parseArgs (long argc, char* const argv[]){
    long opt;

    opterr = 0;
    long t = -1;

    setDefaultParams();

    while ((opt = getopt(argc, argv, "ht:b:x:y:z:")) != -1) {
        switch (opt) {
            case 't':
                t = global_params[(unsigned char)opt] = strtol(optarg, NULL, 10);
                break;
            case 'b':
            case 'x':
            case 'y':
            case 'z':
                global_params[(unsigned char)opt] = atol(optarg);
                break;
            case '?':
            case 'h':
            default:
                opterr++;
                break;
        }
    }

    if (t <= 0) {
        fprintf(stderr, "%s option -t NUMTHREADS not specified or invalid.\n",
                         argv[0]);
        opterr++;
    }

    if (argc - optind != 1) {
        fprintf(stderr, "%s must receive (only) one non-option argument (input file).\n", 
                         argv[0]);
        opterr++;
    }

    if (opterr == 0) {
        global_inputFile = argv[optind];
    } else {
        displayUsage(argv[0]);
    }
}

/* =============================================================================
 * generateOutputFile
 * -- Returns pointer to output file. Returns NULL if failed.
 * =============================================================================
 */
FILE *generateOutputFile() {
    char outputFile[strlen(global_inputFile) + strlen(".res") + 1];
    sprintf(outputFile, "%s.res", global_inputFile);
    /*
     * Check if file exists
     */
    if (access(outputFile, F_OK) != -1) {
        char oldOutputFile[strlen(outputFile) + strlen(".old") + 1];
        sprintf(oldOutputFile, "%s.old", outputFile);
        if (rename(outputFile, oldOutputFile) == -1) {
            perror("Error renaming output file.");
            exit(1);
        }
    }
    FILE *outputFP = fopen(outputFile, "w");
    return outputFP;
}

/* =============================================================================
 * main
 * =============================================================================
 */
int main(int argc, char** argv){
    /*
     * Initialization
     */
    parseArgs(argc, (char** const)argv);
    maze_t* mazePtr = maze_alloc();
    assert(mazePtr);

    FILE *inputFP = fopen(global_inputFile, "r");
    if (inputFP == NULL) {
        fprintf(stderr, "Invalid input file: %s.\n", global_inputFile);
        exit(1);
    }
    FILE *outputFP = generateOutputFile();
    if (outputFP == NULL) {
        fprintf(stderr, "Error creating output file.\n");
        exit(1);
    }

    long numPathToRoute = maze_read(mazePtr, inputFP, outputFP);
    fclose(inputFP);
    router_t* routerPtr = router_alloc(global_params[PARAM_XCOST],
                                       global_params[PARAM_YCOST],
                                       global_params[PARAM_ZCOST],
                                       global_params[PARAM_BENDCOST]);
    assert(routerPtr);
    list_t* pathVectorListPtr = list_alloc(NULL);
    assert(pathVectorListPtr);

    
    //Initialize grid locks.
    long gridWidth = mazePtr->gridPtr->width;
    long gridHeight = mazePtr->gridPtr->height; 
    long gridDepth = mazePtr->gridPtr->depth; 
    locksgrid_t* lgrid = locksgrid_create(gridWidth, gridHeight, gridDepth);
    if (lgrid == NULL) {
        fprintf(stderr, "Error generating mutexes grid.\n");
        exit(1);
    }

    //Initialize "global" mutexes.
    pthread_mutex_t pathsqueue_mutex;
    pthread_mutex_t pathlist_mutex;
    mutils_init(&pathsqueue_mutex);
    mutils_init(&pathlist_mutex);

    router_solve_arg_t routerArg = {routerPtr, mazePtr, pathVectorListPtr, lgrid,
                                    &pathsqueue_mutex, &pathlist_mutex};
    pthread_t tids[PARAM_NUMTHREADS];
    
    TIMER_T startTime;
    TIMER_READ(startTime);
    
    for (int i = 0; i < global_params[PARAM_NUMTHREADS]; i++) {
        if (pthread_create(&tids[i], NULL, router_solve, (void*) &routerArg)) {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }
    for (int i = 0; i < global_params[PARAM_NUMTHREADS]; i++) {
        if (pthread_join(tids[i], NULL)) {
            fprintf(stderr, "Error joining threads\n");
            exit(1);
        }
    }

    TIMER_T stopTime;
    TIMER_READ(stopTime);

    locksgrid_free(lgrid);
    mutils_destroy(&pathsqueue_mutex);
    mutils_destroy(&pathlist_mutex);

    long numPathRouted = 0;
    list_iter_t it;
    list_iter_reset(&it, pathVectorListPtr);
    while (list_iter_hasNext(&it, pathVectorListPtr)) {
        vector_t* pathVectorPtr = (vector_t*)list_iter_next(&it, pathVectorListPtr);
        numPathRouted += vector_getSize(pathVectorPtr);
	}

    fprintf(outputFP, "Paths routed    = %li\n", numPathRouted);
    fprintf(outputFP, "Elapsed time    = %f seconds\n", TIMER_DIFF_SECONDS(startTime, stopTime));


    /*
     * Check solution and clean up
     */
    assert(numPathRouted <= numPathToRoute);
    bool_t status = maze_checkPaths(mazePtr, pathVectorListPtr, global_doPrint, outputFP);
    assert(status == TRUE);
    fprintf(outputFP, "Verification passed.\n");
    fclose(outputFP);

    maze_free(mazePtr);
    router_free(routerPtr);

    list_iter_reset(&it, pathVectorListPtr);
    while (list_iter_hasNext(&it, pathVectorListPtr)) {
        vector_t* pathVectorPtr = (vector_t*)list_iter_next(&it, pathVectorListPtr);
        vector_t* v;
        while((v = vector_popBack(pathVectorPtr))) {
            // v stores pointers to longs stored elsewhere; no need to free them here
            vector_free(v);
        }
        vector_free(pathVectorPtr);
    }
    list_free(pathVectorListPtr);


    exit(0);
}


/* =============================================================================
 *
 * End of CircuitRouter-SeqSolver.c
 *
 * =============================================================================
 */

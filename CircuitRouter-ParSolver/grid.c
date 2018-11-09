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
 * grid.c
 *
 * =============================================================================
 */


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "locksgrid.h"
#include "mutexutils.h"
#include "coordinate.h"
#include "grid.h"
#include "lib/types.h"
#include "lib/vector.h"


const unsigned long CACHE_LINE_SIZE = 32UL;


/* =============================================================================
 * grid_alloc
 * =============================================================================
 */
grid_t* grid_alloc (long width, long height, long depth){
    grid_t* gridPtr;

    gridPtr = (grid_t*)malloc(sizeof(grid_t));
    if (gridPtr) {
        gridPtr->width  = width;
        gridPtr->height = height;
        gridPtr->depth  = depth;
        long n = width * height * depth;
        long* points_unaligned = (long*)malloc(n * sizeof(long) + CACHE_LINE_SIZE);
        assert(points_unaligned);
        gridPtr->points_unaligned = points_unaligned;
        gridPtr->points = (long*)((char*)(((unsigned long)points_unaligned
                                          & ~(CACHE_LINE_SIZE-1)))
                                  + CACHE_LINE_SIZE);
        memset(gridPtr->points, GRID_POINT_EMPTY, (n * sizeof(long)));
    }

    return gridPtr;
}

/* =============================================================================
 * grid_free
 * =============================================================================
 */
void grid_free (grid_t* gridPtr){
    free(gridPtr->points_unaligned);
    free(gridPtr);
}


/* =============================================================================
 * grid_copy
 * =============================================================================
 */
void grid_copy (grid_t* dstGridPtr, grid_t* srcGridPtr){
    assert(srcGridPtr->width  == dstGridPtr->width);
    assert(srcGridPtr->height == dstGridPtr->height);
    assert(srcGridPtr->depth  == dstGridPtr->depth);

    long n = srcGridPtr->width * srcGridPtr->height * srcGridPtr->depth;
    memcpy(dstGridPtr->points, srcGridPtr->points, (n * sizeof(long)));
}


/* =============================================================================
 * grid_isPointValid
 * =============================================================================
 */
bool_t grid_isPointValid (grid_t* gridPtr, long x, long y, long z){
    if (x < 0 || x >= gridPtr->width  ||
        y < 0 || y >= gridPtr->height ||
        z < 0 || z >= gridPtr->depth)
    {
        return FALSE;
    }

    return TRUE;
}


/* =============================================================================
 * grid_getPointIndex
 * =============================================================================
 */
long grid_getPointIndex (grid_t* gridPtr, long* gridPointPtr){
    /*
     * Since we're subtracting pointers it does the conversion automatically,
     * taking care of the size of each element.
     */
    return (long) (gridPointPtr - gridPtr->points);
}


/* =============================================================================
 * grid_getPointRef
 * =============================================================================
 */
long* grid_getPointRef (grid_t* gridPtr, long x, long y, long z){
    return &(gridPtr->points[(z * gridPtr->height + y) * gridPtr->width + x]);
}


/* =============================================================================
 * grid_getPointIndices
 * =============================================================================
 */
void grid_getPointIndices (grid_t* gridPtr, long* gridPointPtr, long* xPtr, long* yPtr, long* zPtr){
    long height = gridPtr->height;
    long width  = gridPtr->width;
    long area = height * width;
    long index3d = (gridPointPtr - gridPtr->points);
    (*zPtr) = index3d / area;
    long index2d = index3d % area;
    (*yPtr) = index2d / width;
    (*xPtr) = index2d % width;
}


/* =============================================================================
 * grid_getPoint
 * =============================================================================
 */
long grid_getPoint (grid_t* gridPtr, long x, long y, long z){
    return *grid_getPointRef(gridPtr, x, y, z);
}


/* =============================================================================
 * grid_isPointEmpty
 * =============================================================================
 */
bool_t grid_isPointEmpty (grid_t* gridPtr, long x, long y, long z){
    long value = grid_getPoint(gridPtr, x, y, z);
    return ((value == GRID_POINT_EMPTY) ? TRUE : FALSE);
}


/* =============================================================================
 * grid_isPointFull
 * =============================================================================
 */
bool_t grid_isPointFull (grid_t* gridPtr, long x, long y, long z){
    long value = grid_getPoint(gridPtr, x, y, z);
    return ((value == GRID_POINT_FULL) ? TRUE : FALSE);
}


/* =============================================================================
 * grid_setPoint
 * =============================================================================
 */
void grid_setPoint (grid_t* gridPtr, long x, long y, long z, long value){
    (*grid_getPointRef(gridPtr, x, y, z)) = value;
}


/* =============================================================================
 * grid_addPath
 * =============================================================================
 */
void grid_addPath (grid_t* gridPtr, vector_t* pointVectorPtr){
    long i;
    long n = vector_getSize(pointVectorPtr);

    for (i = 0; i < n; i++) {
        coordinate_t* coordinatePtr = (coordinate_t*)vector_at(pointVectorPtr, i);
        long x = coordinatePtr->x;
        long y = coordinatePtr->y;
        long z = coordinatePtr->z;
        grid_setPoint(gridPtr, x, y, z, GRID_POINT_FULL);
    }
}


/*==============================================================================
 * grid_pointCmp
 *  Compares two points, p and q should (must( belong to the same grid. The 
 *  comparation criteria is just their memory address because they are in a
 *  contiguous zone of memory, and so we can compare their "3d position" by
 *  simply comparing their position on the memory.
 *=============================================================================
 */
static int grid_pointCmp(const void* p, const void* q) {
    return p - q;
}


/* =============================================================================
 * grid_addPath_Ptr
 * - returns FALSE it if fails adding the path to the grid.
 * =============================================================================
 */
bool_t grid_addPath_Ptr (grid_t* gridPtr, vector_t* pointVectorPtr, locksgrid_t* lgrid){

    long i;
    long n = vector_getSize(pointVectorPtr);
    long* src = (long*)vector_at(pointVectorPtr, 0);
    long* dst = (long*)vector_at(pointVectorPtr, n-1);

    /*
     * The path vector is sorted so that the paths are checked all in the
     * direction, avoiding deadlocks, since no path can go back and collide
     * with itself or another one that was waiting for it.
     */
    vector_sort(pointVectorPtr, &grid_pointCmp);

    /*
     * Test if path is still valid.
     */
    for (i = 0; i < n; i++) {
        long* gridPointPtr = (long*)vector_at(pointVectorPtr, i);
        //Ignore source and destination, as those are already full.
        if (gridPointPtr == src || gridPointPtr == dst) {
            continue;
        }

        long pointIndex = grid_getPointIndex(gridPtr, gridPointPtr);
        mutils_lock(locksgrid_getLock(lgrid, pointIndex));
        if (*gridPointPtr == GRID_POINT_FULL) {
            grid_unlockPath(gridPtr, pointVectorPtr, lgrid, i);
            return FALSE;
        }
    }

    /*
     * Commit to global grid if it is.
     */
    for (i = 0; i < n; i++) {
        long* gridPointPtr = (long*)vector_at(pointVectorPtr, i);
        if (gridPointPtr == src || gridPointPtr == dst) {
            continue;
        }
        long pointIndex = grid_getPointIndex(gridPtr, gridPointPtr);
        *gridPointPtr = GRID_POINT_FULL; 
        mutils_unlock(locksgrid_getLock(lgrid, pointIndex));
    }
    return TRUE;
}


/* =============================================================================
 * grid_unlockPath
 * =============================================================================
 */
void grid_unlockPath (grid_t* gridPtr, vector_t* pointVectorPtr, locksgrid_t* lgrid, int i) {

    for (; i > 0; i--) {
        long* gridPointPtr = (long*)vector_at(pointVectorPtr, i);
        long pointIndex = grid_getPointIndex(gridPtr, gridPointPtr);
        mutils_unlock(locksgrid_getLock(lgrid, pointIndex));
    }
}


/* =============================================================================
 * grid_printToFile
 * =============================================================================
 */
void grid_printToFile (grid_t* gridPtr, FILE* fp){

    long width  = gridPtr->width;
    long height = gridPtr->height;
    long depth  = gridPtr->depth;
    long z;

    for (z = 0; z < depth; z++) {
        fprintf(fp, "[z = %li]\n", z);
        long x;
        for (x = 0; x < width; x++) {
            long y;
            for (y = 0; y < height; y++) {
                fprintf(fp, "%4li", *grid_getPointRef(gridPtr, x, y, z));
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
    }
}


/* =============================================================================
 *
 * End of grid.c
 *
 * =============================================================================
 */

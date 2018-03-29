/**
    Board Utils
    ===========
    BoardUtils.h stores prepared array maps, bit maps and reverse maps
    to help traverse sudoku board more efficiently and transparently.
    These maps are frequently used in BoardData.c and BoardSolver.c

    What excatly we can do with this maps?\
    For example we can count bits (choises) in specific situations
    or trigger specific positions in array, so we don't need
    to write each cycle manualy in further code (no need to traverse
    rows, columns, boxes manualy and no need to check borders of this traverse)

    Everything below is built for Sudoku of size 9 x 9 so this maps basically
    hardwires 9 x 9 board solution. It's possible to change completely
    this maps to work with board size 16 x 16, but as sudoku board rises
    it will be extremly challanging or impossible with current code.

    **Code:**
 */

#pragma once
#define CONST                   const // Faster without const keyword on some compilers
#define POINTS                     81
#define VIEWS                      27
// #define DIGITS                   9 // unused
#define VIEWS_PER_POINT             3
#define POINTS_PER_VIEW             9 
#define REF_POINTS                 20 // neighbours points for each point
#define LOCKED_CANDIDATES          54 // 3 x 9 for rows + 3 x 9 for columns
#define LOCKED_CANDIDATE_SET_SIZE  15 // 3 (group A) + 6 (group B) + 6 (group C)
#define DIGITS_OFFSETTED           10 // offsetted by 1
#define PREPARED_BIT_TO_DIGIT     513
#define PREPARED_INT_TO_BIT_SIZE 1024

#define BU__views(viewIndex, pointI)            BU__views[viewIndex][pointI]
#define BU__viewsReversed(pointIndex, viewI)    BU__viewsReversed[pointIndex][viewI]
#define BU__points(pointIndex, viewI)           BU__points[pointIndex][viewI]
#define BU__refPoints(i1, i2)        BU__refPoints[i1][i2]
#define BU__digits(i1)               BU__digits[i1]
#define BU__bitPositions(i1)         BU__bitPositions[i1]
#define BU__bitCount(i1)             BU__bitCount[i1]
#define BU__lockedCandidates(i1, i2) BU__lockedCandidates[i1][i2]


// Rest of this file contains prepared 4959 Bytes of data
/**
    ## BU__views

    **Input**
    - VIEWS (number 0-26) - iteration through 9*3 possible views (all columns, rows and boxes).
    - POINTS_PER_VIEW (number 0-8) - represents elements inside each VIEWS group

    **Output**
    - Array position of each individual element

    **Description**
    ```
    our board described as array with indexes (0-80)
    [
       0,   1,   2,   3,   4,   5,   6,   7,   8,

       9,  10,  11,  12,  13,  14,  15,  16,  17,

      18,  19,  20,  21,  22,  23,  24,  25,  26,

      27,  28,  29,  30,  31,  32,  33,  34,  35,

      36,  37,  38,  39,  40,  41,  42,  43,  44,

      45,  46,  47,  48,  49,  50,  51,  52,  53,

      54,  55,  56,  57,  58,  59,  60,  61,  62,

      63,  64,  65,  66,  67,  68,  69,  70,  71,

      72,  73,  74,  75,  76,  77,  78,  79,  80
    ]
    than we can map each VIEWS set to this board
    ```
    **Example**
    ```
    { 30, 31, 32, 39, 40, 41, 48, 49, 50 } => VIEW of centered box
    [
        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,  30,  31,  32,    ,    ,    ,

        ,    ,    ,  39,  40,  41,    ,    ,    ,

        ,    ,    ,  48,  49,  50,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,
    ]
    ```
    **Code:**
 */
static CONST unsigned char BU__views[VIEWS][POINTS_PER_VIEW] = {
	{ 0,  9, 18, 27, 36, 45, 54, 63, 72 },
	{ 0,  1,  2,  3,  4,  5,  6,  7,  8 },
	{ 0,  1,  2,  9, 10, 11, 18, 19, 20 },
	{ 1, 10, 19, 28, 37, 46, 55, 64, 73 },
	{ 9, 10, 11, 12, 13, 14, 15, 16, 17 },
	{ 3,  4,  5, 12, 13, 14, 21, 22, 23 },
	{ 2, 11, 20, 29, 38, 47, 56, 65, 74 },
	{ 18, 19, 20, 21, 22, 23, 24, 25, 26 },
	{ 6,  7,  8, 15, 16, 17, 24, 25, 26 },
	{ 3, 12, 21, 30, 39, 48, 57, 66, 75 },
	{ 27, 28, 29, 30, 31, 32, 33, 34, 35 },
	{ 27, 28, 29, 36, 37, 38, 45, 46, 47 },
	{ 4, 13, 22, 31, 40, 49, 58, 67, 76 },
	{ 36, 37, 38, 39, 40, 41, 42, 43, 44 },
	{ 30, 31, 32, 39, 40, 41, 48, 49, 50 },
	{ 5, 14, 23, 32, 41, 50, 59, 68, 77 },
	{ 45, 46, 47, 48, 49, 50, 51, 52, 53 },
	{ 33, 34, 35, 42, 43, 44, 51, 52, 53 },
	{ 6, 15, 24, 33, 42, 51, 60, 69, 78 },
	{ 54, 55, 56, 57, 58, 59, 60, 61, 62 },
	{ 54, 55, 56, 63, 64, 65, 72, 73, 74 },
	{ 7, 16, 25, 34, 43, 52, 61, 70, 79 },
	{ 63, 64, 65, 66, 67, 68, 69, 70, 71 },
	{ 57, 58, 59, 66, 67, 68, 75, 76, 77 },
	{ 8, 17, 26, 35, 44, 53, 62, 71, 80 },
	{ 72, 73, 74, 75, 76, 77, 78, 79, 80 },
	{ 60, 61, 62, 69, 70, 71, 78, 79, 80 }
};


/**
    ## BU__viewsReversed

    **Input**
    - POINTS (number 0-80) - iteration through 81 possible points in 9x9 sudoku
    - VIEWS_PER_POINT (number 0-2) - represents three types - column, row, box

    **Output**
    - Bit map - representing bit position

    **Description**\
    Used for unsetting resolved numbers

    **Example**\
    TODO

    **Code:**
*/
static CONST unsigned short BU__viewsReversed[POINTS][VIEWS_PER_POINT] = {
	{ 1,  1,  1 },
	{ 2,  2,  1 },
	{ 4,  4,  1 },
	{ 8,  1,  1 },
	{ 16,  2,  1 },
	{ 32,  4,  1 },
	{ 64,  1,  1 },
	{ 128,  2,  1 },
	{ 256,  4,  1 },
	{ 2,  8,  1 },
	{ 16,  2,  2 },
	{ 32,  4,  2 },
	{ 8,  8,  2 },
	{ 16, 16,  2 },
	{ 32, 32,  2 },
	{ 64,  8,  2 },
	{ 128, 16,  2 },
	{ 256, 32,  2 },
	{ 4, 64,  1 },
	{ 128,  4,  2 },
	{ 256,  4,  4 },
	{ 64,  8,  4 },
	{ 128, 16,  4 },
	{ 256, 32,  4 },
	{ 64, 64,  4 },
	{ 128, 128,  4 },
	{ 256, 256,  4 },
	{ 8,  1,  1 },
	{ 8,  2,  2 },
	{ 8,  4,  4 },
	{ 8,  8,  1 },
	{ 16,  2,  8 },
	{ 32,  4,  8 },
	{ 64,  1,  8 },
	{ 128,  2,  8 },
	{ 256,  4,  8 },
	{ 16,  8,  1 },
	{ 16, 16,  2 },
	{ 16, 32,  4 },
	{ 16,  8,  8 },
	{ 16, 16, 16 },
	{ 32, 32, 16 },
	{ 64,  8, 16 },
	{ 128, 16, 16 },
	{ 256, 32, 16 },
	{ 32, 64,  1 },
	{ 32, 128,  2 },
	{ 32, 256,  4 },
	{ 32, 64,  8 },
	{ 32, 128, 16 },
	{ 256, 32, 32 },
	{ 64, 64, 32 },
	{ 128, 128, 32 },
	{ 256, 256, 32 },
	{ 64,  1,  1 },
	{ 64,  2,  2 },
	{ 64,  4,  4 },
	{ 64,  8,  1 },
	{ 64, 16,  2 },
	{ 64, 32,  4 },
	{ 64, 64,  1 },
	{ 128, 64,  2 },
	{ 256,  4, 64 },
	{ 128,  8,  1 },
	{ 128, 16,  2 },
	{ 128, 32,  4 },
	{ 128,  8,  8 },
	{ 128, 16, 16 },
	{ 128, 32, 32 },
	{ 128, 64,  8 },
	{ 128, 128, 16 },
	{ 256, 32, 128 },
	{ 256, 64,  1 },
	{ 256, 128,  2 },
	{ 256, 256,  4 },
	{ 256, 64,  8 },
	{ 256, 128, 16 },
	{ 256, 256, 32 },
	{ 256, 64, 64 },
	{ 256, 128, 128 },
	{ 256, 256, 256 }
};


/**
    ## BU__points

    **Input**
    - POINTS (number 0-80) - iteration through 81 possible points in 9x9 sudoku
    - VIEWS_PER_POINT (number 0-2) - represents three types - column, row, box

    **Output**
    - specific VIEW

    **Description**\
    Complementary to BU__views.\
    Each point is falling into three VIEWS (column, row and box) and this is back reference to BU__views

    **Code:**
*/
static CONST unsigned char BU__points[POINTS][VIEWS_PER_POINT] = {
	{ 0,  1,  2 },
	{ 1,  2,  3 },
	{ 1,  2,  6 },
	{ 1,  5,  9 },
	{ 1,  5, 12 },
	{ 1,  5, 15 },
	{ 1,  8, 18 },
	{ 1,  8, 21 },
	{ 1,  8, 24 },
	{ 0,  2,  4 },
	{ 2,  3,  4 },
	{ 2,  4,  6 },
	{ 4,  5,  9 },
	{ 4,  5, 12 },
	{ 4,  5, 15 },
	{ 4,  8, 18 },
	{ 4,  8, 21 },
	{ 4,  8, 24 },
	{ 0,  2,  7 },
	{ 2,  3,  7 },
	{ 2,  6,  7 },
	{ 5,  7,  9 },
	{ 5,  7, 12 },
	{ 5,  7, 15 },
	{ 7,  8, 18 },
	{ 7,  8, 21 },
	{ 7,  8, 24 },
	{ 0, 10, 11 },
	{ 3, 10, 11 },
	{ 6, 10, 11 },
	{ 9, 10, 14 },
	{ 10, 14, 12 },
	{ 10, 14, 15 },
	{ 10, 17, 18 },
	{ 10, 17, 21 },
	{ 10, 17, 24 },
	{ 0, 11, 13 },
	{ 3, 11, 13 },
	{ 6, 11, 13 },
	{ 9, 13, 14 },
	{ 12, 13, 14 },
	{ 13, 14, 15 },
	{ 13, 17, 18 },
	{ 13, 17, 21 },
	{ 13, 17, 24 },
	{ 0, 11, 16 },
	{ 3, 11, 16 },
	{ 6, 11, 16 },
	{ 9, 14, 16 },
	{ 12, 14, 16 },
	{ 14, 15, 16 },
	{ 16, 17, 18 },
	{ 16, 17, 21 },
	{ 16, 17, 24 },
	{ 0, 19, 20 },
	{ 3, 19, 20 },
	{ 6, 19, 20 },
	{ 9, 19, 23 },
	{ 12, 19, 23 },
	{ 15, 19, 23 },
	{ 18, 19, 26 },
	{ 19, 21, 26 },
	{ 19, 26, 24 },
	{ 0, 20, 22 },
	{ 3, 20, 22 },
	{ 6, 20, 22 },
	{ 9, 22, 23 },
	{ 12, 22, 23 },
	{ 15, 22, 23 },
	{ 18, 22, 26 },
	{ 21, 22, 26 },
	{ 22, 26, 24 },
	{ 0, 20, 25 },
	{ 3, 20, 25 },
	{ 6, 20, 25 },
	{ 9, 23, 25 },
	{ 12, 23, 25 },
	{ 15, 23, 25 },
	{ 18, 25, 26 },
	{ 21, 25, 26 },
	{ 24, 25, 26 }
};


/**
    ## BU__refPoints

    **Input**
    - POINTS (number 0-80) - iteration through 81 possible points in 9x9 sudoku
    - REF_POINTS (number 0-19) - represents related points for each point index

    **Output**
    - reference to related point

    **Description**
    ```
    Our board described as array with indexes (0-80)
    [
    [
       0,   1,   2,   3,   4,   5,   6,   7,   8,

       9,  10,  11,  12,  13,  14,  15,  16,  17,

      18,  19,  20,  21,  22,  23,  24,  25,  26,

      27,  28,  29,  30,  31,  32,  33,  34,  35,

      36,  37,  38,  39,  40,  41,  42,  43,  44,

      45,  46,  47,  48,  49,  50,  51,  52,  53,

      54,  55,  56,  57,  58,  59,  60,  61,  62,

      63,  64,  65,  66,  67,  68,  69,  70,  71,

      72,  73,  74,  75,  76,  77,  78,  79,  80
    ]
    ]
    Than we can map each POINTS set to this board
    ```

    **Examples**
    ```
    Example 1 -> for POINT 0 (symbolized as @)
    { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20, 27, 36, 45, 54, 63, 72 },
    [
       @,   1,   2,   3,   4,   5,   6,   7,   8,

       9,  10,  11,    ,    ,    ,    ,    ,    ,

      18,  19,  20,    ,    ,    ,    ,    ,    ,

      27,    ,    ,    ,    ,    ,    ,    ,    ,

      36,    ,    ,    ,    ,    ,    ,    ,    ,

      45,    ,    ,    ,    ,    ,    ,    ,    ,

      54,    ,    ,    ,    ,    ,    ,    ,    ,

      63,    ,    ,    ,    ,    ,    ,    ,    ,

      72,    ,    ,    ,    ,    ,    ,    ,    ,
    ]
    Example 2 -> for POINT 40 (symbolized as @)
    { 4, 13, 22, 30, 31, 32, 36, 37, 38, 39, 41, 42, 43, 44, 48, 49, 50, 58, 67, 76 },
    [
        ,    ,    ,    ,   4,    ,    ,    ,    ,

        ,    ,    ,    ,  13,    ,    ,    ,    ,

        ,    ,    ,    ,  22,    ,    ,    ,    ,

        ,    ,    ,  30,  31,  32,    ,    ,    ,

      36,  37,  38,  39,   @,   41,  42,  43,  44,

        ,    ,    ,  48,  49,  50,    ,    ,    ,

        ,    ,    ,    ,  58,    ,    ,    ,    ,

        ,    ,    ,    ,  67,    ,    ,    ,    ,

        ,    ,    ,    ,  76,    ,    ,    ,    ,
    ]
    ```

    **Code:**
 */
static CONST unsigned char BU__refPoints[POINTS][REF_POINTS] = {
	{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20, 27, 36, 45, 54, 63, 72 },
	{ 0,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20, 28, 37, 46, 55, 64, 73 },
	{ 0,  1,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20, 29, 38, 47, 56, 65, 74 },
	{ 0,  1,  2,  4,  5,  6,  7,  8, 12, 13, 14, 21, 22, 23, 30, 39, 48, 57, 66, 75 },
	{ 0,  1,  2,  3,  5,  6,  7,  8, 12, 13, 14, 21, 22, 23, 31, 40, 49, 58, 67, 76 },
	{ 0,  1,  2,  3,  4,  6,  7,  8, 12, 13, 14, 21, 22, 23, 32, 41, 50, 59, 68, 77 },
	{ 0,  1,  2,  3,  4,  5,  7,  8, 15, 16, 17, 24, 25, 26, 33, 42, 51, 60, 69, 78 },
	{ 0,  1,  2,  3,  4,  5,  6,  8, 15, 16, 17, 24, 25, 26, 34, 43, 52, 61, 70, 79 },
	{ 0,  1,  2,  3,  4,  5,  6,  7, 15, 16, 17, 24, 25, 26, 35, 44, 53, 62, 71, 80 },
	{ 0,  1,  2, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 27, 36, 45, 54, 63, 72 },
	{ 0,  1,  2,  9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 28, 37, 46, 55, 64, 73 },
	{ 0,  1,  2,  9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 29, 38, 47, 56, 65, 74 },
	{ 3,  4,  5,  9, 10, 11, 13, 14, 15, 16, 17, 21, 22, 23, 30, 39, 48, 57, 66, 75 },
	{ 3,  4,  5,  9, 10, 11, 12, 14, 15, 16, 17, 21, 22, 23, 31, 40, 49, 58, 67, 76 },
	{ 3,  4,  5,  9, 10, 11, 12, 13, 15, 16, 17, 21, 22, 23, 32, 41, 50, 59, 68, 77 },
	{ 6,  7,  8,  9, 10, 11, 12, 13, 14, 16, 17, 24, 25, 26, 33, 42, 51, 60, 69, 78 },
	{ 6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 17, 24, 25, 26, 34, 43, 52, 61, 70, 79 },
	{ 6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 24, 25, 26, 35, 44, 53, 62, 71, 80 },
	{ 0,  1,  2,  9, 10, 11, 19, 20, 21, 22, 23, 24, 25, 26, 27, 36, 45, 54, 63, 72 },
	{ 0,  1,  2,  9, 10, 11, 18, 20, 21, 22, 23, 24, 25, 26, 28, 37, 46, 55, 64, 73 },
	{ 0,  1,  2,  9, 10, 11, 18, 19, 21, 22, 23, 24, 25, 26, 29, 38, 47, 56, 65, 74 },
	{ 3,  4,  5, 12, 13, 14, 18, 19, 20, 22, 23, 24, 25, 26, 30, 39, 48, 57, 66, 75 },
	{ 3,  4,  5, 12, 13, 14, 18, 19, 20, 21, 23, 24, 25, 26, 31, 40, 49, 58, 67, 76 },
	{ 3,  4,  5, 12, 13, 14, 18, 19, 20, 21, 22, 24, 25, 26, 32, 41, 50, 59, 68, 77 },
	{ 6,  7,  8, 15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 33, 42, 51, 60, 69, 78 },
	{ 6,  7,  8, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26, 34, 43, 52, 61, 70, 79 },
	{ 6,  7,  8, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 35, 44, 53, 62, 71, 80 },
	{ 0,  9, 18, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 54, 63, 72 },
	{ 1, 10, 19, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 55, 64, 73 },
	{ 2, 11, 20, 27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47, 56, 65, 74 },
	{ 3, 12, 21, 27, 28, 29, 31, 32, 33, 34, 35, 39, 40, 41, 48, 49, 50, 57, 66, 75 },
	{ 4, 13, 22, 27, 28, 29, 30, 32, 33, 34, 35, 39, 40, 41, 48, 49, 50, 58, 67, 76 },
	{ 5, 14, 23, 27, 28, 29, 30, 31, 33, 34, 35, 39, 40, 41, 48, 49, 50, 59, 68, 77 },
	{ 6, 15, 24, 27, 28, 29, 30, 31, 32, 34, 35, 42, 43, 44, 51, 52, 53, 60, 69, 78 },
	{ 7, 16, 25, 27, 28, 29, 30, 31, 32, 33, 35, 42, 43, 44, 51, 52, 53, 61, 70, 79 },
	{ 8, 17, 26, 27, 28, 29, 30, 31, 32, 33, 34, 42, 43, 44, 51, 52, 53, 62, 71, 80 },
	{ 0,  9, 18, 27, 28, 29, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 54, 63, 72 },
	{ 1, 10, 19, 27, 28, 29, 36, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 55, 64, 73 },
	{ 2, 11, 20, 27, 28, 29, 36, 37, 39, 40, 41, 42, 43, 44, 45, 46, 47, 56, 65, 74 },
	{ 3, 12, 21, 30, 31, 32, 36, 37, 38, 40, 41, 42, 43, 44, 48, 49, 50, 57, 66, 75 },
	{ 4, 13, 22, 30, 31, 32, 36, 37, 38, 39, 41, 42, 43, 44, 48, 49, 50, 58, 67, 76 },
	{ 5, 14, 23, 30, 31, 32, 36, 37, 38, 39, 40, 42, 43, 44, 48, 49, 50, 59, 68, 77 },
	{ 6, 15, 24, 33, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44, 51, 52, 53, 60, 69, 78 },
	{ 7, 16, 25, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 44, 51, 52, 53, 61, 70, 79 },
	{ 8, 17, 26, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 51, 52, 53, 62, 71, 80 },
	{ 0,  9, 18, 27, 28, 29, 36, 37, 38, 46, 47, 48, 49, 50, 51, 52, 53, 54, 63, 72 },
	{ 1, 10, 19, 27, 28, 29, 36, 37, 38, 45, 47, 48, 49, 50, 51, 52, 53, 55, 64, 73 },
	{ 2, 11, 20, 27, 28, 29, 36, 37, 38, 45, 46, 48, 49, 50, 51, 52, 53, 56, 65, 74 },
	{ 3, 12, 21, 30, 31, 32, 39, 40, 41, 45, 46, 47, 49, 50, 51, 52, 53, 57, 66, 75 },
	{ 4, 13, 22, 30, 31, 32, 39, 40, 41, 45, 46, 47, 48, 50, 51, 52, 53, 58, 67, 76 },
	{ 5, 14, 23, 30, 31, 32, 39, 40, 41, 45, 46, 47, 48, 49, 51, 52, 53, 59, 68, 77 },
	{ 6, 15, 24, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 52, 53, 60, 69, 78 },
	{ 7, 16, 25, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 53, 61, 70, 79 },
	{ 8, 17, 26, 33, 34, 35, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 62, 71, 80 },
	{ 0,  9, 18, 27, 36, 45, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
	{ 1, 10, 19, 28, 37, 46, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
	{ 2, 11, 20, 29, 38, 47, 54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
	{ 3, 12, 21, 30, 39, 48, 54, 55, 56, 58, 59, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
	{ 4, 13, 22, 31, 40, 49, 54, 55, 56, 57, 59, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
	{ 5, 14, 23, 32, 41, 50, 54, 55, 56, 57, 58, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
	{ 6, 15, 24, 33, 42, 51, 54, 55, 56, 57, 58, 59, 61, 62, 69, 70, 71, 78, 79, 80 },
	{ 7, 16, 25, 34, 43, 52, 54, 55, 56, 57, 58, 59, 60, 62, 69, 70, 71, 78, 79, 80 },
	{ 8, 17, 26, 35, 44, 53, 54, 55, 56, 57, 58, 59, 60, 61, 69, 70, 71, 78, 79, 80 },
	{ 0,  9, 18, 27, 36, 45, 54, 55, 56, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
	{ 1, 10, 19, 28, 37, 46, 54, 55, 56, 63, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
	{ 2, 11, 20, 29, 38, 47, 54, 55, 56, 63, 64, 66, 67, 68, 69, 70, 71, 72, 73, 74 },
	{ 3, 12, 21, 30, 39, 48, 57, 58, 59, 63, 64, 65, 67, 68, 69, 70, 71, 75, 76, 77 },
	{ 4, 13, 22, 31, 40, 49, 57, 58, 59, 63, 64, 65, 66, 68, 69, 70, 71, 75, 76, 77 },
	{ 5, 14, 23, 32, 41, 50, 57, 58, 59, 63, 64, 65, 66, 67, 69, 70, 71, 75, 76, 77 },
	{ 6, 15, 24, 33, 42, 51, 60, 61, 62, 63, 64, 65, 66, 67, 68, 70, 71, 78, 79, 80 },
	{ 7, 16, 25, 34, 43, 52, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 71, 78, 79, 80 },
	{ 8, 17, 26, 35, 44, 53, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 78, 79, 80 },
	{ 0,  9, 18, 27, 36, 45, 54, 55, 56, 63, 64, 65, 73, 74, 75, 76, 77, 78, 79, 80 },
	{ 1, 10, 19, 28, 37, 46, 54, 55, 56, 63, 64, 65, 72, 74, 75, 76, 77, 78, 79, 80 },
	{ 2, 11, 20, 29, 38, 47, 54, 55, 56, 63, 64, 65, 72, 73, 75, 76, 77, 78, 79, 80 },
	{ 3, 12, 21, 30, 39, 48, 57, 58, 59, 66, 67, 68, 72, 73, 74, 76, 77, 78, 79, 80 },
	{ 4, 13, 22, 31, 40, 49, 57, 58, 59, 66, 67, 68, 72, 73, 74, 75, 77, 78, 79, 80 },
	{ 5, 14, 23, 32, 41, 50, 57, 58, 59, 66, 67, 68, 72, 73, 74, 75, 76, 78, 79, 80 },
	{ 6, 15, 24, 33, 42, 51, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 79, 80 },
	{ 7, 16, 25, 34, 43, 52, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 80 },
	{ 8, 17, 26, 35, 44, 53, 60, 61, 62, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79 }
};


/**
    ## BU__digits

    **Input**
    - DIGITS_OFFSETTED (number 0-9) - represents bit position

    **Output**
    - bitmap - NULL / 0000000010 - 1000000000

    **Description**\
    This is complementary map to BU__bitPositions\
    Used typically for iterating through 1-9 and instantly
    translating to map with lowest possible instructions\
    Then this map is used for binary operation

    **Example**\
    How to know that 6th bit is set in this number (bitmap): 0110111100 ?\
    number & BU__digits(6) => False (0) or True (>0)

    **Code:**
 */
static CONST unsigned short BU__digits[DIGITS_OFFSETTED] = { 00, 2, 4, 8, 16, 32, 64, 128, 256, 512 };


/**
    ## BU__bitPositions

    **Input**
    - PREPARED_BIT_TO_DIGIT (number 0-512) - represents bitmap 0000000000-1000000000

    **Output**
    - confirmation - which single! bit (position) is set

    **Description**\
    Used typically for detection on which single position
    is specific number available for instant-solving.\
    Or could be used for detection which candidate
    (1,2,3,4,5,6,7,8,9) is only possible in specific position

    **Example**
    ```
    num  =>   bitmap   =>  confirmation about single bit position
     0  =>          0 =>    00  - nothing
     1  =>          1 =>     0  - first position is set (index starting at 0)
     2  =>         10 =>     1  - second position is set (index starting at 0)
     3  =>         11 =>    00
     4  =>        100 =>     2  - third position is set (index starting at 0)
    ...
    512  => 1000000000 =>     9  - tenth position is set (index starting at 0)
    ```

    **Code:**
 */
static CONST unsigned char BU__bitPositions[PREPARED_BIT_TO_DIGIT] = {
	00,  0,  1, 00,  2, 00, 00, 00,  3, 00, 00, 00, 00, 00, 00, 00,  4, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	5, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	6, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	7, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	8, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
	9
};


/**
    ## BU__bitCount

    **Input**
    - PREPARED_INT_TO_BIT_SIZE (number 0-1023) - represents bitmap 0000000000-1111111111

    **Output**
    - counter - how many bits are set

    **Description**
    Lookup table [https://en.wikipedia.org/wiki/Hamming_weight]

    **Example**
    ```
    num  =>   bitmap   =>  how many bits are set
      0  =>          0 =>     0
      1  =>          1 =>     1
      2  =>         10 =>     1
      3  =>         11 =>     2
      4  =>        100 =>     1
    ....
    1023  => 1111111111 =>    10
    ```

    **Code:**
 */
static CONST unsigned char BU__bitCount[PREPARED_INT_TO_BIT_SIZE] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8, 8, 9,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8, 8, 9,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8, 8, 9,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8, 8, 9,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8, 8, 9,
	5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 7, 8, 8, 9, 7, 8, 8, 9, 8, 9, 9, 10
};


/**
    ## BU__lockedCandidates

    **Input**
    - LOCKED_CANDIDATES (number 0-53) - iteration through 9*3*2 different combinations
    - LOCKED_CANDIDATE_SET_SIZE (number 0-14) - represents elements inside each LOCKED_CANDIDATES group

    **Output**
    - array position of each individual element

    **Description**\
    our board described as array with indexes (0-80)
    ```
    [
    [
       0,   1,   2,   3,   4,   5,   6,   7,   8,

       9,  10,  11,  12,  13,  14,  15,  16,  17,

      18,  19,  20,  21,  22,  23,  24,  25,  26,

      27,  28,  29,  30,  31,  32,  33,  34,  35,

      36,  37,  38,  39,  40,  41,  42,  43,  44,

      45,  46,  47,  48,  49,  50,  51,  52,  53,

      54,  55,  56,  57,  58,  59,  60,  61,  62,

      63,  64,  65,  66,  67,  68,  69,  70,  71,

      72,  73,  74,  75,  76,  77,  78,  79,  80
    ]
    ]
    ```
    than we can map each LOCKED_CANDIDATES set to this board\
    Hidden information - you have to know that:\
    first 3 elements in set we use as group A\
    next 6 elements as group B\
    remaining 6 elements as group C

    **Example**
    ```
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20 }
      A   A   A   B   B   B   B   B   B   C   C   C   C   C   C
    first group of LOCKED_CANDIDATES
    [
       0,   1,   2,   3,   4,   5,   6,   7,   8,

       9,  10,  11,    ,    ,    ,    ,    ,    ,

      18,  19,  20,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,
    ]
    then we can use simplified further code to work with
    elements of group A+B+C and do easy mathematical operations with this subsets
    [
       A,   A,   A,   B,   B,   B,   B,   B,   B,

       C,   C,   C,    ,    ,    ,    ,    ,    ,

       C,   C,   C,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

        ,    ,    ,    ,    ,    ,    ,    ,    ,

    ]
    ```

    **Code:**
 */
static CONST unsigned char BU__lockedCandidates[LOCKED_CANDIDATES][LOCKED_CANDIDATE_SET_SIZE] = {
	{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 18, 19, 20 },
	{ 0,  9, 18, 27, 36, 45, 54, 63, 72,  1,  2, 10, 11, 19, 20 },
	{ 3,  4,  5,  0,  1,  2,  6,  7,  8, 12, 13, 14, 21, 22, 23 },
	{ 27, 36, 45,  0,  9, 18, 54, 63, 72, 28, 29, 37, 38, 46, 47 },
	{ 6,  7,  8,  0,  1,  2,  3,  4,  5, 15, 16, 17, 24, 25, 26 },
	{ 54, 63, 72,  0,  9, 18, 27, 36, 45, 55, 56, 64, 65, 73, 74 },
	{ 9, 10, 11, 12, 13, 14, 15, 16, 17,  0,  1,  2, 18, 19, 20 },
	{ 1, 10, 19, 28, 37, 46, 55, 64, 73,  0,  2,  9, 11, 18, 20 },
	{ 12, 13, 14,  9, 10, 11, 15, 16, 17,  3,  4,  5, 21, 22, 23 },
	{ 28, 37, 46,  1, 10, 19, 55, 64, 73, 27, 29, 36, 38, 45, 47 },
	{ 15, 16, 17,  9, 10, 11, 12, 13, 14,  6,  7,  8, 24, 25, 26 },
	{ 55, 64, 73,  1, 10, 19, 28, 37, 46, 54, 56, 63, 65, 72, 74 },
	{ 18, 19, 20, 21, 22, 23, 24, 25, 26,  0,  1,  2,  9, 10, 11 },
	{ 2, 11, 20, 29, 38, 47, 56, 65, 74,  0,  1,  9, 10, 18, 19 },
	{ 21, 22, 23, 18, 19, 20, 24, 25, 26,  3,  4,  5, 12, 13, 14 },
	{ 29, 38, 47,  2, 11, 20, 56, 65, 74, 27, 28, 36, 37, 45, 46 },
	{ 24, 25, 26, 18, 19, 20, 21, 22, 23,  6,  7,  8, 15, 16, 17 },
	{ 56, 65, 74,  2, 11, 20, 29, 38, 47, 54, 55, 63, 64, 72, 73 },
	{ 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 45, 46, 47 },
	{ 3, 12, 21, 30, 39, 48, 57, 66, 75,  4,  5, 13, 14, 22, 23 },
	{ 30, 31, 32, 27, 28, 29, 33, 34, 35, 39, 40, 41, 48, 49, 50 },
	{ 30, 39, 48,  3, 12, 21, 57, 66, 75, 31, 32, 40, 41, 49, 50 },
	{ 33, 34, 35, 27, 28, 29, 30, 31, 32, 42, 43, 44, 51, 52, 53 },
	{ 57, 66, 75,  3, 12, 21, 30, 39, 48, 58, 59, 67, 68, 76, 77 },
	{ 36, 37, 38, 39, 40, 41, 42, 43, 44, 27, 28, 29, 45, 46, 47 },
	{ 4, 13, 22, 31, 40, 49, 58, 67, 76,  3,  5, 12, 14, 21, 23 },
	{ 39, 40, 41, 36, 37, 38, 42, 43, 44, 30, 31, 32, 48, 49, 50 },
	{ 31, 40, 49,  4, 13, 22, 58, 67, 76, 30, 32, 39, 41, 48, 50 },
	{ 42, 43, 44, 36, 37, 38, 39, 40, 41, 33, 34, 35, 51, 52, 53 },
	{ 58, 67, 76,  4, 13, 22, 31, 40, 49, 57, 59, 66, 68, 75, 77 },
	{ 45, 46, 47, 48, 49, 50, 51, 52, 53, 27, 28, 29, 36, 37, 38 },
	{ 5, 14, 23, 32, 41, 50, 59, 68, 77,  3,  4, 12, 13, 21, 22 },
	{ 48, 49, 50, 45, 46, 47, 51, 52, 53, 30, 31, 32, 39, 40, 41 },
	{ 32, 41, 50,  5, 14, 23, 59, 68, 77, 30, 31, 39, 40, 48, 49 },
	{ 51, 52, 53, 45, 46, 47, 48, 49, 50, 33, 34, 35, 42, 43, 44 },
	{ 59, 68, 77,  5, 14, 23, 32, 41, 50, 57, 58, 66, 67, 75, 76 },
	{ 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 72, 73, 74 },
	{ 6, 15, 24, 33, 42, 51, 60, 69, 78,  7,  8, 16, 17, 25, 26 },
	{ 57, 58, 59, 54, 55, 56, 60, 61, 62, 66, 67, 68, 75, 76, 77 },
	{ 33, 42, 51,  6, 15, 24, 60, 69, 78, 34, 35, 43, 44, 52, 53 },
	{ 60, 61, 62, 54, 55, 56, 57, 58, 59, 69, 70, 71, 78, 79, 80 },
	{ 60, 69, 78,  6, 15, 24, 33, 42, 51, 61, 62, 70, 71, 79, 80 },
	{ 63, 64, 65, 66, 67, 68, 69, 70, 71, 54, 55, 56, 72, 73, 74 },
	{ 7, 16, 25, 34, 43, 52, 61, 70, 79,  6,  8, 15, 17, 24, 26 },
	{ 66, 67, 68, 63, 64, 65, 69, 70, 71, 57, 58, 59, 75, 76, 77 },
	{ 34, 43, 52,  7, 16, 25, 61, 70, 79, 33, 35, 42, 44, 51, 53 },
	{ 69, 70, 71, 63, 64, 65, 66, 67, 68, 60, 61, 62, 78, 79, 80 },
	{ 61, 70, 79,  7, 16, 25, 34, 43, 52, 60, 62, 69, 71, 78, 80 },
	{ 72, 73, 74, 75, 76, 77, 78, 79, 80, 54, 55, 56, 63, 64, 65 },
	{ 8, 17, 26, 35, 44, 53, 62, 71, 80,  6,  7, 15, 16, 24, 25 },
	{ 75, 76, 77, 72, 73, 74, 78, 79, 80, 57, 58, 59, 66, 67, 68 },
	{ 35, 44, 53,  8, 17, 26, 62, 71, 80, 33, 34, 42, 43, 51, 52 },
	{ 78, 79, 80, 72, 73, 74, 75, 76, 77, 60, 61, 62, 69, 70, 71 },
	{ 62, 71, 80,  8, 17, 26, 35, 44, 53, 60, 61, 69, 70, 78, 79 }
};

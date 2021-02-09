//
//  main.c
//  zkouska_pokus1
//
//  Created by MacBook on 02/01/2021.
//  Copyright © 2021 Klara Pacalova. All rights reserved.
//

#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>
#endif /* __PROGTEST__ */

typedef struct DOMINO
{
    char* name;
    int valFront1;
    int valFront2;

    int valBack1;
    int valBack2;
    
} domino;

typedef struct SIDE
{
    int min;
    int max;
    int idxSecondSide;
} side;

domino * allocateBrick(const char * buffer, int nameFrom, int nameTo, int val1, int val2, int val3, int val4)
{
    domino * res = (domino*)malloc(sizeof(domino));
    res->name = (char*)calloc(1, nameTo - nameFrom);
    memcpy(res->name, buffer + nameFrom, nameTo - nameFrom - 1);
    res->valFront1 = val1;
    res->valFront2 = val2;
    res->valBack1 = val3;
    res->valBack2 = val4;
    
    return res;
}

void printSide(side s)
{
    printf("id: %d \n", s.idxSecondSide);
    printf("< %d , %d > \n", s.min, s.max);
}

void destroyDomino(domino** dom, int size)
{
    for (int i = 0; i < size; ++i)
    {
        free(dom[i]->name);
        dom[i]->name = NULL;
        free(dom[i]);
        dom[i] = NULL;
    }
    free(dom);
}

void printDomino(domino dom)
{
    printf("name: %s\nval1: %d, \t val2: %d, \t val 3: %d, \t val4: %d\n", dom.name, dom.valFront1, dom.valFront2, dom.valBack1, dom.valBack2);
}

int parseDominos(const char * list, domino*** allDominos)
{
    int size = 1, count = 0;
    *allDominos = (domino**)calloc(size, sizeof(domino*));
    
    long int parseFrom = 0, bufLen = strlen(list);
    // at least 16 characters should be parsed
    while (parseFrom < bufLen - 16)
    {
        const char* current = list + parseFrom;
        int nameFrom, nameTo, val1, val2, val3, val4, parseEnd;
        //printf("parsing: \\%s\\\n", current);
        sscanf(current, " { '%n", &nameFrom);
        // at least 1 char
        nameTo = nameFrom + 1;
        
        // read while we do not read ending apostroph
        while (current[nameTo++] != '\'') { }
        
        // continue reading
        char c;
        int nRead = sscanf(current + nameTo, " ; %c %d | %d ] ; [ %d | %d ] }%n",
                         &c, &val1, &val2, &val3, &val4, &parseEnd);
        
        if (c != '[')
        {
            return -1;
        }
        
        if (nRead != 5)
        {
            //printf("error: got: %d\n", nRead);
            return -1;
        }
        
        if (count + 1 >= size) {
            size *= 2;
            *allDominos = (domino**)realloc(*allDominos, size * sizeof(domino*));
        }
        // allocate new brick
        (*allDominos)[count++] = allocateBrick(current, nameFrom, nameTo, val1, val2, val3, val4);
        
        //printf("domino brick: #%d; ", count);
        //printDomino(*(*allDominos)[count-1]);
        
        // shift to next brick
        parseFrom += nameTo + parseEnd;
    }

    return count;
}

int countCollectible ( const char * list )
{
    domino** allDominos;
    static const char * goldenEdition = "Core Dump Gold";
    int size = parseDominos(list, &allDominos);
    int ret = 0;
    
    for (int i = 0 ; i < size; ++i)
    {
      //  printDomino(*allDominos[i]);
        int valuesFront = allDominos[i]->valFront1 + allDominos[i]->valFront2;
        int valuesBack = allDominos[i]->valBack1 + allDominos[i]->valBack2;
        if ((valuesBack == 40 || valuesFront == 40) && (valuesBack + valuesFront != 80)
            && !strcmp(allDominos[i]->name, goldenEdition))
        {
            ++ret;
        }
    }
        
    destroyDomino(allDominos, size);
    return ret;
}

typedef struct descriptor
{
    struct side
    {
        // a is the smaller value
        int a,b;
    } shorter, longer;
} descriptor;

descriptor brickToDescriptor(domino * brick)
{
    descriptor res;
    int minFront = (brick->valFront1 < brick->valFront2) ? brick->valFront1 : brick->valFront2;
    int minBack = (brick->valBack1 < brick->valBack2) ? brick->valBack1 : brick->valBack2;
    int sumF = brick->valFront1 + brick->valFront2;
    int sumB = brick->valBack1 + brick->valBack2;
    
    // front is the shorter side - has lesser sum
    if (sumF < sumB || (sumF == sumB && minFront < minBack))
    {
        if (brick->valFront1 < brick->valFront2)
        {
            res.shorter.a = brick->valFront1;
            res.shorter.b = brick->valFront2;
        }
        else
        {
            res.shorter.a = brick->valFront2;
            res.shorter.b = brick->valFront1;
        }
        
        if (brick->valBack1 < brick->valBack2)
        {
            res.longer.a = brick->valBack1;
            res.longer.b = brick->valBack2;
        }
        else
        {
            res.longer.a = brick->valBack2;
            res.longer.b = brick->valBack1;
        }
    }
    else
    {
        // front is the longer side - has bigger sum
        if (brick->valFront1 < brick->valFront2)
        {
            res.longer.a = brick->valFront1;
            res.longer.b = brick->valFront2;
        }
        else
        {
            res.longer.a = brick->valFront2;
            res.longer.b = brick->valFront1;
        }
        
        if (brick->valBack1 < brick->valBack2)
        {
            res.shorter.a = brick->valBack1;
            res.shorter.b = brick->valBack2;
        }
        else
        {
            res.shorter.a = brick->valBack2;
            res.shorter.b = brick->valBack1;
        }
    }
    
    return res;
}

/*int comparator(const void* p1, const void* p2);
Return value meaning
<0 The element pointed by p1 goes before the element pointed by p2
0  The element pointed by p1 is equivalent to the element pointed by p2
>0 The element pointed by p1 goes after the element pointed by p2*/
int compareDominos(const void* LHS, const void* RHS)
{
    // cast to domino** and dereference
    descriptor left = brickToDescriptor(*((domino**)LHS));
    descriptor right = brickToDescriptor(*((domino**)RHS));
  /*  printf("descriptor left shorter: a: %d b: %d ", left.shorter.a, left.shorter.b);
    printf("descriptor left longer: a: %d b: %d ", left.longer.a, left.longer.b);
    printf("descriptor right shorter: a: %d b: %d ", right.shorter.a, right.shorter.b);
    printf("descriptor right longer: a: %d b: %d ", right.longer.a, right.longer.b);
    */
    if (left.shorter.a == right.shorter.a)
    {
        if (left.shorter.b == right.shorter.b)
        {
            if (left.longer.a == right.longer.a)
            {
                if (left.longer.b == right.longer.b)
                {
                    // cast to domino** and dereference
                    return strcmp((*(domino**)LHS)->name, ((*(domino**)RHS)->name));
                }
                
                return left.longer.b - right.longer.b;
            }
            
            return left.longer.a - right.longer.a;
        }
        
        return left.shorter.b - right.shorter.b;
    }
    
    return left.shorter.a - right.shorter.a;
}

int countUnique  ( const char * list )
{
    domino** allDominos;
    int size = parseDominos(list, &allDominos);
    
    qsort((void *) allDominos, (size_t)size, (size_t)(sizeof(domino*)), compareDominos);
    
    // shifted for cycle
    int countUnique = size > 0;
    
    for (int i = 1; i < size; ++i)
    {
        //printf("Domino #%d\n", i);
        //printDomino(*allDominos[i]);
        //printf("Count unique: %d\n", countUnique);
        
        int res = compareDominos((void*)&allDominos[i], (void*)&allDominos[i-1]);
        
        if (res != 0)
        {
            ++countUnique;
        }
    }
    
    destroyDomino(allDominos, size);
    
    //printf("Count unique final: %d\n", countUnique);
    return countUnique;
}

/*int comparator(const void* p1, const void* p2);
Return value meaning
<0 The element pointed by p1 goes before the element pointed by p2
0  The element pointed by p1 is equivalent to the element pointed by p2
>0 The element pointed by p1 goes after the element pointed by p2*/
int compareSides(const void* lhs, const void* rhs)
{
    side LHS = *(side*)lhs;
    side RHS = *(side*)rhs;
    
    if (LHS.min == RHS.min)
    {
        return LHS.max - RHS.max;
    }
    
    return LHS.min - RHS.min;
}

int fits(side * bottom, int val)
{
    // is val within the interval?
    return val >= bottom->min && val <= bottom->max;
}

uint64_t processBranch(side * sides, uint8_t * used, int id, int size)
{
    uint64_t res = 1;
    
    // direction instead of 2 for cycles
    // 2 iterations, at first falling, then rising
    for (int dir = -1; dir <= 1; dir += 2)
    {
        // to where it should run -1 or size (first invalid index)
        int end = (dir == -1 ? -1 : size);
        for (int i = id + dir; i != end; i += dir)
        {
            // min is fitted - first key
            if (fits(&sides[id], sides[i].min))
            {
                // if minimum and maximum fits, the brick is stackable
                if (!used[i] && fits(&sides[id], sides[i].max))
                {
                    used[i] = 1;
                    // match index of second side of domino
                    used[sides[i].idxSecondSide] = 1;
                    
                    // how many towers in this branch
                    // should continue on the other side of the domino
                    res += processBranch(sides, used, sides[i].idxSecondSide, size);
                    
                    // for future use
                    used[i] = 0;
                    used[sides[i].idxSecondSide] = 0;
                }
            }
            // optimize for not iterating the whole array
            // not fitting from some index
            else
            {
                break;
            }
        }
    }
    
    return res;
}

uint64_t countTowers ( const char * list )
{
    domino** dm;
    int size = parseDominos(list, &dm);
    
    side* allSides = (side*)calloc(sizeof(side), size*2);
    
    for (int i = 0; i < size; ++i)
    {
        domino * brick = dm[i];
        side s;
        if (brick->valFront1 < brick->valFront2)
        {
            s.min = brick->valFront1;
            s.max = brick->valFront2;
        }
        else
        {
            s.min = brick->valFront2;
            s.max = brick->valFront1;
        }
        // if the brick is not processed yet, set unique(but for both sides of the brick the same) and not valid idx
        s.idxSecondSide = -(i+1);
        
        // insert into the array the front side - even index
        allSides[2 * i] = s;
        
        if (brick->valBack1 < brick->valBack2)
        {
            s.min = brick->valBack1;
            s.max = brick->valBack2;
        }
        else
        {
            s.min = brick->valBack2;
            s.max = brick->valBack1;
        }
        // s remains the same for this brick
        
        // back side is always odd index
        allSides[2 * i + 1] = s;
    }
    
    qsort((void *) allSides, (size_t)(2 * size), (size_t)(sizeof(side)), compareSides);
    
    for (int i = 0; i < 2 * size; ++i)
    {
        // processed
        if (allSides[i].idxSecondSide >= 0)
            continue;
        for (int j = i+1; j < 2 * size; ++j)
        {
            // find a friend
            if (allSides[i].idxSecondSide == allSides[j].idxSecondSide)
            {
                // set second side and break
                allSides[i].idxSecondSide = j;
                allSides[j].idxSecondSide = i;
                break;
            }
        }
    }
    
    // default 0
    uint8_t* used = (uint8_t*)calloc(2 * size, 1);
    
    uint64_t res = 0;
    for (int i = size * 2 - 1; i >= 0; --i)
    {
        // both sides are used
        used[i] = 1;
        used[allSides[i].idxSecondSide] = 1;
        
        // start a branch
        res += processBranch(allSides, used,
                             allSides[i].idxSecondSide, 2 * size);
        
        // for future use - not used for next iterations
        used[i] = 0;
        used[allSides[i].idxSecondSide] = 0;
    }
    // printf("counted: %lld\n", res);
    
    free(used);
    free(allSides);
    destroyDomino(dm, size);
    
    return res;
}

#ifndef __PROGTEST__
int main ( void )
{
  const char * str1 =
    " { 'Progtest Exam' ; [ 1 | 2 ] ; [ 3 | 4 ] }{'PA1 2020/2021';[2|2];[3|1]}\n"
    "{'Progtest Exam' ; [ 2 | 1 ] ; [ 3 | 4 ] }\n"
    "{'Progtest Exam' ; [ 2 | 3 ] ; [ 1 | 4 ] }\n";
  const char * str2 =
    "{'Crash';  [1|2];[3|4]}\n"
    "{'MemLeak';[1|2];[3|4]}\n"
    "{'MemLeak';[2|3];[3|1]}\n"
    "{'MemLeak';[1|3];[3|2]}\n"
    "{'MemLeak';[1|4];[1|5]}\n"
    "{'MemLeak';[4|1];[1|5]}\n"
    "{'MemLeak';[4|1];[5|1]}\n"
    "{'MemLeak';[1|4];[5|1]}\n"
    "{'MemLeak';[1|5];[1|4]}\n"
    "{'MemLeak';[5|1];[1|4]}\n"
    "{'MemLeak';[5|1];[4|1]}\n"
    "{'MemLeak';[1|5];[4|1]}\n";
  const char * str3 =
    "{'-Wall -pedantic -Werror -Wno-long-long -O2';[2|2];[3|3]}\n"
    "{'-Wall -pedantic -Werror -Wno-long-long -O2';[4|4];[5|5]}\n"
    "{'-Wall -pedantic -Werror -Wno-long-long -O2';[3|4];[4|5]}\n";
  const char * str4 =
    "{'-fsanitize=address -g';[1|5];[5|7]}\n"
    "{'-fsanitize=address -g';[3|7];[1|9]}\n"
    "{'-fsanitize=address -g';[2|2];[4|7]}\n"
    "{'-fsanitize=address -g';[3|9];[2|6]}\n"
    "{'-fsanitize=address -g';[2|2];[2|2]}\n";

  assert ( countCollectible ( str1 ) == 0 );
  countCollectible ( str2 );
  countCollectible ( str3 );
  countCollectible ( str4 );
  
  assert ( countUnique ( str1 ) == 3 );
  assert ( countUnique ( str2 ) == 4 );
  assert ( countUnique ( str3 ) == 3 );
  assert ( countUnique ( str4 ) == 5 );
  assert ( countTowers ( str3 ) == 10 );
  assert ( countTowers ( str4 ) == 114 );
  return 0;
}
#endif /* __PROGTEST__ */


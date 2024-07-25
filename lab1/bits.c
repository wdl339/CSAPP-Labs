/* 
 * CS:APP Data Lab 
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#include "btest.h"
#include <limits.h>

/*
 * Instructions to Students:
 *
 * STEP 1: Fill in the following struct with your identifying info.
 */
team_struct team =
{
   /* Team name: Replace with either:
      Your login ID if working as a one person team
      or, ID1+ID2 where ID1 is the login ID of the first team member
      and ID2 is the login ID of the second team member */
    "522031910516", 
   /* Student name 1: Replace with the full name of first team member */
   "WEI Dongliang",
   /* Login ID 1: Replace with the login ID of first team member */
   "522031910516",

   /* The following should only be changed if there are two team members */
   /* Student name 2: Full name of the second team member */
   "",
   /* Login ID 2: Login ID of the second team member */
   ""
};

#if 0
/*
 * STEP 2: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.
#endif

/*
 * STEP 3: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the btest test harness to check that your solutions produce 
 *      the correct answers. Watch out for corner cases around Tmin and Tmax.
 */
/* Copyright (C) 1991-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    /*Use mask to count how many 1's in 8 bits at a time, it needs to count four times.
    Then compress s to 16 bits. To avoid overflow, change method to compress s to 8 bits.
    Use 0x3f to take the last 6 bits of s as result.(Because at most 32 1's.)
    */
    int m = 0x11 | (0x11 << 8);
    int mask = m | (m << 16);
    int s = x & mask;
    s += (x >> 1) & mask;
    s += (x >> 2) & mask;
    s += (x >> 3) & mask;
    s = s + (s >> 16);
    mask = 0xf | (0xf << 8);
    s = (s & mask) + ((s >> 4) & mask);
    return (s + (s >> 8)) & 0x3f;
}
/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) {
    /*Exploit that int is 4 bytes, and the machine performs right shifts arithmetically.
    */
    return (x << 31) >> 31;
}
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 2
 */
int evenBits(void) {
    /*Exploit that 0x5 = 0101, where the 0th and 2rd bits are 1*/
    int m = (0x55 | 0x55 << 8);
    return m | m << 16;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
    /*If x can be represented, x can be truncate to n bit, which means that x << (32 - n) >> (32 - n) is equal to x itself.*/
    int offset = 33 + ~n;
    int y = (x << offset) >> offset;
    return !(y ^ x);
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    /*Use mask = 0xff << (n << 3) to get the byte. The machine performs right shifts arithmetically, so the result should & 0xff.*/
    int move = n << 3;
    int mask = 0xff << move;
    return ((x & mask) >> move) & 0xff;
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
    /*If x > y, y - x < 0. When x < 0, y < 0 or x < 0, y > 0 , overflow may occur. 
    The former should return 0 and the latter should return 1.*/
    int sub = y + ~x + 1;
    return (((sub >> 31) & 0x1) | ((!(x >> 31)) & (y >> 31) & 0x1)) & ((y >> 31) | !((x >> 31) & 0x1));
}
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
    /*if x >= 0, the most significant bit is 0.*/
    return !(x >> 31);
}
/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNotEqual(int x, int y) {
    /*If x == y, y ^ x = 0.
    If x != y, y ^ x != 0.*/
    return !(!(y ^ x));
}
/* 
 * leastBitPos - return a mask that marks the position of the
 *               least significant 1 bit. If x == 0, return 0
 *   Example: leastBitPos(96) = 0x20
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 4 
 */
int leastBitPos(int x) {
    /*The way to turn x into -x : applying operation ~ to all bits before the least significant 1 bit. So mask = x & ~x.*/
    return x & (~x + 1);
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 1 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
    /*Use mask = 00...11... to turn the first n bits of (x >> n) into 0.
    mask = ~((1 << 31) >> (n - 1))*/
    int mask = ~((1 << 31) >> (n + ~1 + 1));
    return (x >> n) & mask;
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          the maximum value, and when negative overflow occurs,
 *          it returns the minimum value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
    /*If x > 0, y > 0, add <= 0, positive overflow occurs.
    If x < 0, y < 0, add >= 0, negative overflow occurs.
    over = 0 if no overflow occurs, and 0xffffffff otherwise. posOver is similar.
    the maximum value is ~(1 << 31) and the minimum value is (1 << 31).*/
    int add = x + y;
    int x_move = x >> 31;
    int over = (x_move ^ (add >> 31)) & ~(x_move ^ (y >> 31));
    int posOver = over ^ x_move;
    return (~over & add) + (over & posOver & ~(1 << 31)) + (over & ~posOver & (1 << 31));
}

/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    /*When x > 0, the minimum number of bits required to represent x in two's complement = 1 + the position number of leftmost 1.
    e.g. 12 -> 0000...01100, represent 12 in two's complement: 01100. howManyBits(12) = 1 + 4 = 5.
    Generate the mask "e" that all bits after leftmost 1 are 1. 
    e.g. for 12, e = 0000...01111.
    To get the position number of leftmost 1, count the number of 1 in e, use the same method in bitCount(x).
    If x < 0, change it into ~x and apply the method above.*/
    int x_move = x >> 31;
    int x_change = (x_move & ~x) + (~x_move & x);
    int a = x_change | (x_change >> 1);
    int b = a | (a >> 2);
    int c = b | (b >> 4);
    int d = c | (c >> 8);
    int e = d | (d >> 16);

    int number_of_1;
    int m = 0x11 | (0x11 << 8);
    int mask = m | (m << 16);
    int s = e & mask;
    s += (e >> 1) & mask;
    s += (e >> 2) & mask;
    s += (e >> 3) & mask;
    s = s + (s >> 16);
    mask = 0xf | (0xf << 8);
    s = (s & mask) + ((s >> 4) & mask);
    number_of_1 = (s + (s >> 8)) & 0x3f;

    return number_of_1 + 1;
}

/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
    /*Only when x = 0 the most significant bit of (x | (~x + 1) is 0.*/
    return ((x | (~x + 1)) >> 31) + 1;
}

/* 
 * dividePower2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: dividePower2(15,1) = 7, dividePower2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int dividePower2(int x, int n) {
    /*For x >= 0, the division should yield x >> n, which is [x / (2 ^ n)] - 1. (rounding down)
    For x < 0, the division should yield (x + (1 << n) - 1) >> n, which is [x / (2 ^ n)]. (rounding up)*/
    int x_move = x >> 31;
    return (x_move & (x + (1 << n) + ~0) >> n) + (~x_move & (x >> n));
}

/* 
 * bang - Convert from two's complement to sign-magnitude 
 *   where the MSB is the sign bit
 *   You can assume that x > TMin
 *   Example: bang(-5) = 0x80000005.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int bang(int x) {
    /*If x < 0, turn it to its absolute value and change its most significant bit to 1.*/
    int x_move = x >> 31;
    int x_positive = (x_move & (~x + 1)) + (~x_move & x);
    int mask = (x_move & (1 << 31));
    return x_positive | mask;
}


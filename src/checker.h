/** themisv2 Project (compiled with MinGW - GNU C++11)
    ---
    Copyright (C) 2017 @quyenjd

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.ved.
    ---
    This header is a part of themisv2 Project.
    It contains themisv2's own stream for carefully checking users' answers.
**/
#ifndef __THEMISV2_CHECKER__
#define __THEMISV2_CHECKER__

#include "themisv2.h"
#include <limits>
#include <stdexcept>

string input, output, answer;

/* Max of an arithmetic type. */
template<typename T>
T limit (const T& a) {
    return numeric_limits<T>::max();
}

/* Safely addition of two numbers. The type of the returning is same as a.
   I only deal with a > 0 && b > 0.
*/
template<typename T, typename _T>
T add (const T& a, const _T& b) {
    T upT = limit(a);
    if (b <= upT - a)
        return a + b;
    throw invalid_argument("Addition is not safe");
    return 0;
}

/* Safely multiplication of two numbers. The type of the returning is same as a.
   I only deal with a > 0 && b > 0.
*/
template<typename T, typename _T>
T mul (T a, _T b) {
    T upT = limit(a);
    if (!a || !b)
        return 0;
    if (b <= upT / a)
        return a * b;
    throw invalid_argument("Multiplication is not safe");
    return 0;
}

/* New override input stream for handling formal output!
   STRICTLY RECOMMENDED using *foo* instead of based stream.

   EXITCODES:
   ----------
   0  -  OK!
   1  -  Invalid type
   2  -  Invalid sp (not in range, too many or too less spaces)
   3  -  Unexpected EOL or EOF

   WARNING: sp must be in range [0, 1000000000].

   Be careful to always remember checking the returning exit code from our functions.
*/
class __themisv2_instream__ {
private:
    /* ifstream bar for reading file, stringstream ss for handling cases and bool _f for
       reminding judger to check exitcodes.
    */
    ifstream bar;
    stringstream ss;
    bool _f;
    string z;
public:
    /* Registration with file name. */
    template<typename T>
    __themisv2_instream__ (const T& fname) {
        bar.open(fname);
        if (!bar.is_open())
            halt(crash, "Instream: Can't open file!");
		if (!getline(bar, z))
			halt(crash, "Instream: Empty file!");
        ss.str(z);
        ss.seekg(0);
        _f = 0;
    }

    /* Make sure everything before reading. */
    void analyze() {
        if (!bar.is_open())
            halt(crash, "Instream: You must register before using my input stream!");
        if (_f)
            halt(crash, "Instream: Be careful to check all exitcodes! There was an non-zero returned exitcode at the previous use of my input stream.");
    }

    /* You have checked the exitcodes and it does not impact the result? Please let me know. */
    void itsOK() {
        _f = 0;
    }

    /* Skip white spaces.
       Putting an int sp will skip exactly a number of spaces.
       Otherwise, it will skip until it successfully found a char that is not space.
    */
    int readspaces (int sp = inf) {
        analyze();
        if (sp != inf && (sp > (int)1e9 || sp < 0))
            return _f = 1, 2;

        // TODO:
        // Skip spaces. If the next character is still a space, it must be an error.

        char x;
        while (sp && ss.peek() == ' ')
            --sp, ss.get(x);
        // sp = inf = 2^63 - 1 so after skipping, it should remain larger than 10^9.
        if (sp && sp < (int)1e9)
            return _f = 1, 2;
        // Unexpected EOF error.
        if (ss.peek() == EOF)
            return _f = 1, 3;
        return 0;
    }

    /* Read one integer from stream.
       Skip exactly a number of spaces after by putting another int, called sp, to the function.
    */
    template<typename T>
    int readint (T& n, int sp = inf) {
        analyze();

        // Skip white spaces.
        int rp = readspaces(sp);
        if (rp)
            return rp;

        // TODO:
        // Read numbers until a space appears.

        char x;
        n = 0;
        bool neg = 0;

        if (ss.peek() == ' ')
            return _f = 1, 2;
        if (ss.peek() == '-')
            neg = 1, ss.get(x);
        while (isdigit(ss.peek())) {
            ss.get(x);
            // Check whether the number can be too big after the multiplication.
            try {
                n = mul(n, 10);
            } catch (const invalid_argument& e) {
                n = 0;
                return _f = 1, 1;
            }
            // Check whether the number can be too big after the addition.
            try {
                n = add(n, x - '0');
            } catch (const invalid_argument& e) {
                n = 0;
                return _f = 1, 1;
            }
        }
        n = n * (neg ? -1 : 1);
        if (ss.peek() == ' ' || ss.peek() == EOF)
            return 0;
        return n = 0, _f = 1, 1;
    }

    /* Skip all remaining characters and jump to new line.
       The function returns 1 iff the line before jumping has unread characters.
       If it reaches end of file, EOF will be returned.
    */
    int readline() {
        analyze();

        // TODO:
        // If present line has remain characters, it must be an error.
        // Otherwise, renew the buffer.

        if (ss.peek() != EOF) {
            if (getline(bar, z)) {
                ss.str(z);
                ss.seekg(0);
            }
            return _f = 1, 1;
        }
        if (!getline(bar, z))
            return _f = 1, EOF;
        ss.str(z);
        ss.seekg(0);
        return 0;
    }

    /* Read next char, including all ASCII chars, except spaces.
       Skip exactly a number of spaces after by putting another int, called sp, to the function.
    */
    int readchar (char& s, int sp = inf) {
        analyze();

        // Skip white spaces.
        int rp = readspaces(sp);
        if (rp)
            return rp;

        // TODO:
        // Just read next char and return an error if it is a space.

        if (ss.peek() == ' ')
            return _f = 1, 2;
        ss.get(s);
        return 0;
    }

    /* Read word (string without spaces).
       Skip exactly a number of spaces after by putting another int, called sp, to the function.
    */
    int readword (string& s, int sp = inf) {
        analyze();

        // Skip white spaces.
        int rp = readspaces(sp);
        if (rp)
            return rp;

        // TODO:
        // Add characters (except spaces) until a space appears.

        char x;
        s = "";

        if (ss.peek() == ' ')
            return _f = 1, 2;
        while (ss.peek() != ' ' && ss.peek() != EOF) {
            ss.get(x);
            s = s + x;
        }
        return 0;
    }

    /* Read string with spaces.
       Skip exactly a number of spaces after by putting another int, called sp, to the function.
    */
    int readstring (string& s, int sp = inf) {
        analyze();

        // Skip white spaces.
        int rp = readspaces(sp);
        if (rp)
            return rp;

        // TODO:
        // Just read all remain character and add it to the main string.

        char x;
        s = "";

        while (ss.peek() != EOF) {
            ss.get(x);
            s = s + x;
        }
        return 0;
    }

    /* Read one double from stream.
       Skip exactly a number of spaces after by putting another int, called sp, to the function.
       CAUTION: It will return error at doubles with exponent of 10 symbols (E or e)!
       Maximum of 18 digits behind dot is allowed. Commas are not allowed!
    */
    int readdouble (double& n, int sp = inf) {
        analyze();

        // Skip white spaces.
        int rp = readspaces(sp);
        if (rp)
            return rp;

        // TODO:
        // Read before dot/comma and save to *n*.
        // Read after dot/comma and save to another *t* then divide *t* by 10^(number of *t*'s digits).
        // Add *t* to *n*.

        char x;
        ll d = 1, t = n = 0;
        bool cont = 0, neg = 0;
        // Another space? Exit Code 2.
        if (ss.peek() == ' ')
            return _f = 1, 2;
        // '-' character? Set negative bool to TRUE.
        if (ss.peek() == '-')
            neg = 1, ss.get(x);
        // Get the integral part.
        while (isdigit(ss.peek())) {
            ss.get(x);
            n = n * 10 + x - 48;
        }
        // No '.'? Ok I will stop here!
        if (ss.peek() == ' ' || ss.peek() == EOF)
            return n = n * (neg ? -1 : 1), 0;
        // '.'? Yes, I will get the fractional part for you. Otherwise, it must be an error.
        if (ss.peek() == '.')
            ss.get(x);
        else
            return n = 0, _f = 1, 1;
        // Get the fractional part.
        while (isdigit(ss.peek())) {
            ss.get(x);
            if (d <= 100000000000000000LL) {
                t = t * 10 + x - 48;
                d = d * 10;
            }
            cont = 1;
        }
        // Have a '.' but no fractional part? It must be an error.
        // Strange character? It must be an error.
        if (   (!cont)
            || (ss.peek() != ' ' && ss.peek() != EOF))
            return n = 0, _f = 1, 1;
        // Add fractional part to integral part.
        n = (n + (double)t / d) * (neg ? -1 : 1);
        return 0;
    }

    /* Skip all remaining characters and jump to new line.
       The function returns 1 iff the line before jumping has unread characters.
       If it does not reach end of file, -1 will be returned.
    */
    int readEOF() {
        // TODO:
        // Call readline() and checks the EOF exitcode.

        int chk = readline();
        if (chk == 1)
            return 1;
        if (chk == EOF)
            return 0;
        return -1;
    }
};

typedef __themisv2_instream__ foo;

/* Usage: checker.exe [input] [output] [expected output]
   Output format:
   - The first line contains a double in range [0, 1], your willing score.
   - The rest of the output is where you put comment (checker log).
*/
void regis_checker (int argc, char* argv[]) {
    if (argc < 4)
        halt(crash, "Checker: Wrong checker usage!");
    input  = argv[1];
    output = argv[2];
    answer = argv[3];
}

#endif // __THEMISV2_CHECKER__

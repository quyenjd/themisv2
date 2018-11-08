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
    It contains some useful stuffs used for themisv2.
**/
#ifndef __THEMISV2_UTILITY__
#define __THEMISV2_UTILITY__

#include "runner.h"

/* Maximum of 10KB source code is allowed! */
const size_t allowed_size = 10240;

/* Create a copy of a text file. Specifically, *b* is a copy of *a*. */
void duplicate (const string& a, const string& b) {
    ifstream p;
    p.open(a);
    if (!p.is_open())
        halt(crash, "Utility header: Cannot open file for reading!");

    ofstream q;
    q.open(b);
    if (!q.is_open())
        halt(crash, "Utility header: Cannot open file for writing!");

    string t;
    while (getline(p, t)) q << t << endl;
    p.close();
    q.close();
}

/* --- Scoring tools begin here --- */

/* Pre-calculate score for all tests. */
vector<double> scoring (const int& max_score, const int& tests) {
    return vector<double> (tests, (double)max_score / tests);
}
/* Pre-calculate score for some tests. Put the value "0" at the index you need. */
template<typename T, typename _T>
vector<double> scoring (const T& max_score, const vector<_T>& strength) {
    int tests = (int)strength.size();
    double sp = 0;
    vector<double> score(tests);
    for (int i = 0; i < tests; ++i)
        sp += strength[i];
    if (sp == 0)
        sp = inf;
    for (int i = 0; i < tests; ++i)
        score[i] = (double)max_score / sp * strength[i];
    return score;
}

/* --- Compilation tools begin here --- */

/* Check the source code. Its size must be in allowed size. */
bool size_limit_exceeded (const string& code) {
    ifstream cppstream;
    cppstream.open(code, ios::binary | ios::ate);
    return cppstream.tellg() > allowed_size;
}

/* I provide you a C++11 MinGW GNU GCC Compiler 4.9.2.
   All Windows libraries and shell commands (system, rename, remove) are not allowed!
   This function returns the destination to the executable file after compiling the code.
   Default compilation config: [g++ -O2 -Wall -std=c++11 -d"THEMISV2"]
*/
string cpp_compile (const string& code, const string& __compilationlog__) {
    string def = rfmt("-O2 -Wall -std=c++11 -D\"THEMISV2\" \"%s.cpp\" -o \"%s.exe\"", code.c_str(), code.c_str());
    proc compiler(".\\src\\C++\\bin\\mingw32-g++.exe", def, inf, inf, "", __compilationlog__, __compilationlog__);
    compiler.run_and_wait();
    if (compiler.exitcode())
        return "-1";
    return code + ".exe";
}

/* I provide you a FPC 3.0.2 compiler.
   All Windows units are not allowed!
   This function returns the destination to the executable file after compiling the code.
   Default compilation config: [fpc -O2 -vewnh -Sm -Sc -Mfpc]
*/
string pas_compile (const string& code, const string& __compilationlog__) {
    string def = rfmt("-O2 -vewnh -Sm -Sc -Mfpc \"%s.pas\"", code.c_str());
    proc compiler(".\\src\\Pascal\\bin\\i386-win32\\fpc.exe", def, inf, inf, "", __compilationlog__, __compilationlog__);
    compiler.run_and_wait();
    if (compiler.exitcode())
        return "-1";
    return code + ".exe";
}

/* This is where you put your code file in.
   This function will auto recognize your language by checking the extension.
   It will return "@@" if your file code's size is larger than my allowed size.
   It will return "!!" if your file code's extension does not match with my supported extensions.
   It will return "-1" if your file code has a compilation error.
*/
string compile (string code, const string& __compilationlog__ = "") {
    // First, I skip the extension part
    int i = (int)code.length() - 1;
    string chk; // Used for checking the extension
    while (code[i] != '.')
        chk += code[i--];
    code = code.substr(0, i);

    // Second, I check your file's extension
    if (chk != "sap" && chk != "ppc")
        return "!!";

    // Finally, I call the compiler
    if (chk == "pas")
        return pas_compile(code, __compilationlog__);
    return cpp_compile(code, __compilationlog__);
}

/* --- Communication tools begin here --- */

/* This function replaces judger's stub with problem solver's code.
   You must put in the destinations of your willing full code, judger's stub and problem solver's code.

   UNDERSTANDING:
   ---
   If you are a judger, make sure to put the line that equals to *fnd* in your stub so that
   I can find the first line that equals to it and replace it with problem solver's code.
   The function returns 1 iff replacement has been done succesfully.
*/
bool fnr (const string& destination,
          const string& stub,
          const string& solver,
          const string& fnd
          ) {
    ofstream des(destination);
    ifstream jud(stub), fco(solver);
    string s, t;
    bool ok = 0;

    // Read stub
    while (getline(jud, s)) {
        size_t i = 0;
        while (i < s.length() && s[i] == ' ' && s[i] == '\t') ++i;
        size_t j = s.length() - 1;
        while (j >= 0 && s[j] == ' ' && s[j] == '\t') --j;
        t = s.substr(i, j - i + 1);
        // Replacement
        if (t == fnd) {
            ok = 1;
            // Read solver's code
            while (getline(fco, s))
                des << s << endl;
        } else
            des << s << endl;
    }

    return ok;
}

#endif // __THEMISV2_UTILITY__

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
    This is the main driver program.
**/
#include "src/utility.h"
#include <set>

// Constants.
string __temp__,             /* Temporary folder. */
       __themisv2_version__, /* themisv2's version. */
       __config__,           /* themisv2's config file. */
       __checkerlog__,       /* Checker's log. */
       __compilationlog__,   /* Compilation log. */
       __scorelog__,         /* Score log (used for saving solver's score). */
       __stub_wscode__,      /* Stub with source code's file name. */
       __stub_fnr__,         /* Stub's replacement sign. */
       __path__;             /* themisv2's running directory. */

const ui __MAX_CHECKERLOG_LINES__ = 3;     /* Max checker's log's number of lines. */
const ui __MAX_PATH_SIZE__        = 1024;  /* Max temporary folder's path's length. */
const ui __MAX_NUMBER_OF_TESTS__  = 500;   /* Max number of tests. */
const double __MAX_SCOREPREC__    = 100.0; /* Max score's precision. */

string         mode,               /* Problem's mode. */
               scoringmode,        /* Scoring mode. */
               solution,           /* Solution's destination. */
               checker,            /* Checker's destination. */
               iomode,             /* Input/Output mode. */
               fixed_input,        /* Fixed input form (used in fixedio scoring mode). */
               fixed_output,       /* Fixed output form (used in fixedio scoring mode). */
               tests,              /* Tests' destination. */
               stub;               /* Stub (used in communication problem mode). */
ui             time_limit,         /* Time limit. */
               mem_limit,          /* Memory limit. */
               num_tests,          /* Number of tests counted by verifytests(). */
               num_subtasks;       /* Number of counted subtasks. */
vector<double> score,              /* Scores of all tests. */
               subs;               /* Scores of all subtasks. */
vector<ui>     tl,                 /* Time limits of all tests. */
               ml,                 /* Memory limits of all tests. */
               chksub;             /* Subtask checker. */
vector<int>    subtask;            /* Subtask of all tests. */
double         max_score;          /* Maximum score. */
bool           subtask_scoring;    /* Use subtask scoring? */

/* Get temp folder. */
void Temp() {
    char s[__MAX_PATH_SIZE__];
    if (!GetTempPath(__MAX_PATH_SIZE__, s))
        halt(crash, "themisv2: Cannot get temporary folder!");

    // Prepare constants.
    __temp__             = string(s) + "themisv2\\";
    __themisv2_version__ = "1.0";
    __config__           = "themisv2.cfg";
    __checkerlog__       = __temp__ + "checkerlog.txt";
    __compilationlog__   = __temp__ + "compilationlog.txt";
    __scorelog__         = __temp__ + "score.txt";
    __stub_wscode__      = __temp__ + "solutionwithstub.";
    __stub_fnr__         = "// Your code goes here";
}

/* Get current directory. */
void GetDir() {
    char s[__MAX_PATH_SIZE__];
    if (!GetModuleFileName(NULL, s, __MAX_PATH_SIZE__))
        halt(crash, "themisv2: Cannot get current directory!");
    vector<string> k = split(s, '\\');
    k.pop_back();
    __path__ = join(k, '\\');
}

/* Read compilation log. */
void readcompilationlog() {
    ifstream ins(__compilationlog__);
    if (!ins.is_open())
        halt(crash, "themisv2: There's a problem with the compilation log!");
    string s;
    while (getline(ins, s))
        tolog(s);
    tolog("---");
    ins.close();
}

/* Count and verify tests. */
void verifytests() {
    if (iomode == "stdio")
        while (num_tests < __MAX_NUMBER_OF_TESTS__) {
            ifstream ins(rfmt("%s\\%s", tests.c_str(), (string("0") * (num_tests < 10) + to_string(num_tests) + ".in").c_str())),
                     ans(rfmt("%s\\%s", tests.c_str(), (string("0") * (num_tests < 10) + to_string(num_tests) + ".ans").c_str()));
            if (!ins.is_open() || !ans.is_open())
                break;
            ++num_tests;
        }
    else
        while (num_tests < __MAX_NUMBER_OF_TESTS__) {
            ifstream ins(rfmt(("%s\\%s\\%s"), tests.c_str(), (string("0") * (num_tests < 10) + to_string(num_tests)).c_str(), fixed_input.c_str())),
                     ans(rfmt(("%s\\%s\\%s"), tests.c_str(), (string("0") * (num_tests < 10) + to_string(num_tests)).c_str(), fixed_output.c_str()));
            if (!ins.is_open() || !ans.is_open())
                break;
            ++num_tests;
        }
}

/* This function will run a test with *id* and call the checker to check it.
   ---
   If stdout = 0,
   - Test's name form: <testid>.in for input and <testid>.ans for output.
   - <testid> starts at 00, eg. 00.in for input and 00.ans for output.
   - While processing, 00.out is solution solver's output.
   Otherwise,
   - Test's name form: you give me fixed_input and fixed_output.
   - The two files must be put in a folder named tests\<testid>.
   ---
   A score for the processed test will be returned.
*/
double runtest (const ui& id, bool _stdio, const int& _sub = -1) {
    ui mem_used = mem_limit, time_used = time_limit;

    tolog(rfmt("\n--- TEST %d%s ---\nCopying ...", id, (rfmt(" (Subtask %d)", _sub) * (_sub >= 0)).c_str()));

    string t = rfmt("%s\\%s", tests.c_str(), (string("0") * (id < 10) + to_string(id)).c_str());

    // Copy test files.
    if (_stdio) {
        duplicate(t + ".in", __temp__ + "a.in");
        duplicate(t + ".ans", __temp__ + "a.ans");
    } else {
        t = t + "\\";
        duplicate(t + fixed_input, __temp__ + fixed_input);
    }

    // Run the process.
    tolog(rfmt("Running ..."));
    ui k;
    if (_stdio) {
        if (mode == "communication") {
            proc a(solution, rfmt("\"%s\" \"%s\"", (__temp__ + "a.in").c_str(), (__temp__ + "a.ans").c_str()), time_limit, mem_limit, __temp__ + "a.in", __temp__ + "a.out");
            k = a.run_and_wait_in_time_limit(mem_used, time_used);
            a.stop();
        } else {
            proc a(solution, "", time_limit, mem_limit, __temp__ + "a.in", __temp__ + "a.out");
            k = a.run_and_wait_in_time_limit(mem_used, time_used);
            a.stop();
        }
    } else {
        if (mode == "communication") {
            proc a(solution, rfmt("\"%s\" \"%s\"", (__temp__ + fixed_input).c_str(), (t + fixed_output).c_str()), time_limit, mem_limit);
            k = a.run_and_wait_in_time_limit(mem_used, time_used);
            a.stop();
        } else {
            proc a(solution, "", time_limit, mem_limit);
            k = a.run_and_wait_in_time_limit(mem_used, time_used);
            a.stop();
        }
    }

    // Some information
    tolog(rfmt("Memory used: %d KB --- Time used: %d ms", min(mem_used, mem_limit), min(time_used, time_limit)));

    // Check some cases.
    tolog("Checking answer ...");
    if (k == inf) {
        tolog("Verdict: Time limit exceeded!");
        return 0;
    }
    if (k == inf * 2) {
        tolog("Verdict: Memory limit exceeded!");
        return 0;
    }
    if (k != 0) {
        tolog(rfmt("Verdict: Runtime error! Process returned exitcode %d.", k));
        return 0;
    }

    // Call the checker.
    if (_stdio) {
        proc a(checker, rfmt("\"%s\" \"%s\" \"%s\"", (__temp__ + "a.in").c_str(), (__temp__ + "a.out").c_str(), (__temp__ + "a.ans").c_str()), inf, inf, "", __checkerlog__, "");
        k = a.run_and_wait();
        a.stop();
    } else {
        proc a(checker, rfmt("\"%s\" \"%s\" \"%s\"", (__temp__ + fixed_input).c_str(), (__temp__ + fixed_output).c_str(), (t + fixed_output).c_str()), inf, inf, "", __checkerlog__, "");
        k = a.run_and_wait();
        a.stop();
    }
    tolog(rfmt("Verdict: %s", k ? "Bad Answer!" : "Accepted!"));

    // Read checker log and score.
    tolog("Checker logs\n---");
    ifstream ins(__checkerlog__);
    string s;
    double p = 0;
    ui lines = 0;
    while (++lines < __MAX_CHECKERLOG_LINES__ && getline(ins, s)) {
        if (lines == 1) {
            stringstream r(s);
            r >> p;
            if (p < 0 || p > 1)
                halt(crash, "themisv2: Given score is not in range [0, 1]");
        } else
            tolog(s);
    }
    ins.close();
    tolog("---");

    return score[id] * p;
}

ui __themisv2_compile__ (bool stub = 0) {
    tolog(rfmt("Processing %s ...", stub ? "stub" : split(solution, '\\').back().c_str()));
    solution = compile(solution, __compilationlog__);
    if (solution == "@@") {
        tolog("Too large solution file!");
        return TBS;
    }
    if (solution == "!!") {
        tolog("Unsupported language!");
        return UKNL;
    }
    if (solution == "-1") {
        tolog("Compilation error!\nLogs\n---");
        readcompilationlog();
        return CE;
    }
    return 0;
}

/* themisv2's config will have the form:
   [1st line]  <problem mode> ("normal", "communication")
   [2nd line]  <scoring mode> ("normal", "ACM")
   [3st line]  <solution's destination>
   [4th line]  <checker's destination>
   [5th line]  <i/o mode> ("stdio", "fixedio")
   [6th line]  <fixed_input> (if <scoring mode> is "fixedio", otherwise leave it blank)
   [7th line]  <fixed_output> (if <scoring mode> is "fixedio", otherwise leave it blank)
   [8th line]  <tests' destination>
   [9th line]  <tests' strength>
   [10th line] <max score>
   [11st line] <time limit>
   [12nd line] <memory limit>
   [13rd line] <stub's destination> (if you are using communication problems, otherwise leave it blank)
   [14th line] <subtask> (if you want to use subtask-scoring)
   [last line] <subtasks' strength> (if you want to use subtask-scoring)
   ---
   <fixed_input> should only contains the input file name, eg. THEMISV2.INP.
   <fixed_output> should only contains the output file name, eg. THEMISV2.OUT.
   <tests' strength> begin with an integer *n* specifying the number of judger's tests. *n* following integers, split by spaces, are tests' strength (must be greater than 1).
   <time limit> is same as <strength> and so are <memory limit> and <subtask>. All the four must have the same first integer *n*.
   <subtasks' strength> starts with an integer *m* specifying the number of subtasks. The rest are same as <tests' strength>.
   All the unneeded lines must be left blank.
   ---
   [ACCEPTED]
   The solution can only be assumed as "passed" a test iff it has the number "1" returned from checker.
   Please note this or you (judger) will mischeck sometimes.
   ---
   [CHECKER'S LOG]
   Your checker's stdout must be printed as follows:
   + The first line contains only ONE real number in the range [0, 1], the percent of score problem solver have for the answer.
   + From the second line to EOF, your comment (this will be given to the problem solver).
   I only give to the problem solver the __MAX_CHECKERLOG_LINES__ first lines of your comment so it should be short!
   ---
   [COMMUNICATION MODE]
   ** Stub and solution must have the same language so you (judger) should prepare stubs for all languages.
   Your stub and checker will have to communicate with each other through your stub's output:
   + In "stdio" mode, your stub's output should be stdout.
   + In "fixedio" mode, your stub's output should be <fixed_output>.
   I will use the last result on checker's stdout to determine score and log.
   ---
   [SUBTASK SCORING]
   If you want to use this additional scoring mode, you must give us a sequence of *n* integers that defines which tests should belong to a subtask.
   Your subtask(s) must be numbered by a sequence that starts at 0 and increase by 1 at each step, eg. 0, 1, 2, 3, ...
   Note that, if you have already set your scoring mode to "ACM", subtask scoring will not work!
*/
ui __themisv2_doall__() {
    ifstream ins(__config__);
    ofstream result(__scorelog__);
    if (!ins.is_open())
        halt(crash, "themisv2: Can't find config file!");

    /** Everything starts here! **/

    // Get Date and Time.
    tolog(rfmt("[%s]", dt().c_str()));

    // Get problem mode.
    if (!getline(ins, mode))
        halt(crash, "themisv2: Unable to get problem mode!");
    if (mode != "normal" && mode != "communication")
        halt(crash, "themisv2: Invalid problem mode!");
    tolog(rfmt("Problem mode: %s", mode.c_str()));

    // Get scoring mode.
    if (!getline(ins, scoringmode))
        halt(crash, "themisv2: Unable to get scoring mode!");
    if (scoringmode != "normal" && scoringmode != "ACM")
        halt(crash, "themisv2: Invalid scoring mode!");
    tolog(rfmt("Scoring mode: %s", scoringmode.c_str()));

    // Get solution's destination.
    if (!getline(ins, solution))
        halt(crash, "themisv2: Unable to get solution's destination!");

    // Copy the solution to temp folder.
    tolog("Preparing solution ...");
    duplicate(solution, __temp__ + split(solution, '\\').back());
    solution = __temp__ + split(solution, '\\').back();

    // Set stub language.
    if (mode == "communication")
        __stub_wscode__ += split(solution, '.').back();

    // Compile the solution.
    if (mode == "normal") {
        tolog("Compiling solution ...");
        ui ret = __themisv2_compile__();
        if (ret)
            return ret;
    }

    // Get checker's destination.
    if (!getline(ins, checker))
        halt(crash, "themisv2: Unable to get checker's destination!");

    // Copy the checker to temp folder.
    tolog("Preparing checker ...");
    duplicate(checker, __temp__ + split(checker, '\\').back());
    checker = __temp__ + split(checker, '\\').back();

    // Compile the checker.
    tolog("Compiling checker ...");
    checker = compile(checker, __compilationlog__);
    if (checker == "@@")
        halt(crash, "themisv2: Checker: Too large checker source code!");
    if (checker == "!!")
        halt(crash, "themisv2: Checker: Unsupported language!");
    if (checker == "-1") {
        tolog("Logs\n---");
        readcompilationlog();
        halt(crash, "themisv2: Checker: Compilation error!");
    }

    // Get scoring mode.
    if (!getline(ins, iomode))
        halt(crash, "themisv2: Unable to get scoring mode!");
    if (iomode != "stdio" && iomode != "fixedio")
        halt(crash, "themisv2: Unsupported scoring mode!");
    tolog(rfmt("I/O mode: %s", iomode.c_str()));

    // Get fixed_input and fixed_output.
    if (!getline(ins, fixed_input))
        if (iomode == "fixedio")
            halt(crash, "themisv2: Unable to get fixed_input!");
    if (!getline(ins, fixed_output))
        if (iomode == "fixedio")
            halt(crash, "themisv2: Unable to get fixed_output!");

    // Get tests' destination.
    if (!getline(ins, tests))
        halt(crash, "themisv2: Unable to get tests' destination!");
    verifytests();
    if (!num_tests)
        halt(crash, "themisv2: No vaild test found!");

    // Get tests' strength.
    ui n;
    string tmp;
    if (!getline(ins, tmp))
        halt(crash, "themisv2: Unable to get tests' strength!");
    if (tmp.empty())
        halt(crash, "themisv2: Unable to get tests' strength!");
    stringstream r(tmp);
    r >> n;

    // Conflict numbers of tests.
    if (n != num_tests)
        halt(crash, "themisv2: Number of verified tests is different from your number of tests!");

    for (size_t i = 0; i < n; ++i) {
        double x;
        if (!(r >> x))
            halt(crash, "themisv2: Invaild number of tests!");
        score.pb(x);
    }

    // Get max score.
    if (!getline(ins, tmp))
        halt(crash, "themisv2: Unable to get max score!");
    if (tmp.empty())
        halt(crash, "themisv2: Unable to get max score!");
    r.str(tmp);
    r.seekg(0);
    r >> max_score;

    // Get time limits.
    tolog("Getting time limits ...");
    if (!getline(ins, tmp))
        halt(crash, "themisv2: Unable to get time limits!");
    if (tmp.empty())
        halt(crash, "themisv2: Unable to get time limits!");
    r.str(tmp);
    r.seekg(0);
    r >> n;

    // Conflict numbers of tests.
    if (n != num_tests)
        halt(crash, "themisv2: Number of verified tests is different from your number of tests!");

    for (size_t i = 0; i < n; ++i) {
        ui x;
        if (!(r >> x))
            halt(crash, "themisv2: Invaild number of tests!");
        tl.pb(x);
    }

    // Get memory limits.
    tolog("Getting memory limits ...");
    if (!getline(ins, tmp))
        halt(crash, "themisv2: Unable to get memory limits!");
    if (tmp.empty())
        halt(crash, "themisv2: Unable to get memory limits!");
    r.str(tmp);
    r.seekg(0);
    r >> n;

    // Conflict numbers of tests.
    if (n != num_tests)
        halt(crash, "themisv2: Number of verified tests is different from your number of tests!");

    for (size_t i = 0; i < n; ++i) {
        ui x;
        if (!(r >> x))
            halt(crash, "themisv2: Invaild number of tests!");
        ml.pb(x);
    }

    // Get stub's destination.
    if (!getline(ins, stub) && mode == "communication")
        halt(crash, "themisv2: Communication problem must have stub!");

    // Copy the stub to temp folder.
    tolog("Preparing stub ...");
    duplicate(stub, __temp__ + split(stub, '\\').back());
    stub = __temp__ + split(stub, '\\').back();

    // Replace and compile.
    if (mode == "communication") {
        if (!fnr(__temp__ + __stub_wscode__, stub, solution, __stub_fnr__))
            halt(crash, "themisv2: Wrong stub's form!");

        // Compile the solution (with stub).
        tolog("Compiling solution (with stub) ...");
        solution = __stub_wscode__;
        ui ret   = __themisv2_compile__();
        if (ret)
            return ret;
    }

    // Get subtask scoring.
    if (scoringmode == "normal") {
        tolog("Getting subtask scoring ...");
        if (!getline(ins, tmp))
            halt(crash, "themisv2: Unable to get subtask scoring!");
        subtask_scoring = !tmp.empty();
        if (subtask_scoring)
            tolog("Additional mode: Subtask-scoring is ON");

        if (subtask_scoring) {
            r.str(tmp);
            r.seekg(0);
            r >> n;

            // Conflict numbers of tests.
            if (n != num_tests)
                halt(crash, "themisv2: Number of verified tests is different from your number of tests!");

            set<int> w;
            for (size_t i = 0; i < n; ++i) {
                ui x;
                if (!(r >> x))
                    halt(crash, "themisv2: Invaild number of tests!");
                chksub.pb(x);
                w.insert(x);
            }

            // Check if set *w* is an increasing sequence 0, 1, 2, ... or not.
            int bg = -1;
            for (set<int>::iterator it = w.begin(); it != w.end(); ++it) {
                if ((*it - bg) != 1)
                    halt(crash, "themisv2: Wrong subtask form!");
                bg = *it;
                subtask.pb(1);
            }
            num_subtasks = w.size();
        }
    }

    // Get subtasks' scores.
    if (subtask_scoring) {
        tolog("Getting subtasks' scores ...");
        if (!getline(ins, tmp))
            halt(crash, "themisv2: Unable to get subtasks' scores!");
        if (tmp.empty())
            halt(crash, "themisv2: Unable to get subtasks' scores!");
        r.str(tmp);
        r.seekg(0);
        r >> n;

        // Conflict numbers of tests.
        if (n != num_subtasks)
            halt(crash, "themisv2: Number of counted subtasks is different from your number of subtasks!");

        for (size_t i = 0; i < n; ++i) {
            ui x;
            if (!(r >> x))
                halt(crash, "themisv2: Invaild number of subtasks!");
            subs.pb(x);
        }
        subs = scoring(max_score, subs);
    }

    // If everything's OK, run all tests.
    bool err = 0; // Error checker
    score = scoring(max_score, score);
    double main_score = 0;
    for (ui i = 0; i < num_tests; ++i) {
        time_limit  = tl[i];
        mem_limit   = ml[i];
        double x    = subtask_scoring ? runtest(i, iomode == "stdio", chksub[i]) : runtest(i, iomode == "stdio");
        main_score += x;

        // Fails one test in "ACM" scoring mode.
        if (x < score[i] && scoringmode == "ACM") {
            err = 1;
            break;
        }

        // Subtask-scoring.
        if (scoringmode == "normal" && subtask_scoring)
            subtask[chksub[i]] &= (x == score[i]);
    }

    // Recalculate score for subtask-scoring.
    if (scoringmode == "normal" && subtask_scoring) {
        main_score = 0;
        for (ui i = 0; i < num_subtasks; ++i)
            main_score += subtask[i] * subs[i];
    }

    // Write logs.
    tolog(rfmt("\n---> Your score: %f/%f", round(main_score * __MAX_SCOREPREC__) / __MAX_SCOREPREC__, max_score));

    // Write scores.
    result << main_score << endl;

    /** Everything stops here! **/

    ins.close();
    result.close();

    return err;
}

/* Check C++ compiler source function. */
bool CheckCPPSource() {
    ofstream out(__temp__ + "test.cpp");
    out << "#include<stdio.h>\nmain(){\n#ifdef __THEMISV2_CPP_COMPILER_SOURCE_MACRO__\njwebqkegbqwbgwqegb9795295g2q29qw\n#endif\n}" << endl;
    out.close();
    string k = cpp_compile(__temp__ + "test", __compilationlog__);
    return k != "-1";
}

/* Check Pascal compiler source function. */
bool CheckPASSource() {
    ofstream out(__temp__ + "test.pas");
    out << "uses dos;begin end." << endl;
    out.close();
    string k = pas_compile(__temp__ + "test", __compilationlog__);
    return k != "-1";
}

/* This function checks whether the program is running as Administrator or not.
   Thanks microsoft.com for the source code.
*/
BOOL IsAdmin() {
    BOOL b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    b = AllocateAndInitializeSid(&NtAuthority,
                                 2,
                                 SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS,
                                 0, 0, 0, 0, 0, 0,
                                 & AdministratorsGroup);
    if (b) {
        if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
            b = FALSE;
        FreeSid(AdministratorsGroup);
    }
    return b;
}

int main (int argc, char* argv[]) {
    // Prepare.
    Temp();
    GetDir();
    system(rfmt("mkdir \"%s\" >nul 2>&1", __temp__.c_str()).c_str());

    // Intro.
    if (!SetConsoleTitle(rfmt("themisv2 %s", __themisv2_version__.c_str()).c_str()))
        halt(crash, "themisv2: Cannot set console title!");

    // About.
    cout << "themisv2 Project by @quyenjd" << endl;
    cout << rfmt("*\\/* Build version: %s *\\/*", __themisv2_version__.c_str()) << endl;
    cout << "----------------------------\n" << endl;

    // Check Administrator's priviledges.
    if (!IsAdmin())
        halt(crash, "themisv2: You MUST run themisv2 with Administrator privileges!");

    // Check C++ compiler source.
    if (CheckCPPSource())
        halt(crash, "themisv2: C++ compiler source is not themisv2's original source!");

    // Check Pascal compiler source.
    if (CheckPASSource())
        halt(crash, "themisv2: Pascal compiler source is not themisv2's original source!");

    /* The program will return 0 iff everything's OK.
       The program will return 1 iff there is an error (WA, TLE, RTE, etc.).
       The program will return 2 iff there is a compilation error (CE).
       The program will return 3 iff problem solver's source code is too big (TBS).
       The program will return 4 iff problem solver's language is not supported (UKNL).
       Otherwise, it returns 5 (crash) whenever there is an unfixable error while running.
    */
    return __themisv2_doall__();
}

# themisv2 (2017)

A re-implemented version of Themis, a program used in scoring competitive programming problems in Vietnam.

## Installation

1. Download the source.
2. Compile `themisv2.cpp` (with `-lpsapi` and `--stack` if needed).

## How to use

```
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
```

Follow those requirements to write the `themisv2.cfg` file then run the program.

For more information, read the code.

## Changelog

Initial release (on Github). The project was abandoned.

## License

This project is licensed under the GNU GPL version 2.0 - see the [LICENSE](LICENSE) file for details.
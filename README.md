# CS-300 Course Planner

A C++ console program I built for SNHU's CS-300 Data Structures and Algorithms course. It loads course information from a text file, lists courses in order, and looks up a course and its prerequisites.

## How it works

The program stores courses in a binary search tree, using the course number as the key. An in-order traversal prints the course list in alphanumeric order.

Before loading a file, it checks for missing course numbers or titles, duplicate courses, and invalid prerequisites. If a new file fails validation, the previously loaded courses stay available.

## Build and run

You need Git and a C++ compiler such as g++ or clang++.

```bash
git clone https://github.com/jcoppola86/CS-300-Course-Planner.git
cd CS-300-Course-Planner
g++ -std=c++11 -Wall -Wextra -pedantic ProjectTwo.cpp -o course_planner
./course_planner
```

On macOS, you can use `clang++` instead of `g++`. On Windows with MinGW, run `course_planner.exe`.

To try the included sample:

1. Enter `1` to load data.
2. Enter `data/sample_courses.txt` as the file name.
3. Enter `2` to print the course list.
4. Enter `3`, then `CS301`, to see a course and its prerequisites.
5. Enter `9` to exit.

Course lookup is case-insensitive, so `cs301` also works.

Example lookup output:

```text
CS301, Software Engineering
Prerequisites:
CS201, Data Structures
CS202, Database Fundamentals
```

## Course data format

Use one course per line, with fields separated by commas:

```text
course number,course title,prerequisite number,prerequisite number
```

Prerequisites are optional, but each prerequisite must also have its own course record in the file. Do not include a header row. Blank lines are ignored.

The included sample contains six fictional courses for demonstrating the program. It is not an official SNHU course list.

## Files

- `ProjectTwo.cpp`: course records, binary search tree, file validation, and menu.
- `Project One.docx`: the course's Project One document.
- `data/sample_courses.txt`: fictional sample data.
- `tests/test_planner.py`: console checks that build and run the program.

## Checks

With Python 3 and a C++ compiler installed, run:

```bash
python3 tests/test_planner.py
```

For clang++, use `CXX=clang++ python3 tests/test_planner.py`.

All six test methods passed. They check sorted output, course lookup, prerequisites, invalid files, keeping existing data after a failed load, menu errors, and clean exits when input ends. The code compiled with the warning flags above without warnings.

## Limitations

The tree is not self-balancing, so insertion and lookup can take longer if course data arrives in sorted order. The file reader handles simple comma-separated fields, not quoted titles containing commas. It checks direct self-prerequisites but does not detect longer prerequisite cycles. The program displays courses and prerequisites; it does not create a full semester plan.

## Course reflection

The reflection below is from the course. Sample data, run instructions, console checks, and the input-ending fix were added during later portfolio updates.

# CS-300

1. What was the problem you were solving in the projects for this course?

The main problem was figuring out the best way to organize, search, and sort all of the course information. In Project One, I compared the different ways I could do it, and in Project Two I actually put it into practice with C++.

2. How did you approach the problem?

I started by looking at what the program needed to do and then compared the different data structures. Before this class, I honestly did not think much about what was happening behind the scenes as long as my code worked. Comparing the different options helped me understand why the way you store data actually matters.

3. How did you overcome any roadblocks?

When something was not working, I found it was a lot easier to go through the program piece by piece instead of trying to find the problem all at once. I also went back to my pseudocode a lot. It was helpful for checking what I meant for the program to do against what my code was actually doing.

4. How has this project expanded your approach to designing software?

I definitely plan things out more now before jumping straight into the code. I used to be more focused on just getting a program to work. Now I think more about how I am going to organize everything first and whether there might be a better or more efficient way to do it.

5. How has this project changed the way you write programs that are maintainable, readable, and adaptable?

I have gotten better about keeping my code organized and making it easier to follow. I try to use names that actually make sense, keep different jobs in their own functions, and not make things more complicated than they need to be. I also think more about whether I would be able to come back to my code later and still understand what I was doing.


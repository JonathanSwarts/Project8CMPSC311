// CMPSC 311, Project 8, emulate "make -n"

/* sketch of a possible solution
 * 1. read the command line
 *    options -h -v -f something
 *    which file should we read?
 *      hakefile, Hakefile, or something
 *      makefile, Makefile, or something
 *    what are the goals?
 *      argv[] elements after the last option
 *      default goal, see step 2
 * 2. read the file
 *    determine the default goal
 *      the first target mentioned in the file
 *    version 1: list of rules, direct from the input
 *      for each rule, 
 *        target-dependency line, file name and line number
 *        list of targets
 *        list of sources
 *        list of recipes
 *    version 2: list of targets, easier to search
 *      for each target,
 *        target-dependency line, file name and line number
 *        list of sources
 *        list of recipes
 * 3. extract appropriate recipes from the goals and the lists
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// ... more later

int  pr8_read(char *file);	// return 1 if successful, 0 if not
void pr8_work(char *goal);	// recursive
// ... more later

// global data, accumulated from reading the file
char *default_goal = NULL;	// first-mentioned target
// ... lists

int main(int argc, char *argv[])
{
  // command-line arguments
  extern char *optarg;
  extern int optind;
  extern int optopt;
  extern int opterr;
  int c;
  // ... more later

  // read the file
  // ... more later

  // for debugging purposes, print the global data
  // ... more later

  // evaluate the goals
  if (optind == argc)
    { pr8_work(default_goal); }
  else
    {
      for (int i = optind; i < argc; i++)
        { pr8_work(argv[i]); }
    }

  return 0;
}

int pr8_read(char *file)
{
  if (file == NULL) return 0;

  printf("file: %s\n", file);

  // ... more later

  return 1;
}

void pr8_work(char *goal)
{
  if (goal == NULL) return;

  printf("goal: %s\n", goal);

  // if goal is a known target,
  //   iterate through goal's list of sources,
  //     call pr8_work() on each source
  //   iterate through goal's list of sources,
  //     compare times of goal and its sources
  //   if necessary, iterate through goal's list of recipes,
  //     print the recipe

  // ... more later

  return;
}


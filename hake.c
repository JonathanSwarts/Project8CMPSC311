/* CMPSC 311, Spring 2013, Project 5 solution
 *
 * Author:   Nick Dyszel, Jon Swarts, Don Heller
 * Email:    nwd5069@psu.edu jbs5144@psu.edu dheller@cse.psu.edu
 *
 * Hake -- a fishy version of Make
 *
 * version 2, 25 Feb. 2013
 *   macro definition, expansion
 *   all required Project 5 features implemented
 * version 1, 25 Feb. 2013
 *   Project 4 solution
 *
 * Command-line Options
 *    -h           print help
 *    -v           verbose mode; enable extra printing; can be repeated
 *    -f file      input filename; default is hakefile or Hakefile
 *    -n           no-op mode; don't actually run any commands; just print them
 */

//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "cmpsc311.h"
#include "names.h"
#include "macro.h"
#include "lister.h"

//------------------------------------------------------------------------------

static int read_file(char *filename, int quiet);
// return 1 if successful, 0 if not
// "success" means the file could be opened for reading, or that we had seen
//    the file before and don't need to read it again
// quiet == 0 enables error messages if the file can't be opened
// quiet == 1 suppresses error messages if the file can't be opened

static void read_lines(char *filename, FILE *fp);
// fp comes from the file (named filename) opened by read_file() using fopen()

int pr8_work(char *goal);	// recursively processes target

// last-modification time of a file

// The appropriate field of struct stat varies according to the OS.
//   Posix, 2001        time_t st_mtime
//   Posix, 2004        time_t st_mtime
//   Posix, 2008        struct timespec st_mtim
//                      macro st_mtime is st_mtim.tv_sec
//   Solaris            time_t st_mtime
//   Linux              time_t st_mtime
//   Mac OS X           struct timespec st_mtimespec
//                      or, st_mtime

// The struct timespec fields are defined in <time.h>.
//   time_t tv_sec;     // seconds
//   long   tv_nsec;    // nanoseconds

// The type time_t (time in seconds) is defined in <time.h> and <sys/types.h>.
// The Posix Standard allows time_t to be an integer, or a real-floating type.
// If time_t isn't an integer, we would be surprised, but the code should
//   still work properly.

// A nonexistent file is treated as extremely old.

static struct timespec mtime(const char *file);

// global data, accumulated from reading the file
char *default_goal = NULL;	  // first-mentioned target
struct pr8_target_list target_list;  // list of all targets

// maximum line length in an input file (buffer size in read_lines)
#define MAXLINE 4096

//------------------------------------------------------------------------------

bool no_op = false;  // for -n option

static void usage(int status)
{
  if (status == EXIT_SUCCESS)
  {
    printf("usage: %s [-h] [-v] [-f file] [-n]\n", prog);
    printf("  -h           print help\n");
    printf("  -v           verbose mode; enable extra printing; can be repeated\n");
    printf("  -f file      input filename; default is hakefile or Hakefile\n");
    printf("  -n           no-op mode; don't actually run any commands; just print them\n");
  }
  else
  {
    fprintf(stderr, "%s: Try '%s -h' for usage information.\n", prog, prog);
  }
  
  exit(status);
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  // for use with getopt(3)
  int ch;
  extern char *optarg;
  extern int optind;
  extern int optopt;
  extern int opterr;
  
  pr8_list_init(&target_list);
  
  // program name as actually used
  prog = argv[0];
  /* In extremely strange situations, argv[0] could be NULL, or point to an
   * empty string.  Let's just ignore that for now.
   */
  
  // exit status
  int status = EXIT_SUCCESS;
  
  // option flags and option-arguments set from the command line
  int f_flag = 0;	// number of -f options supplied
  
  // first pass, everything except -f options (let the -v options accumulate)
  while ((ch = getopt(argc, argv, ":hvf:n")) != -1)
  {
    switch (ch) {
      case 'h':
        usage(EXIT_SUCCESS);
        break;
      case 'v':
        verbose++;
        break;
      case 'f':
        // later
        break;
      case 'n':
        no_op = true;
        break;
      case '?':
        fprintf(stderr, "%s: invalid option '%c'\n", prog, optopt);
        usage(EXIT_FAILURE);
        break;
      case ':':
        fprintf(stderr, "%s: invalid option '%c' (missing argument)\n", prog, optopt);
        usage(EXIT_FAILURE);
        break;
      default:
        usage(EXIT_FAILURE);
        break;
    }
  }
  
  // scan the argv array again, from the beginning
  optind = 1;
  while ((ch = getopt(argc, argv, ":hvf:")) != -1)
  {
    switch (ch) {
      case 'f':
        f_flag++;		// number of -f options supplied
        (void) read_file(optarg, 0);
        break;
      default:
        break;
    }
  }
  
  if (f_flag == 0 && !read_file("hakefile", 1) && !read_file("Hakefile", 1))
  {
    fprintf(stderr, "%s: no input\n", prog);
    usage(EXIT_FAILURE);
  }
  
  // evaluate the goals
  if (optind == argc) pr8_work(default_goal);
  else
  {
    for (int i = optind; i < argc; i++)
    { pr8_work(argv[i]); }
  }
  
  return status;
}

//------------------------------------------------------------------------------

// return 1 if successful, 0 if not
// "success" means the file could be opened for reading, or that we had seen
//    the file before and don't need to read it again
// quiet == 0 enables error messages if the file can't be opened
// quiet == 1 suppresses error messages if the file can't be opened

static int read_file(char *filename, int quiet)
{
  verify(filename != NULL, "null arg filename");
  verify(filename[0] != '\0', "empty arg filename");
  
  if (verbose > 0)
  { fprintf(stderr, "%s: read_file(%s)\n", prog, filename); }
  
  // file names come from -f and include
  static struct list_names * filenames = NULL;
  if (filenames == NULL) filenames = list_names_allocate("filenames");
  // by construction, filenames is now not NULL
  
  if (verbose > 1)
  { list_names_print(filenames); }
  
  // if (filename is on the list already) { return 1 }
  // else { put filename on the list and continue }
  if (list_names_append_if_new(filenames, filename) == 1)
  { return 1; }
  
  if (verbose > 0)
  { list_names_print(filenames); }
  
  if (strcmp(filename, "-") == 0)
  { read_lines("[stdin]", stdin); return 1; }
  
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
  {
    if (quiet == 0)
      fprintf(stderr, "%s: could not open input file %s: %s\n", prog, filename, strerror(errno));
    return 0;
  }
  
  read_lines(filename, fp);
  
  if (fclose(fp) != 0)
  {
    fprintf(stderr, "%s: could not close input file %s: %s\n", prog, filename, strerror(errno));
  }
  
  return 1;
}

//------------------------------------------------------------------------------

static void read_lines(char *filename, FILE *fp)
{
  verify(filename != NULL, "null arg filename");
  verify(filename[0] != '\0', "empty arg filename");
  verify(fp != NULL, "null arg fp");
  
  if (verbose > 0)
  { fprintf(stderr, "%s: read_lines(%s)\n", prog, filename); }
  
  char original[MAXLINE+2];	// from fgets()
  char expanded[MAXLINE+2];	// after macro expansion
  char buffer[MAXLINE+2];	// working copy, safe to modify
  
  char whsp[] = " \t\n\v\f\r";			// whitespace characters
  int line_number = 0;
  int recipe_line_number = 0;
  
  struct pr8_target *cur_target = NULL;
  struct pr8_target *pt;
  int have_target = 0;			// recipes must follow targets
  
  while (fgets(original, MAXLINE, fp) != NULL) {
    // it is possible that the input line was too long, so terminate the string cleanly
    original[MAXLINE] = '\n';
    original[MAXLINE+1] = '\0';
    
    line_number++;
    if (verbose > 0) printf("%s: %s: line %d: %s", prog, filename, line_number, original);
    
    // assume original[] is constructed properly
    // assume expanded[] is large enough
    macro_expand(original, expanded);
    if (verbose > 0) printf("%s: %s: line %d: %s", prog, filename, line_number, expanded);
    
    strcpy(buffer, expanded);			// copy, safe to modify
    
    char *buf = buffer;
    
    while (*buf == ' ') buf++;			// skip past leading spaces (not tabs!)
    
    char *p_hash = strchr(buf, '#');		// a comment starts with #
    if (p_hash != NULL)
    { *p_hash = '\0'; }			// remove the comment
    
    int n = 0;					// remove trailing whitespace
    while (buf[n] != '\0')
    {
      int n1 = strspn(&buf[n], whsp);		// buf[n .. n+n1-1] is whitespace
      int n2 = strcspn(&buf[n + n1], whsp);	// buf[n+n1 .. n+n1+n2-1] is not
      if (n2 == 0) { buf[n] = '\0'; break; }	// remove trailing whitespace
      n += n1 + n2;
    }
    
    if (buf[0] == '\0')				// nothing left?
    { continue; }
    
    char *p_colon = strchr(buf, ':');		// : indicates a target-prerequisite line
    char *p_equal = strchr(buf, '=');		// = indicates a macro definition
    
    if (buffer[0] == '\t')
    {
      recipe_line_number++;
      if (verbose > 0) printf("  diagnosis: recipe line %d\n", recipe_line_number);
      if (have_target != 0)
      {
        list_add_recipe(cur_target, strdup(buffer + 1));
        pt = cur_target;
        for (int i=2; i<=have_target; i++) {
          pt = pt->prev;
          pt->r_head = cur_target->r_head;
        }
      }
      else
      {
        fprintf(stderr, "%s: %s: line %d: recipe but no target\n", prog, \
                filename, line_number);
        continue;
      }
      // (save this for a later project)
    }
    else if (p_colon != NULL)
    {
      recipe_line_number = 0;
      if (verbose > 0) printf("  diagnosis: target-prerequisite\n");
      have_target++;
      
      char *tar_prereq = strdup(buffer);
      char *p_colon_new = strchr(tar_prereq, ':');
      
      cur_target = list_add(&target_list, strtok(tar_prereq, " \t\n\v\f\r:"), \
                            filename, line_number);
      if (default_goal == NULL) default_goal = cur_target->name;
      
      while ((tar_prereq = strtok(NULL, " \t\n\v\f\r:")) != NULL) {
        if (tar_prereq < p_colon_new)  // another target
        {
          list_add(&target_list, tar_prereq, filename, line_number);
          have_target++;
        }
        else  // prerequisites
        { list_add_source(cur_target, tar_prereq); }
      }
      
      pt = cur_target;
      for (int i=2; i<=have_target; i++) {
        pt = pt->prev;
        pt->s_head = cur_target->s_head;
      }
    }
    else if (p_equal != NULL)
    {
      if (verbose > 0) printf("  diagnosis: macro definition\n");
      have_target = 0;
      cur_target = NULL;
      // name = body
      // *p_equal is '='
      char *name_start = buf;
      while (*name_start == ' ' || *name_start == '\t')       // skip past spaces and tabs
      { name_start++; }
      char *name_end = p_equal-1;
      while (*name_end == ' ' || *name_end == '\t')
      { name_end--; }
      name_end++;
      *name_end = '\0';
      char *body_start = p_equal+1;
      while (*body_start == ' ' || *body_start == '\t')
      { body_start++; }
      char *body_end = body_start;
      while (*body_end != '\0')       // end of string
      { body_end++; }
      while (*body_end == ' ' || *body_end == '\t')
      { body_end--; }
      body_end++;
      *body_end = '\0';
      if (verbose > 1) macro_list_print();
      macro_set(name_start, body_start);
      if (verbose > 1) macro_list_print();
    }
    else if (strncmp("include", buf, 7) == 0)
    {
      if (verbose > 0) printf("  diagnosis: include\n");
      have_target = 0;
      cur_target = NULL;
      char *name_start = buf + 7;				// skip past "include"
      while (*name_start == ' ' || *name_start == '\t')	// skip past spaces and tabs
      { name_start++; }
      if (*name_start == '\0')
      {
        // following GNU Make, this is not an error
        if (verbose > 0) fprintf(stderr, "%s: %s: line %d: include but no filename\n", prog, filename, line_number);
        continue;
      }
      else if (*name_start == '\'' || *name_start == '"')		// quoted filename
      {
        // find matching quote, remove it
        char *q = name_start + 1;				// skip past ' or "
        while (*q != *name_start && *q != '\0') q++;	// find end of string or line
        if (*q == '\0')
        {
          fprintf(stderr, "%s: %s: line %d: file name error [%s]\n", prog, filename, line_number, name_start);
          continue;
        }
        name_start++;	// skip past opening quote
        *q = '\0';		// remove closing quote
      }
      read_file(name_start, 0);
    }
    else
    {
      if (verbose > 0) printf("  diagnosis: something else\n");
      have_target = 0;
      cur_target = NULL;
      fprintf(stderr, "%s: %s: line %d: not recognized: %s", prog, filename, line_number, original);
    }
  }
  
  if (ferror(fp))	// error when reading the file
  { fprintf(stderr, "%s: %s: read error: %s\n", prog, filename, strerror(errno)); }
  
  return;
}

int pr8_work(char *goal)
{
  if (goal == NULL)
  { fprintf(stderr, "%s: no goal specified\n", prog); return 0; }
  
  // if goal is a known target,
  //   iterate through goal's list of sources,
  //     call pr8_work() on each source
  //   iterate through goal's list of sources,
  //     compare times of goal and its sources
  //   if necessary, iterate through goal's list of recipes,
  //     print the recipe
  struct pr8_target *target = list_search(&target_list, goal);
  if (target == NULL) return 1;
  
  printf("goal: %s\n", goal);
  
  struct pr8_source *source = target->s_head;
  while (source != NULL)
  {
    if (pr8_work(source->string) == 0) return 0;
    source = source->next;
  }
  
  bool update = false;
  struct timespec t1 = mtime(goal);   // target
  struct timespec t2;                 // source
  
  source = target->s_head;
  while (source != NULL) {
    t2 = mtime(source->string);
    if (t2.tv_sec == 0)
    {
      fprintf(stderr, "%s: no rule to make target '%s', needed by '%s'\n", \
              prog, source->string, goal);
      return 0;
    }
    if (t1.tv_sec == 0                    // target does not exist
        || t1.tv_sec < t2.tv_sec          // target is older than source
        || ((t1.tv_sec == t2.tv_sec) && (t1.tv_nsec < t2.tv_nsec)))
    { update = true; break; }
    source = source->next;
  }
  
  if (update)
  {
    FILE *f = fopen(goal, "a+");
    if (no_op) pr8_list_print_recipe(target);
    fclose(f);
  }
  else
  {
    printf("%s: target '%s' up-to-date\n", prog, goal);
  }

  return 1;
}

// last-modification time of a file
static struct timespec mtime(const char *file)
{
  struct stat s;
  struct timespec t = { 0, 0 };
  
  if (stat(file, &s) == 0)
#if   defined(MTIME) && MTIME == 1      // Linux
  { t = s.st_mtim; }
#elif defined(MTIME) && MTIME == 2      // Mac OS X
  { t = s.st_mtimespec; }
#elif defined(MTIME) && MTIME == 3      // Mac OS X, with some additional settings
  { t.tv_sec = s.st_mtime; t.tv_nsec = s.st_mtimensec; }
#else                                   // Solaris
  { t.tv_sec = s.st_mtime; }
#endif
  
  return t;
}


//------------------------------------------------------------------------------


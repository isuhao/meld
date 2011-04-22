#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <sys/stat.h>
#include <iostream>

#include "config.h"

#include "process/machine.hpp"
#include "runtime/list.hpp"
#include "utils/utils.hpp"
#include "process/router.hpp"

using namespace utils;
using namespace process;

static size_t num_threads = 0;
static char *program = NULL;
static char *progname = NULL;
static bool show_database = false;

static void
help(void)
{
  fprintf(stderr, "meld: execute meld program\n");
  fprintf(stderr, "\t-f <name>:\tmeld program\n");
  fprintf(stderr, "\t-t <threads>:\tnumber of threads\n");
  fprintf(stderr, "\t-s shows database\n");

  exit(EXIT_SUCCESS);
}

static void
read_arguments(int argc, char **argv)
{
   progname = *argv++;
   --argc;

   while (argc > 0 && (argv[0][0] == '-')) {
      switch(argv[0][1]) {
         case 'f': {
            if (program != NULL || argc < 2)
               help();

            program = argv[1];

            argc--; argv++;
         }
         break;
         case 't': {
            if (num_threads > 0 || argc < 2)
               help();

            num_threads = (size_t)atoi(argv[1]);
            argc--; argv++;
         }
         break;
         case 's':
            show_database = true;
            break;
         default:
            help();
      }

      /* advance */
      argc--; argv++;
   }
}

int
main(int argc, char **argv)
{
   read_arguments(argc, argv);

   if (program == NULL)
      help();

   if (num_threads == 0)
      num_threads = number_cpus();

   router rout(num_threads, argc, argv);

   machine mac(program, rout, num_threads);

   if(show_database)
      mac.show_database();
      
   mac.start();

   return EXIT_SUCCESS;
}

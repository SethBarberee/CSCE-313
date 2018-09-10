#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "Ackerman.h"
#include "BuddyAllocator.h"

int main(int argc, char ** argv) {
    int bvalue = 128;
    int svalue = 512 * 1024;
    int c;

    opterr = 0;

    while (1)
    {
      static struct option long_options[] =
        {
           /*  We distinguish them by their indices. */
          {"blocksize",  required_argument, 0, 'b'},
          {"memsize",    required_argument, 0, 's'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "b:s:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'b':
          bvalue = atoi(optarg);
          break;

        case 's':
          svalue = atoi(optarg);
          break;

        case '?':
          break;

        default:
          abort ();
        }
    }
    if (optind < argc){
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
    }

    // defaults are below... override them with the command-line args
    // TODO can svalue be smaller than bvalue??
    if(bvalue > svalue){ // check to make sure we have enough memory
        fprintf(stderr,"Not enough memory\n");
        return 1;
    }
    

    // create memory manager
    BuddyAllocator * allocator = new BuddyAllocator(bvalue, svalue);

    // test memory manager
    Ackerman* am = new Ackerman ();
    am->test(allocator); // this is the full-fledged test.
  
    // destroy memory manager
    delete allocator;
}

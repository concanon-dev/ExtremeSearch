/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
   char inbuf[4096];
   int state = 0;

   if (argc != 3)
   {
       fprintf(stderr, "Usage %s inputFile outputFile\n", basename(argv[0]));
       exit(0);
   }

   FILE *in = fopen(argv[1], "r");
   FILE *out = fopen(argv[2], "w");

   if (in != NULL)
       fprintf(stderr, "Reading file %s\n", argv[1]);
   else
   {
       fprintf(stderr, "Can't read file %s\n", argv[1]);
       exit(0);
   }

   if (out != NULL)
       fprintf(stderr, "Writing file %s\n", argv[2]);
   else
   {
       fprintf(stderr, "Can't write file %s\n", argv[2]);
       exit(0);
   }

   while(!feof(in))
   {
       fgets(inbuf, 4096, in);
       if (state == 0)
       {
           if (!strncmp(inbuf, "def outputInfo", strlen("def outputInfo")))
           {
               state = 1;
               fprintf(stderr, "Found 'def outputInfo'\n");
           }
       }
       else if (state == 1)
       {
           if (!strncmp(inbuf, "    outputResults(", strlen("    outputResults(")))
           {
               state = 2;
               fprintf(stderr, "Found '    outputResults'\n");
           }
       }
       else if (state == 2)
       {
           if (!strncmp(inbuf, "    sys.exit()", strlen("    sys.exit()")))
           {
               state = 3;
               fprintf(stderr, "Found '    sys.exit()'\n");
           }
       }
       if (state == 3)
       {
           fprintf(stderr, "Added 2 lines\n");
           fputs("    sys.stdout.flush()\n", stderr);
           fputs("    sys.stdout.flush()\n", out);
           fputs("    time.sleep(0.5)\n", stderr);
           fputs("    time.sleep(0.5)\n", out);
           state = 4;
       }
       fputs(inbuf, out);
   }
   if (state != 4)
       fprintf(stderr, "Modification failed! state=%d\n", state);
   else
       fprintf(stderr, "Modification Succeeded!\n");
}


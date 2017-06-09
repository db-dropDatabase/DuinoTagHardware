#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

void generatePrint(const FILE *printer, const char *regName, const char *printWords, short recurseLevel, const short startVal){
    recurseLevel--;
    if(recurseLevel >= 0){
            fprintf(printer, "#ifdef %s%i\n", regName, recurseLevel);
            generatePrint(printer, regName, printWords, recurseLevel, startVal + pow(2, recurseLevel));
            fprintf(printer, "#else\n");
            generatePrint(printer, regName, printWords, recurseLevel, startVal);
            fprintf(printer, "#endif\n");
    }
    else{
        fprintf(printer, printWords, startVal);
        fprintf(printer, "\n");
    }

}


int main(int argc, char *argv[])
{
    //step one: parse arguments
    char *bitNames = "rega";
    char *printStatment = "#pragma message(\"%i\")";
    short recursionDepth = 8;
    opterr = 0;
    int c;

    while ((c = getopt (argc, argv, "r:n:p:")) != -1)
        switch (c)
            {
            case 'r':
                recursionDepth = (int)strtold(optarg, NULL);
                break;
            case 'n':
                bitNames = optarg;
                break;
            case 'p':
                printStatment = optarg;
                break;
            case '?':
                fprintf(stderr, "Usage: [-r recusionDepth (int)] [-n bitName (string)] [-p printStatement (string)] file.txt\n");
                if (optopt == 'r' || optopt == 'n' || optopt == 'p')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                       "Unknown option character `\\x%x'.\n",
                       optopt);
                return 1;
            default:
                return 1;
          }
    //step 2: open file
    if(optind >= argc) {
        fprintf(stderr, "A filename is required\n");
        return -1;
    }
    FILE *f = fopen(argv[optind], "w");
    if(f == NULL){
        fprintf(stderr, "Error opening file\n");
        return -1;
    }
    printf("Using depth of %i\n", recursionDepth);
    printf("Using bit name %s\n", bitNames);
    printf("Using print statment %s\n", printStatment);
    printf("Using filename %s\n", argv[optind]);

    generatePrint(f, bitNames, printStatment, recursionDepth, 0);

    return 0;
}

## FineRoots

Produce Lignum xml files from WinRhizo data.

First, make sure you have both *lignum-core* and *FineRoots*
at the same directory level side by side. 

To compile extract_linkinformation.cc just type

+ c++  extract_linkinformation.cc -o extract

To compile `mfr` (i.e. make fine roots) type

+  qmake -r MakeFineRoots.pro
+  make

To compile `mfr` with optimization on (faster, no debug) type:

  qmake -r "CONFIG+=release" MakeFineRoots.pro
  make

To remove all compilation work type

  make distclean

## Using FineRoots

First, run `extract` without any command line options. It will read the file *files.txt* and
produce root link information (connections) to standard output and to file *kaikki.dat*. The file
*files.txt* contains WinRhizo file names to be processed.

Then run `mfr` to produce Lignum XML files from *kaikki.dat*:
+ mfr kaikki.dat -writeXML

Type `mfr` on command line to see usage information. 





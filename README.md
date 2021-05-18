## FineRoots
To compile extract_linkinformation.cc just type

+ c++  extract_linkinformation.cc -o extract

To compile `mfr` (i.e. make fine roots) type

+  qmake -r MakeFineRoots.pro
+  make

To compile `mfr` with optimization on 
(faster, no debug) type

  qmake -r "CONFIG+=release" MakeFineRoots.pro
  make

To remove all compilation work type

  make distclean

## Using FineRoots

First, run `extract`. It will read the file *files.txt* and
produce link information to standard output and to file *kaikki.dat*.

Run `mfr`, to produce Lignum XML files:
+ mfr kaikki.dat -writeXML

Type `mfr` only to see usage information. 





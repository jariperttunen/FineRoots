######################################################################
# Automatically generated by qmake (2.00a) Thu Aug 17 15:33:51 2006
######################################################################
CONFIG -= app_bundle
CONFIG += qt
QT += xml
TEMPLATE = app
TARGET = mfr
INCLUDEPATH += . include /opt/local/include ../c++adt/include ../stl-lignum/include ../Firmament/include \
 ../stl-voxelspace/include  ../Pine ../XMLTree 
#CGAL requires libraries including and after -CGAL
#You may also need to update LD_LIBRARY_PATH on Linux to find CGAL libraries runtime.
#Usually the path "/usr/local/lib" is the right one to add in your "<homedirectory>/.profile" file:
#export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
INCLUDEPATH += ../LEngine/include
DEPENDPATH += $$INCLUDEPATH
LIBS += -L/opt/local/lib -L../c++adt/lib -L../stl-lignum/lib -L../Firmament/lib -L../LEngine/lib -L../stl-voxelspace/lib -lsky -lL \
 -lvoxel -lLGM  -lcxxadt -lCGAL -lCGAL_Core -lgmp -lmpfr -lboost_thread-mt -lboost_system-mt 
     
macx:LIBS +=  -L../Graphics -lVisual -F/usr/local/Trolltech/Qt-4.1.4/lib -framework GLUT -framework OpenGL
win32:CONFIG += console
#CGAL requires -frounding-math on Linux, not necessary on Mac
QMAKE_CXXFLAGS += -frounding-math 

HEADERS += include/MakeFineRoots.h

SOURCES += make_fineroots.cc

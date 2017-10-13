TEMPLATE        = app
TARGET          = load_textured_OBJ
QT             += core opengl xml gui
CONFIG         += c++11
CONFIG         -= app_bundle
INCLUDEPATH    += /usr/local/include
INCLUDEPATH    += /Users/cino/Documents/research/devel/lib/CinoLib/include
DEPENDPATH     += /Users/cino/Documents/research/devel/lib/CinoLib/include # force recompilation if cinolib changes
DEFINES        += CINOLIB_PROFILER_ENABLED


# ------------------------------------ #
# --------- WARNINGS CONTROL --------- #
# ------------------------------------ #

QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override # shows up in vtk7...
QMAKE_CXXFLAGS += -Wno-deprecated-declarations       # gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9
QMAKE_CXXFLAGS += -Wno-deprecated-register           # GraphCut uses "register" variables (they are deprecated in C++1z)


# ------------------------------------ #
# ------- EXTERNAL DEPENDENCIES ------ #
# ------------------------------------ #

# enable a OpenGL, QT and QGLViewer based GUI (used in cinolib/gui/qt/glcanvas.cpp)
DEFINES      += CINOLIB_USES_OPENGL
DEFINES      += CINOLIB_USES_QT_AND_QGLVIEWER
INCLUDEPATH  += /Library/Frameworks/QGLViewer.framework/Versions/2/Headers
QMAKE_LFLAGS *= -F/Library/Frameworks
LIBS         += -framework QGLViewer

## enable VTK readers (used in cinolib/io/read_write.h -- .vtu and .vtk file formats)
#DEFINES      += CINOLIB_USES_VTK
#INCLUDEPATH  += /usr/local/include/vtk-7.1
#QMAKE_LFLAGS *= -L/usr/local/lib
#LIBS         += -lvtkCommonCore-7.1 -lvtkCommonDataModel-7.1 -lvtkIOCore-7.1 -lvtkIOLegacy-7.1 -lvtkIOXML-7.1

## enable Tetgen (used in cinolib/tetgen_wrap.cpp)
#DEFINES += CINOLIB_USES_TETGEN
#DEFINES += TETLIBRARY # this is to link tetgen in "library mode"
#LIBS    += -ltet

## enable Triangle (used in cinolib/triangle_wrap.cpp)
##
#DEFINES += CINOLIB_USES_TRIANGLE
#LIBS    += -ltriangle


# ------------------------------------ #
# -------------- SOURCES ------------- #
# ------------------------------------ #

SOURCES += main.cpp

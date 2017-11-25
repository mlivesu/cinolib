TEMPLATE        = app
TARGET          = polyharmonic_functions_vol
QT             += core opengl xml
CONFIG         += c++11 release
CONFIG         -= app_bundle
INCLUDEPATH    += /usr/local/include
INCLUDEPATH    += $$PWD/../../include
DEPENDPATH     += $$PWD/../../include

DATA_PATH = \\\"$$PWD/../data/\\\"
DEFINES  += DATA_PATH=$$DATA_PATH

# ------------------------------------ #
# --------- WARNINGS CONTROL --------- #
# ------------------------------------ #

QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override # shows up in vtk7...
QMAKE_CXXFLAGS += -Wno-deprecated-declarations       # gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9

# ------------------------------------ #
# ------- EXTERNAL DEPENDENCIES ------ #
# ------------------------------------ #

# enable a OpenGL, QT and QGLViewer based GUI (used in cinolib/gui/qt/glcanvas.cpp)
DEFINES      += CINOLIB_USES_OPENGL
DEFINES      += CINOLIB_USES_QT_AND_QGLVIEWER
INCLUDEPATH  += /Library/Frameworks/QGLViewer.framework/Versions/2/Headers
QMAKE_LFLAGS *= -F/Library/Frameworks
LIBS         += -framework QGLViewer

# enable VTK readers (used in cinolib/io/read_write.h -- .vtu and .vtk file formats)
DEFINES      += CINOLIB_USES_VTK
INCLUDEPATH  += /usr/local/include/vtk-7.1
QMAKE_LFLAGS *= -L/usr/local/lib
LIBS         += -lvtkCommonCore-7.1 -lvtkCommonDataModel-7.1 -lvtkIOCore-7.1 -lvtkIOLegacy-7.1 -lvtkIOXML-7.1

# ------------------------------------ #
# -------------- SOURCES ------------- #
# ------------------------------------ #

SOURCES += main.cpp

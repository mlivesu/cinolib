TEMPLATE        = app
TARGET          = harmonic_map
QT             += core opengl xml
CONFIG         += c++11 release
CONFIG         -= app_bundle
INCLUDEPATH    += /usr/local/include
INCLUDEPATH    += /usr/local/include/eigen3
INCLUDEPATH    += $$PWD/../../include
DEPENDPATH     += $$PWD/../../include

DATA_PATH = \\\"$$PWD/../data/\\\"
DEFINES  += DATA_PATH=$$DATA_PATH

# ------------------------------------ #
# --------- WARNINGS CONTROL --------- #
# ------------------------------------ #

QMAKE_CXXFLAGS += -Wno-deprecated-declarations       # gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9

# ------------------------------------ #
# ------- EXTERNAL DEPENDENCIES ------ #
# ------------------------------------ #

# enable a OpenGL, QT and QGLViewer based GUI (used in cinolib/gui/qt/glcanvas.cpp)
# see http://libqglviewer.com/compilation.html for details
        DEFINES += CINOLIB_USES_OPENGL
        DEFINES += CINOLIB_USES_QT_AND_QGLVIEWER
    INCLUDEPATH += /Library/Frameworks/QGLViewer.framework/Versions/2/Headers
      macx:LIBS += -F/Library/Frameworks -framework QGLViewer
unix:!macx:LIBS += -L. -lQGLViewer
     win32:LIBS += -L. -lQGLViewer2

# ------------------------------------ #
# -------------- SOURCES ------------- #
# ------------------------------------ #

SOURCES += main.cpp

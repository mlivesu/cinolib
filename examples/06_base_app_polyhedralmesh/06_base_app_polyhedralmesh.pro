TEMPLATE        = app
TARGET          = base_app_polyhedralmesh
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

# ------------------------------------ #
# -------------- SOURCES ------------- #
# ------------------------------------ #

SOURCES += main.cpp

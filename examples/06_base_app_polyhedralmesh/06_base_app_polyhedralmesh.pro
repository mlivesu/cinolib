TEMPLATE        = app
TARGET          = base_app_polyhedralmesh
QT             += core opengl
CONFIG         += c++11 release
CONFIG         -= app_bundle
INCLUDEPATH    += $$PWD/../../external/eigen
INCLUDEPATH    += $$PWD/../../include
DEFINES        += CINOLIB_USES_OPENGL
DEFINES        += CINOLIB_USES_QT
QMAKE_CXXFLAGS += -Wno-deprecated-declarations # gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9
DATA_PATH       = \\\"$$PWD/../data/\\\"
DEFINES        += DATA_PATH=$$DATA_PATH
SOURCES        += main.cpp

# just for Linux
unix:!macx {
DEFINES += GL_GLEXT_PROTOTYPES
LIBS    += -lGLU
}

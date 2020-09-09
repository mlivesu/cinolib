TEMPLATE        = app
TARGET          = $$PWD/../02_base_app_quadmesh_demo
QT             += core opengl
#latest C++ stable standard:
CONFIG         += C++17 release 
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

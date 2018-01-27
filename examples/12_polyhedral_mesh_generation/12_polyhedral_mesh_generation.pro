TEMPLATE        = app
TARGET          = polyhedral_mesh_generation
QT             += core opengl
CONFIG         += c++11 release
CONFIG         -= app_bundle
INCLUDEPATH    += /usr/local/include
INCLUDEPATH    += /usr/local/include/eigen3
INCLUDEPATH    += $$PWD/../../include
DEPENDPATH     += $$PWD/../../include
DEFINES        += CINOLIB_USES_OPENGL
DEFINES        += CINOLIB_USES_QT
QMAKE_CXXFLAGS += -Wno-deprecated-declarations # gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9

DATA_PATH = \\\"$$PWD/../data/\\\"
DEFINES  += DATA_PATH=$$DATA_PATH

# ------------------------------------ #
# ------- EXTERNAL DEPENDENCIES ------ #
# ------------------------------------ #
# enable Tetgen
DEFINES += CINOLIB_USES_TETGEN
DEFINES += TETLIBRARY # this is to link tetgen in "library mode"
LIBS    += -ltet

# ------------------------------------ #
# -------------- SOURCES ------------- #
# ------------------------------------ #
SOURCES += main.cpp

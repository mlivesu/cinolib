TEMPLATE        = app
TARGET          = base_app_hexmesh
QT             += core opengl
CONFIG         += c++11 release
CONFIG         -= app_bundle
INCLUDEPATH    += $$PWD/../../external/eigen
INCLUDEPATH    += $$PWD/../../include
DEFINES        += CINOLIB_USES_OPENGL
DEFINES        += CINOLIB_USES_QT
QMAKE_CXXFLAGS += -Wno-deprecated-declarations # gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9
QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override # shows up in vtk7...

DATA_PATH = \\\"$$PWD/../data/\\\"
DEFINES  += DATA_PATH=$$DATA_PATH

# ------------------------------------ #
# ------- EXTERNAL DEPENDENCIES ------ #
# ------------------------------------ #
# enable VTK readers (used in cinolib/io/read_write.h -- .vtu and .vtk file formats)
DEFINES      += CINOLIB_USES_VTK
INCLUDEPATH  += /usr/local/include/vtk-7.1
QMAKE_LFLAGS *= -L/usr/local/lib
LIBS         += -lvtkCommonCore-7.1 -lvtkCommonDataModel-7.1 -lvtkIOCore-7.1 -lvtkIOLegacy-7.1 -lvtkIOXML-7.1

# ------------------------------------ #
# -------------- SOURCES ------------- #
# ------------------------------------ #
SOURCES += main.cpp

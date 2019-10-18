TEMPLATE        = app
TARGET          = $$PWD/../26_export_surface_demo
QT             += core
CONFIG         += c++11 release
CONFIG         -= app_bundle
INCLUDEPATH    += $$PWD/../../external/eigen
INCLUDEPATH    += $$PWD/../../include
DATA_PATH       = \\\"$$PWD/../data/\\\"
DEFINES        += DATA_PATH=$$DATA_PATH
SOURCES        += main.cpp

# ------------------------------------ #
# ------- EXTERNAL DEPENDENCIES ------ #
# ------------------------------------ #
# Enable VTK readers (used in cinolib/io/read_write.h -- .vtu and .vtk file formats)
# NOTE: this is an OPTIONAL dependency. If you are not interested in reading VTK
# files, just comment the lines below (with #)
DEFINES        += CINOLIB_USES_VTK
INCLUDEPATH    += /usr/local/include/vtk-7.1
QMAKE_LFLAGS   *= -L/usr/local/lib
LIBS           += -lvtkCommonCore-7.1 -lvtkCommonDataModel-7.1 -lvtkIOCore-7.1 -lvtkIOLegacy-7.1 -lvtkIOXML-7.1
QMAKE_CXXFLAGS += -Wno-inconsistent-missing-override # shows up in vtk7...

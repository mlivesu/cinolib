TEMPLATE        = app
TARGET          = tetrahedralize
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
# enable Tetgen
DEFINES     += CINOLIB_USES_TETGEN
DEFINES     += TETLIBRARY # this is to link tetgen in "library mode"
INCLUDEPATH += /usr/local/include
LIBS        += -L/usr/local/lib -ltet

######################################################################
# Automatically generated by qmake (2.01a) Wed May 16 15:42:02 2007
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += /usr/include/dcmtk

DEFINES += HAVE_CONFIG_H

LIBS += -lpthread -lz -ldcmdata -ldcmimgle -ldcmimage -ltiff -ldcmdata -lofstd

# Input
HEADERS += DcmPixmap.h \
	   findStringLineEdit.h
SOURCES += DcmPixmap.cpp \
	   dcm_fastview_main.cpp \
	   findStringLineEdit.cpp

TARGET = dcm_fastview
	   
QT +=  widgets
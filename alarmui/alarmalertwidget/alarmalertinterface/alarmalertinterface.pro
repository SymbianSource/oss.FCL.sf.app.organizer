######################################################################
# Automatically generated by qmake (2.01a) Thu Dec 3 15:16:30 2009
######################################################################

TEMPLATE = lib
TARGET = alarmalertwidget

INCLUDEPATH  += ./inc \
				../alarmalertplugin/inc \
				../../inc \
				./traces

DEPENDPATH += ./inc \
			  ./src \
			  /epoc32/include

CONFIG += hb \
		  

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
}

# Input
HEADERS +=  alarmalertwrapper.h \
			alarmalertwidget.h
			
SOURCES +=  alarmalertwrapper.cpp \
			alarmalertwidget.cpp

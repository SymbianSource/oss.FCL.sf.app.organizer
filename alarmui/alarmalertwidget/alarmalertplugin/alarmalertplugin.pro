######################################################################
# Automatically generated by qmake (2.01a) Thu Dec 3 15:16:30 2009
######################################################################

TEMPLATE = lib
TARGET = alarmalertplugin

DEPENDPATH += ./inc \
			   ./src \
			   ./data

CONFIG += hb \
		  plugin

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL

    pluginstub.sources = alarmalertplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub
}

# Input
HEADERS +=  alarmalertplugin.h \
			alarmalertwidget_p.h \
			alarmalertdocloader.h
			
SOURCES +=  alarmalertplugin.cpp \
			alarmalertwidget_p.cpp \
			alarmalertdocloader.cpp

RESOURCES += alarmalert.qrc

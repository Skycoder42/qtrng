HEADERS += \
	$$PWD/qrng.h

SOURCES += \
	$$PWD/qrng.cpp

win32:LIBS += -ladvapi32

INCLUDEPATH += $$PWD

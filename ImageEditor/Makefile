CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		ImageEditor.o

LIBS =

TARGET =	ImageEditor.exe

CPP=		ImageEditor.cpp

$(TARGET):	$(OBJS)
	g++ -g -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
  
$(OBJS): $(CPP)
	g++ -c -g $(CPP)
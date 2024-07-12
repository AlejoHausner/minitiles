CC            = gcc
CCC	      = g++

CS770DIR      =  $(HOME)
INCDIR        =  $(CS770DIR)/include
LIBDIR        =  $(CS770DIR)/lib
CPPFLAGS      = -I$(INCDIR) -I/usr/X11R6/include
LDFLAGS	      = -L$(LIBDIR)

GLLIBS=-lglui -lglut32 -lglu32 -lopengl32 # Cygwin or Mingw
XLIBS         = -L/usr/X11R6/lib -lXmu -lXext -lX11 -lXi

LIBS	      = $(GLLIBS) $(XLIBS)

LINKER	      = g++

MAKEFILE      = Makefile

PROGRAMS = minitiles

RANLIB        = ranlib

CPPSRCS = \
	main.cpp Canvas.cpp Colour.cpp ColourSet.cpp Image.cpp Point2.cpp \
 Tiling.cpp ui.cpp

OBJS = $(CPPSRCS:.cpp=.o) $(CSRCS:.c=.o)

all:		$(PROGRAMS)

%.o : %.cpp %.h
	$(CCC) -c $(CCCFLAGS) $(CPPFLAGS) $*.cpp

%.o : %.c %.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $*.c

minitiles:	$(OBJS)
	$(LINKER) $(LDFLAGS) -o minitiles $(OBJS) $(LIBS)

clean:
	rm -f minitiles $(OBJS)

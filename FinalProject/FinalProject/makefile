# Final
EXE=final

# Main target
all: $(EXE)

# #  MinGW
# ifeq "$(OS)" "Windows_NT"
# CFLG=-O3 -Wall
# LIBS=-lfreeglut -lglu32 -lopengl32 -lm
# CLEAN=rm -f *.exe *.o *.a
# else
# #  OSX
# ifeq "$(shell uname)" "Darwin"
# CFLG=-O3 -Wall -Wno-deprecated-declarations
# LIBS=-framework GLUT -framework OpenGL
# #  Linux/Unix/Solaris
# else
# CFLG=-O3 -Wall
# LIBS=-lglut -lGLU -lGL -lm
# endif
# #  OSX/Linux/Unix/Solaris
# CLEAN=rm -f $(EXE) *.o *.a
# endif


# Main target
all: $(EXE)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=$(RES)
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
final.o: final.c CSCIx229.h
fatal.o: fatal.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
print.o: print.c CSCIx229.h
project.o: project.c CSCIx229.h
errcheck.o: errcheck.c CSCIx229.h
object.o: object.c CSCIx229.h

#  Create archive
CSCIx229.a:fatal.o loadtexbmp.o print.o project.o errcheck.o object.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
final:final.o CSCIx229.a
	gcc -O3 -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
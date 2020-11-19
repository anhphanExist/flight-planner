ifeq ($(OS),Windows_NT)
RM = del /Q /F
CP = copy /Y
ifdef ComSpec
SHELL := $(ComSpec)
endif
ifdef COMSPEC
SHELL := $(COMSPEC)
endif
else
RM = rm -rf
CP = cp -f
endif

flight_nav: flight_nav.o
	gcc flight_nav.o -o flight_nav

flight_nav.o: flight_nav.c
	gcc -c flight_nav.c

clean:
	-$(RM) flight_nav *.exe *.o
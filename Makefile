OBJ = mytime
override CFLAGS += -Ofast -funroll-loops -flto -Wall
override DEVEL_FLAGS += -g -Wall -Wextra 
override DEBUG_FLAGS += -DDEBUG
override LFLAGS += 
CC = gcc
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

%.o : %.c
	$(CC) $(DEVEL_FLAGS) -c $^

$(OBJ) : mytime.o
	$(CC) $(DEVEL_FLAGS) $^ -o $@ $(LFLAGS)

clean :
	rm -f *.o $(OBJ)

clear : clean

debug: clear
	$(CC) $(DEVEL_FLAGS) $(DEBUG_FLAGS) -c *.c
	$(CC) $(DEVEL_FLAGS) $(DEBUG_FLAGS) -o $(OBJ) *.o $(LFLAGS)

ofast : clear
	$(CC) $(CFLAGS) -c *.c
	$(CC) $(CFLAGS) -o $(OBJ) *.o $(LFLAGS)

install : $(OBJ)
	@echo "Installing program to $(DESTDIR)$(BINDIR) ..."
	@mkdir -p $(DESTDIR)$(BINDIR)
	@install -pm0755 $(OBJ) $(DESTDIR)$(BINDIR)/$(TARGET) || \
		echo "Failed. Try "make PREFIX=~ install" ?"

all : install

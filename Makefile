OBJ = mytime
override CFLAGS += -O3 -funroll-loops -flto -Wall
override DEBUG_FLAGS += -DDEBUG
override LFLAGS += 
CC = gcc
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

%.o : %.c
	$(CC) $(CFLAGS) -c $^

$(OBJ) : mytime.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

clean :
	rm -f mytime.o $(OBJ)

clear : clean

install : $(OBJ)
	@echo "Installing program to $(DESTDIR)$(BINDIR) ..."
	@mkdir -p $(DESTDIR)$(BINDIR)
	@install -pm0755 $(OBJ) $(DESTDIR)$(BINDIR)/$(TARGET) || \
		echo "Failed. Try "make PREFIX=~ install" ?"

all : install

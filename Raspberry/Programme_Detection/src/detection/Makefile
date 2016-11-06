#---------------------------------------------------------------------------------
#/bin/make
#---------------------------------------------------------------------------------

PROG     = detection

BIN      = $(DIR_BIN)/$(PROG).x

PC_CS    =

PC_OBJS  =

C_OBJS   = $(DIR_OBJET)/$(PROG).o \
		   $(DIR_OBJET)/det_timer.o \
		   $(DIR_OBJET)/det_init.o \
		   $(DIR_OBJET)/det_quit.o \
		   $(DIR_OBJET)/det_signal.o \
		   $(DIR_OBJET)/det_sqlite.o \
		   $(DIR_OBJET)/det_env.o \

CPP_OBJS =

SRH      = dif_const.h dif_type.h dif_var.h dif_proto.h

OBJLIB   = $(DIR_OBJET)/libdetec.a

TILDE    = $(CURDIR)/*~

# definition des flags pour compilation
# --------------------------------------------------------------------------------
CC=/usr/bin/gcc
CC_FLAGS =	-DLINUX -D_GNU_SOURCE -D_REENTRANT -g -Wpadded   

APPLI_FLAGS = -I/usr/include -I/usr/local/include -I$(CURDIR) -I../../include  -L/usr/local/lib -lcurl -lwiringPi -lsqlite3

CFLAGS = $(CC_FLAGS) $(APPLI_FLAGS)


# definitions pour la compilation
# --------------------------------------------------------------------------------
.SUFFIXES: .o .c .cpp .pc

all:	pgm


$(CPP_OBJS): $(DIR_OBJET)/%.o : %.cpp
	@echo ""
	@echo "<< COMPILATION de "$(notdir $*).cpp" >> "
	@echo ""
	$(CC) -c $(CFLAGS) $(notdir $*).cpp -o $@

$(C_OBJS): $(DIR_OBJET)/%.o : %.c
	@echo ""
	@echo "<< COMPILATION de "$(notdir $*).c" >> "
	@echo ""
	$(CC) -c $(CFLAGS) $(notdir $*).c -o $@

# construction de l executable
# --------------------------------------------------------------------------------

pgm:	$(C_OBJS) $(CPP_OBJS)
	@echo ""
	@echo "<< EDITION de LIEN pour "$(PROG)" >> "
	@echo ""
	$(CC) $(CFLAGS) -o $(BIN) $(C_OBJS) $(CPP_OBJS) $(OBJLIB)
	@echo ""
	@echo "<< GENERATION de l EXECUTABLE "$(PROG)"  TERMINEE >> "

clean: 
	rm -f $(CPP_OBJS) $(C_OBJS) $(BIN) $(TILDE)



# --------------------------------------------------------------------------------
# Makefile d'un fichier lib_...c 
# --------------------------------------------------------------------------------
 
# Definition des programmes a compiler
# --------------------------------------------------------------------------------
PC_CS    =

PC_OBJS  =

C_OBJS   = $(DIR_OBJET)/lib_date.o

CPP_OBJS =


# definition des flags pour compilation
# --------------------------------------------------------------------------------
CC=/usr/bin/gcc
CC_FLAGS =      -DLINUX -D_GNU_SOURCE -D_REENTRANT -g -Wpadded

ORA_FLAGS =     -I/usr/local/include
OCI_FLAGS =
APPLI_FLAGS =   -I$(CURDIR) -I$(DIR_INCLUDE)

CFLAGS = $(CC_FLAGS) $(OCI_FLAGS) $(ORA_FLAGS) $(APPLI_FLAGS)

# definition des flags pour precompilation oracle
# --------------------------------------------------------------------------------

PCCFLAGS =  include=/usr/include include=/usr/include/sys \
	        include=/usr/lib/gcc/arm-linux-gnueabihf/4.6/include

# definition pour librairies wiringPi
# -------------------------------------------------------------------------------- 

WIPI_LIB= -L/usr/local/lib -llibwiringPi.so

LIB = $(WIPI_LIB)

# definitions pour la compilation
# --------------------------------------------------------------------------------
.SUFFIXES: .o .c .cpp .pc

all:    $(C_OBJS) $(PC_OBJS) $(CPP_OBJS)
	@echo ""
	ar rv $(DIR_OBJET)/libdetec.a $(C_OBJS) $(PC_OBJS) $(CPP_OBJS)
	@echo ""

$(PC_CS): $(DIR_OBJET)/%.c : %.pc
	@echo ""
	@echo "<< PRE-COMPILATION de "$(notdir $*).pc" >> "
	@echo ""
	$(PCC) iname=$(notdir $*).pc $(PCCFLAGS) oname=$@

$(PC_OBJS): %.o : %.c
	@echo ""
	@echo "<< PRECOMP => COMPILATION de "$(notdir $*).c" >> "
	@echo ""
	$(CC) -c $(CFLAGS) $< -o $@

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

clean: 
	rm -f $(PC_CS) $(PC_OBJS) $(CPP_OBJS) $(C_OBJS)


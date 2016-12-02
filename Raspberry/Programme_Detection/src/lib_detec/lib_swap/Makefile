# --------------------------------------------------------------------------------
# Makefile d'un fichier lib_...c 
# --------------------------------------------------------------------------------
 
# Definition des programmes a compiler
# --------------------------------------------------------------------------------
PC_CS    =

PC_OBJS  =

C_OBJS   = $(DIR_OBJET)/lib_swap.o

CPP_OBJS =


# definition des flags pour compilation
# --------------------------------------------------------------------------------
CC=/usr/bin/gcc
CC_FLAGS =      -DLINUX -D_GNU_SOURCE -D_REENTRANT -g -Wpadded

OCI_FLAGS =

ORA_FLAGS =    

APPLI_FLAGS =   -I$(CURDIR) -I$(DIR_INCLUDE)*

CFLAGS = $(CC_FLAGS) $(OCI_FLAGS) $(ORA_FLAGS) $(APPLI_FLAGS)

# definition des flags pour precompilation oracle
# --------------------------------------------------------------------------------
PCC = 

PCCINC = 
PCC_PRECOMP = 
PCC_RDBMS = 
PCC_PLSQL = 
PCC_NETWORK = 
PCC_DEMO =
ORA_USER_ID = 

PCCFLAGS =  

# definition pour librairies oracle
# --------------------------------------------------------------------------------
LIBHOME = 
LDFLAGS =
ORACLE_LIB = 

OCI_LIB=

LIB = $(ORACLE_LIB) $(OCI_LIB)

# definitions pour la compilation
# --------------------------------------------------------------------------------
.SUFFIXES: .o .c .cpp .pc

all:    $(C_OBJS) $(PC_OBJS) $(CPP_OBJS)
	@echo "ar rv $(DIR_OBJET)/libdetec.a $(C_OBJS) $(PC_OBJS) $(CPP_OBJS)"
	ar rv $(DIR_OBJET)/libdetec.a $(C_OBJS) $(PC_OBJS) $(CPP_OBJS)
	@echo ""

$(PC_CS): $(DIR_OBJET)/%.c : %.pc
	@echo ""
	@echo "<< PRE-COMPILATION de "$(notdir $*).pc" >> "
	@echo ""
	@echo"$(PCC) iname=$(notdir $*).pc $(PCCFLAGS) oname=$@"
	$(PCC) iname=$(notdir $*).pc $(PCCFLAGS) oname=$@

$(PC_OBJS): %.o : %.c
	@echo ""
	@echo "<< PRECOMP => COMPILATION de "$(notdir $*).c" >> "
	@echo ""
	$(CC) -c $(CFLAGS) $< -o $@

$(CPP_OBJS): $(DIR_OBJET)/%.o : %.cpp
	@echo ""
	@echo "<< COMPILATION de "$(notdir $*).cpp" >> "
	@echo "$(CC) -c $(CFLAGS) $(notdir $*).cpp -o $@"
	$(CC) -c $(CFLAGS) $(notdir $*).cpp -o $@

$(C_OBJS): $(DIR_OBJET)/%.o : %.c
	@echo ""
	@echo "<< COMPILATION de "$(notdir $*).c" >> "
	@echo "$(CC) -c $(CFLAGS) $(notdir $*).c -o $@"
	$(CC) -c $(CFLAGS) $(notdir $*).c -o $@

clean: 
	rm -f $(PC_CS) $(PC_OBJS) $(CPP_OBJS) $(C_OBJS)


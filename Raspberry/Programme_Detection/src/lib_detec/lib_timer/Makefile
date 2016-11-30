# --------------------------------------------------------------------------------
# Makefile d'un fichier lib_...c 
# --------------------------------------------------------------------------------
 
# Definition des programmes a compiler
# --------------------------------------------------------------------------------
PC_CS    =

PC_OBJS  =

C_OBJS   = $(DIR_OBJET)/lib_timer.o

CPP_OBJS =


# definition des flags pour compilation
# --------------------------------------------------------------------------------
CC=/usr/bin/gcc
CC_FLAGS =      -DLINUX -D_GNU_SOURCE -D_REENTRANT -g -Wpadded

OCI_FLAGS =     -DOCI_IMPORT_LINKAGE -DOCI_CHARSET_ANSI

ORA_FLAGS =     -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/precomp/public \
	        -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public \
	        -I/usr/local/include

APPLI_FLAGS =   -I$(CURDIR) -I$(DIR_INCLUDE)

CFLAGS = $(CC_FLAGS) $(OCI_FLAGS) $(ORA_FLAGS) $(APPLI_FLAGS)

# definition des flags pour precompilation oracle
# --------------------------------------------------------------------------------
PCC = $(ORACLE_HOME)/bin/proc

PCCINC = $(ORACLE_HOME)/precomp/lib
PCC_PRECOMP = $(ORACLE_HOME)/precomp/public
PCC_RDBMS = $(ORACLE_HOME)/rdbms/public
PCC_PLSQL = $(ORACLE_HOME)/plsql/public
PCC_NETWORK = $(ORACLE_HOME)/network/public
PCC_DEMO = $(ORACLE_HOME)/rdbms/demo
ORA_USER_ID = lhp/lhp@PLAN

PCCFLAGS =      include=$(PCCINC) include=$(PCC_PRECOMP) include=$(PCC_RDBMS) \
	        include=$(PCC_PLSQL) include=$(PCC_NETWORK) include=. \
	        include=/usr/include include=/usr/include/sys \
	        include=/usr/lib/gcc/x86_64-redhat-linux/4.4.6/include \
	        ireclen=2048 oreclen=2048 select_error=yes \
	        sqlcheck=full char_map=STRING userid=$(ORA_USER_ID) ltype=none \
	        MODE=ORACLE DBMS=V7 UNSAFE_NULL=YES

# definition pour librairies oracle
# --------------------------------------------------------------------------------
LIBHOME = $(ORACLE_HOME)/lib
LDFLAGS = -L$(LIBHOME)
ORACLE_LIB = $(LDFLAGS) -lclntsh 

OCI_LIB= -L/usr/local/lib -locilib

LIB = $(ORACLE_LIB) $(OCI_LIB)

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


# SSRM Makefile
# This is based on the nrc Makefile(it's in this repo lol)

EXE:=ssrm

MODS:=/cli

SRC:=$(foreach mod,$(MODS),$(wildcard src$(mod)/*.c))
HDR:=$(foreach mod,$(MODS),$(wildcard src$(mod)/*.h))
RCT:=$(wildcard src/r_test/*.rct)

OBJ:=$(patsubst %.c,%.o,$(SRC))

MKDIR:=mkdir -p
CP:=cp
RM:=rm -rf

CC:=gcc
LD:=gcc

CCFLAGS:=-g -Imemstack/include
LDFLAGS:=-g
LIBRARY:=

$(BIN):
	$(info [MK] CREATING BUILD DIR)
	@$(MKDIR) $(BIN)

%.o: %.c
	$(info [CC] BUILDING $< => $@)
	@$(CC) $(CCFLAGS) -c $< -o $@

obj: $(OBJ)

binary: obj
	$(info [LD] LINKING $(OBJ) => $(EXE))
	@$(LD) $(LDFLAGS) $(OBJ) -o $(EXE) $(LIBRARY)

all: binary $(B_RCT)

run: all
	./$(EXE)

clean:
	$(RM) $(BIN)
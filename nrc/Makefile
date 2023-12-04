## NRC Makefile

# This makefile was thrown together by khhs in the span of 10 minutes.
# Due to this, it's not the most well written thing in the world.
# But it still "works", and it does some stuff which I've never tried before.

## LICENSE

# Feel free to use it in your own projects, I do not guarantee any form
# of warranty tho.

# All I'd want you to do is to allow others to also use it, in OSS projects.
# You should also credit me(khhs)

# Just don't be an ass, okay?

## Variable descriptions

# 	BIN - The output/binary directory
# 	EXE - The binary file
# 	MODS - All the source directories(under src)
# 	SRC - All the files that should be compiled
# 	HDR - All the header files in the project
# 	B_SRC - The source files as found in the build directory
# 	B_HDR - The header files as found in the build directory
# 	OBJ - The object files
# 	MKDIR - The command used to create a dir(without error)
# 	RM - The command used to delete files/dirs(without errors)
# 	CP - The command used to copy files
# 	CC - The C compiler command
# 	LD - The C linker command
#	CCFLAGS - The C compiler flags
#	LDFLAGS - The C linker flags
#	LIBRARY - The C linker libraries

## Targets

#	$(BIN) - Makes the build directory
#	buildfiles - Copies C and header files to build dir
#	obj - Builds C files into .o
#	binary - Links the .o files into the $(EXE) file
#	all - Build everything(binary)
#	run - Builds and runs the project
#	clean - Deletes the build directory
#
#		Special targets:
#	$(BIN)/%.c - Copies C files to build directory
#	$(BIN)/%.h - Copies header files to build directory
#	$(BIN)/%.o - Build an object file

EXE:=nrc

MODS:=/cli /lexer /util

SRC:=$(foreach mod,$(MODS),$(wildcard src$(mod)/*.c))
HDR:=$(foreach mod,$(MODS),$(wildcard src$(mod)/*.h))
RCT:=$(wildcard src/r_test/*.rct)

OBJ:=$(patsubst %.c,%.o,$(SRC))

MKDIR:=mkdir -p
CP:=cp
RM:=rm -rf

CC:=gcc
LD:=gcc

CCFLAGS:=-g
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

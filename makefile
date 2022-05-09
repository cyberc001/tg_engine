SRC := $(shell find . -name "*.c")
INCLUDE := -I.
OBJ := $(addsuffix .o,$(basename $(SRC)))
BIN := libtg_engine.a
FLAGS := -fms-extensions -g

all: $(BIN)

clean:
	rm $(OBJ)
	rm $(BIN)

$(BIN): $(OBJ)
	ar rcs $(BIN) $(OBJ)

%.o: %.c
	gcc -c $(INCLUDE) $(FLAGS) $< -o $@

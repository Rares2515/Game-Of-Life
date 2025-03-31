SRC = GOL.c

INPUT_FILES = $(wildcard InputData/data*.in)
OUTPUT_FILES = $(patsubst InputData/%.in,OutputData/%.out,$(INPUT_FILES))

$(info $(INPUT_FILES) , $(OUTPUT_FILES))

all: build

build: $(SRC)
	gcc -O2 -Wall $(SRC) -o a.out

run: $(OUTPUT_FILES)
	echo "Bun"

OutputData/data%.out: InputData/data%.in build
	mkdir -p $(dir $@)
	./a.out $< $@

clean:
	rm -f a.out
	rm -rf OutputData

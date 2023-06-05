EXEC=compiler
TEST_FOLDER=tests
TEST_OUT_FOLDER=tests/out

all: bison flex gcc
	@echo "Done."

bison: parser.y
	bison parser.y

flex: scanner.l
	flex scanner.l

gcc: scanner.c parser.c
	gcc -Wall -o $(EXEC) scanner.c parser.c types.c tables.c -ly

test:
	echo "Starting test"; \
	find $(TEST_FOLDER)/ -maxdepth 1 -type f -printf "%f\n" | while read file; do \
		echo "Testing file: $$file" ; \
		./$(EXEC) < $(TEST_FOLDER)/$$file | diff - $(TEST_OUT_FOLDER)/$$file ; \
	done; \
	echo "Test DONE"

clean:
	@rm -f *.o *.output scanner.c parser.h parser.c $(EXEC)

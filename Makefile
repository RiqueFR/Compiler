SCANNER=./scanner
TEST_FOLDER=tests
TEST_OUT_FOLDER=tests/out

scanner:
	flex scanner.l
	gcc -o $(SCANNER) lex.yy.c -lfl

test:
	find $(TEST_FOLDER)/ -maxdepth 1 -type f -printf "%f\n" | while read file; do \
		$(SCANNER) < $(TEST_FOLDER)/$$file | diff - $(TEST_OUT_FOLDER)/$$file ; \
	done

clean:
	rm lex.yy.c scanner

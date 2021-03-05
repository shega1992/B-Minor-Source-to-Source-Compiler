bminor: parser.o scanner.o main.o ast.o scope.o  symbol.o semantic_analysis.o helper_functions.o hash_table.o translate.o
	gcc -o bminor main.o scanner.o parser.o ast.o symbol.o scope.o semantic_analysis.o helper_functions.o hash_table.o translate.o

parser.o: parser.c token.h
	gcc -c parser.c -o parser.o

scanner.o: scanner.c
	gcc -c scanner.c -o scanner.o

main.o: main.c
	gcc -c main.c -o main.o

ast.o: ast.c ast.h
	gcc -c ast.c -o ast.o

scope.o: scope.c scope.h symbol.o
	gcc -c scope.c -o scope.o

symbol.o: symbol.c symbol.h
	gcc -c symbol.c -o symbol.o

semantic_analysis.o: semantic_analysis.c semantic_analysis.h
	gcc -c semantic_analysis.c -o semantic_analysis.o

helper_functions.o: helper_functions.c helper_functions.h
	gcc -c helper_functions.c -o helper_functions.o

hash_table.o: hash_table.c hash_table.h
	gcc -c hash_table.c -o hash_table.o

translate.o: translate.c translate.h
	gcc -c translate.c -o translate.o

scanner.c: scanner.flex token.h
	flex -o scanner.c scanner.flex

parser.c token.h: parser.bison 
	bison -v --defines=token.h --output=parser.c parser.bison

clean:
	rm -f scanner.c bminor parser.c token.h parser.output *.o scan

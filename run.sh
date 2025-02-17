g++ -c src/main.cpp -o build/main.o
g++ -c src/lexer.cpp -o build/lexer.o
g++ -c src/preprocess.cpp -o build/preprocess.o

g++ build/main.o build/lexer.o build/preprocess.o -o assembler

./assembler test.s

g++ -c src/main.cpp -o build/main.o
g++ -c src/lexer.cpp -o build/lexer.o
g++ -c src/preprocess.cpp -o build/preprocess.o
g++ -c src/write.cpp -o build/write.o

g++ build/main.o build/lexer.o build/preprocess.o build/write.o -o assembler

./assembler test.s test_exec
./test_exec

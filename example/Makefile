all: text text-cpp no-kerning simple
clean:
	rm text text-cpp no-kerning rltextkerner.o

text: text.c ../stb_truetype.h ../raylib.h ../rltextkerner.h
	gcc -g -Wall text.c -o $@ -lraylib -lglfw -lGL -lm -lpthread -ldl -I../
text-cpp: text.cpp text.c ../stb_truetype.h ../raylib.h ../rltextkerner.h ../rltextkerner.c
	gcc -g -Wall ../rltextkerner.c -o rltextkerner.o -lraylib -lglfw -lGL -lm -lpthread -ldl -c -I../
	g++ -g -Wall rltextkerner.o text.cpp -o $@ -lraylib -lglfw -lGL -lm -lpthread -ldl -I../
no-kerning: no-kerning.c ../stb_truetype.h ../raylib.h ../rltextkerner.h
	gcc -g -Wall no-kerning.c -o $@ -lraylib -lglfw -lGL -lm -lpthread -ldl -I../
simple: simple.c ../stb_truetype.h ../raylib.h ../rltextkerner.h
	gcc -g -Wall simple.c -o $@ -lraylib -lglfw -lGL -lm -lpthread -ldl -I../

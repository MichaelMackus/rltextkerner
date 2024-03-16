all: text text-cpp no-kerning
clean:
	rm text text-cpp no-kerning rltextkerner.o

text: text.c ../stb_truetype.h ../raylib.h ../rltextkerner.h
	gcc -Wall text.c -o $@ -lraylib -lglfw -lGL -lm -lpthread -ldl -I../
text-cpp: text.cpp ../stb_truetype.h ../raylib.h ../rltextkerner.h ../rltextkerner.c
	gcc -Wall ../rltextkerner.c -o rltextkerner.o -lraylib -lglfw -lGL -lm -lpthread -ldl -c -I../
	g++ -Wall rltextkerner.o text.cpp -o $@ -lraylib -lglfw -lGL -lm -lpthread -ldl -I../
no-kerning: no-kerning.c ../stb_truetype.h ../raylib.h ../rltextkerner.h
	gcc -Wall no-kerning.c -o $@ -lraylib -lglfw -lGL -lm -lpthread -ldl -I../

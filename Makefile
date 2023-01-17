build:
	gcc -o tema1 src/main.c src/utils.c src/list.c -lpthread -Wall -Werror -lm -g

build_debug:
	gcc -o tema1 src/main.c src/utils.c src/list.c -lpthread -DDEBUG -g3 -O0 -Werror -Wall -lm -g

clean:
	rm tema1
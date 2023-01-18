all: prepare main_shell cat rm ls mkdir date

prepare:
	mkdir -p bin

main_shell: main_shell.c
	gcc -pthread -o main_shell main_shell.c -lm

cat: external_cmd/cat.c
	gcc -o bin/cat external_cmd/cat.c -lm

rm: external_cmd/rm.c
	gcc -o bin/rm external_cmd/rm.c

ls: external_cmd/ls.c
	gcc -o bin/ls external_cmd/ls.c

mkdir: external_cmd/mkdir.c
	gcc -o bin/mkdir external_cmd/mkdir.c

date: external_cmd/date.c
	gcc -o bin/date external_cmd/date.c

clean:
	rm -r main_shell .main_shell_history bin

run:
	./main_shell

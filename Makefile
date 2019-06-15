all: principal
principal: main.c
	gcc main.c -o m.exe -pthread -lm

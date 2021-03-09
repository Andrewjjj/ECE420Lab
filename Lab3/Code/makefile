C = gcc -ggdb -pthread -Wall

main: main.c Lab3IO.c
	$(C) main.c Lab3IO.c -o main -fopenmp
	@echo 'main.exe produced'

serial: serial.c Lab3IO.c
	$(C) serial.c Lab3IO.c -o serial
	@echo 'serial.exe produced'

datagen: datagen.c Lab3IO.c
	$(C) datagen.c Lab3IO.c -o datagen
	@echo 'datagen.exe produced'

tester: serialtester.c Lab3IO.c
	$(C) serialtester.c Lab3IO.c -o tester -lm
	@echo 'tester.exe produced'

clean: 
	@rm -f tester datagen main data_input data_output
	@rm -r *.dSYM
	@echo 'directory cleaned'
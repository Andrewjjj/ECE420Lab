C = gcc -g -lpthread -Wall

main: 
	$(C) -o main lab1.c lab1_IO.c
	@echo 'main.exe produced'

gen:
	gcc -o gen matrixgen.c
	@echo 'gen.exe produced for producing matrix'

test:
	gcc -o test serialtester.c lab1_IO.c
	@echo 'test.exe produced for testing'

clean:
	@rm -f *.o main gen test
	@rm -f data_input data_output
	@echo 'directory cleaned'
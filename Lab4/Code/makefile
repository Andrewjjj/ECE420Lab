C = mpicc -ggdb -Wall

main: main.c Lab4_IO.c
	$(C) main.c Lab4_IO.c -o main
	@echo 'main.exe produced'

datatrim: datatrim.c Lab4_IO.c
	$(C) datatrim.c Lab4_IO.c -o datatrim
	@echo 'datagen.exe produced'

tester: serialtester.c Lab4_IO.c
	$(C) serialtester.c Lab4_IO.c -o tester -lm
	@echo 'tester.exe produced'

clean: 
	@rm -f tester datatrim main data_input_link data_input_meta data_output
	@rm -r *.dSYM
	@echo 'directory cleaned'
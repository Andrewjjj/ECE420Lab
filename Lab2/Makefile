C = gcc -g -pthread -Wall

main1: main1.c
	$(C) main1.c -o main1
	@echo 'main1.exe produced'

main3: main3.c
	$(C) main3.c -o main3
	@echo 'main3.exe produced'

simpClient: demos/simpleClient.c
	$(C) demos/simpleClient.c -o simpClient
	@echo 'simpClient.exe produced'

attacker: attacker.c
	$(C) attacker.c -o attacker
	@echo 'attacker.exe produced'

client: client.c
	$(C) client.c -o client
	@echo 'client.exe produced'

clean: 
	@rm -f client simpClient main1 main3
	@echo 'directory cleaned'
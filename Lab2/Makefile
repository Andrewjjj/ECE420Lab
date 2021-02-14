C = gcc -g -pthread -Wall

main1: main1.c
	$(C) main1.c -o main1
	@echo 'main1.exe produced'

simpClient: demos/simpleClient.c
	$(C) demos/simpleClient.c -o simpClient
	@echo 'simpClient.exe produced'

client: client.c
	$(C) client.c -o client
	@echo 'client.exe produced'

clean: 
	@rm -f client simpClient main1
	@echo 'directory cleaned'
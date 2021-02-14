C = gcc -g -pthread -Wall

main1: main1.c
	$(C) main1.c -o main1

client: client.c 
	$(C) client.c -o client

attacker: attacker.c 
	$(C) attacker.c -o attacker

clean:
	rm main1 client attacker
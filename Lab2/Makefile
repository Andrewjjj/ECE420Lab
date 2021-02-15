C = gcc -g -pthread -Wall

main1: main1.c
	$(C) main1.c -o main1

main2: main2.c 
	$(C) main2.c -o main1

client: client.c 
	$(C) client.c -o client

attacker: attacker.c 
	$(C) attacker.c -o attacker -lm

clean:
	rm main1 main2 client attacker
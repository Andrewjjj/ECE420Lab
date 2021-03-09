#python 3

f = open("results/results_main2_n1000.txt")
tot = 0
n=0
for line in f:
    vs = line.split('e-')
    v = float(vs[0]) * 10**(-int(vs[1]))
    # print(v)
    n+=1
    tot += v
print("final")
print(tot/n)
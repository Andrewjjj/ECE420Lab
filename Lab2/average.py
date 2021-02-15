#python 3

f = open("server_output_time_aggregated")
tot = 0
n=0
for line in f:
    vs = line.split('e-')
    v = float(vs[0]) * 10**(-int(vs[1]))
    print(v)
    n+=1
    tot += v
print("final")
print(tot/n)
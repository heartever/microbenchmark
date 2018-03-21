import os

a = []
b = []
for fn in os.listdir("data"):
    fin = open("data/" + fn)
    lines = fin.readlines()
    for i in range(len(lines) / 3):
        a.append(lines[i * 3 + 1])
        b.append(lines[i * 3 + 2])

a = a[:100000]
b = b[:100000]

a = [[int('0x' + x[2*i: 2*i + 2], 16) for i in range(8)] for x in a]
b = [[int('0x' + x[2*i: 2*i + 2], 16) for i in range(8)] for x in b]


print "prob of passing unit tests",
print "t0", max(sum(x[i] for x in a) for i in range(8)) / float(len(a)) / 256,
print max(sum(x[i] for x in b) for i in range(8)) / float(len(b)) / 256

print "prob of passing co-location tests", 
print sum(any(c >= 242 for c in x) and any(c >= 225 for c in y) for x, y in zip(a, b)) / float(len(a))


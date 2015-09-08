sums = []
c = 0

import sys

for line in sys.stdin:
	values = list(map(float,line.split()))
	if sums == []:
		sums = [0.0]*len(values)
	for i in range(len(values)):
		sums[i] += values[i]

	c += 1

for n in sums:
	print(str((n+.0)/(c+.0)) + "\t",end='')

print('\n',end='')
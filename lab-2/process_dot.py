import sys

lines=[]
for line in sys.stdin:
	lines.append(line[len("dot: "):(line.index("... action")-4)])

print "digraph G{"
for line in lines:
	print "\t" +  line
print "}"

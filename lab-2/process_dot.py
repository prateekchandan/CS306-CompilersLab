import sys

lines=[]
for line in sys.stdin:
	lines.append(line[len("dot: "):-1])

print "strict digraph G{"
print "\t rank = same"
for line in lines:
	print "\t" +  line
print "}"

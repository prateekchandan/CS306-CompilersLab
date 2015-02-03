import sys

def get_last_pos(s):
    last_pos = -1
    for i in range(0,len(s)):
	if s[i]=='_':
	    last_pos = i
    return last_pos

lines=[]
for line in sys.stdin:
	lines.append(line[len("dot: "):-1])

print "digraph {"
print "\tordering = out;"
#print "\t rank = same"
for line in lines:
	l = line.split()
	if len(l)<1:
	    continue

	is_edge = False
	children = []
	for i in range(0,len(l)):
	    if l[i]=="->":
		is_edge = True
	    if is_edge == True:
		pos = get_last_pos(l[i])
		if pos != -1:
		    l[i] = l[i][pos+1:]
		    children += [l[i]]
		
	pos = get_last_pos(l[0])
	if pos!=-1:
	    l[0] = l[0][pos+1:]

	if is_edge == True:
	    for i in range(0,len(children)):
		print "\t" + l[0] + " -> " + children[i]
	    continue
	line = " ".join(l)
	print "\t" + line
print "}"

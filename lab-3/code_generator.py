file = open("parse.y")

for line in file.readlines():
	line = line.strip()
	
	if line !="" and line[0]!="%":
		tokens = line.split()
		if  tokens[0] == ";":
			pass
		elif tokens[0] == "|" or tokens[0] == ":":
			pass
		else:
			class_name = tokens[0]
			print "class "+class_name+''' : public abstract_astnode{
	public:
	void print(){
	cout<<"'''+class_name+'''\n";
	}
};'''
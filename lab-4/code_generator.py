file = open("grammar.y")

for line in file.readlines():
	line = line.strip()
	
	if line !="" and line[0]!="%" and line[0] != "/":
		tokens = line.split()
		if  tokens[0] == ";":
			pass
		elif tokens[0] == "|" or tokens[0] == ":":
			# This is for generating the rules
			pass
		else:
			#This part is for generation of classes
			class_name = tokens[0]
			print "class "+class_name+''' : public abstract_astnode{
	public:
	void print(){
		cout<<"'''+class_name+'''\\n";
	}
};'''
//directives and definitions

%% 

// patterns and actions

\/\*([^*]*\*+[^*/])*[^*]*\*+\/	{std::cout<<"Comment: "<<matched()<<std::endl;}
[ \t\n]+		 	{std::cout<<"whitespaces:"<<matched()<<std::endl;}
. 				{std::cout<<"others:"<<matched()<<std::endl;}


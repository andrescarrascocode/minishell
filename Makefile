minishell: minishell.o internas.o ejecutar.o libparser.a
	gcc -Wall minishell.o internas.o ejecutar.o -L./ -lparser -o minishell

minishell.o: minishell.c
	gcc -c -Wall minishell.c

internas.o: internas.c internas.h
	gcc -c -Wall internas.c
ejecutar.o: ejecutar.c ejecutar.h
	gcc -c -Wall ejecutar.c

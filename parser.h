enum TEstado
{
   INICIO_ARG, ARG
};
 
char **parse_orden(const char *orden, int *indentrada, int *indsalida, int *background);
char **parser_pipes(const char *orden, int *numordenes);

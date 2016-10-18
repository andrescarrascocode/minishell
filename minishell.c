#include <signal.h> /* SIGACTION */
#include <stdio.h> /* PRINTF, FGETS */
#include <stdlib.h> /* MALLOC, CALLOC, EXIT, */
#include <string.h> /* MEMSET, STRCMP, STRLEN, STRCPY */

#define BUF_STRINGS 100


void imprimir_prompt();
void leer_linea_ordenes(char *buffer);
char *eliminar_salto_linea(char *cadena);
void manejar_sigchild(int signo);
char **parse_orden(const char *orden, int *indentrada, int *indsalida, int *background);
int orden_interna(const char *buffer);
void funcion_exit();
void funcion_cd(const char *directorio);
void funcion_pwd();
void funcion_umask(const char *mascara);
void ejecutar_linea_ordenes(const char *orden);


int main(void)
{
  struct sigaction sa;
  char *buffer;
  char **array_ordenes;
  int indent = 0, indsal = 0, background = 0, seguir = 1;
  

  buffer = (char *) malloc(BUF_STRINGS * sizeof(char));
  array_ordenes = (char **) malloc(BUF_STRINGS * BUF_STRINGS * sizeof(char));

  printf("\n\tTeclear 'SALIR' para finalizar.\n\n");

  /* INICIALIZAMOS EL MANEJADOR DE SEÑAL */
  memset(&sa, 0, sizeof(&sa));
  sa.sa_handler = manejar_sigchild;
  sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;
  sigaction(SIGCHLD, &sa, NULL);
  
  while (seguir == 1)
  {
    imprimir_prompt();
    //printf("Antes de leerlineadeordenes");
    leer_linea_ordenes(buffer);
    if (strlen(buffer) == 0) {}
    else
    {
      array_ordenes = parse_orden(buffer, &indent, &indsal, &background);
      //printf("Despues de analizar orden");
      if ((strcmp(array_ordenes[0], "SALIR")) == 0)
        seguir = 0;
      else if (orden_interna(array_ordenes[0]) == 1) /* ES ORDEN INTERNA */
      {
        if ((strcmp(array_ordenes[0], "exit")) == 0)
          funcion_exit();
        else if ((strcmp(array_ordenes[0], "pwd")) == 0)
          funcion_pwd();
        else if ((strcmp(array_ordenes[0], "cd")) == 0)
        {
          if (array_ordenes[1] == NULL)
            array_ordenes[1] = "\0";
        
          funcion_cd(array_ordenes[1]);
        }
        else /* LA ORDEN INTERNA ES UMASK */
        {
          if (array_ordenes[1] == NULL)
            array_ordenes[1] = "\0";
        
          funcion_umask(array_ordenes[1]);
        }
      }
      else ejecutar_linea_ordenes(buffer);
    }
  }
  
  /* LIBERAMOS MEMORIA */
  free(buffer);
  free(array_ordenes);

  return 0;
}


void imprimir_prompt()
{
  printf("#");
}


void leer_linea_ordenes(char *buffer)
{
  fgets(buffer, BUF_STRINGS, stdin);
  //printf("Antes de eliminar_salto_linea\n");
  buffer = eliminar_salto_linea(buffer);
}


char *eliminar_salto_linea(char *cadena)
{
  int contador = 0, contador_aux = 0;
  char *cadena_aux;

  cadena_aux = (char *) calloc(BUF_STRINGS, sizeof(char));

  //printf("Antes del bucle.Strlen(cadena) = %i.Cadena = %s\n", strlen(cadena), cadena);
  while (contador < strlen(cadena)) /* HASTA FIN DE CADENA */
  {
    //printf("Entra en el while\n");
    if (cadena[contador] != '\n') /* COPIA SI DISTINTO \n */
    {
      //printf("Entra en el if\n");
      cadena_aux[contador_aux] = cadena[contador];
      contador_aux++;
    }
    //printf("Despues del if\n");
    contador++;
  }
  strcpy(cadena, cadena_aux);
  free(cadena_aux);
  //printf("Antes de salir del eliminar_salto_linea.Strlen(cadena) = %i\n",strlen(cadena));

  return cadena;
}

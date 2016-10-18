#include <fcntl.h> /* OPEN */
#include <signal.h> /* SIGNAL */
#include <stdio.h> /* PRINTF */
#include <stdlib.h> /* MALLOC */
#include <string.h> /* STRLEN */
#include <sys/stat.h> /* OPEN */
#include <sys/types.h> /* WAITPID, OPEN, FORK */
#include <sys/wait.h> /* WAITPID */
#include <unistd.h> /* FORK, CLOSE, DUP, EXECVP, GETPID */

#define E_DEFECTO 0
#define S_DEFECTO 1
#define BUF_STRINGS 100


char **parser_pipes(const char *linea,int *total);
int **crear_pipes(int nordenes);
pid_t ejecutar_orden(const char *orden, int entrada, int salida, int background);
char **parse_orden(const char *buf,int *indentrada,int *indsalida,int *background);
void manejar_sigchild(int signo);


void ejecutar_linea_ordenes(const char *orden)
{
  char **ordenes;
  int **tuberias;
  int background = 0, num_ord, contador;
  pid_t p;
  int tuberia_entrada, tuberia_salida;

  ordenes = (char **) malloc(BUF_STRINGS * BUF_STRINGS * sizeof(char));
  tuberias = (int **) malloc(2 * sizeof(int));
  
  ordenes = parser_pipes(orden, &num_ord); /* OBTENEMOS ARRAY DINAMICO DE ORDENES */
  tuberias = crear_pipes(num_ord); /* CREAMOS TUBERIAS SEGUN NUMERO DE ORDENES */
  
  for (contador = 0; contador < num_ord; contador++)
  {
    if (contador > 0) /* TUBERIA ENTRADA */
      tuberia_entrada = tuberias[contador-1][E_DEFECTO];
    else 
      tuberia_entrada = 0;
    if (contador < num_ord -1) /* TUBERIA SALIDA */
      tuberia_salida = tuberias[contador][S_DEFECTO];
    else
      tuberia_salida = 1;

    p = ejecutar_orden(ordenes[contador], tuberia_entrada, tuberia_salida, background); /* EJECUTAMOS LA ORDEN */
  }
  
  free(ordenes);
  free(tuberias);
}


int **crear_pipes(int nordenes)
{
  int **pipes = NULL;
  int i;
  pipes = (int **) malloc((nordenes-1) * sizeof(int *));

  for (i = 0; i < (nordenes-1); i++)
  {
    pipes[i] = (int *) malloc(sizeof(int)*2);
    pipe(pipes[i]);
  }
  return (pipes);
}


pid_t ejecutar_orden(const char *orden, int entrada, int salida, int background)
{
  char **p_ordenes = (char **) malloc(BUF_STRINGS * BUF_STRINGS * sizeof(char));
  char **ordenes_aux = (char **) malloc(BUF_STRINGS * BUF_STRINGS * sizeof(char));
  int descriptor, contador = 0, indice_entrada = 0, indice_salida = 0;
  pid_t pid_fork;

  p_ordenes = parse_orden(orden, &indice_entrada, &indice_salida, &background);
  
  if (indice_entrada > 0)/* HAY REDIRECCION DE ENTRADA */
  {
    if ((descriptor = open(p_ordenes[indice_entrada + 1], O_RDONLY)) < 0)
      printf("ERROR! Apertura de redireccion (Entrada)\n");
    
    /* ELIMINAMOS DEL ARRAY LA REDIRECCION */
    while (p_ordenes[contador][0] != '<')
    {
      ordenes_aux[contador] = p_ordenes[contador];
      contador++;
    }
    
    ordenes_aux[contador] = NULL; /* CERRAMOS COPIA */
    p_ordenes = ordenes_aux; /* DEVOLVEMOS NUEVOS VALORES A ordenes */
  }
  else if (indice_salida > 0)/* HAY REDIRECCION DE SALIDA */
  {
    if ((descriptor = (open((p_ordenes[indice_salida + 1]), O_RDWR | O_CREAT | O_TRUNC, 0777))) < 0)
      printf("ERROR! Apertura de redireccion (Salida)\n");
    
    /* ELIMINAMOS DEL ARRAY LA REDIRECCION */
    while (p_ordenes[contador][0] != '>')
    {
      ordenes_aux[contador] = p_ordenes[contador];
      contador++;
    }
    
    ordenes_aux[contador] = NULL; /* CERRAMOS COPIA */
    p_ordenes = ordenes_aux; /* DEVOLVEMOS NUEVOS VALORES A ordenes */
  }
  
  if ((pid_fork = fork()) == -1)
  {
    printf("ERROR! fork\n");
    exit(-1);
  }
  else if (pid_fork == 0) /* ES EL HIJO */
  {
    if (salida != 1) /* HAY TUBERIA DE REDIRECCION DE SALIDA */
    {
      close(STDOUT_FILENO); /* CERRAR ARCHIVO SALIDA ESTANDAR */
      dup(salida);
      close(salida);
    }
    else if (indice_salida > 0) /* HAY REDIRECCION DE SALIDA */
    {
      close(STDOUT_FILENO); /* CERRAR ARCHIVO SALIDA ESTANDAR */
      dup(descriptor);
      close(descriptor);
    }

    if (entrada != 0) /* HAY TUBERIA DE REDIRECCION DE ENTRADA */
    {
      close(STDIN_FILENO); /* CERRAR ARCHIVO ENTRADA ESTANDAR */
      dup(entrada);
      close(entrada);
    }
    else if (indice_entrada > 0) /* HAY REDIRECCION DE ENTRADA */
    {
      close(STDIN_FILENO); /* CERRAR ARCHIVO ENTRADA ESTANDAR */
      dup(descriptor);
      close(descriptor);
    }
    else if (background == 1) /* NO HAY TUBERIA NI REDIRECCION DE ENTRADA O SALIDA Y HAY BACKGROUND */
    {
      close(STDOUT_FILENO);
      if (open("/dev/null", O_WRONLY) < 0)
	printf("ERROR! Problema en /dev/null\n");
    }    
    
    /* EJECUTAMOS LA ORDEN */
    if (execvp(p_ordenes[0], p_ordenes) == -1)
      printf("ERROR! Problemas en exec\n");
  }
  else /* ES EL PADRE */
  {
    if (background == 1) /* HAY BACKGROUND */
    {
      /* NECESARIO EL SIGCHLD */
      if (signal(SIGCHLD, manejar_sigchild) == SIG_ERR)
      {
	printf("ERROR! SIGCHLD\n");
	exit(-1);
      }
    }
    else while (waitpid(pid_fork, NULL, NULL) > 0);

    /* LIBERAMOS MEMORIA */
    free(ordenes_aux);
    
    /* CERRAMOS LOS DESCRIPTORES NO ESTANDARES */
    if (entrada != 0)
      close(entrada);
    if (salida != 1)
      close(salida);
  }
  
  return getpid();
}


void manejar_sigchild(int signo)
{
  waitpid(-1, NULL, WNOHANG);
}

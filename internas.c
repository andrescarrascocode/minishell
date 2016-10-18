#include <stdio.h> /* PRINTF */
#include <stdlib.h> /* EXIT, GETENV, STRTOL */
#include <string.h> /* STRCMP */
#include <sys/types.h> /* UMASK */
#include <sys/stat.h> /* UMASK */
#include <unistd.h> /* GETCWD, CHDIR, */

#define BUF_STRINGS 100
#define BASE_OCTAL 8
#define MASCARA_BASE 022


int orden_interna(const char *buffer)
{
  if ((strcmp(buffer, "exit")) == 0)
      return 1;
  if ((strcmp(buffer, "pwd")) == 0)
      return 1;
  if ((strcmp(buffer, "cd")) == 0)
      return 1;
  if ((strcmp(buffer, "umask")) == 0)
      return 1;
  
  return 0;
}


void funcion_exit()
{
  printf("Fin Minishell\n");
  exit(0);
}


void funcion_pwd()
{
  char directorio_actual[BUF_STRINGS];

  /* TRATAMOS DE OBTENER LA RUTA DE TRABAJO ACTUAL */
  if (getcwd(directorio_actual, BUF_STRINGS) == NULL) /* ERROR EN GETCWD */
    printf("ERROR! No se pudo encontrar la ruta actual de trabajo\n");
  else
    printf("Directorio actual: %s\n", directorio_actual);
}


void funcion_cd(const char *directorio)
{
  char *nuevo_directorio;

  /* PRIMERO BUSCAMOS A QUE DIRECTORIO HAY QUE IR */
  if ((strcmp(directorio, "\0")) == 0) /* NO HAY ARGUMENTO */
    nuevo_directorio = getenv("HOME");
  else
    nuevo_directorio = directorio;

  /* UNA VEZ SABEMOS A DONDE HAY QUE IR */
  if ((chdir(nuevo_directorio)) == -1) /* ERROR EN CHDIR */
    printf("ERROR! No se pudo cambiar de directorio\n");
  else if ((getcwd(nuevo_directorio, BUF_STRINGS)) == NULL) /* ERROR EN GETCWD */
    printf("ERROR! No se pudo encontrar la ruta actual de trabajo\n");
  else
    printf("Directorio actual: %s\n", nuevo_directorio);
}


void funcion_umask(const char *mascara)
{
  unsigned int nueva_mascara;

  /* PRIMERO MIRAMOS SI REESTABLECER LA MASCARA BASE*/
  if (strcmp(mascara, "\0") == 0)
  {
    umask(MASCARA_BASE);
    printf("Mascara establecida: %03o\n", MASCARA_BASE);
  }
  else /* SE ESTABLECE NUEVA MASCARA */
  {
    /* TRANSFORMAMOS LA CADENA EN UN NUMERO DE BASE ESTABLECIDA EN CONSTANTES */
    if ((nueva_mascara = (strtol(mascara, NULL, BASE_OCTAL))) == 0)
      printf("ERROR! No se pudo establecer la mascara\n");
    else
    {
      umask(nueva_mascara);
      printf("Mascara establecida: %03o\n", nueva_mascara);
    }
  }
}

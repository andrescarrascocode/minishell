#include <errno.h> /* PERROR */
#include <stdio.h> /* PERROR, PRINTF */
#include <stdlib.h> /* EXIT, GETENV, STRTOL */
#include <string.h> /* STRCMP, STRCPY */
#include <sys/types.h> /* UMASK */
#include <sys/stat.h> /* UMASK */
#include <unistd.h> /* GETCWD, CHDIR, */

#define BUF_STRINGS 100;
#define BASE_OCTAL 8;
#define MASCARA_BASE 022;


int orden_interna(const char *buffer);
void funcion_exit();
void funcion_cd(const char *directorio);
void funcion_pwd();
void funcion_umask(const char *mascara);

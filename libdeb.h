#ifndef TD_LIBDEB_H
#define TD_LIBDEB_H

/*
 * Définit deux macros de debuggage et une fonction de 
 * sortie sur erreur :
 *
 * trace() (utilisable comme fonction) :
 *		affiche le nom du fichier source et le n° de la ligne 
 *		où cette fonction apparaît sous la forme :
 *			Fichier:Ligne>
 * debug(x) (utilisable comme fonction) :
 *		affiche le nom du fichier source, le n° de la ligne 
 *		et le message formaté (cf. printf) passé en argument, 
 *		où cette fonction apparaît, sous la forme :
 *			Fichier:Ligne> x
 * exit_error
 *		comme debug (ou printf), affiche le message formaté.
 *
 * Mise en œuvre : option de compilation :
 *		-DDEBUG
 *
 */
 
#include <stdio.h>

#ifdef DEBUG

#define debug(x, ...) do { \
	fprintf(stderr,"%s:%d:%s> ",__FILE__,__LINE__,__func__); \
	fprintf(stderr,x, ##__VA_ARGS__); \
	fprintf(stderr,"\n"); \
} while (0)

#define trace() do { \
	fprintf(stderr,"%s:%d> Trace\n",__FILE__,__LINE__); \
} while (0)

#else

#define debug(x, ...)
#define trace()

#endif

#define MAX_ERR 100

void exit_error(const char *fmt, ...);

#endif
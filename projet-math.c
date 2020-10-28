#include "libdeb.h"
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Pas de temps */
#define TPAS 250.0
/* Nombre de points */
#define POINTS 131072

/* Unités du système international : s, kg, m */
const double G = 6.67408e-11;
const double ML = 7.342e22;
const double MT = 5.9722e24;
const double MS = 1.989e30;
const double dTL = 3.844e8;
const double dST = 1.496e11;
const double vTL = 1.023e3;
const double vST = 3.0e4;
double t = 0.0;
int N;

typedef struct {
	char* nom;
	double m;
	double px, py;
	double vx, vy;
	double ax, ay;
} objet_t;

typedef struct {
	double x, y;
} coord_t;

void afficheTitre(objet_t* objets);
void afficher(objet_t* objets);
double distance(objet_t* o1, objet_t* o2);
coord_t forceGravitation(objet_t* o1, objet_t* o2);
double f_x(double t, double y, double z, objet_t* objets, int index);
double f_y(double t, double y, double z, objet_t* objets, int index);
void dupliquer(objet_t* objets, objet_t* r);
void simul(objet_t* objets);

void afficheTitre(objet_t* objets)
{
	int i;
	for (i = 0; i < N; i++) {
		printf("\t%s\t\t\t\t\t", objets[i].nom);
	}
	printf("\nt");
	for (i = 0; i < N; i++) {
		printf("\tpx\tpy\tvx\tvy\tax\tay");
	}
	printf("\n");
}

void afficher(objet_t* objets)
{
	int i;
	printf("%e", t);
	for (i = 0; i < N; i++) {
		printf("\t%e\t%e\t%e\t%e\t%e\t%e", objets[i].px, objets[i].py, objets[i].vx,
			objets[i].vy, objets[i].ax, objets[i].ay);
	}
	printf("\n");
}

double distance(objet_t* o1, objet_t* o2)
{
	return sqrt((o1->px - o2->px) * (o1->px - o2->px) + (o1->py - o2->py) * (o1->py - o2->py));
}

/* Force de gravitation de o1 appliquée sur o2 */
coord_t forceGravitation(objet_t* o1, objet_t* o2)
{
	coord_t coord;
	double dis = distance(o1, o2);
	coord.x = G * o1->m * o2->m * (o1->px - o2->px) / (dis * dis * dis);
	coord.y = G * o1->m * o2->m * (o1->py - o2->py) / (dis * dis * dis);
	return coord;
}

/* x" = f_x(t,x,x') */
double f_x(double t, double y, double z, objet_t* objets, int index)
{
	double f = 0, den;
	int i;
	for (i = 0; i < N; i++) {
		if (i == index)
			continue;
		den = (y - objets[i].px) * (y - objets[i].px) + (objets[index].py - objets[i].py) * (objets[index].py - objets[i].py);
		f += objets[i].m * (objets[i].px - y) / sqrt(den * den * den);
	}
	return G * f;
}

/* y" = f_y(t,y,y') */
double f_y(double t, double y, double z, objet_t* objets, int index)
{
	double f = 0, den;
	int i;
	for (i = 0; i < N; i++) {
		if (i == index)
			continue;
		den = (objets[index].px - objets[i].px) * (objets[index].px - objets[i].px) + (y - objets[i].py) * (y - objets[i].py);
		f += objets[i].m * (objets[i].py - y) / sqrt(den * den * den);
	}
	return G * f;
}

/* Copie de objets dans r */
void dupliquer(objet_t* objets, objet_t* r)
{
	int i;
	for (i = 0; i < N; i++) {
		r[i].m = objets[i].m;
		r[i].px = objets[i].px;
		r[i].py = objets[i].py;
		r[i].vx = objets[i].vx;
		r[i].vy = objets[i].vy;
		r[i].ax = objets[i].ax;
		r[i].ay = objets[i].ay;
	}
}

void simul(objet_t* objets)
{
	int i, j;
	unsigned long long k;
	double fx, fy;
	double k1, k2, k3, k4;
	coord_t f;
	objet_t copie[N];
	/* Calcul de l'accélération initiale */
	for (i = 0; i < N; i++) {
		fx = 0.0;
		fy = 0.0;
		for (j = 0; j < N; j++) {
			if (i == j)
				continue;
			f = forceGravitation(&objets[j], &objets[i]);
			fx += f.x;
			fy += f.y;
		}
		objets[i].ax = fx / objets[i].m;
		objets[i].ay = fy / objets[i].m;
	}
	afficheTitre(objets);
	afficher(objets);
	for (k = 1; k < POINTS; k++) {
		t += TPAS;
		dupliquer(objets, copie);
		for (i = 0; i < N; i++) {
			/* Intégration RK4 pour px et vx */
			k1 = f_x(t, objets[i].px, objets[i].vx, objets, i);
			k2 = f_x(t + TPAS / 2, objets[i].px + TPAS * objets[i].vx / 2,
				objets[i].vx + TPAS * k1 / 2, objets, i);
			k3 = f_x(t + TPAS / 2,
				objets[i].px + TPAS * objets[i].vx / 2 + TPAS * TPAS * k1 / 4,
				objets[i].vx + TPAS * k2 / 2, objets, i);
			k4 = f_x(t + TPAS,
				objets[i].px + TPAS * objets[i].vx + TPAS * TPAS * k2 / 2,
				objets[i].vx + TPAS * k3, objets, i);
			copie[i].px += TPAS * objets[i].vx + TPAS * TPAS * (k1 + k2 + k3) / 6;
			copie[i].vx += TPAS * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
			/* Intégration RK4 pour py et vy */
			k1 = f_y(t, objets[i].py, objets[i].vy, objets, i);
			k2 = f_y(t + TPAS / 2, objets[i].py + TPAS * objets[i].vy / 2,
				objets[i].vy + TPAS * k1 / 2, objets, i);
			k3 = f_y(t + TPAS / 2,
				objets[i].py + TPAS * objets[i].vy / 2 + TPAS * TPAS * k1 / 4,
				objets[i].vy + TPAS * k2 / 2, objets, i);
			k4 = f_y(t + TPAS,
				objets[i].py + TPAS * objets[i].vy + TPAS * TPAS * k2 / 2,
				objets[i].vy + TPAS * k3, objets, i);
			copie[i].py += TPAS * objets[i].vy + TPAS * TPAS * (k1 + k2 + k3) / 6;
			copie[i].vy += TPAS * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
			/* Calcul de l'accélération */
			fx = 0.0;
			fy = 0.0;
			for (j = 0; j < N; j++) {
				if (i == j)
					continue;
				f = forceGravitation(&objets[j], &objets[i]);
				fx += f.x;
				fy += f.y;
			}
			copie[i].ax = fx / objets[i].m;
			copie[i].ay = fy / objets[i].m;
		}
		dupliquer(copie, objets);
		afficher(objets);
	}
}

int main()
{
	objet_t objets[] = {
		/* nom, masse, px, py, vx, vy, ax, ay à l'état initial */
		{ "Soleil\0", MS, 0, 0, 0, 0, 0, 0 },
		{ "Terre\0", MT, -dST, 0, 0, -vST, 0, 0 },
		{ "Lune\0", ML, -dST - dTL, 0, 0, -vST - vTL, 0, 0 },
		//   {"Jupiter\0", 1.898e27, 7.785e11, 0, 0, 1.30572e4, 0, 0},
		//   {"Mercure\0", 3.285e23, -5.791e10, 0, 0, -4.736e4, 0, 0},
		//   {"Venus\0", 4.867e24, -1.082e11, 0, 0, -3.502e4, 0, 0},
		//   {"Mars\0", 6.39e23, -2.27e11, 0, 0, -2.413e4, 0, 0},
	};
	N = sizeof(objets) / sizeof(*objets);
	// setlocale(LC_NUMERIC, "fr_FR");
	simul(objets);
	return 0;
}

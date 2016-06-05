/*****************************************************************************
File: exercice1.c

Informatique Graphique IG1
IFIPS
Christian Jacquemin, Université Paris 11

Copyright (C) 2007 University Paris 11 
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#include <GL/gl.h>           
#include <GL/glu.h>         
#include <GL/glut.h>    

#include <stdio.h>      
#include <stdlib.h>     
#include <math.h>
#include <tiffio.h>     /* Sam Leffler's libtiff library. */
int writetiff(char *filename, char *description,
	      int x, int y, int width, int height, int compression);

#define    windowWidth 300
#define    windowHeight 300

#define RED   0
#define GREEN 0
#define BLUE  0
#define ALPHA 1

#define true  1
#define false 0

#define KEY_ESC 27

#define PI 3.1415926535898

#define position_Ini -60.0

float t = 0.f;
float delta = 1.f;
float k = 0.001f;
float K = 0.002f;
int IdleRunning = true;
int TraceEcran = false;
int RangFichierStockage = 0;
float position = position_Ini;

int  Ma_Tete;
int  Mon_Tronc;
int  Mon_Bras;
int  Mon_AvantBras;
int  Ma_Cuisse;
int  Mon_Mollet;
int  Mon_Repere;

enum lateralite{ Gauche = 0, Droit };

float angle_Bras[2];
float angle_AvantBras[2];
float angle_Cuisse[2];
float angle_Mollet[2];

float angle_Bras_Ini[2] = {-100.0, 100.0};
float angle_AvantBras_Ini[2] = {0.0, 90.0};
float angle_Cuisse_Ini[2] = {100.0, -100.0};
float angle_Mollet_Ini[2] = {0.0, -90.0};

float amplitude_Bras;
float amplitude_AvantBras;
float amplitude_Cuisse;
float amplitude_Mollet;
float med_Bras;
float med_AvantBras;
float med_Cuisse;
float med_Mollet;

static GLfloat mat_specular[] = { 1.0 , 1.0 , 1.0 , 1.0 };
static GLfloat mat_ambientanddiffuse[] = { 0.4, 0.4 , 0.0 , 1.0 };
static GLfloat mat_shininess[] = { 20.0};

static GLfloat light_position0[] = { 15.0 , 15.0 , 15.0 , 0.0 };
static GLfloat light_position1[] = { 15.0 , 15.0 , -15.0 , 0.0 };

static GLfloat ambient_light0[] = { 0.0 , 0.0 , 0.0 , 0.0 };
static GLfloat diffuse_light0[] = { 0.7 , 0.7 , 0.7 , 1.0 };
static GLfloat specular_light0[] = { 0.1 , 0.1 , 0.1 , 0.1 };

static GLfloat ambient_light1[] = { 0.50 , 0.50 , 0.50 , 1.0 };
static GLfloat diffuse_light1[] = { 0.5 , 1.0 , 1.0 , 1.0 };
static GLfloat specular_light1[] = { 0.5 , 1.0 , 1.0 , 1.0 };

int Step = 0;
int latence =4;

typedef struct Point {
	GLfloat x;
	GLfloat y;
	GLfloat z;
}Point;

void init_scene();
void render_scene();
void init_angles();
GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height); 
GLvoid window_key(unsigned char key, int x, int y); 
GLvoid window_timer(); 
void Faire_Composantes();
void Dessine_Repere();
GLvoid calcNorm(Point p1, Point p2, Point p3, GLfloat* norm);
GLvoid pointToVertex(struct Point p);

int main(int argc, char **argv) 
{  
  // initialisation  des paramètres de GLUT en fonction
  // des arguments sur la ligne de commande
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // définition et création de la fenêtre graphique
  glutInitWindowSize(windowWidth,windowHeight);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Avatar animé");

  // initialisation de OpenGL et de la scène
  initGL();  
  init_scene();

  // choix des procédures de callback pour 
  // le tracé graphique
  glutDisplayFunc(&window_display);
  // le redimensionnement de la fenêtre
  glutReshapeFunc(&window_reshape);
  // la gestion des événements clavier
  glutKeyboardFunc(&window_key);
  // fonction appelée régulièrement entre deux gestions dŽévénements
  glutTimerFunc(latence,&window_timer,Step);

  // la boucle prinicipale de gestion des événements utilisateur
  glutMainLoop();  

  return 1;
}

GLvoid calcNorm(Point p1, Point p2, Point p3, GLfloat* norm)
{
	GLfloat ax, ay, az, bx, by, bz, nx, ny, nz, l;
	
	ax = p2.x - p1.x;
	ay = p2.y - p1.y;
	az = p2.z - p1.z;
	
	bx = p3.x - p1.x;
	by = p3.y - p1.y;
	bz = p3.z - p1.z;
	
	nx = (ay * bz) - (az * by);
	ny = (az * bx) - (ax * bz);
	nz = (ax * by) - (ay * bx);
	
	l = sqrt(nx * nx + ny * ny + nz * nz);
	nx /= l;
	ny /= l;
	nz /= l;
	
	norm[0] = nx;
	norm[1] = ny;
	norm[2] = nz;
}

GLvoid pointToVertex(struct Point p)
{
	glVertex3f(p.x, p.y, p.z);
}

// initialisation du fond de la fenêtre graphique : noir opaque

GLvoid initGL() 
{
  // initialisation de lŽéclairement

  // définition de deux source lumineuses
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light0);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light1);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

  // activation de lŽéclairement
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  // propriétés matérielles des objets
  // glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambientanddiffuse);
  // glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  // glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glShadeModel( GL_SMOOTH );
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_POINT_SMOOTH);

  // initialisation du fond
  glClearColor(RED, GREEN, BLUE, ALPHA);        
  // z-buffer
  glEnable(GL_DEPTH_TEST);
}

void init_angles()
{
	angle_Bras[Droit] = angle_Bras_Ini[Droit];
	angle_Bras[Gauche] = angle_Bras_Ini[Gauche];
	angle_AvantBras[Droit] = angle_AvantBras_Ini[Droit];
	angle_AvantBras[Gauche] = angle_AvantBras_Ini[Gauche];
	angle_Cuisse[Droit] = angle_Cuisse_Ini[Droit];
	angle_Cuisse[Gauche] = angle_Cuisse_Ini[Gauche];
	angle_Mollet[Droit] = angle_Mollet_Ini[Droit];
	angle_Mollet[Gauche] = angle_Mollet_Ini[Gauche];
}

void init_scene()
{
  // initialise des display lists des composantes cylindriques du corps
  Faire_Composantes();
  init_angles();

  amplitude_Bras 
    = .5 * (angle_Bras_Ini[ Droit ] - angle_Bras_Ini[ Gauche ]);
  amplitude_AvantBras 
    = .5 * (angle_AvantBras_Ini[ Droit ] - angle_AvantBras_Ini[ Gauche ]);
  amplitude_Cuisse 
    = .5 * (angle_Cuisse_Ini[ Droit ] - angle_Cuisse_Ini[ Gauche ]);
  amplitude_Mollet 
    = .5 * (angle_Mollet_Ini[ Droit ] - angle_Mollet_Ini[ Gauche ]);
  med_Bras 
    = .5 * (angle_Bras_Ini[ Droit ] + angle_Bras_Ini[ Gauche ]);
  med_AvantBras 
    = .5 * (angle_AvantBras_Ini[ Droit ] + angle_AvantBras_Ini[ Gauche ]);
  med_Cuisse 
    = .5 * (angle_Cuisse_Ini[ Droit ] + angle_Cuisse_Ini[ Gauche ]);
  med_Mollet 
    = .5 * (angle_Mollet_Ini[ Droit ] + angle_Mollet_Ini[ Gauche ]);
}

// fonction de call-back pour lŽaffichage dans la fenêtre

GLvoid window_display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  render_scene();

  // trace la scène grapnique qui vient juste d'être définie
  glFlush();
}

// fonction de call-back pour le redimensionnement de la fenêtre

GLvoid window_reshape(GLsizei width, GLsizei height)
{  
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-20, 20, -20, 20, -1000, 1000);
  // glFrustum(-20, 20, -20, 20, 10, 1000);
  // glScalef(10, 10, 10);

  // toutes les transformations suivantes sŽappliquent au modèle de vue 
  glMatrixMode(GL_MODELVIEW);
}

// fonction de call-back pour la gestion des événements clavier

GLvoid window_key(unsigned char key, int x, int y) 
{  
  switch (key) {    
	case KEY_ESC:  
	    exit(1);                    
	    break; 
	case ' ':  
    	if (IdleRunning)
		{
    		glutTimerFunc(latence,NULL,Step);
			IdleRunning = false;
	    } 
	    else
		{
			glutTimerFunc(latence,&window_timer,Step);
    		IdleRunning = true;
    	}
    	break; 
	case '+':  
    	delta += .01;
    	break; 
	case '-':  
	    delta -= .01;
	    break; 
  	default:
    	printf ("La touche %d nŽest pas active.\n", key);
    	break;
  }     
}

// fonction de call-back appelée régulièrement

GLvoid window_timer() 
{
  // On effecture une variation des angles de chaque membre
  // de l'amplitude associée et de la position médiane
  // ********* A FAIRE **************
  angle_Bras[Droit] = med_Bras + amplitude_Bras * sin(position/10);
  angle_Bras[Gauche] = med_Bras - amplitude_Bras * sin(position/10);
  angle_AvantBras[Droit] = med_AvantBras + amplitude_AvantBras * sin(position/10);
  angle_AvantBras[Gauche] = med_AvantBras - amplitude_AvantBras * sin(position/10);
  angle_Cuisse[Droit] = med_Cuisse + amplitude_Cuisse * sin(position/10);
  angle_Cuisse[Gauche] = med_Cuisse - amplitude_Cuisse * sin(position/10);
  angle_Mollet[Droit] = med_Mollet + amplitude_Mollet * sin(position/10);
  angle_Mollet[Gauche] = med_Mollet - amplitude_Mollet * sin(position/10);

	

  // On déplace la position de l'avatar pour qu'il avance
  // ********* A FAIRE **************
	
	/*position += delta;*/

	glutTimerFunc(latence,&window_timer,++Step);

	glutPostRedisplay();
}

// un cylindre
void Faire_Composantes() {
  GLUquadricObj* /*GLAPIENTRY*/ qobj;
  GLfloat norm[3];
  Point A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, AA;

  // attribution des indentificateurs de display lists
  Ma_Tete =  glGenLists(6);
  Mon_Tronc = Ma_Tete + 1;
  Mon_Bras = Ma_Tete + 2;
  Mon_AvantBras = Ma_Tete + 3;
  Ma_Cuisse = Ma_Tete + 4;
  Mon_Mollet = Ma_Tete + 5;

  // compilation de la display list de la sphère
  // ********* A FAIRE **************
	glNewList(Ma_Tete, GL_COMPILE);
		/*glColor3f(0.7, 0, 1);*/
		glutSolidSphere(1.5, 10, 10);
	glEndList();

  // allocation dŽune description de quadrique
  qobj = gluNewQuadric();
  // la quadrique est pleine 
  gluQuadricDrawStyle(qobj, GLU_FILL); 
  // les ombrages, sŽil y en a, sont doux
  gluQuadricNormals(qobj, GLU_SMOOTH);

  // compilation des display lists des cylindres
  // ********* A FAIRE **************
	glNewList(Mon_Tronc, GL_COMPILE);
		/*glColor3f(0, 1, 1);*/
		//gluCylinder(qobj, 2.5, 2.5, 7.0, 10, 10);
		A.x = -5;	A.y = 1.5;	A.z = 15;
		B.x = -2;	B.y = 2;	B.z = 16;
		D.x = 2;	D.y = 2;	D.z = 16;
		E.x = 5;	E.y = 1.5;	E.z = 15;
		F.x = -3;	F.y = 1.5;	F.z = 6;
		G.x = -1.4;	G.y = 2;	G.z = 6;
		I.x = 1.4;	I.y = 2;	I.z = 6;
		J.x = 3;	J.y = 1.5;	J.z = 6;
		K.x = -4;	K.y = 1.5;	K.z = 2;
		L.x = -.5;	L.y = 1.5;	L.z = 0;
		N.x = .5;	N.y = 1.5;	N.z = 0;
		O.x = 4;	O.y = 1.5;	O.z = 2;
		P.x = -5;	P.y = -1.5;	P.z = 15;
		Q.x = -2;	Q.y = -2;	Q.z = 16;
		R.x = 2;	R.y = -2;	R.z = 16;
		S.x = 5;	S.y = -1.5;	S.z = 15;
		T.x = -2;	T.y = -1.5;	T.z = 6;
		U.x = -1;	U.y = -2;	U.z = 6;
		V.x = 1;	V.y = -2;	V.z = 6;
		W.x = 2;	W.y = -1.5;	W.z = 6;
		X.x = -3.5;	X.y = -1.5;	X.z = 2;
		Y.x = -.5;	Y.y = -1.5;	Y.z = 0;
		Z.x = .5;	Z.y = -1.5;	Z.z = 0;
		AA.x = 3.5;	AA.y = -1.5;	AA.z = 2;
		glBegin(GL_TRIANGLES);
			calcNorm(L, K, F, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(K);	//K
			pointToVertex(F);	//F
			pointToVertex(L);	//L
			
			calcNorm(L, F, G, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(F);	//F
			pointToVertex(L);	//L
			pointToVertex(G);	//G
			
			calcNorm(L, G, N, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(L);	//L
			pointToVertex(G);	//G
			pointToVertex(N);	//N
			
			calcNorm(N, G, I, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(G);	//G
			pointToVertex(N);	//N
			pointToVertex(I);	//I
			
			calcNorm(N, I, J, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(N);	//N
			pointToVertex(I);	//I
			pointToVertex(J);	//J
			
			calcNorm(N, J, O, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(N);	//N
			pointToVertex(J);	//J
			pointToVertex(O);	//O
		glEnd();
		glBegin(GL_TRIANGLES);
			calcNorm(F, A, G, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(F);	//F
			pointToVertex(A);	//A
			pointToVertex(G);	//G
			
			calcNorm(A, B, G, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(A);	//A
			pointToVertex(G);	//G
			pointToVertex(B);	//B
			
			calcNorm(G, B, D, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(G);		//G
			pointToVertex(B);		//B
			pointToVertex(D);		//D
			
			calcNorm(G, D, I, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(G);		//G
			pointToVertex(D);		//D
			pointToVertex(I);		//I
			
			calcNorm(I, D, E, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(D);		//D
			pointToVertex(I);		//I
			pointToVertex(E);		//E
			
			calcNorm(I, E, J, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(I);		//I
			pointToVertex(E);		//E
			pointToVertex(J);		//J
		glEnd();
		glBegin(GL_TRIANGLES);
			calcNorm(U, P, T, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(P);	//P
			pointToVertex(T);	//T
			pointToVertex(U);		//U
			
			calcNorm(Q, P, U, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(P);	//P
			pointToVertex(Q);		//Q
			pointToVertex(U);		//U
			
			calcNorm(R, Q, U, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(Q);		//Q
			pointToVertex(U);		//U
			pointToVertex(R);		//R
			
			calcNorm(R, U, V, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(U);		//U
			pointToVertex(R);		//R
			pointToVertex(V);		//V
			
			calcNorm(S, R, V, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(R);		//R
			pointToVertex(V);		//V
			pointToVertex(S);	//S
			
			calcNorm(S, V, W, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(V);		//V
			pointToVertex(S);	//S
			pointToVertex(W);		//W
		glEnd();
		glBegin(GL_TRIANGLES);
			calcNorm(T, X, Y, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(X);	//X
			pointToVertex(T);	//T
			pointToVertex(Y);	//Y
			
			calcNorm(T, Y, U, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(T);	//T
			pointToVertex(Y);	//Y
			pointToVertex(U);		//U
			
			calcNorm(U, Y, Z, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(Y);	//Y
			pointToVertex(U);		//U
			pointToVertex(Z);		//Z
			
			calcNorm(U, Z, V, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(U);		//U
			pointToVertex(Z);		//Z
			pointToVertex(V);		//V
			
			calcNorm(V, Z, W, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(Z);		//Z
			pointToVertex(V);		//V
			pointToVertex(W);		//W
			
			calcNorm(W, Z, AA, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(W);		//W
			pointToVertex(Z);		//Z
			pointToVertex(AA);		//AA
		glEnd();
		glBegin(GL_TRIANGLES);
			calcNorm(P, A, F, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(A);	//A
			pointToVertex(P);	//P
			pointToVertex(F);	//F
			
			calcNorm(P, F, T, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(P);	//P
			pointToVertex(F);	//F
			pointToVertex(T);	//T
			
			calcNorm(T, F, K, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(F);	//F
			pointToVertex(T);	//T
			pointToVertex(K);	//K
			
			calcNorm(T, K, X, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(T);	//T
			pointToVertex(K);	//K
			pointToVertex(X);	//X
			
			calcNorm(X, K, L, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(K);	//K
			pointToVertex(X);	//X
			pointToVertex(L);	//L
			
			calcNorm(X, L, Y, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(X);	//X
			pointToVertex(L);	//L
			pointToVertex(Y);	//Y
			
			calcNorm(Y, L, N, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(L);	//L
			pointToVertex(Y);	//Y
			pointToVertex(N);	//N
			
			calcNorm(Y, N, Z, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(Y);	//Y
			pointToVertex(N);	//N
			pointToVertex(Z);	//Z
			
			calcNorm(Z, N, O, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(N);	//N
			pointToVertex(Z);	//Z
			pointToVertex(O);	//O
			
			calcNorm(Z, O, AA, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(Z);	//Z
			pointToVertex(O);	//O
			pointToVertex(AA);	//AA
			
			calcNorm(AA, O, J, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(O);	//O
			pointToVertex(AA);	//AA
			pointToVertex(J);	//J
			
			calcNorm(AA, J, W, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(AA);	//AA
			pointToVertex(J);	//J
			pointToVertex(W);	//W
			
			calcNorm(W, J, E, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(J);	//J
			pointToVertex(W);	//W
			pointToVertex(E);	//E
			
			calcNorm(W, E, S, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(W);	//W
			pointToVertex(E);	//E
			pointToVertex(S);	//S
			
			calcNorm(S, E, D, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(E);	//E
			pointToVertex(S);	//S
			pointToVertex(D);	//D
			
			calcNorm(S, D, R, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(S);	//S
			pointToVertex(D);	//D
			pointToVertex(R);	//R
			
			calcNorm(R, D, B, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(D);	//D
			pointToVertex(R);	//R
			pointToVertex(B);	//B
			
			calcNorm(R, B, Q, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(R);	//R
			pointToVertex(B);	//B
			pointToVertex(Q);	//Q
			
			calcNorm(Q, B, A, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(B);	//B
			pointToVertex(Q);	//Q
			pointToVertex(A);	//A
			
			calcNorm(Q, A, P, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(Q);	//Q
			pointToVertex(A);	//A
			pointToVertex(P);	//P
		glEnd();
		
		glPushMatrix();
			glRotatef(30, 0, 1, 0);
			glTranslatef(-2.1, 0, 0);
			glScalef(1, .6, .75);
			glutSolidSphere(2, 10, 10);
			glPopMatrix();
		
		glPushMatrix();
			glRotatef(-30, 0, 1, 0);
			glTranslatef(2.1, 0, 0);
			glScalef(1, .6, .75);
			glutSolidSphere(2, 10, 10);
			glPopMatrix();
		
	glEndList();
	glNewList(Mon_Bras, GL_COMPILE);
		/*glColor3f(0, 1, 1);*/
		gluCylinder(qobj, 0.5, 0.5, 5.0, 10, 10);
	glEndList();
	glNewList(Mon_AvantBras, GL_COMPILE);
		/*glColor3f(0.7, 0, 1);*/
		gluCylinder(qobj, 0.45, 0.25, 5.0, 10, 10);
	glEndList();
	glNewList(Ma_Cuisse, GL_COMPILE);
		/*glColor3f(1, 1, 0);*/
		/*gluCylinder(qobj, 1.25, 0.75, 5.0, 10, 10);*/
		A.x = -1.5;	A.y = 1.5;	A.z = 1;
		B.x = 2;	B.y = 1.5;	B.z = -1;
		C.x = -1.5;	C.y = 1;	C.z = -7;
		D.x = 1.5;	D.y = 1;	D.z = -7;
		E.x = -1;	E.y = -1.5;	E.z = 1;
		F.x = 2;	F.y = -1.5;	F.z = -1;
		G.x = -1.5;	G.y = -1;	G.z = -7;
		H.x = 1.5;	H.y = -1;	H.z = -7;
		glBegin(GL_TRIANGLES);
			calcNorm(A, B, D, norm);
			glNormal3f(norm[0], norm[1], norm[2]);
			pointToVertex(A);	//A
			pointToVertex(B);	//B
			pointToVertex(D);	//D
		glEnd();
	glEndList();
	glNewList(Mon_Mollet, GL_COMPILE);
		/*glColor3f(0.7, 0, 1);*/
		gluCylinder(qobj, 0.70, 0.25, 5.0, 10, 10);
	glEndList();
}

void  Dessine_Repere() {
  glNewList(Mon_Repere, GL_COMPILE);
    glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);
      glVertex3f(-10 , 0 , 0);
      glVertex3f(10 , 0 , 0);
    glEnd();
    glBegin(GL_LINES);
      glColor3f(0.0, 1.0, 0.0);
      glVertex3f(0 , -10 , 0);
      glVertex3f(0 , 10 , 0);
    glEnd();
    glPointSize( 10.0 );
    glBegin(GL_POINTS);
      glColor3f(1.0, 1.0, 1.0);
      glVertex3f(10.0 , 0 , 0);
    glEnd();
    glBegin(GL_POINTS);
      glColor3f(1.0, 1.0, 1.0);
      glVertex3f(0 , 10.0 , 0);
    glEnd();
  glEndList();
}

void render_scene()
{

  // rotation de 90 degres autour de Ox pour mettre l'axe Oz 
  // vertical comme sur la figure
  glRotatef(-90, 1, 0, 0);

  // rotation de 160 degres autour de l'axe Oz pour faire
  // avancer l'avatar vers le spectateur
  glRotatef(380*delta, 0, 0, 1);

  // rotation de 25 degres autour de la bissectrice de $Oy$ pour
  // voir la figure en perspective
  glRotatef(25, -1, 0, 0);


  // déplacement horizontal selon lŽaxe Oy pour donner 
  // une impression de déplacement horizontal accompagnant
  // la marche
  glTranslatef(0, 0, 4*fabs(sin(position/10)));
  glRotatef(30, 0, 0, 1);

  // tracé du tronc, aucune transformation nŽest
  // requise
  glColor3f(0.6, 0.1, 0.1);
  glPushMatrix();
  	glScalef(0.5, 0.5, 0.5);
  	glCallList(Mon_Tronc);
  	glPopMatrix();
  
  // tracé de la tête avec une translation positive
  // selon Oz pour la placer au-dessus du tronc
  // les appels à glPushMatrix et glPopMatrix servent 
  // à sauvegarder et restaurer le contexte graphique
  // ********* A FAIRE **************
  glColor3f(0.95, 0.87, 0.7);
	glPushMatrix();
	glTranslatef(0, 0, 9.5);
	glCallList(Ma_Tete);
	glPopMatrix();

  // tracé de la cuisse droite avec une translation vers
  // la droite et une rotation de 180° autour de Ox
  // pour lŽorienter vers le bas
  // ********* A FAIRE **************
	glPushMatrix();
	glTranslatef(1.25, 0, 0);
	glRotatef(180+angle_Cuisse[Droit], 1, 0, 0);
	glColor3f(0.3, 0.2, 0.1);
	glCallList(Ma_Cuisse);

  // pour tracer le mollet, on reste dans le même
  // contexte graphique et on translate de
  // +5 selon Oz afin de mettre le repère au niveau
  // du genou
  // ********* A FAIRE **************
    glColor3f(0.95, 0.87, 0.7);
	glTranslatef(0, 0, 5);
	glRotatef(angle_Mollet[Droit], 1, 0, 0);
	glCallList(Mon_Mollet);
	glPopMatrix();

  // cuisse et mollet gauches
  // seule la translation initiale change
  // ********* A FAIRE **************
	glPushMatrix();
	glTranslatef(-1.25, 0, 0);
	glRotatef(180+angle_Cuisse[Gauche], 1, 0, 0);
	glColor3f(0.3, 0.2, 0.1);
	glCallList(Ma_Cuisse);
	glTranslatef(0, 0, 5);
	glRotatef(angle_Mollet[Gauche], 1, 0, 0);
	glColor3f(0.95, 0.87, 0.7);
	glCallList(Mon_Mollet);
	glPopMatrix();
	
  // tracé du bras droit avec une translation vers
  // la droite et vers le haut composée avec une 
  // rotation de 180° autour de Ox pour lŽorienter
  // vers le bas
  // ********* A FAIRE **************
	glPushMatrix();
	glTranslatef(3, 0, 7);
	glRotatef(180+angle_Bras[Droit], 1, 0, 0);
	glColor3f(0.6, 0.1, 0.1);
	glCallList(Mon_Bras);

  // pour tracer lŽavant-bras, on reste dans le même
  // contexte graphique et on translate de
  // +5 selon Oz afin de mettre le repère au niveau
  // du coude
  // ********* A FAIRE **************
    glColor3f(0.95, 0.87, 0.7);
	glTranslatef(0, 0, 5);
	glRotatef(angle_AvantBras[Droit], 1, 0, 0);
	glCallList(Mon_AvantBras);
	glPopMatrix();

  // bras et avant-bras gauches
  // seule la translation initiale change
  // ********* A FAIRE **************
	glPushMatrix();
	glTranslatef(-3, 0, 7);
	glRotatef(180+angle_Bras[Gauche], 1, 0, 0);
	glColor3f(0.6, 0.1, 0.1);
	glCallList(Mon_Bras);
	glTranslatef(0, 0, 5);
	glRotatef(angle_AvantBras[Gauche], 1, 0, 0);
	glColor3f(0.95, 0.87, 0.7);
	glCallList(Mon_AvantBras);
	glPopMatrix();

  // permutation des buffers lorsque le tracé est achevé
  glutSwapBuffers();
}


#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define DELTA_B 10
#define DELTA_L 10

#define ANGLE_KF M_PI / 180

double CAMERA_RADIUS = 15.0;
double FIGURE_RADIUS = 10.0;

double YZ_ANGLE = M_PI / 100;
double ZX_ANGLE = M_PI / 100;

float OBJ_X = 0, OBJ_Y = 0, OBJ_Z = 0;
float LIGHT_X = 0, LIGHT_Y = 0, LIGHT_Z = 0;

void init();

double getSphereX(int B, int L);
double getSphereY(int B, int L);
double getSphereZ(int B);
double displaceZ(double z);

void display();

void drawDots();

void drawCarcas();
void drawMeridians();
void drawParallels();

void OnKeyboard(int key, int x, int y);
void onReshape(int width, int height) {}

void setCameraPosition();
double getXEye();
double getYEye();
double getZEye();

int main(int argc, char **argv) {
  if (argc > 1)
    FIGURE_RADIUS = atof(argv[2]);
  else
    FIGURE_RADIUS = 3;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(50, 10);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("RGZ - a drop of water");
  glutReshapeFunc(onReshape);
  init();
  glutSpecialFunc(OnKeyboard);
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutMainLoop();
  return 0;
}

void init() {
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
}

double getSphereX(int B, int L) {
  return FIGURE_RADIUS * cos(B * ANGLE_KF) * sin(L * ANGLE_KF);
}
double getSphereY(int B, int L) {
  return FIGURE_RADIUS * cos(B * ANGLE_KF) * cos(L * ANGLE_KF);
}
double getSphereZ(int B) { return FIGURE_RADIUS * sin(B * ANGLE_KF); }

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1, 1, -1, 1, 2, 100);
  setCameraPosition();
  glMatrixMode(GL_MODELVIEW);

  glColor3d(255, 130, 255);
  glLineWidth(3);

  glPointSize(1);

  drawCarcas();

  glutSwapBuffers();
}

void drawCarcas() {
  drawMeridians();
  drawParallels();
}

double displaceZ(double z) {
  return 2.5 * FIGURE_RADIUS * (z / FIGURE_RADIUS - 0.5) *
         (z / FIGURE_RADIUS - 0.5);
}

void drawMeridians() {
  glBegin(GL_LINE_STRIP);
  for (int L = 0; L <= 360; L += DELTA_L) {
    for (int B = -90; B <= 90; B += DELTA_B) {
      double z = getSphereZ(B) + OBJ_Z;
      if (z > FIGURE_RADIUS / 2) z += displaceZ(z);
      glVertex3d(getSphereY(B, L) + OBJ_Y, z, getSphereX(B, L) + OBJ_X);
    }
  }
  glEnd();
}
void drawParallels() {
  glBegin(GL_LINE_STRIP);
  for (int B = -90; B <= 90; B += DELTA_B) {
    for (int L = 0; L <= 360; L += DELTA_L) {
      double z = getSphereZ(B) + OBJ_Z;
      if (z > FIGURE_RADIUS / 2) z += displaceZ(z);

      glVertex3d(getSphereY(B, L) + OBJ_Y, z, getSphereX(B, L) + OBJ_X);
    }
  }
  glEnd();
}

void setCameraPosition() {
  double a = cos(YZ_ANGLE);
  gluLookAt(getXEye(), getYEye(), getZEye(), 0, 0, 0, 0,
            YZ_ANGLE < 0 || YZ_ANGLE > M_PI ? -1 : 1, 0);
}

double getXEye() { return CAMERA_RADIUS * sin(YZ_ANGLE) * sin(ZX_ANGLE); }

double getYEye() { return CAMERA_RADIUS * cos(YZ_ANGLE); }

double getZEye() { return CAMERA_RADIUS * sin(YZ_ANGLE) * cos(ZX_ANGLE); }

void OnKeyboard(int key, int x, int y) {
  if (key == GLUT_KEY_UP) YZ_ANGLE -= M_PI / 10;

  if (key == GLUT_KEY_DOWN) YZ_ANGLE += M_PI / 10;

  if (key == GLUT_KEY_LEFT) ZX_ANGLE -= M_PI / 10;

  if (key == GLUT_KEY_RIGHT) ZX_ANGLE += M_PI / 10;

  if (YZ_ANGLE > M_PI) YZ_ANGLE = -M_PI;

  if (YZ_ANGLE < -M_PI) YZ_ANGLE = M_PI;

  if (YZ_ANGLE == 0) YZ_ANGLE -= 0.001;

  if (ZX_ANGLE > M_PI * 2) ZX_ANGLE -= M_PI * 2;

  if (ZX_ANGLE < -M_PI * 2) ZX_ANGLE *= -1;
}

void drawDots() {
  double h = 2.5;
  double kf = 1.5;

  glBegin(GL_POINTS);

  for (double B = 0; B < 2 * M_PI; B += M_PI / 100) {
    for (double L = 0; L < 2 * M_PI; L += M_PI / 100) {
      double x = FIGURE_RADIUS * cos(B) * sin(L) + OBJ_X;
      double y = FIGURE_RADIUS * cos(B) * cos(L) + OBJ_Y;
      double z = FIGURE_RADIUS * sin(B);

      if (z > FIGURE_RADIUS / 2) {
        z += h * (z / FIGURE_RADIUS - 0.5) * (z / FIGURE_RADIUS - 0.5);
      }

      glVertex3d(x, y, z);
    }
  }

  glEnd();
}
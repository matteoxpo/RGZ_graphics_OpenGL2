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

#define COLORFULL_NO_LIGHT_POLYGON 'c'
#define COLORFULL_LIGHT_POLYGON 'l'
#define TEXTURE_POLYGON 't'

double CAMERA_RADIUS = 15.0;
double FIGURE_RADIUS = 10.0;

double YZ_ANGLE = M_PI / 100;
double ZX_ANGLE = M_PI / 100;

void init();

double getSphereX(int B, int L);
double getSphereY(int B, int L);
double getSphereZ(int B, int L);
double addBiasZ(double z);

void display();

void drawDots();

void drawCarcas();
void drawMeridians();
void drawParallels();

void drawPolygon(char mode);
// double* getPolygon(double* res, int B, int L,
//                    double (*getSphere)(int b, int l));

double* getPolygon(double* res, int B, int L,
                   double (*getSphere)(int b, int l)) {
  res[0] = getSphere(B, L);
  res[1] = getSphere(B + DELTA_B, L);
  res[2] = getSphere(B + DELTA_B, L + DELTA_L);
  res[3] = getSphere(B, L + DELTA_L);

  return res;
}

void OnKeyboard(int key, int x, int y);
void onReshape(int width, int height) {}

void setCameraPosition();
double getXEye();
double getYEye();
double getZEye();

int main(int argc, char** argv) {
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
double getSphereZ(int B, int L) {
  double z = FIGURE_RADIUS * sin(B * ANGLE_KF);
  if (z > FIGURE_RADIUS / 2) z += addBiasZ(z);
  return z;
}

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

double addBiasZ(double z) {
  return 2.5 * FIGURE_RADIUS * (z / FIGURE_RADIUS - 0.5) *
         (z / FIGURE_RADIUS - 0.5);
}

void drawPolygon(char mode) {
  double* x = (double*)malloc(4 * sizeof(double));
  double* y = (double*)malloc(4 * sizeof(double));
  double* z = (double*)malloc(4 * sizeof(double));
  double normal[3];

  for (int L = 0; L <= 360; L += DELTA_L) {
    for (int B = -90; B <= 90; B += DELTA_B) {
      x = getPolygon(x, B, L, getSphereX);
      y = getPolygon(y, B, L, getSphereY);
      z = getPolygon(z, B, L, getSphereZ);
      switch (mode) {
        case TEXTURE_POLYGON:
          break;
        case COLORFULL_LIGHT_POLYGON:
          break;
        case COLORFULL_NO_LIGHT_POLYGON:
          break;
        default:
          printf("Ошибка выбора модификации\n");
          return;
      }
    }
  }
  if (x != NULL) free(x);
  if (y != NULL) free(y);
  if (z != NULL) free(z);
}

void drawMeridians() {
  glBegin(GL_LINE_STRIP);
  for (int L = 0; L <= 360; L += DELTA_L)
    for (int B = -90; B <= 90; B += DELTA_B)
      glVertex3d(getSphereY(B, L), getSphereZ(B, L), getSphereX(B, L));
  glEnd();
}
void drawParallels() {
  glBegin(GL_LINE_STRIP);
  for (int B = -90; B <= 90; B += DELTA_B)
    for (int L = 0; L <= 360; L += DELTA_L)
      glVertex3d(getSphereY(B, L), getSphereZ(B, L), getSphereX(B, L));
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
      double x = FIGURE_RADIUS * cos(B) * sin(L);
      double y = FIGURE_RADIUS * cos(B) * cos(L);
      double z = FIGURE_RADIUS * sin(B);

      if (z > FIGURE_RADIUS / 2) {
        z += h * (z / FIGURE_RADIUS - 0.5) * (z / FIGURE_RADIUS - 0.5);
      }

      glVertex3d(x, y, z);
    }
  }

  glEnd();
}
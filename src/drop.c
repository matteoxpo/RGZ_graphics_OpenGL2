#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define DELTA_B 10
#define DELTA_L 10

#define TEX_STEP_X ((DELTA_B + 0.0) / 180.0)
#define TEX_STEP_Y ((DELTA_L + 0.0) / 360.0)

#define ANGLE_KF (M_PI / 180)

#define COLORFULL_NO_LIGHT_POLYGON_TYPE 'c'
#define COLORFULL_LIGHT_POLYGON_TYPE 'l'
#define TEXTURE_POLYGON_TYPE 't'

typedef enum {
  CarcasMode = '1',
  ColorfullCarcasMode = '2',
  ColorfullMode = '3',
  PerspectiveProjectionMode = '4',
  AxonometricProjectionMode = '5',
  TextureMode = '6'

} WorkingMode;

WorkingMode MODE = ColorfullCarcasMode;

double CAMERA_RADIUS = 15.0;
double FIGURE_RADIUS = 10.0;

int TEXTURE;

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

double* getPolygon(double* res, int B, int L,
                   double (*getSphere)(int b, int l));

void createTexture(const char image_src[]);

void OnKeyboardRotate(int key, int x, int y);
void OnKeyBoardSwitchMode(unsigned char key, int x, int y);

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

  glutKeyboardFunc(OnKeyBoardSwitchMode);
  glutSpecialFunc(OnKeyboardRotate);

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
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  GLfloat mat_specular[] = {0.0, 0.0, 1.0, 1.0};
  GLfloat mat_shininess[] = {50.0};

  float lpos[] = {20, 20, 20, 1.0f};

  glShadeModel(GL_SMOOTH);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, lpos);
  glColor3f(0.3, 0.5, 0.7);

  glLineWidth(3);
  glPointSize(1);

  createTexture("../Assets/water.jpg");
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glFrustum(-1, 1, -1, 1, 2, 100);
  setCameraPosition();

  switch (MODE) {
    case ColorfullCarcasMode:
      glMatrixMode(GL_PROJECTION);
      glEnable(GL_NORMALIZE);
      drawPolygon(COLORFULL_LIGHT_POLYGON_TYPE);
      glMatrixMode(GL_MODELVIEW);
    case CarcasMode:
      drawCarcas();
      break;
    case ColorfullMode:
      glMatrixMode(GL_PROJECTION);
      glEnable(GL_NORMALIZE);
      drawPolygon(COLORFULL_LIGHT_POLYGON_TYPE);
      glMatrixMode(GL_MODELVIEW);
      break;
    case TextureMode:
      glDisable(GL_LIGHTING);
      glMatrixMode(GL_PROJECTION);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, TEXTURE);
      drawPolygon(TEXTURE_POLYGON_TYPE);
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_LIGHTING);
      break;
    case PerspectiveProjectionMode:
      break;
    case AxonometricProjectionMode:
      break;
  }
  glLoadIdentity();

  glutSwapBuffers();
}

void drawPolygon(char type) {
  double* x = (double*)calloc(4, sizeof(double));
  double* y = (double*)calloc(4, sizeof(double));
  double* z = (double*)calloc(4, sizeof(double));
  double a[3];
  double b[3];
  GLfloat* normal = (GLfloat*)calloc(3, sizeof(GLfloat));
  float texX = 0, texY = 0;

  for (int L = 0; L <= 360; L += DELTA_L) {
    for (int B = -90; B <= 90; B += DELTA_B) {
      x = getPolygon(x, B, L, getSphereX);
      y = getPolygon(y, B, L, getSphereY);
      z = getPolygon(z, B, L, getSphereZ);
      normal[0] = a[1] * b[2] - a[2] * b[1];
      normal[1] = a[0] * b[2] - a[2] * b[0];
      normal[2] = a[0] * b[1] - a[1] * b[0];
      switch (type) {
        case TEXTURE_POLYGON_TYPE:
          glBegin(GL_POLYGON);
          glNormal3fv(normal);
          glTexCoord2d(texX, texY);
          glVertex3d(y[0], z[0], x[0]);
          glTexCoord2d(texX, texY + TEX_STEP_Y);
          glVertex3d(y[1], z[1], x[1]);
          glTexCoord2d(texX + TEX_STEP_X, texY + TEX_STEP_Y);
          glVertex3d(y[2], z[2], x[2]);
          glTexCoord2d(texX + TEX_STEP_X, texY);
          glVertex3d(y[3], z[3], x[3]);
          glEnd();
          break;
        case COLORFULL_LIGHT_POLYGON_TYPE:
          a[0] = x[1] - x[0];
          a[1] = y[1] - y[0];
          a[2] = z[1] - z[0];
          b[0] = x[2] - x[0];
          b[1] = y[2] - y[0];
          b[2] = z[2] - z[0];
          glBegin(GL_QUADS);
          glNormal3fv(normal);
          for (int i = 0; i < 4; i++) glVertex3d(y[i], z[i], x[i]);
          glEnd();
          break;
        case COLORFULL_NO_LIGHT_POLYGON_TYPE:
          glBegin(GL_POLYGON);
          for (int i = 0; i < 4; i++) glVertex3d(y[i], z[i], x[i]);
          glEnd();
          break;
      }
      texX += TEX_STEP_X;
    }
    texY += TEX_STEP_Y;
    texX = 0;
  }
  if (x != NULL) free(x);
  if (y != NULL) free(y);
  if (z != NULL) free(z);
}

double* getPolygon(double* res, int B, int L,
                   double (*getSphere)(int b, int l)) {
  res[0] = getSphere(B, L);
  res[1] = getSphere(B + DELTA_B, L);
  res[2] = getSphere(B + DELTA_B, L + DELTA_L);
  res[3] = getSphere(B, L + DELTA_L);

  return res;
}

void drawCarcas() {
  drawMeridians();
  drawParallels();
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

double addBiasZ(double z) {
  return 2.5 * FIGURE_RADIUS * (z / FIGURE_RADIUS - 0.5) *
         (z / FIGURE_RADIUS - 0.5);
}

void createTexture(const char image_src[]) {
  glGenTextures(1, &TEXTURE);
  glBindTexture(GL_TEXTURE_2D, TEXTURE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  unsigned char* data =
      SOIL_load_image(image_src, &width, &height, 0, SOIL_LOAD_RGB);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);

  SOIL_free_image_data(data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void setCameraPosition() {
  double a = cos(YZ_ANGLE);
  gluLookAt(getXEye(), getYEye(), getZEye(), 0, 0, 0, 0,
            YZ_ANGLE < 0 || YZ_ANGLE > M_PI ? -1 : 1, 0);
}

double getXEye() { return CAMERA_RADIUS * sin(YZ_ANGLE) * sin(ZX_ANGLE); }

double getYEye() { return CAMERA_RADIUS * cos(YZ_ANGLE); }

double getZEye() { return CAMERA_RADIUS * sin(YZ_ANGLE) * cos(ZX_ANGLE); }

void OnKeyBoardSwitchMode(unsigned char key, int x, int y) {
  if (key == CarcasMode || key == ColorfullCarcasMode ||
      key == PerspectiveProjectionMode || key == AxonometricProjectionMode ||
      key == TextureMode || ColorfullMode)
    MODE = key;
}

void OnKeyboardRotate(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP:
      YZ_ANGLE -= M_PI / 10;
      break;
    case GLUT_KEY_DOWN:
      YZ_ANGLE += M_PI / 10;
      break;
    case GLUT_KEY_LEFT:
      ZX_ANGLE -= M_PI / 10;
      break;
    case GLUT_KEY_RIGHT:
      ZX_ANGLE += M_PI / 10;
      break;
  }

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
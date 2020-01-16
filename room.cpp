#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void exit(int);

//using names for their array number
#define BURT 0
#define WAVES 1
#define QUANTUM 2
#define JELLY 3
#define ORANGE 4
#define COMO 5
#define TEX_COUNT 6   //Number of image files/textures
#define MAX_PTS  12   /* fixed maximum number of control points */

static int n,m;
static float s;
static int width = 800, height = 600;

//arrays of images
GLuint *image[TEX_COUNT];

//array of textures
GLuint textures[TEX_COUNT];

//array of file names
const char *tex[TEX_COUNT] = {"burt.ppm","waves.ppm","quantum.ppm","jelly.ppm","orange.ppm","como.ppm"};


//b-spline to bezier matrix
GLfloat bspline[] = {(1.0/6.0), (4.0/6.0), (1.0/6.0), 0,
    0, (4.0/6.0), (2.0/6.0), 0,
    0, (2.0/6.0), (4.0/6.0), 0,
    0, (1.0/6.0), (4.0/6.0), (1.0/6.0)};

//interp to bezier matrix
GLfloat interp[]={1, 0, 0, 0,
    (-5/6), 3,  (-3/2), (1/3),
    (1/3), (-3/2), 3, (-5/6),
    0, 0,  0, 1};


//first set of control points
GLfloat ctrlpoints[4][3] = {{-1,0,0},{-0.8333,0.25,0},{-0.6667,0.25,0},
    {-0.5,0,0}};
GLfloat ctrlpoints1[4][3] = {{-0.5,0,0},{-0.4167,-0.25,0},{-0.3333,-0.25,0},
    {-0.25,0,0}};
GLfloat ctrlpoints2[4][3] = {{-0.25,0,0},{-0.16667,0.25,0},{-0.083333,0.5,0},
    {0,0,0}};
GLfloat ctrlpoints3[4][3] = {{0,0,0},{0.25, -0.16667,0},{0.25,-0.3333,0},
    {0,-0.5,0}};

//second curve set of control points
GLfloat ctrlpoints4[4][3] = {{-0.5,0.5,1},{-0.5,0.15,0.8333},
    {-0.5,0.15,0.66667},{-0.5,0.5,0.5}};
GLfloat ctrlpoints5[4][3] = {{-0.5,0.5,0.5},{-0.417,0.65,0.4167},
    {-0.35,0.65,0.3333}, {-0.25,0.5,0.25}};
GLfloat ctrlpoints6[4][3] = {{-0.25,0.5,0.25}, {-0.17,0.35,0.16667},
    {-0.083,0.15,0.083}, {0,0.5,0}};
GLfloat ctrlpoints7[4][3] = {{0,0.5,0},{0,0.35,-0.16667},{0,0.25,-0.3333},
    {0,0.5,-0.5}};


//third curve divided into control points
GLfloat ctrlpoints8[4][3] = {{0.75,1,0},{0.5,0.833,0},{0.5,0.6667,0},
    {0.75,0.5,0}};
GLfloat ctrlpoints9[4][3] = {{0.75,0.5,0},{0.8, 0.3333,0},{0.6,0.16667,0},
    {0.75,0,0}};
GLfloat ctrlpoints10[4][3] = {{0.75,0,0},{0.4, -0.16667,0},{0.6,-0.3333,0},
    {0.75,-0.5,0}};
GLfloat ctrlpoints11[4][3] = {{0.75,-0.5,0},{0.9, -0.6667,0},{0.8,-0.8333,0},
    {0.75,-1,0}};


//reads ppm and saves as array of unsigned ints for texturing
void readFile(GLuint *image, GLuint index){
    FILE *fd;
    int  k, nm;
    char c;
    int i;
    char b[100];
    int red, green, blue;
    
    fd = fopen(tex[index], "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3')
    {
        printf("%s is not a PPM file!\n", b);
        exit(0);
    }
    printf("%s is a PPM file\n",b);
    fscanf(fd, "%c",&c);
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);
    
    printf("%d rows  %d columns  max value= %d\n",n,m,k);
    
    nm = n*m;
    
    image=(GLuint*)malloc(3*sizeof(GLuint)*nm);
    
    
    s=255./k;
    
    for(i=0;i<nm;i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        image[3*nm-3*i-3]=red;
        image[3*nm-3*i-2]=green;
        image[3*nm-3*i-1]=blue;
    }
    
    printf("read image\n");
    glPixelTransferf(GL_RED_SCALE, s);
    glPixelTransferf(GL_GREEN_SCALE, s);
    glPixelTransferf(GL_BLUE_SCALE, s);
    glPixelStorei(GL_UNPACK_SWAP_BYTES,GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    glTexImage2D(GL_TEXTURE_2D,0,3,n,m,0,GL_RGB,GL_UNSIGNED_INT, image);
    if(index==ORANGE){
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }else{
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }
}


//initializer
void myInit(void){
    glClearColor(1.0f,1.0f,1.0f,1.0f);        // set white background color
    glViewport(3,3,500,500);                  // set viewport size
    //not in window origin
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the screen
    glLineWidth(1.0);                         //set line width
    glPointSize(1.0);                         //set point size
    glEnable(GL_DEPTH_TEST);                  // Enable depth test(for cube)
    glDepthFunc(GL_LESS);
    glEnable(GL_NORMALIZE);
    
    glMatrixMode(GL_PROJECTION);              //Load projection matrix
    glLoadIdentity();
    //Set perspective to 60 degree, 1/1 ratio (viewport is 500/500), near clip 1, far clip 100
    gluPerspective(60.0, 1.0, 1.0, 100.0);
    //set camera
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glShadeModel(GL_FLAT);
    
    glGenTextures(TEX_COUNT, textures);
}

void room(){
    glColor3f(0.5,0.5,0.5);
    //back wall
    glBegin(GL_POLYGON);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(1.0,-1.0,-1.0);
    glVertex3f(1.0,1.0,-1.0);
    glVertex3f(-1.0,1.0,-1.0);
    glEnd();
    
    
    glColor3f(0.75,0.25,0.5);
    //ceiling
    glBegin(GL_POLYGON);
    glTexCoord2d(0.0,0.0);
    glVertex3f(-1.0,1.0,-1.0);
    glTexCoord2d(1.0,0.0);
    glVertex3f(1.0,1.0,-1.0);
    glTexCoord2d(1.0,1.0);
    glVertex3f(1.0,1.0,1.0);
    glTexCoord2d(0.0,1.0);
    glVertex3f(-1.0,1.0,1.0);
    glEnd();
    
    
    readFile(image[JELLY], JELLY);
    glBindTexture(GL_TEXTURE_2D, textures[JELLY]);
    glColor3f(1,1,1);
    //ceiling poster of Jelly fish
    glBegin(GL_POLYGON);
    glTexCoord2d(0.0,0.0);
    glVertex3f(-0.5,1.0,-0.5);
    glTexCoord2d(0.0,1.0);
    glVertex3f(0.5,1.0,-0.5);
    glTexCoord2d(1.0,1.0);
    glVertex3f(0.5,1.0,0.5);
    glTexCoord2d(1.0,0.0);
    glVertex3f(-0.5,1.0,0.5);
    glEnd();
    
    
    glColor3f(0.25,0.5,0.25);
    //right wall
    glBegin(GL_POLYGON);
    glVertex3f(1.0,1.0,1.0);
    glVertex3f(1.0,1.0,-1.0);
    glVertex3f(1.0,-1.0,-1.0);
    glVertex3f(1.0,-1.0,1.0);
    glEnd();
    
    readFile(image[COMO], COMO);
    glBindTexture(GL_TEXTURE_2D, textures[COMO]);
    glColor3f(1,1,1);
    //Window of Lake Como picture
    glBegin(GL_POLYGON);
    glTexCoord2d(1.0,1.0);
    glVertex3f(1.0,0.5,0.25);
    glTexCoord2d(0.0,1.0);
    glVertex3f(1.0,0.5,-0.25);
    glTexCoord2d(0.0,0.0);
    glVertex3f(1.0,-0.5,-0.25);
    glTexCoord2d(1.0,0.0);
    glVertex3f(1.0,-0.5,0.25);
    glEnd();
    
    
    glColor3f(0.15,0.15,0.15);
    //floor
    glBegin(GL_POLYGON);
    glVertex3f(-1.0,-1.0,-1.0);
    glVertex3f(1.0,-1.0,-1.0);
    glVertex3f(1.0,-1.0,1.0);
    glVertex3f(-1.0,-1.0,1.0);
    glEnd();
    
    
    
    glColor3f(0.5,0.2,0.0);
    //left wall
    glBegin(GL_POLYGON);
    glVertex3f(-1.0,1.0,-1.0);
    glVertex3f(-1.0,1.0,1.0);
    glVertex3f(-1.0,-1.0,1.0);
    glVertex3f(-1.0,-1.0,-1.0);
    glEnd();
    
}

//Draw cube of side length .25
void cube(){
    glBegin(GL_POLYGON);
    glColor3f(1.0,1.0,1.0);
    //front
    
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.25,0.25,0.0);      //TFR top front right
    glTexCoord2d(0.0,1.0);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.0,0.25,0.0);       //TFL top front left
    glTexCoord2d(0.0,0.0);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.0,0.0,0.0);        //BFL bottom front left
    glTexCoord2d(1.0,0.0);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.25,0.0,0.0);       //BFR bottom front right
    glEnd();
    
    
    //back, green
    glBegin(GL_POLYGON);
    glColor3f(0.0,1.0,0.0);
    
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.25,0.25,0.25);     //TBR top back right
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.0,0.25,0.25);      //TBL top back left
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.0,0.0,0.25);       //BBL bottom back left
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.25,0.0,0.25);      //BBR bottom back right
    glEnd();
    
    
    //top, blue
    glBegin(GL_POLYGON);
    glColor3f(0.0,0.0,1.0);
    
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(0.25,0.25,0.25);     //TBR top back right
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(0.0,0.25,0.25);      //TBL top back left
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(0.0,0.25,0.0);       //TFL top front left
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(0.25,0.25,0.0);      //TFR top front right
    glEnd();
    
    //bottom, purple
    glBegin(GL_POLYGON);
    glColor3f(1.0,0.0,1.0);
    
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(0.25,0.0,0.0);       //BFR bottom front right
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(0.25,0.0,0.25);      //BBR bottom back right
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(0.0,0.0,0.25);       //BBL bottom back left
    glTexCoord2d(0.75,1.0);
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(0.0,0.0,0.0);        //BFL bottom front left
    glEnd();
    
    //left, orange
    glBegin(GL_POLYGON);
    glColor3f(1.0,1.0,0.0);
    
    glTexCoord2d(0.75,1.0);
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.25);       //BBL bottom back left
    glTexCoord2d(0.75,1.0);
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(0.0,0.25,0.25);      //TBL top back left
    glTexCoord2d(0.75,1.0);
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(0.0,0.25,0.0);       //TFL top front left
    glTexCoord2d(0.75,1.0);
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.0);        //BFL botton front left
    glEnd();
    
    //right, cyan
    glBegin(GL_POLYGON);
    glColor3f(0.0,1.0,1.0);
    
    glTexCoord2d(0.75,1.0);
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(0.25,0.0,0.25);       //BBR bottom back right
    glTexCoord2d(0.75,1.0);
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(0.25,0.25,0.25);      //TBR top back right
    glTexCoord2d(0.75,1.0);
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(0.25,0.25,0.0);       //TFR top front right
    glTexCoord2d(0.75,1.0);
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(0.25,0.0,0.0);        //BFR bottom front right
    glEnd();
    glFlush();
}

void normalize3(GLfloat *a){
    GLfloat len = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0]/=len;
    a[1]/=len;
    a[2]/=len;
}

void tessel(int subdiv, GLfloat radius, GLfloat *a, GLfloat *b, GLfloat *c){
    glPushMatrix();
    glColor3f(1.0,0.2,0.0);
    glBegin(GL_TRIANGLES);
    glNormal3fv(a);
    glVertex3f(a[0]*radius, a[1]*radius, a[2]*radius);
    glNormal3fv(b);
    glVertex3f(b[0]*radius, b[1]*radius, b[2]*radius);
    glNormal3fv(c);
    glVertex3f(c[0]*radius, c[1]*radius, c[2]*radius);
    glEnd();
    glPopMatrix();
    if(subdiv>1){
        GLfloat d[3] = {a[0]+b[0], a[1]+b[1], a[2]+b[2]};
        GLfloat e[3] = {b[0]+c[0], b[1]+c[1], b[2]+c[2]};
        GLfloat f[3] = {c[0]+a[0], c[1] +a[1], c[2]+a[2]};
        
        normalize3(d);
        normalize3(e);
        normalize3(f);
        
        tessel(subdiv-1, radius, a, d, f);
        tessel(subdiv-1, radius, d, b, e);
        tessel(subdiv-1, radius, f, e, c);
        tessel(subdiv-1, radius, f, d, e);
    }
}

void sphere(GLfloat radius){
    GLfloat a[] = {1,0,0};
    GLfloat b[] = {0,0,-1};
    GLfloat c[] = {-1,0,0};
    GLfloat d[] = {0,0,1};
    GLfloat e[] = {0,1,0};
    GLfloat f[] = {0,-1,0};
    
    int subdiv = 4;
    
    tessel(subdiv, radius, d, a, e);
    tessel(subdiv, radius, a, b, e);
    tessel(subdiv, radius, b, c, e);
    tessel(subdiv, radius, c, d, e);
    tessel(subdiv, radius, a, d, f);
    tessel(subdiv, radius, b, a, f);
    tessel(subdiv, radius, c, b, f);
    tessel(subdiv, radius, d, c, f);
}

//display function
void myDisplay(void){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //draw room
    room();
    
    
    
    //Draw curves
    glPushMatrix();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints1[0][0]);
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints2[0][0]);
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints3[0][0]);
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    
    glPopMatrix();
    
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.0,-.75,0.0);
    glMultMatrixf(interp);
    
    //glMultMatrixf(a1);
    
    
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints4[0][0]);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints5[0][0]);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints6[0][0]);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints7[0][0]);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    
    glPopMatrix();
    
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.25,-0.75,-0.5);
    glScalef(0.2,0.2,0.2);
    //multiply by bspline matrix
    glMultMatrixf(bspline);
    
    
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints8[0][0]);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints9[0][0]);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints10[0][0]);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints11[0][0]);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++)
        glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    glPopMatrix();
    glFlush();
}




//keyboard input function
void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
    
    switch(theKey)
    {
            //Circle with repeated orange texture
        case 'C':
        case 'c':{
            glPushMatrix();                                 //save current matrix
            
            glTranslatef(0.0, -0.75, 0.25);
            glRotatef(20.0,0.0,0.0,1.0);
            glScalef(0.5,0.25,0.75);
            //glEnable(GL_TEXTURE_2D);
            
            readFile(image[ORANGE], ORANGE);
            glBindTexture(GL_TEXTURE_2D, textures[ORANGE]);
            glColor3f(1,1,1);
            glBegin(GL_POLYGON);                            //180 edges radius 1
            for(double r = 0; r<=360; r++){
                double angle = 2*r * M_PI/360;
                double x = .5*cos(angle);
                double y = .5*sin(angle);
                glTexCoord2d(2*y,2*x);
                glNormal3f(0.0,0.0,1.0);
                glVertex2d(y,x);
            }
            glEnd();
            glPopMatrix();                                  //revert matrix
            break;
        }
            //hexagon with Burt Reynolds texture
        case 'H':
        case 'h':{
            glPushMatrix();                                 //save current matrix
            
            glTranslatef(-0.75, -0.75, -0.5);
            glRotatef(50.0,0.0,0.0,1.0);
            glScalef(0.75,0.25,0.5);
            
            readFile(image[BURT], BURT);
            glBindTexture(GL_TEXTURE_2D, textures[BURT]);
            glColor3f(1,1,1);
            glBegin(GL_POLYGON);
            glTexCoord2d(0.75,1.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex2f(0.25,0.5);//.5 length sides
            glTexCoord2d(0.25,1.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex2f(-0.25,0.5);
            glTexCoord2d(0.0,0.5);
            glNormal3f(0.0,0.0,1.0);
            glVertex2f(-0.5,0.0);
            glTexCoord2d(0.25,0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex2f(-0.25,-0.5);
            glTexCoord2d(0.75,0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex2f(0.25,-0.5);
            glTexCoord2d(1.0,0.5);
            glNormal3f(0.0,0.0,1.0);
            glVertex2f(0.5,0.0);
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();                                  //revert matrix
            break;
        }
            //square with opengl created image of the room texture
        case 'S':
        case 's':{
            glPushMatrix();                                 //save current matrix
            
            glTranslatef(0.0, 0.25, -0.75);
            glRotatef(35.0,0.0,1.0,0.0);
            glScalef(0.5,0.5,0.5);
            glColor3f(1,1,1);
            readFile(image[QUANTUM], QUANTUM);
            glBindTexture(GL_TEXTURE_2D, textures[QUANTUM]);
            
            glBegin(GL_POLYGON);
            glTexCoord2d(0.0,1.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(-0.5,0.5,0.0);
            glTexCoord2d(0.0,0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(-0.5,-0.5,0.0);
            glTexCoord2d(1.0,0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(0.5,-0.5,0.0);
            glTexCoord2d(1.0,1.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(0.5,0.5,0.0);
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();                                  //revert matrix
            break;
        }
            //Triangle with waves texture
        case 'T':
        case 't':{
            glPushMatrix();                                 //save current matrix
            
            glTranslatef(0.75, -0.75, -0.5);
            glRotatef(15.0,1.0,0.0,0.0);
            glScalef(0.45,0.45,0.45);
            readFile(image[WAVES], WAVES);
            glBindTexture(GL_TEXTURE_2D, textures[WAVES]);
            glColor3f(1,1,1);
            glBegin(GL_TRIANGLES);
            glTexCoord2d(0.5,0.8);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(0.0,0.4,0.0);
            glTexCoord2d(0.0,0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(-0.5,-0.4,0.0);
            glTexCoord2d(1.0,0.0);
            glNormal3f(0.0,0.0,1.0);
            glVertex3f(0.5,-0.4,0.0);
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();                                  //revert matrix
            break;
        }
            //one cube rendered, then translated and rendered again
        case 'U':
        case 'u':{
            glPushMatrix();
            glTranslatef(-0.75, 0.75, -0.5);
            cube();
            glTranslatef(1.0,-1,0);
            glRotatef(45.0,1.0,1.0,1.0);
            cube();
            glPopMatrix();                 //restore matrix
            break;
        }
            //red sphere
        case 'V':
        case 'v':{
            glPushMatrix();
            glTranslatef(0.75, 0.75, 0.0);
            glScalef(0.5,0.5,0.5);
            sphere(0.25);
            glPopMatrix();
            break;
            
        }
        case 'Q':
        case 'q':
            exit(-1); //terminate the program
        default:
            break; // do nothing
    }
}


//reshape window resize
void reshape(int w, int h)
{
    width = w;
    height = h;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, w/h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}

//Main
int main(int argc, char** argv)
{
    glutInit(&argc, argv);           // initialize the toolkit
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);//set display mode
    glutReshapeFunc(reshape);
    glutInitWindowSize(800,600);      // set window size
    glutInitWindowPosition(100, 150); // set window position on screen
    glutCreateWindow("Britain Mackenzie - S5, Q-quit"); // open the window
    glutDisplayFunc(myDisplay);       // register redraw function
    glutKeyboardFunc(myKeyboard);     // register the keyboard action function
    myInit();                         //initializer
    glutMainLoop();                   // go into a perpetual loop
}


#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //Amount of Radians that are in a degree.
float px,py,pdx,pdy,pa;
int FOV=70;
void drawPlayer()
{
    glColor3f(1,0,1);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px,py);
    glVertex2i(px+pdx*5,py+pdy*5);
    glEnd();
}
int mapX=8,mapY=8,mapS=64;
int map[]=
        {
1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,1,
1,1,1,0,1,1,1,1,
1,0,0,0,0,1,0,1,
1,1,1,0,0,1,0,1,
1,0,1,0,1,1,0,1,
1,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,
        };
void draw2dMap()
{
    int x,y,xo,yo;
    for(y=0;y<mapY;y++)
    {for(x=0;x<mapX;x++)
    {if(map[y*mapX+x]==1){glColor3f(1,1,1);} else{glColor3f(0,0,0);}
        xo=x*mapS; yo=y*mapS;
        glBegin(GL_QUADS);
        glVertex2i(xo+1,yo+1);
        glVertex2i(xo+1,yo+mapS-1);
        glVertex2i(xo-1+mapS,yo+mapS-1);
        glVertex2i(xo-1+mapS,yo+1);
        glEnd();
        }
    }
}
float dist(float ax,float ay, float bx, float by, float ang) {return(sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)));}
void castRay()
{
    float DistH=999999,hx=px,hy=py;
    int r,mx,my,mp,dof; float rx,ry,ra,xo,yo,disT;
    ra=pa-DR*(FOV/2); if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}
    for(r=0;r<512;r++) {
        //Horizontal
        dof = 0;
        float aTan = -1 / tan(ra);
        if (ra > PI) {
            ry = ((int) py >> 6 << 6) - 0.0001;
            rx = (py - ry) * aTan + px;
            yo = -64;
            xo = -yo * aTan;
        }
        if (ra < PI) {
            ry = ((int) py >> 6 << 6) + 64;
            rx = (py - ry) * aTan + px;
            yo = 64;
            xo = -yo * aTan;
        }
        if (ra == 0 || ra == PI) {
            rx = px;
            ry = py;
            dof = 8;
        }
        while (dof < 8) {
            mx = (int) (rx) >> 6;
            my = (int) (ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                hx = rx;
                hy = ry;
                DistH = dist(px, py, hx, hy, ra);
                dof = 8;
            }
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        //Vertical
        float DistV = 999999, vx = px, vy = py;
        dof = 0;
        float nTan = -tan(ra);
        if (ra > P2 && ra < P3) {
            rx = ((int) px >> 6 << 6) - 0.0001;
            ry = (px - rx) * nTan + py;
            xo = -64;
            yo = -xo * nTan;
        }
        if (ra < P2 || ra > P3) {
            rx = ((int) px >> 6 << 6) + 64;
            ry = (px - rx) * nTan + py;
            xo = 64;
            yo = -xo * nTan;
        }
        if (ra == 0 || ra == PI) {
            ry = py;
            rx = px;
            dof = 8;
        }
        while (dof < 8) {
            mx = (int) (rx) >> 6;
            my = (int) (ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                vx = rx;
                vy = ry;
                DistV = dist(px, py, vx, vy, ra);
                dof = 8;
            }
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        // Draw stuff on minimap
        if (DistV < DistH) {
            rx = vx;
            ry = vy;
            disT = DistV;
            glColor3f(0,1,0);
        }
        if (DistH < DistV) {
            rx = hx;
            ry = hy;
            disT = DistH;
            glColor3f(0,.9,0);
        }
        glLineWidth(2);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();
        // Draw Walls
        float ca=pa-ra; if(ca<0){ca+=2*PI;}if(ca>2*PI){ca-=2*PI;} disT=disT*cos(ca);//fisheye fix
        float lineH=(mapS*512)/disT; if(lineH>512){lineH=512;}
        float lineO=250-lineH/2;
        glLineWidth(1);glBegin(GL_LINES);glVertex2i(r*1+512,lineO);glVertex2i(r*1+512,lineH+lineO);glEnd();
        // Prepare for next ray
        ra += (DR/512)*FOV;
        if(ra<0){ra+=2*PI;}if(ra>2*PI){ra-=2*PI;}
    }
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw2dMap();
    castRay();
    drawPlayer();
    glutSwapBuffers();
}
void movement(unsigned char key,int x,int y)
{
    if(key=='a') {pa-=0.1; if(pa<0)   {pa+=2*PI;} pdx=cos(pa)*5;pdy=sin(pa)*5;}
    if(key=='d') {pa+=0.1; if(pa>2*PI){pa-=2*PI;} pdx=cos(pa)*5;pdy=sin(pa)*5;}
    if(key=='w') {px+=pdx; py+=pdy;}
    if(key=='s') {px-=pdx; py-=pdy;}
    glutPostRedisplay();
}
void init()
{
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,1024,512,0);
    px=300;py=300;pdx=cos(pa)*5;pdy=sin(pa)*5;
}
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1024,512);
    glutCreateWindow("Raycaster");
    init();
    glutKeyboardFunc(movement);
    glutDisplayFunc(display);
    glutMainLoop();
}
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

const float Pi = 3.1415926535, DTR = 0.0174533;
float px, py, pdx, pdy, pa;
int mapX = 8, mapY = 8, mapS = 64; // mapS stands for size of the squares in the map


//0 = free space, 1 = wall
int map[] = {
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,0,0,1,1,1,
	1,1,1,1,1,1,1,1
};

//this function is how I realised that C isn't pass by reference language
void normalize_angle(float *pa){
	float angle = *pa;
	if(angle < 0){
		angle += 2*Pi;
	}else if (angle > 2*Pi){
		angle = angle - 2*Pi;
	}
	*pa = angle;
}

// implementation of the pythogoras theorum
float pytho(float px, float py, float rx, float ry){
	float dist = sqrt(((rx - px)*(rx-px)) + ((ry - py)* (ry-py)));
	return dist;
}

void raycast(){
	int r, mx, my, mp, dof, distance; 
	float rx, ry, ra = pa, xo, yo;

  //rays start from an offset of 30 degrees from the players angles.
	ra = ra - 30*DTR;
	normalize_angle(&ra); //dont wanna go out of range 
	
	for(r = 0; r < 60; r++){
		float atan = -1/tan(ra), ntan = -tan(ra); // negative sign since the tangent function is negative in 2nd and 4th quads 
		
		//vertical implementation
		dof = 0;
		//intializing all the dist values with a large value so if the pythogars results in 0 it doesnt ruin the comparsion
		float distv = 100000, vx = px, vy = py;
		
		//did this part with cos since 
		if (cos(ra) < -0.001) { // Facing left
            rx = (((int)px >> 6) << 6) - 0.0001;
            ry = ((px - rx)*ntan) + py;
            xo = -64;
            yo = -xo*ntan;
        }
		if (cos(ra) > 0.001) { // Facing right
            rx = (((int)px >> 6) << 6) + 64;
            ry = ((px - rx)*ntan) + py;
            xo = 64;
            yo = -xo*ntan;
        } 
		if (cos(ra) == 0) { // Ray exactly vertical
            rx = px; //Making the position coordinates to ray coordinates so the ray isnt drawn
            ry = py;
            dof = 8;
        }
    
		while(dof < 8){
				mx = (int)rx >> 6; 
				my = (int)ry >> 6;
				mp = my*mapX + mx;
				if(mp >= 0 && mp < mapX*mapY && map[mp] == 1){
					dof = 8;
					vx = rx; 
					vy = ry; 
					distv = pytho(px, py, vx, vy);
				} 
				else{
					rx += xo; 
					ry += yo; 
					dof +=1;
				}
		}
		
		//horizontal implementation
		float disth = 10000, hx, hy;
		dof = 0;
		if (ra > Pi) { // Facing up
            ry = (((int)py >> 6) << 6) - 0.0001;
            rx = ((py - ry)*atan) + px;
            yo = -64;
            xo = -yo*atan;
        }
		if (ra < Pi) { // Facing down
            ry = (((int)py >> 6) << 6) + 64;
            rx = ((py - ry)*atan) + px;
            yo = 64;
            xo = -yo * atan;
        } 
		if (ra == 0 || ra == Pi) { // Ray exactly horizontal
            rx = px;
            ry = py;
            dof = 8;
        }
		while(dof < 8){
			mx = (int)rx >> 6; 
			my = (int)ry >> 6;
			mp = my*mapX + mx;
			if(mp >= 0 && mp < mapX*mapY && map[mp] == 1){
				dof = 8;
				hx = rx;
				hy = ry;
				disth = pytho(px, py, rx, ry);
			} 
			else{rx += xo; ry += yo; dof +=1;}
		}
		
		
		//calculating the smallest distance 
		if(distv < disth){
			rx = vx; 
			ry = vy;
			distance = distv;
			glColor3f(0,0.9,0);
		}else{
			rx = hx;
			ry = hy;
			distance = disth;
			glColor3f(0,0.7,0);
		}
		
		//drawing ray lines
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(px, py);
		glVertex2i(rx, ry);
		glEnd();	
		
		ra = ra + DTR;
		normalize_angle(&ra);
		int count = 0;
		
		//calculating the wall height
		float da = pa-ra;
		normalize_angle(&da);
		distance = distance * cos(da);
		float wallheight = (mapS*320)/distance, wallO = 160 - wallheight/2;
		
		//drawing the wall 
		glLineWidth(8); 
		glBegin(GL_LINES);
		glVertex2i((r*8)+530, wallO);
		glVertex2i((r*8)+530, wallheight + wallO);
		glEnd();
	
		
	}
}	


void draw_map(){
	int x, y, xo ,yo;
	
	for(y=0; y < mapY; y++){
		for(x=0; x < mapX; x++){
			if(map[(y*mapX)+x] == 1){ glColor3f(1,1,1);}else {glColor3f(0,0,0);}
			xo = x*mapS; yo = y*mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo+1, yo+1);
			glVertex2i(xo+1, yo + mapS -1);
			glVertex2i(xo + mapS -1, yo + mapS-1);
			glVertex2i(xo + mapS-1, yo+1);
			glEnd();
		
		}
		
	}
}

void draw_player(){
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px,py);
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px,py);
	glVertex2i(px + pdx*5, py + pdy*5);
	glEnd();
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	draw_map();
	draw_player();
	raycast();
	glutSwapBuffers();
}

void init(){
	glClearColor(0.3,0.3,0.3,0.3);
	gluOrtho2D(0, 1024, 512, 0);
	px = 300; py = 300;
	pdy = sin(pa) * 5; pdx = cos(pa) * 5;
}

void buttons(unsigned char key, int x, int y){
	if(key == 'w'){px += pdx; py += pdy;}
	if(key == 's'){px -= pdx; py -= pdy;}
	if(key == 'd'){
		pa += 0.1;
		normalize_angle(&pa);
		pdy = sin(pa) * 5; 
		pdx = cos(pa) * 5;
	}
	if(key == 'a'){
		pa -= 0.1;
		normalize_angle(&pa);
		pdy = sin(pa) * 5; 
		pdx = cos(pa) * 5;		
	}
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	//dont ask me how this works idk too
 	glutInit(&argc, argv);
 	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
 	glutInitWindowSize(1024,512);
 	glutCreateWindow("RayCaster");
 	init();
 	glutDisplayFunc(display);
 	glutKeyboardFunc(buttons);
 	glutMainLoop();
 	return 0;
}

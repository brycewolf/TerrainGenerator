// ProceduralTerrain.cpp : Creates a 2D array heightmap based on intensity and roughness
// Bryce Wolf
// CS334 Final Project

#include <cstdlib>
#include <stdio.h>
#include <time.h>
#include <ctime>
#include <iostream> 
#include "GL/glut.h"
using namespace std;


int mSize = 1025;
float heightMap[1025][1025];

float intensity = 100;
int roughness = 30;

float roughValue = .001 * roughness;
float heightArray[3151875];

int ortho = 1;
float maxHeight;
float minHeight;


// Generate the random noise based off of the intensity
float randomNoise(float intensity) {
	return (float(rand()) / RAND_MAX) * (2 * intensity) - intensity;
}


// Find the minimum and maximum height in the heightmap
void minMaxHeightmap() {
	maxHeight = -100000;
	minHeight = 100000;
	
	for (int i = 0; i < mSize; i++) {
		for (int j = 0; j < mSize; j++) {
			if (heightMap[i][j] > maxHeight) {
				maxHeight = heightMap[i][j];
			}
			else if (heightMap[i][j] < minHeight) {
				minHeight = heightMap[i][j];
			}
		}
	}
};


// Generate the heightMap
void DSGenerator() {

	// Set all the corners to a random start height
	heightMap[0][0] = randomNoise(intensity);
	heightMap[mSize - 1][0] = randomNoise(intensity);
	heightMap[0][mSize - 1] = randomNoise(intensity);
	heightMap[mSize - 1][mSize - 1] = randomNoise(intensity);

	// Set the rest
	int step = mSize - 1;
	while (step > 1) {
		int half = step / 2;

		//%%%%%%%%%%%%%%%%%%%%% SQUARE %%%%%%%%%%%%%%%%%%%%% 
		for (int j = 0; j < mSize - 1; j += step) {
			for (int i = 0; i < mSize - 1; i += step) {
				float total =	heightMap[i][j] +
								heightMap[i + step][j] +
								heightMap[i][j + step] +
								heightMap[i + step][j + step];
				float average = total / 4;
				heightMap[i + half][j + half] = average + randomNoise(intensity);
			}
		}

		//%%%%%%%%%%%%%%%%%%%%% DIAMOND %%%%%%%%%%%%%%%%%%%%% 
		for (int i = 0; i < mSize - 1; i += half) {
			for (int j = (i + half) % step; j < mSize - 1; j += step) {
				float total =	heightMap[(i - half + mSize - 1) % (mSize - 1)][j] +
								heightMap[(i + half) % (mSize - 1)][j] +
								heightMap[i][(j + half) % (mSize - 1)] +
								heightMap[i][(j - half + mSize - 1) % (mSize - 1)];
				float average = total / 4;
				heightMap[i][j] = average + randomNoise(intensity);
			}
		}

		// Reduce stepsize by half
		step = step / 2;
	}
};


//Smooth the terrain
void DSSmoother() {

	//Smooth the 4 Edges First
	for (int j = 0; j < mSize; j++) {
		heightMap[0][j] = heightMap[0][j] * roughValue + heightMap[0][j + 1] * (1 - roughValue);
		heightMap[mSize - 1][j] = heightMap[mSize - 1][j] * roughValue + heightMap[mSize - 1][j + 1] * (1 - roughValue);
	}
	for (int i = 1; i < mSize - 1; i++) {
		heightMap[i][0] = heightMap[i][0] * roughValue + heightMap[i + 1][0] * (1 - roughValue);
		heightMap[i][mSize - 1] = heightMap[i][mSize - 1] * roughValue + heightMap[i + 1][mSize - 1] * (1 - roughValue);
	}


	//Smooth the rest of the heightmap
	// Pass Left to Right (prior to rotation)
	for (int i = 1; i < mSize; i++) {
		for (int j = 0; j < mSize - 1; j++) {
			heightMap[i][j] = roughValue * heightMap[i][j] + (1 - roughValue) * heightMap[i - 1][j];
		}
	}
	// Pass Right to Left
	for (int i = mSize - 3; i >= 0; i--) {
		for (int j = 0; j < mSize - 1; j++) {
			heightMap[i][j] = roughValue * heightMap[i][j] + (1 - roughValue) * heightMap[i + 1][j];
		}
	}
	// Pass bottom to Top
	for (int i = 0; i < mSize; i++) {
		for (int j = 1; j < mSize - 1; j++) {
			heightMap[i][j] = roughValue * heightMap[i][j] + (1 - roughValue) * heightMap[i][j - 1];
		}
	}
	// Pass Top to Bottom
	for (int i = 0; i < mSize; i++) {
		for (int j = mSize - 1; j >= 1; j--) {
			heightMap[i][j] = roughValue * heightMap[i][j] + (1 - roughValue) * heightMap[i][j + 1];
		}
	}
}


	


void display() {

	glClear(GL_COLOR_BUFFER_BIT);

	//cout << "MIN: " << minHeight << "\n" << "MAX: " << maxHeight;

	// Set the cutoffs for the topological lines
	float heightDif = maxHeight - minHeight;
	float water = minHeight + 0.3 * heightDif;
	float sand = minHeight + 0.33 * heightDif;
	float grass = minHeight + 0.6 * heightDif;
	float earth = minHeight + 0.75 * heightDif;
	float rock = minHeight + 0.8 * heightDif;


	//Draw the triangles between points, colored based on height
	for (int i = 0; i < mSize - 1; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < mSize; j++) {


			// Set the color and draw the point
			float height = heightMap[i][j];
			
			if (height < water) {
				glColor3f(0.0, 0.0, 1.0);
			}
			else if (water <= height && height < sand) {
				glColor3f(0.957, 0.643, 0.376);
			}
			else if (sand <= height && height < grass) {
				if (height <= minHeight + 0.433 * heightDif) {
					glColor3f(0.0, 0.566, 0.0);
				}
				else if (height <= minHeight + 0.5 * heightDif) {
					glColor3f(0.0, 0.533, 0.0);
				}
				else if (height <= minHeight + 0.533 * heightDif) {
					glColor3f(0.0, 0.5, 0.0);
				}
				else if (height <= minHeight + 0.566 * heightDif) {
					glColor3f(0.0, 0.466, 0.0);
				}
				else {
					glColor3f(0.0, 0.433, 0.0);
				}
			}
			else if (grass <= height && height < earth) {
				if (height <= minHeight + 0.675 * heightDif) {
					glColor3f(0.0, 0.4, 0.0);
				}
				else {
					glColor3f(0.0, 0.366, 0.0);
				}
			}
			else if (earth <= height && height < rock) {
				glColor3f(0.5, 0.5, 0.5);
			}
			else {
				glColor3f(1.0, 0.98, 0.98);
			}
			glVertex3f(i, j, heightMap[i][j]);

			
			// Set the color and draw the neightbor point
			height = heightMap[i+1][j];
			
			if (height < water) {
				glColor3f(0.0, 0.0, 1.0);
			}
			else if (water <= height && height < sand) {
				glColor3f(0.957, 0.643, 0.376);
			}
			else if (sand <= height && height < grass) {
				if (height <= minHeight + 0.433 * heightDif)
					glColor3f(0.0, 0.566, 0.0);
				else if (height <= minHeight + 0.5 * heightDif) {
					glColor3f(0.0, 0.533, 0.0);
				}
				else if (height <= minHeight + 0.533 * heightDif) {
					glColor3f(0.0, 0.5, 0.0);
				}
				else if (height <= minHeight + 0.566 * heightDif) {
					glColor3f(0.0, 0.466, 0.0);
				}
				else {
					glColor3f(0.0, 0.433, 0.0);
				}
			}
			else if (grass <= height && height < earth) {
				if (height <= minHeight + 0.675 * heightDif) {
					glColor3f(0.0, 0.4, 0.0);
				}
				else {
					glColor3f(0.0, 0.366, 0.0);
				}
			}
			else if (earth <= height && height < rock) {
				glColor3f(0.5, 0.5, 0.5);
			}
			else {
				glColor3f(1.0, 0.98, 0.98);
			}
			glVertex3f(i + 1, j, heightMap[i+1][j]);
		}
		glEnd();
		glFlush();
	}
}


// Initialize the camera views, ortho and perspective
void init() {

	// Top-down orthographic view
	if (ortho == 1) {
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0, 1.0, 1.0);
		glOrtho(-1.0, mSize, -1.0, mSize, -600.0, 600.0);
	}

	// Perspective projection
	if (ortho == 0) {

		// Clear the frame to blue (for sky)
		glClearColor(0.529, 0.808, 0.922, 1.0);

		// Camera placement
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1, 1, -1, 1, 1, 2000);
		glRotatef(40, 1, 0, 0);

		// Move the terrain so we can see it
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-mSize/2, -maxHeight - 350, -270);
		glRotatef(270, 1, 0, 0);
	}
}


// Handle the keyboard presses
void keyboard(unsigned char k, int x, int y)
{
	if (k == 'p')	// Display perspective view
	{
		if (ortho != 0) {
			ortho = 0;
			init();
			display();
		}
	}
	else if (k == 'o')	// Display ortho view
	{
		if (ortho != 1) {
			ortho = 1;
			//glClearColor(1.0, 1.0, 1.0, 0.0);
			//init();
			//display();
		}
	}
	else if (k == 27)	// Close program
	{
		exit(0);
	}
}


int main(int argc, char** argv) {
	
	// Seed the random function
	srand(time(0));
	
	// Generate the heightmap
	DSGenerator();
	DSSmoother();
	minMaxHeightmap();
	
	// Initialize GLUT and create window
	glutInit(&argc, argv);                 
	glutInitWindowSize(900, 900);   
	glutInitWindowPosition(500, 50); 
	glutCreateWindow("Generated Terrain"); 
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	init();

	glutMainLoop();
	return 0;
}
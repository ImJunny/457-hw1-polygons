/*
Please write what this program does or also mention what doesn't work .

Status:
1.	This code can create vertices, lines, and polygons based on user mouse
	input. Users can also press certain keys to change polygon color, clear
	the board, or quit.
2.	The extra credit portion is also done. Everything should be working as
	intended.
*/

#include <GL/glut.h>
#include <vector>
#include<iostream>

using namespace std;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;

bool g_bDragging = false;

// new global variables to control things
vector<pair<int, int>> points;
vector < vector<pair<int, int>>> polygons;
bool firstPoint = true;
pair<int, int> mousePos;
vector<pair<int, vector<GLfloat>>> colorCodes;

// MOUSE Function
void mouse(int button, int state, int x, int y) {
	// click and drag left mouse; make vertex at first position
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		g_bDragging = true;
		if (firstPoint) {
			points.push_back(make_pair(x, WINDOW_HEIGHT - y));
			firstPoint = false;
		}
	}

	// click and release left mouse; make vertex at new position
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		g_bDragging = false;
		points.push_back(make_pair(x, WINDOW_HEIGHT - y));
	}

	// click right mouse; make vertax at first position and create polygon
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (points.size() > 2)
			points.push_back(points[0]);
		else return;
		polygons.push_back(points);
		points.clear();
		firstPoint = true;
	}
}

// DISPLAY all vertices, lines, and polygons
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(2.5f);
	// set color to black (default)
	glColor3f(0, 0, 0);

	// display all vertices
	glBegin(GL_POINTS);
	for (auto& point : points)
		glVertex2i(point.first, point.second);
	glEnd();

	// display all made lines
	glBegin(GL_LINES);
	for (int i = 1; i < points.size(); i++) {
		glVertex2i(points[i - 1].first, points[i - 1].second);
		glVertex2i(points[i].first, points[i].second);
	}
	glEnd();

	// display current dragging lines
	if (g_bDragging && !points.empty()) {
		glBegin(GL_LINES);
		glVertex2i(points.back().first, points.back().second);
		glVertex2i(mousePos.first, mousePos.second);
		glEnd();
	}

	// display all polygons
	for (int i = 0; i < polygons.size(); i++) {
		glBegin(GL_POLYGON);

		// set polygon color to what is stored in colorCodes vector
		for (auto& entry : colorCodes) {
			if (entry.first == i) {
				glColor3f(entry.second[0], entry.second[1], entry.second[2]);
			}
		}

		// display polygon
		auto& poly = polygons[i];
		for (int j = 0; j < poly.size(); j++) {
			glVertex2i(poly[j].first, poly[j].second);
		}
		glEnd();
		// reset colors back to black (default)
		glColor3f(0, 0, 0);
	}
	glFlush();
}

void OnReshape(int w, int h) {
	printf("Reshape");
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

void Init(void) {
	printf("init");
	// change color of background to help distinguish polygons with no color
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glPointSize(5.0);
	glLogicOp(GL_XOR);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
}

void OnMouseMove(int mouse_x, int mouse_y) {
	if (g_bDragging) {
		// redisplay and update everything on mouse movement
		mousePos = make_pair(mouse_x, WINDOW_HEIGHT - mouse_y);
		glutPostRedisplay();
	}
}

//	MOVEPOLYGON function to move last polygon; this works so long as no other 
//	polygons are being made; if a new polygon is being made, stop moving. This
//	function works by taking all points of the last polygon and moving its y
//	position downward.
void movePolygon(int value) {
	if (!polygons.empty()) {
		auto& lastPolygon = polygons.back();
		for (auto& point : lastPolygon) {
			point.second -= 3;
			if (point.second == 0 || g_bDragging == true) return;
		}
		glutPostRedisplay();
	}
	// this guarentees polygons will stop moving, even after clearing
	if (g_bDragging == true) return;
	glutTimerFunc(16, movePolygon, 0);
}

// KEYBOARD function to read keyboard inputs
void keyboard(unsigned char key, int x, int y) {
	// r, g, f stores color into colorCodes vector for last polygon; this
	// vector is used to render respective colors in the display function
	if (key == 'r') {
		pair<int, vector<GLfloat>> colorEntry(polygons.size() - 1, { 1,0,0 });
		colorCodes.push_back(colorEntry);
		glutPostRedisplay();
	}
	else if (key == 'g') {
		pair<int, vector<GLfloat>> colorEntry(polygons.size() - 1, { 0,1,0 });
		colorCodes.push_back(colorEntry);
		glutPostRedisplay();
	}
	else if (key == 't') {
		pair<int, vector<GLfloat>> colorEntry(polygons.size() - 1, { 1,1,1 });
		colorCodes.push_back(colorEntry);
		glutPostRedisplay();
	}
	// c resets all necessary variables
	else if (key == 'c') {
		points.clear();
		polygons.clear();
		colorCodes.clear();
		firstPoint = true;
		glutPostRedisplay();
	}
	// q simply quits
	else if (key == 'q') {
		exit(1);
	}
	else if (key == 'd') {
		glutTimerFunc(16, movePolygon, 0);
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Polygon Creation using mouse");
	Init();
	glutMouseFunc(mouse);
	glutMotionFunc(OnMouseMove);
	glutDisplayFunc(display);
	//added for keyboard functionality
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(OnReshape);
	glutMainLoop();
	return 0;
}
#include <math.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <SFML/Audio.hpp>
#include <glut.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)//convert degrees to radians
#define GLUT_KEY_SPACE 32



class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;
	//to adjust distance of camera from subject, change values of eye originally {1.0f,1.0f,1.0f}
	//to adjust where the camera is looking change values of center originally {0.0f, 0.0f, 0.0f}
	Camera(float eyeX = 2.0f, float eyeY = 0.8f, float eyeZ = 5.7f, float centerX = 2.0f, float centerY = 0.5f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}
	void topV() {//get top view
		eye = { 2.0f, 5.0f, 3.0f };//top view
		center = { 2.0f, 0.0f, 1.5f };//looking down
	}
	void frontV() {//get front view
		eye = { 2.0f, 0.8f, 5.7f };//front view
		center = { 2.0f, 0.5f, 0.0f };//horizontal center of game scene
	}
	void opfrontV() {//opposite of front view
		eye = { 2.0f, 0.5f, 1.0f };//horizontal center of game scene
		center = { 2.0f, 0.8f, 5.7f };//front view
	}
	void sideV() {//get side view
		eye = { 3.5f, 0.8f, 3.0f };//side view
		center = { 0.0f, 0.8f, 3.0f };//looking to the left
	}
	void backV() {//get opposite of side view
		eye = { 0.1f, 0.8f, 3.0f };//opposite of side view
		center = { 3.5f, 0.8f, 3.0f };//looking to right
	}
	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

sf::Music music;
//game object sfx
sf::Sound gameObj;
sf::SoundBuffer sound_Buffer;
//start target shoot sfx
sf::Sound start;
sf::SoundBuffer sound_Buffer2;
//fence collision sfx
sf::Sound fcollide;
sf::SoundBuffer sound_Buffer3;
//hit target sfx
sf::Sound success;
sf::SoundBuffer sound_Buffer4;
//did not hit target sfx
sf::Sound fail;
sf::SoundBuffer sound_Buffer5;
//game win
sf::Sound winner;
sf::SoundBuffer sound_Buffer6;
//game lose
sf::Sound loser;
sf::SoundBuffer sound_Buffer7;

int times = 75;//total time of game (1:15min) 75sec
int timer = 1000;//switch to 5000 to play game loss or game win sound
bool end = false;//time ran out or not
int gCount = 0;//number of goal objects collected
bool win = false;//collected all goal objects or not

bool played = false;//game object 1 sound played or not
bool played2 = false;//game object 2 sound played or not
bool played3 = false;//game object 3 sound played or not

bool appear = true;//goal object 1 appears or not (for collision)
bool appear2 = true;//goal object 2 appears or not (for collision)
bool appear3 = true;//goal object 3 appears or not (for collision)

bool cappear = false;
bool cappear2 = false;
bool cappear3 = false;

bool anim = false;//animate objects or not

double pX = 1.5;//player x position start at 1.5 to collect should be at 2.5 collide with right fence at 2.7 collide with left fence at 0.3
double pZ = 4.5;//player z position start at 4.5 to collect should be at 1.6 collide with back fence at -0.15 player goes up if <1.2
double pY = 0.388;//player y position start at 0.388 if pZ<1.2 pY = 0.5

double pR = 90;//player rotation angle

int indNum = 0;//which target activated
double arrY1 = 0.12;//powerBar 1 indicator height max 0.3 min -0.05 green min 0.07 green max 0.17
bool arrUp1 = true;//powerBar 1 indicator going up or not
bool arr1 = false;//indicator 1 activated or not
bool hit1 = false;//hit target 1 or not
bool start1 = false;//started target 1 or not
double arrY2 = 0.12;//powerBar 2 indicator height
bool arrUp2 = true;//powerBar 2 indicator going up or not
bool arr2 = false;//indicator 2 activated or not
bool hit2 = false;//hit target 2 or not
bool start2 = false;//started target 2 or not
double arrY3 = 0.12;//powerBar 3 indicator height
bool arrUp3 = true;//powerBar 3 indicator going up or not
bool arr3 = false;//indicator 3 activated or not
bool hit3 = false;//hit target 3 or not
bool start3 = false;//started target 3 or not

double goY = 1.5;//goal object 1 height min is 0.5 max is 2
double goY2 = 1.5;//goal object 2 height min is 0.5 max is 2
double goY3 = 1.5;//goal object 3 height min is 0.5 max is 2

double goR = 0;//rotation of goal object

int wCount = 0;//count for colour change of walls
double wR = 0.592;//wall Red 1 1
double wG = 0.918;//wall Green 0.741 0.949
double wB = 1;//wall Blue 0.945 0.741

double camX = 1.5;//camera object x position
bool camRight = true;//camera moving right or not

double treeH = 1.5;//tree scaling in Y
bool treeUp = false;//tree growing up or not

double crowdR = 0;//crowd rotation angle
double crowdY1 = 0;//crowd y position
double crowdY2 = 0;//crowd 2 y position
bool crowdLeft = false;//rotating left or not

double flagR = 0;//flag rotation angle
double flagLeft = false;//rotating left or not

double screenR = 0;//screen Red
double screenG = 0;//screen Green
double screenB = 0;//screen Blue

//print text in 3d space
void print(double x, double y, double z, void* font, std::string string) {
	glRasterPos3d(x, y, z);
	for (int i = 0; i < string.length(); i++) {
		glutBitmapCharacter(font, string[i]);
	}
}
//display the time
void printTime() {
	glColor3f(0, 0, 0);
	int min = times / 60;
	int sec = times % 60;
	std::string t = std::to_string(min) + " : " + ((std::to_string(sec).length() < 2) ? ("0" + std::to_string(sec)) : std::to_string(sec));
	print(1.5, 0.25, 2, GLUT_BITMAP_HELVETICA_18, t);

}

//boundary walls and ground
void drawWall(double thickness) {//back wall
	glColor3f(wR, wG, wB);
	// switch to: 0.757 0.561 1    
	//0.62 0.949 0.804
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(2.0, thickness, 1.0);
	glutSolidCube(0.5);
	glPopMatrix();
}
void drawGround(double thickness, double R, double G, double B) {//ground
	glColor3f(R, G, B);
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(2.0, thickness, 3.0);
	glutSolidCube(0.5);//10
	glPopMatrix();
}
void drawWall1(double thickness, double R, double G, double B) {//side walls
	glColor3f(R, G, B);
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 3.0);
	glutSolidCube(0.5);//10
	glPopMatrix();
}

void fence() {//2 primitives
	glColor3f(0.09, 0.365, 0.788);
	glPushMatrix();//fence
	glScaled(4, 1, 0.1);
	glutSolidCube(0.2);
	glPopMatrix();
	glColor3f(0.369, 0.62, 1);
	glPushMatrix();//rail
	glTranslated(-0.4, 0.13, 0);
	glRotated(90, 0, 1, 0);
	GLUquadricObj* rail;
	rail = gluNewQuadric();
	gluQuadricDrawStyle(rail, GLU_FILL);
	gluCylinder(rail, 0.03, 0.03, 0.8, 15, 15);
	glPopMatrix();

}
void olympicRings() {//5 primitives
	//olympic rings
	glColor3f(0, 0.506, 0.78);
	glPushMatrix();//top right ring
	glTranslated(-0.04, 0.02, 0.05);
	glutSolidTorus(0.003, 0.02, 15, 15);
	glPopMatrix();
	glColor3f(0.933, 0.2, 0.306);
	glPushMatrix();//top middle ring
	glTranslated(0, 0.02, 0.05);
	glutSolidTorus(0.003, 0.02, 15, 15);
	glPopMatrix();
	glColor3f(0, 0, 0);
	glPushMatrix();//top right ring
	glTranslated(0.04, 0.02, 0.05);
	glutSolidTorus(0.003, 0.02, 15, 15);
	glPopMatrix();
	glColor3f(0.984, 0.698, 0.188);
	glPushMatrix();//bottom left ring
	glTranslated(-0.02, -0.01, 0.05);
	glutSolidTorus(0.003, 0.02, 15, 15);
	glPopMatrix();
	glColor3f(0, 0.651, 0.322);
	glPushMatrix();//bottom right ring
	glTranslated(0.02, -0.01, 0.05);
	glutSolidTorus(0.003, 0.02, 15, 15);
	glPopMatrix();

}
void GoalObject() {//collectible goal object 8 primitives

	glColor3f(1, 0.718, 0.153);
	glPushMatrix();//main
	glScaled(1.5, 1, 1);
	glutSolidCube(0.1);
	glPopMatrix();
	olympicRings();
	glColor3f(1, 0.106, 0.369);
	glPushMatrix();//top
	glTranslated(0, 0.05, 0);
	glScaled(1.5, 1, 1);
	glRotated(45, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	GLUquadricObj* gTop;
	gTop = gluNewQuadric();
	gluQuadricDrawStyle(gTop, GLU_FILL);
	gluCylinder(gTop, 0.1, 0, 0.1, 4, 15);
	glPopMatrix();
	glColor3f(1, 0.106, 0.369);
	glPushMatrix();
	glPushMatrix();//bottom
	glTranslated(0, -0.05, 0);
	glScaled(1.5, -1, 1);
	glRotated(45, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	GLUquadricObj* bTop;
	bTop = gluNewQuadric();
	gluQuadricDrawStyle(bTop, GLU_FILL);
	gluCylinder(bTop, 0.1, 0, 0.1, 4, 15);
	glPopMatrix();
	glPopMatrix();
}
void drawArrow() {//arrow 2 primitives
	glColor3f(0, 0, 0);
	glPushMatrix();//shaft
	glTranslated(0.028, 0.18, 0.18);
	glRotated(10, 0, 1, 0);
	glRotated(90, 0, 1, 0);
	GLUquadricObj* shaft;
	shaft = gluNewQuadric();
	gluQuadricDrawStyle(shaft, GLU_FILL);
	gluCylinder(shaft, 0.01, 0.01, 0.5, 15, 15);
	glPushMatrix();//tip
	glTranslated(0, 0, 0.5);
	glutSolidCone(0.01, 0.015, 15, 15);
	glPopMatrix();
	glPopMatrix();

}
void target(double tR, double tG, double tB, bool hit) {//8-9 primitives
	glColor3f(tR, tG, tB);//green if hit, red if not hit
	glPushMatrix();//board
	glScaled(1.5, 2, 0.3);
	glutSolidCube(0.3);
	glPopMatrix();
	glColor3f(0.51, 0.337, 0);
	glPushMatrix();//vertical support
	glTranslated(0, -0.05, -0.15);
	glRotated(25, 1, 0, 0);
	glScaled(1, 13, 1);
	glutSolidCube(0.05);
	glPopMatrix();
	glColor3f(0.51, 0.337, 0);
	glPushMatrix();//horizontal support left
	glTranslated(0.05, -0.35, -0.15);
	glRotated(20, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	glScaled(1, 7, 1);
	glutSolidCube(0.05);
	glPopMatrix();
	glColor3f(0.51, 0.337, 0);
	glPushMatrix();//horizontal support right
	glTranslated(-0.05, -0.35, -0.15);
	glRotated(-20, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	glScaled(1, 7, 1);
	glutSolidCube(0.05);
	glPopMatrix();

	//rings
	glColor3f(0.988, 0.933, 0.106);
	glPushMatrix();//inner ring
	glTranslated(0, 0.02, 0.05);
	glScaled(1, 1, 0.5);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
	glColor3f(0.922, 0.122, 0.157);
	glPushMatrix();//middle ring
	glTranslated(0, 0.02, 0.05);
	glutSolidTorus(0.03, 0.1, 15, 15);
	glPopMatrix();
	glColor3f(0.141, 0.671, 0.886);
	glPushMatrix();//outer ring1
	glTranslated(0, 0.02, 0.05);
	glutSolidTorus(0.03, 0.15, 15, 15);
	glPopMatrix();
	glColor3f(0, 0, 0);
	glPushMatrix();//outer ring2
	glTranslated(0, 0.02, 0.05);
	glutSolidTorus(0.03, 0.2, 15, 15);
	glPopMatrix();
	if (hit) {
		glPushMatrix();
		glTranslated(-0.1, -0.15, 0.5);
		glRotated(90, 0, 1, 0);
		drawArrow();
		glPopMatrix();
	}


}
void cam() {//5 primitives
	glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();//main
	glScaled(1, 1, 2);
	glutSolidCube(0.1);
	glPopMatrix();
	glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();//lens outline
	glTranslated(0, 0, 0.1);
	glutSolidTorus(0.01, 0.03, 15, 15);
	glPopMatrix();
	glColor3f(0.678, 0.929, 1);
	glPushMatrix();//lens
	glTranslated(0, 0, 0.1);
	glScaled(1, 1, 0.5);
	glutSolidSphere(0.03, 15, 15);
	glPopMatrix();
	glColor3f(0.878, 0, 0.075);
	glPushMatrix();//recording light
	glTranslated(0.04, 0.04, 0.1);
	glScaled(1, 1, 0.5);
	glutSolidSphere(0.01, 15, 15);
	glPopMatrix();
	glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();//stand
	glTranslated(0, -0.15, 0);
	glScaled(1, 8, 1);
	glutSolidCube(0.03);
	glPopMatrix();

}
void tree() {//4 primitives
	glColor3f(0.51, 0.337, 0);
	glPushMatrix();//trunk
	glRotated(90, 1, 0, 0);
	GLUquadricObj* trunk;
	trunk = gluNewQuadric();
	gluQuadricDrawStyle(trunk, GLU_FILL);
	gluCylinder(trunk, 0.07, 0.07, 0.8, 15, 15);
	glPopMatrix();
	glColor3f(0.125, 0.529, 0);
	glPushMatrix();//leaves
	glTranslated(0, 0.3, 0);
	glScaled(1, treeH, 1);
	glutSolidCube(0.4);
	glPopMatrix();
	glColor3f(0.51, 0.337, 0);
	glPushMatrix();//branch
	glTranslated(0.18, 0.02, 0);
	glRotated(-20, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	GLUquadricObj* branch;
	branch = gluNewQuadric();
	gluQuadricDrawStyle(branch, GLU_FILL);
	gluCylinder(branch, 0.02, 0.02, 0.35, 15, 15);
	glPopMatrix();
	glColor3f(0.51, 0.337, 0);
	glPushMatrix();//hole
	glTranslated(0, -0.3, 0.07);
	glutSolidTorus(0.01, 0.05, 15, 15);
	glPopMatrix();

}
void crowd(double R, double G, double B) {//3 primitives
	glColor3f(0.949, 0.769, 0.549);
	glPushMatrix();//head
	glTranslated(0, 0.17, 0);
	glScaled(1, 1.5, 1);
	glutSolidSphere(0.05, 15, 15);
	glPopMatrix();
	glColor3f(R, G, B);
	glPushMatrix();//body
	glScaled(1.5, 4, 1.5);
	glutSolidCube(0.05);
	glPopMatrix();
	glColor3f(0.988, 0.831, 0.416);
	glPushMatrix();//hat
	glTranslated(0, 0.2, 0);
	glScaled(1, 1, 1);
	glRotated(45, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	GLUquadricObj* hat;
	hat = gluNewQuadric();
	gluQuadricDrawStyle(hat, GLU_FILL);
	gluCylinder(hat, 0.1, 0.05, 0.1, 8, 15);
	glPopMatrix();
}
void screen() {//3 primitives
	glColor3f(0.09, 0.365, 0.788);
	glPushMatrix();//backing
	glTranslated(0, 0, 0);
	glRotated(-15, 1, 0, 0);
	glScaled(5, 1, 0.3);
	glutSolidCube(0.3);
	glPopMatrix();
	//glColor3f(1, 0, 0);
	//glPushMatrix();//borders
	//	glTranslated(0, 0, 0.05);
	//	glRotated(-15, 1, 0, 0);
	//	glScaled(5, 1, 0.1);
	//	glutSolidCube(0.3);
	//glPopMatrix();
	glColor3f(screenR, screenG, screenB);
	glPushMatrix();//screen
	glTranslated(0, 0.02, 0.05);
	glRotated(-15, 1, 0, 0);
	glScaled(4, 1, 0.1);
	glutSolidCube(0.3);
	glPopMatrix();
	glColor3f(0.388, 0.282, 0);
	glPushMatrix();//support
	glTranslated(-0.9, -0.05, -0.08);
	glRotated(90, 0, 1, 0);
	glScaled(0.4, 2, 1);
	GLUquadricObj* supp;
	supp = gluNewQuadric();
	gluQuadricDrawStyle(supp, GLU_FILL);
	gluCylinder(supp, 0.1, 0.1, 1.8, 3, 15);
	glPopMatrix();


}
void flag() {//8 primitives
	glColor3f(0.388, 0.282, 0);
	glPushMatrix();//pole
	glScaled(0.05, 2, 0.05);
	glutSolidCube(0.5);
	glPopMatrix();
	glColor3f(1, 0.831, 0);
	glPushMatrix();//sphere on top
	glTranslated(0, 0.5, 0);
	glScaled(1, 1.05, 1);
	glutSolidSphere(0.03, 15, 15);
	glPopMatrix();
	glColor3f(1, 1, 1);
	glPushMatrix();//flag
	glTranslated(0.15, 0.35, 0);
	glScaled(0.6, 0.5, 0.01);
	glutSolidCube(0.5);
	glPopMatrix();
	glPushMatrix();//rings
	glTranslated(0.15, 0.35, -0.044);
	glScaled(2, 2, 1);
	olympicRings();
	glPopMatrix();
}
void powerBar(double indH) {//7 primitives
	glColor3f(0.388, 0.282, 0);
	glPushMatrix();//stand
	glTranslated(0, -0.2, -0.02);
	glScaled(1, 8, 1);
	glutSolidCube(0.03);
	glPopMatrix();
	glColor3f(1, 1, 1);
	glPushMatrix();//backing
	glTranslated(0, 0.12, -0.02);
	glScaled(0.6, 0.9, 0.05);
	glutSolidCube(0.5);
	glPopMatrix();
	glColor3f(1, 0, 0);
	glPushMatrix();//screen red
	glTranslated(0, 0.245, 0);
	glScaled(0.5, 0.25, 0.02);
	glutSolidCube(0.5);
	glPopMatrix();
	glColor3f(0, 1, 0);
	glPushMatrix();//screen green
	glTranslated(0, 0.12, 0);
	glScaled(0.5, 0.25, 0.02);
	glutSolidCube(0.5);
	glPopMatrix();
	glColor3f(1, 0, 0);
	glPushMatrix();//screen red
	glTranslated(0, -0.005, 0);
	glScaled(0.5, 0.25, 0.02);
	glutSolidCube(0.5);
	glPopMatrix();
	glColor3f(0, 0, 0);
	glPushMatrix();//indicator
	glTranslated(0.2, indH, 0.05);
	glScaled(-1, 2, 0.5);
	glRotated(90, 0, 1, 0);
	GLUquadricObj* shaft;
	shaft = gluNewQuadric();
	gluQuadricDrawStyle(shaft, GLU_FILL);
	gluCylinder(shaft, 0.01, 0.01, 0.1, 15, 15);
	glPushMatrix();//tip
	glTranslated(0, 0, 0.1);
	glutSolidCone(0.01, 0.02, 15, 15);
	glPopMatrix();
	glPopMatrix();

}
void drawHead() {//head 1 primitive
	glColor3f(0.949, 0.769, 0.549);
	glPushMatrix();
	glTranslated(0, 0.35, 0);
	glScaled(1, 1.5, 1);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
}
void drawBody() {//torso 1 primitive
	glColor3f(0.38, 0.969, 1);
	glPushMatrix();
	glScaled(1, 2, 1);
	glutSolidCube(0.2);
	glPopMatrix();
}
void drawLeg(double d) {//both legs 1 primitive each
	glColor3f(0.3, 0.3, 0.3);
	glPushMatrix();
	glTranslated(d, -0.5, 0);
	glScaled(0.3, 2, 0.3);
	glutSolidSphere(0.2, 15, 15);
	glPopMatrix();
}
void drawArmL(double d) {//extended left arm 1 primitive
	glColor3f(0.949, 0.769, 0.549);
	glPushMatrix();
	glTranslated(d, 0.1, 0);
	glRotated(90, 0, 0, 1);
	glScaled(0.4, 2, 0.4);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
}
void drawArmR(double d) {//bent right arm 2 primitives
	glColor3f(0.949, 0.769, 0.549);
	glPushMatrix();//shoulder to elbow
	glTranslated(d, 0.2, 0.1);
	glRotated(-30, 0, 1, 0);
	glRotated(40, 1, 0, 0);
	glScaled(0.4, 1.5, 0.4);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
	glColor3f(0.949, 0.769, 0.549);
	glPushMatrix();//forearm
	glTranslated(d + 0.05, 0.25, 0.18);
	glRotated(60, 0, 0, 1);
	glScaled(0.4, 1.5, 0.4);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
}
void drawConfetti() {//9 primitives
	glPushMatrix();
	glScaled(0.5, 0.5, 0.5);
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslated(1, 2, 1);
	glScaled(0.02, 0.02, 0.02);
	glutSolidDodecahedron();
	glPopMatrix();
	glColor3f(0.5, 0, 0);
	glPushMatrix();
	glTranslated(1, 2.5, 1);
	glScaled(0.02, 0.02, 0.02);
	glutSolidDodecahedron();
	glPopMatrix();
	glColor3f(0.5, 0.5, 0);
	glPushMatrix();
	glTranslated(1, 1.5, 1);
	glScaled(0.02, 0.02, 0.02);
	glutSolidDodecahedron();
	glPopMatrix();
	glColor3f(0, 0.5, 0.5);
	glPushMatrix();
	glTranslated(1.5, 2, 1);
	glScaled(0.02, 0.02, 0.02);
	glutSolidDodecahedron();
	glPopMatrix();
	glColor3f(0, 0.5, 0);
	glPushMatrix();
	glTranslated(0.5, 2, 1);
	glScaled(0.02, 0.02, 0.02);
	glutSolidDodecahedron();
	glPopMatrix();
	glColor3f(0, 0, 0.5);
	glPushMatrix();
	glTranslated(1.2, 2, 1);
	glScaled(0.05, 0.05, 0.05);
	glutSolidTetrahedron();
	glPopMatrix();
	glColor3f(0.5, 0.5, 0);
	glPushMatrix();
	glTranslated(0.8, 2, 1);
	glScaled(0.05, 0.05, 0.05);
	glutSolidTetrahedron();
	glPopMatrix();
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslated(1, 2.2, 1);
	glScaled(0.05, 0.05, 0.05);
	glutSolidTetrahedron();
	glPopMatrix();
	glColor3f(0, 0.5, 0.5);
	glPushMatrix();
	glTranslated(1, 1.8, 1);
	glScaled(0.05, 0.05, 0.05);
	glutSolidTetrahedron();
	glPopMatrix();
	glPopMatrix();
}
void drawBow() {//bow 5 primitives
	glColor3f(0.396, 0.416, 0.812);
	glPushMatrix();//handle
	glTranslated(0.45, 0.1, 0);
	glScaled(0.1, 1, 0.1);
	glutSolidCube(0.2);
	glPopMatrix();
	glColor3f(0.627, 0.647, 1);
	glPushMatrix();//top part
	glTranslated(0.36, 0.35, 0);
	glRotated(30, 0, 0, 1);
	glScaled(0.1, 2, 0.1);
	glutSolidCube(0.2);
	glPopMatrix();
	glColor3f(0.627, 0.647, 1);
	glPushMatrix();//bottom part
	glTranslated(0, 0.15, 0);
	glScaled(1, -1, 1);
	glPushMatrix();
	glTranslated(0.36, 0.3, 0);
	glRotated(30, 0, 0, 1);
	glScaled(0.1, 2, 0.1);
	glutSolidCube(0.2);
	glPopMatrix();
	glPopMatrix();
	glColor3f(0.722, 0.576, 0.424);
	glPushMatrix();//top rope
	glTranslated(0.03, 0.2, 0.18);
	glRotated(36, 0, 1, 0);
	glRotated(45, 0, 0, 1);
	glRotated(90, 0, 1, 0);
	GLUquadricObj* Trope;
	Trope = gluNewQuadric();
	gluQuadricDrawStyle(Trope, GLU_FILL);
	gluCylinder(Trope, 0.005, 0.005, 0.42, 15, 15);
	glPopMatrix();
	glColor3f(0.722, 0.576, 0.424);
	glPushMatrix();//bottom rope
	glTranslated(0.03, 0.18, 0.19);
	glRotated(-62, 0, 0, 1);
	glRotated(20, 0, 1, 0);
	glRotated(90, 0, 1, 0);
	GLUquadricObj* Brope;
	Brope = gluNewQuadric();
	gluQuadricDrawStyle(Brope, GLU_FILL);
	gluCylinder(Brope, 0.005, 0.005, 0.58, 15, 15);
	glPopMatrix();

}
void player() {//player character 12 primitives
	drawHead();
	drawBody();
	drawLeg(0.06);
	drawLeg(-0.06);
	drawArmR(-0.15);
	drawArrow();
	drawBow();
	drawArmL(0.25);
}

void setupLights() {
	GLfloat ambient[] = { 1.0f, 1.0f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 1.0f, 1.0f, 1, 1.0f };
	GLfloat lightPosition[] = { 3.0f, 10.0f, 6.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 720 / 576, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (end && !win) {//time ran out before collecting goal(GAME LOSE)
		camera.frontV();
		glColor3f(1, 0, 0);
		print(1.5, 0.5, 0, GLUT_BITMAP_HELVETICA_18, "GAME LOSE");
	}
	else if (end && win) {//collected goal(GAME WIN)
		camera.frontV();
		glColor3f(0, 1, 0);
		print(1.5, 0.5, 0, GLUT_BITMAP_HELVETICA_18, "GAME WIN");
	}
	else {
		glPushMatrix();//game space
		glScaled(4, 3, 4);
		drawGround(0.02, 0.133, 0.62, 0.212);//ground
		glPushMatrix();//elevated ground
		glScaled(1, 1, 0.4);
		glTranslated(0, 0, -0.4);
		drawGround(0.05, 0.133, 0.62, 0.212);
		glPopMatrix();
		glPushMatrix();//back wall
		glTranslated(0, -0.25, -0.25);
		glRotated(-90, 1, 0, 0);
		drawWall(0.02);
		glPopMatrix();
		glPushMatrix();//left wall
		glTranslated(0.01, -0.25, -0.01);
		glRotated(90, 0, 0, 1);
		drawWall1(0.02, wR, wG, wB);
		glPopMatrix();
		glPushMatrix();//right wall
		glTranslated(1, -0.25, -0.01);
		glRotated(90, 0, 0, 1);
		drawWall1(0.02, wR, wG, wB);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0.5, 0, 0);
		glPushMatrix();
		glScaled(1, 1, 1.32);
		glPushMatrix();//back fence
		glTranslated(1.5, 0, -0.4);
		glScaled(3.5, 5, 1.5);
		fence();
		glPopMatrix();
		glPushMatrix();//left fence
		glTranslated(0.2, 0.2, 1);
		glRotated(90, 0, 1, 0);
		glScaled(7, 1, 1.5);
		fence();
		glPopMatrix();
		glPushMatrix();//right fence
		glTranslated(2.8, 0.2, 1);
		glRotated(90, 0, 1, 0);
		glScaled(7, 1, 1.5);
		fence();
		glPopMatrix();
		glPopMatrix();
		if (appear) {
			glPushMatrix();//goalObject 1 rotates 360
			glTranslated(2, goY, 2.5);//min height is 0.3
			glRotated(goR, 0, 1, 0);
			GoalObject();
			glPopMatrix();
		}
		if (appear2) {
			glPushMatrix();//goalObject 2 rotates 360
			glTranslated(1.5, goY2, 2);//min height is 0.3
			glRotated(goR, 0, 1, 0);
			GoalObject();
			glPopMatrix();
		}
		if (appear3) {
			glPushMatrix();//goalObject 3 rotates 360
			glTranslated(1, goY3, 2.5);//min height is 0.3
			glRotated(goR, 0, 1, 0);
			GoalObject();
			glPopMatrix();
		}
		glPushMatrix();//player
		glTranslated(pX, pY, pZ);
		glRotated(pR, 0, 1, 0);
		glScaled(0.4, 0.4, 0.4);
		player();
		glPopMatrix();
		glPushMatrix();//target 1
		glTranslated(0.8, 0.4, 0.2);
		glScaled(0.8, 0.8, 0.8);
		if (hit1)
			target(0, 1, 0, hit1);
		else
			target(1, 1, 1, false);
		glPopMatrix();
		glPushMatrix();//target 2
		glTranslated(1.5, 0.4, 0.2);
		glScaled(0.8, 0.8, 0.8);
		if (hit2)
			target(0, 1, 0, hit2);
		else
			target(1, 1, 1, false);
		glPopMatrix();
		glPushMatrix();//target 3
		glTranslated(2.2, 0.4, 0.2);
		glScaled(0.8, 0.8, 0.8);
		if (hit3)
			target(0, 1, 0, hit3);
		else
			target(1, 1, 1, false);
		glPopMatrix();
		glPushMatrix();//camera translates left and right
		glTranslated(camX, 0.3, 2.5);
		cam();
		glPopMatrix();
		glPushMatrix();//left tree leaves grow and shrink
		glTranslated(-0.1, 0.5, 0.2);
		glRotated(45, 0, 1, 0);
		glScaled(0.5, 0.5, 0.5);
		tree();
		glPopMatrix();
		glPushMatrix();//right tree leaves grow and shrink
		glTranslated(3.2, 0.5, 0.2);
		glRotated(-45, 0, 1, 0);
		glScaled(0.5, 0.5, 0.5);
		tree();
		glPopMatrix();
		glPushMatrix();//power bar 1
		glTranslated(1.2, 0.4, 1);
		glScaled(0.5, 1, 1);
		powerBar(arrY1);
		glPopMatrix();
		glPushMatrix();//power bar 2
		glTranslated(1.8, 0.4, 1);
		glScaled(0.5, 1, 1);
		powerBar(arrY2);
		glPopMatrix();
		glPushMatrix();//power bar 3
		glTranslated(2.4, 0.4, 1);
		glScaled(0.5, 1, 1);
		powerBar(arrY3);
		glPopMatrix();
		glPushMatrix();//screen changes colours
		glTranslated(1.5, 0.2, 1.4);
		glScaled(1, 0.75, 1);
		screen();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0, crowdY1, -0.5);//crowd members jump up and down
		glPushMatrix();//crowd member 1 rotates
		glTranslated(-0.2, 0.2, 2);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(1, 0, 0);
		glPopMatrix();
		glPushMatrix();//crowd member 2 rotates
		glTranslated(0, 0.2, 2.7);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.463, 0.227, 0.788);
		glPopMatrix();
		glPushMatrix();//crowd member 3 rotates
		glTranslated(3.3, 0.2, 2);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.216, 0.988, 0.494);
		glPopMatrix();
		glPushMatrix();//crowd member 4 rotates
		glTranslated(3.05, 0.2, 2.7);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(1, 0.651, 0);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0, crowdY2, -1.5);//crowd members jump up and down
		glPushMatrix();//crowd member 5 rotates
		glTranslated(-0.2, 0.2, 2);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.216, 0.988, 0.494);
		glPopMatrix();
		glPushMatrix();//crowd member 6 rotates
		glTranslated(0, 0.2, 2.7);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(1, 0.651, 0);
		glPopMatrix();
		glPushMatrix();//crowd member 7 rotates
		glTranslated(3.3, 0.2, 2);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(1, 0, 0);
		glPopMatrix();
		glPushMatrix();//crowd member 8 rotates
		glTranslated(3.05, 0.2, 2.7);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.463, 0.227, 0.788);
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, crowdY1, 0);//crowd members jump up and down
		glPushMatrix();//crowd member 9 rotates
		glTranslated(-0.2, 0.2, 2);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(1, 1, 0);
		glPopMatrix();
		glPushMatrix();//crowd member 10 rotates
		glTranslated(0, 0.2, 2.7);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(1, 0, 0.5);
		glPopMatrix();
		glPushMatrix();//crowd member 11 rotates
		glTranslated(3.3, 0.2, 2);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.3, 0, 1);
		glPopMatrix();
		glPushMatrix();//crowd member 12 rotates
		glTranslated(3.05, 0.2, 2.7);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.7, 0, 0.2);
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, crowdY2, 0.5);//crowd members jump up and down
		glPushMatrix();//crowd member 13 rotates
		glTranslated(-0.2, 0.2, 2);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0, 0.5, 0.5);
		glPopMatrix();
		glPushMatrix();//crowd member 14 rotates
		glTranslated(0, 0.2, 2.7);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.8, 0, 0);
		glPopMatrix();
		glPushMatrix();//crowd member 15 rotates
		glTranslated(3.3, 0.2, 2);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.5, 0, 0.5);
		glPopMatrix();
		glPushMatrix();//crowd member 16 rotates
		glTranslated(3.05, 0.2, 2.7);
		glRotated(crowdR, 0, 1, 0);
		glScaled(1, 1.2, 1);
		crowd(0.788, 0.788, 0);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();//flag rotates
		glRotated(flagR, 0, 1, 0);
		glTranslated(0.05, 0.7, 0);
		glScaled(1.5, 1.5, 1.5);
		flag();
		glPopMatrix();
		glPopMatrix();
		//goal object 1 collision detection (right target)
		if (goY == 0.5 && (pX >= 1.8 && pX <= 2.2) && (pZ >= 2.3 && pZ <= 2.7)) {
			appear = false;
			glPushMatrix();
			//glScaled(Sx, Sy, Sz);
			glTranslated(2, 0, 2);
			drawConfetti();
			glPopMatrix();


		}
		//goal object 2 collision detection (middle target)
		if (goY2 == 0.5 && (pX >= 1.3 && pX <= 1.7) && (pZ >= 1.8 && pZ <= 2.2)) {
			appear2 = false;

			glPushMatrix();
			//glScaled(Sx, Sy, Sz);
			glTranslated(1.5, 0, 1.5);
			drawConfetti();
			glPopMatrix();
		}
		//goal object 3 collsion detection (left target)
		if (goY3 == 0.5 && (pX >= 0.8 && pX <= 1.2) && (pZ >= 2.3 && pZ <= 2.7)) {
			appear3 = false;
			glPushMatrix();
			//glScaled(Sx, Sy, Sz);
			glTranslated(1, 0, 2);
			drawConfetti();
			glPopMatrix();
		}
		if (gCount == 3)
			win = true;
		if (pZ < 1.4)
			pY = 0.5;
		else
			pY = 0.388;
		if (!end) {
			glColor3f(1, 0, 0);
			print(2, 0.32, 2, GLUT_BITMAP_HELVETICA_10, "shoot the targets!");
			print(2, 0.22, 2, GLUT_BITMAP_HELVETICA_10, "collect the gem!");
			printTime();
		}


	}

	glFlush();
}
//keyboard functions
void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;
	float a = 1.0;

	switch (key) {
	case 'q'://move cam up
		camera.moveY(d);
		break;
	case 'e'://move cam down
		camera.moveY(-d);
		break;
	case 'a'://move cam right
		camera.moveX(d);
		break;
	case 'd'://move cam left
		camera.moveX(-d);
		break;
	case 'w'://move cam forward
		camera.moveZ(d);
		break;
	case 's'://move cam backward 
		camera.moveZ(-d);
		break;
	case GLUT_KEY_ESCAPE://close game
		exit(EXIT_SUCCESS);
		break;
	case 't'://get top view
		camera.topV();
		break;
	case 'f'://get front view
		camera.frontV();
		break;
	case 'h':
		camera.opfrontV();
		break;
	case 'g'://get side view
		camera.sideV();
		break;
	case 'b'://get opposite of side view
		camera.backV();
		break;
	case 'i'://rotate camera up
		camera.rotateX(a);
		break;
	case 'k'://rotate camera down
		camera.rotateX(-a);
		break;
	case 'j'://rotate camera right
		camera.rotateY(a);
		break;
	case 'l'://rotate camera left
		camera.rotateY(-a);
		break;
	case'm':
		anim = (anim == true) ? false : true;
		break;
	case GLUT_KEY_SPACE://power bar indicator animation
		if (pX <= 1.2 && pX >= 0.8 && pR == 90) {
			indNum = 1;
			if (!start1) {
				start.play();
				start1 = true;
			}

			if (0.07 <= arrY1 && arrY1 <= 0.17 && arr1 == true && !hit1) {
				hit1 = true;

				goY3 = 0.5;
				success.play();
			}
			else if (arr1)
				fail.play();
			arr1 = (arr1 == true) ? false : true;
		}
		else if (pX <= 1.7 && pX >= 1.3 && pR == 90) {
			indNum = 2;
			if (!start2) {
				start.play();
				start2 = true;
			}

			if (0.07 <= arrY2 && arrY2 <= 0.17 && arr2 == true && !hit2) {
				hit2 = true;

				goY2 = 0.5;
				success.play();
			}
			else if (arr2)
				fail.play();
			arr2 = (arr2 == true) ? false : true;
		}
		else if (pX <= 2.2 && pX >= 1.8 && pR == 90) {
			indNum = 3;
			if (!start3) {
				start.play();
				start3 = true;
			}

			if (0.07 <= arrY3 && arrY3 <= 0.17 && arr3 == true && !hit3) {
				hit3 = true;

				goY = 0.5;
				success.play();
			}
			else if (arr3)
				fail.play();
			arr3 = (arr3 == true) ? false : true;
		}
		else
			indNum = 0;

		break;
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {


	switch (key) {
	case GLUT_KEY_UP://move character forward +z
		if (pZ > -0.15)
			pZ -= 0.01;
		if (pZ >= -0.15 && pZ <= -0.13)
			fcollide.play();
		pR = 90;
		break;
	case GLUT_KEY_DOWN://move character backward -z
		if (pZ < 4.8)
			pZ += 0.01;
		if (pZ >= 4.6 && pZ <= 5)
			fcollide.play();
		pR = 270;
		break;
	case GLUT_KEY_LEFT://move character right +x
		if (pX > 0.4)
			pX -= 0.01;
		if (pX >= 0.4 && pX <= 0.42)
			fcollide.play();
		pR = 180;
		break;
	case GLUT_KEY_RIGHT://move character left -x
		if (pX < 2.6)
			pX += 0.01;
		if (pX >= 2.58 && pX <= 2.6)
			fcollide.play();
		pR = 360;
		break;

	}

	glutPostRedisplay();
}
//rotate character using mouse 
void actM(int button, int state, int x, int y)//mouse function takes 4 parameters: button: which button has been clicked (GLUT_RIGHT_BUTTON or GLUT_LEFT_BUTTON),
//state whether the button is clicked or released (GLUT_UP or GLUT_DOWN)
// x and y are the position of the mouse cursor
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//if the left button has been clicked rotate character left
	{
		pR += 10;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) //if the right button has been clicked rotate character right
	{
		pR -= 10;
	}


	glutPostRedisplay();
}
//count time down
void countDown(int val) {
	if (times > 0)
		times -= 1;
	if (times == 0)
		end = true;
	if (end && win) {
		music.stop();
		winner.play();
		timer = 5000;
	}
	if (end && !win) {
		music.stop();
		loser.play();
		timer = 5000;
	}


	glutPostRedisplay();
	glutTimerFunc(timer, countDown, 0);
}
//crowd jumping animation
void jumping(int val) {
	if (anim && times % 2 == 0) {
		crowdY1 += 0.005;
		crowdY2 -= 0.005;
	}
	else if (anim && times % 2 == 1) {
		crowdY1 -= 0.005;
		crowdY2 += 0.005;
	}
	glutPostRedisplay();
	glutTimerFunc(100, jumping, 0);
}
//wall colour change
void colourChange(int val) {
	wCount += 1;
	if (wCount % 2 == 0) {
		wR = 0.592;//wall Red 1 1
		wG = 0.918;//wall Green 0.741 0.949
		wB = 1;//wall Blue 0.945 0.741
	}
	else if (wCount % 3 == 0) {
		wR = 1;//wall Red 1 1
		wG = 0.741;//wall Green 0.741 0.949
		wB = 0.945;//wall Blue 0.945 0.741
	}

	else {
		wR = 1;//wall Red 1 1
		wG = 0.949;//wall Green 0.741 0.949
		wB = 0.741;//wall Blue 0.945 0.741
	}

	glutPostRedisplay();
	glutTimerFunc(5000, colourChange, 0);
}
//objectAnimations
void Animate() {
	if (goR <= 360)
		goR += 0.1;
	else
		goR = 0;
	if (anim) {
		//tree animation (minor)
		if (treeH <= 2 && treeUp)
			treeH += 0.005;
		else if (treeH > 1.5 && !treeUp)
			treeH -= 0.005;
		else {
			if (treeUp)
				treeUp = false;
			else
				treeUp = true;
		}
		//crowd member animation (minor)
		if (crowdR <= 50 && crowdLeft)
			crowdR += 0.1;
		else if (crowdR > 0 && !crowdLeft)
			crowdR -= 0.1;
		else {
			if (crowdLeft)
				crowdLeft = false;
			else
				crowdLeft = true;
		}
		//flag animation (major)
		if (flagR <= 30 && flagLeft)
			flagR += 0.1;
		else if (flagR > 0 && !flagLeft)
			flagR -= 0.1;
		else {
			if (flagLeft)
				flagLeft = false;
			else
				flagLeft = true;
		}
		//camera object animation (major)
		if (camX <= 2 && camRight)
			camX += 0.001;
		else if (camX > 1 && !camRight)
			camX -= 0.001;
		else {
			if (camRight)
				camRight = false;
			else
				camRight = true;
		}
		//screen animation(minor)
		if (times % 7 == 0) {
			screenR = 1;
			screenG = 0;
			screenB = 0.957;
		}
		else if (times % 3 == 0) {
			screenR = 0;
			screenG = 1;
			screenB = 0.043;
		}
		else if (times % 5 == 0) {
			screenR = 0;
			screenG = 0.98;
			screenB = 1;
		}
		else {
			screenR = 1;
			screenG = 0.906;
			screenB = 0;
		}
	}
	if (indNum == 1 && arr1) {
		//powerBars animations:
		if (arrY1 <= 0.3 && arrUp1)
			arrY1 += 0.001;
		else if (arrY1 > -0.05 && !arrUp1)
			arrY1 -= 0.001;
		else {
			if (arrUp1)
				arrUp1 = false;
			else
				arrUp1 = true;
		}
	}
	else if (indNum == 2 && arr2) {
		//powerBars animations:
		if (arrY2 <= 0.3 && arrUp2)
			arrY2 += 0.001;
		else if (arrY2 > -0.05 && !arrUp2)
			arrY2 -= 0.001;
		else {
			if (arrUp2)
				arrUp2 = false;
			else
				arrUp2 = true;
		}
	}
	else if (indNum == 3 && arr3) {
		//powerBars animations:
		if (arrY3 <= 0.3 && arrUp3)
			arrY3 += 0.001;
		else if (arrY3 > -0.05 && !arrUp3)
			arrY3 -= 0.001;
		else {
			if (arrUp3)
				arrUp3 = false;
			else
				arrUp3 = true;
		}
	}
	/*if (goY == 0.5 && (pX >= 2.4 && pX <= 2.7) && (pZ >= 1.5 && pZ <= 1.7) && !played) {
		gameObj.play();
		played = true;
	}*/
	//goal object 1 collection (right)
	if (goY == 0.5 && (pX >= 1.8 && pX <= 2.2) && (pZ >= 2.3 && pZ <= 2.7) && !played) {
		gCount += 1;
		gameObj.play();
		played = true;

	}
	//goal object 2 collection (middle)
	if (goY2 == 0.5 && (pX >= 1.3 && pX <= 1.7) && (pZ >= 1.8 && pZ <= 2.2) && !played2) {
		gCount += 1;
		gameObj.play();
		played2 = true;

	}
	//goal object 3 collection (left)
	if (goY3 == 0.5 && (pX >= 0.8 && pX <= 1.2) && (pZ >= 2.3 && pZ <= 2.7) && !played3) {
		gCount += 1;
		gameObj.play();
		played3 = true;

	}
	glutPostRedisplay();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(720, 576);
	glutInitWindowPosition(100, 60);

	glutCreateWindow("Olympic Archery");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutTimerFunc(1000, countDown, 0);
	glutTimerFunc(5000, colourChange, 0);
	glutTimerFunc(200, jumping, 0);
	glutIdleFunc(Animate);
	glutMouseFunc(actM);

	music.openFromFile("P29_58_1501_Bill Conti - Power.ogg");
	music.setVolume(100.0f);
	music.setLoop(true);
	music.play();

	sound_Buffer.loadFromFile("P29_58_1501_chaos-emerald-15983.wav");
	gameObj.setBuffer(sound_Buffer);
	gameObj.setVolume(80.0f);

	sound_Buffer2.loadFromFile("P29_58_1501_086354_8-bit-arcade-video-game-start-sound-effect-gun-reload-and-jump-81124.wav");
	start.setBuffer(sound_Buffer2);
	start.setVolume(100.0f);

	sound_Buffer3.loadFromFile("P29_58_1501_error-10-206498.wav");
	fcollide.setBuffer(sound_Buffer3);
	fcollide.setVolume(100.0f);

	sound_Buffer4.loadFromFile("P29_58_1501_success-1-6297.wav");
	success.setBuffer(sound_Buffer4);
	success.setVolume(100.0f);

	sound_Buffer5.loadFromFile("P29_58_1501_error-5-199276.wav");
	fail.setBuffer(sound_Buffer5);
	fail.setVolume(400.0f);

	sound_Buffer6.loadFromFile("P29_58_1501_level-win-6416.wav");
	winner.setBuffer(sound_Buffer6);
	winner.setVolume(100.0f);

	sound_Buffer7.loadFromFile("P29_58_1501_fail-144746.wav");
	loser.setBuffer(sound_Buffer7);
	loser.setVolume(100.0f);


	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}

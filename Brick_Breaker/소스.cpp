#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>
#include <stdio.h>

#define	width 			400
#define	height			600
#define	PI				3.1415
#define	polygon_num		50

int		left = 0;
int		bottom = 0;

int		collision_count = 0;

float	radius1, moving_ball_radius;

float	stick_x, stick_y;
float	move = 10;

// 공의 위치 정보를 저장할 구조체
typedef struct _Point {
	float	x;
	float	y;
} Point;

Point	fixed_ball, moving_ball, velocity;

// 공을 그리는 함수
void Modeling_Circle(float radius, Point CC);

// 공과 공의 충돌을 검사하는 함수
void Collision_Detection_Between_Balls(void);

// 공과 벽의 충돌을 검사하는 함수
void Collision_Detection_to_Walls(void);

// 공과 스틱의 충돌을 검사하는 함수
void Collision_Detection_With_Stick(void);

// 초기화 함수
void init(void);

// 윈도우 크기 변경 시 호출되는 함수
void MyReshape(int w, int h);

// 화면을 그리는 함수
void RenderScene(void);

// 화면 배경색 설정 함수
void frame_reset(void);

// 스틱 그리는 함수
void stick(void);

void init(void) {
	// 고정된 공의 반지름과 초기 위치 설정
	radius1 = 20.0;
	fixed_ball.x = width / 2;
	fixed_ball.y = height / 2;

	// 움직이는 공의 반지름과 초기 위치, 속도 설정
	moving_ball_radius = 10.0;
	moving_ball.x = width / 2;
	moving_ball.y = height / 4;

	velocity.x = 0.0;
	velocity.y = 0.05;

	collision_count = 1;

	// 스틱 초기위치 
	stick_x = width / 2 - 95.0 / 2;
	stick_y = 10.0;
}


void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 마우스 왼쪽 버튼 클릭 시 좌표계 원점 이동
	gluOrtho2D(left, left + width, bottom, bottom + height); // mouse2()
}


void	Modeling_Circle(float radius, Point CC) {
	float	delta;

	delta = 2 * PI / polygon_num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
	glEnd();
}

void Collision_Detection_Between_Balls(void) {
	float	distance;

	float	delta_x;
	float	delta_y;

	delta_x = fixed_ball.x - moving_ball.x;
	delta_y = fixed_ball.y - moving_ball.y;

	distance = sqrt(delta_x * delta_x + delta_y * delta_y);

	if (distance <= radius1 + moving_ball_radius) {
		printf("fixed 공 충돌함\n");
		velocity.x = velocity.x * -1;
		velocity.y = velocity.y * -1;
		collision_count++;
	}
}

void Collision_Detection_to_Walls(void) {

	if (left >= moving_ball.x - moving_ball_radius) {
		printf("왼쪽 충돌함\n");
		velocity.x = velocity.x * -1;
	}
	else if (width <= moving_ball.x + moving_ball_radius) {
		printf("오른쪽 충돌함\n");
		velocity.x = velocity.x * -1;
	}
	else if (height <= moving_ball.y + moving_ball_radius) {
		printf("위 충돌함\n");
		velocity.y = velocity.y * -1;
	}
	else if (bottom >= moving_ball.y - moving_ball_radius) {
		printf("아래 충돌함\n");
		velocity.y = velocity.y * -1;

	}

}

void Collision_Detection_With_Stick(void) {
	if (moving_ball.y - moving_ball_radius <= stick_y + 25.0 &&
		moving_ball.x > stick_x && moving_ball.x < stick_x + 95.0) {
		printf("슬러브 봉에 충돌함\n");
		velocity.y = velocity.y * -1;
	}
}

void frame_reset(void) {
	glClearColor(1.0, 1.0, 0.0, 0.0); // Set display-window color to Yellow
	glClear(GL_COLOR_BUFFER_BIT);
}

void ball(void) {
	// 윈도우 중심의 위치에 고정된 공 그리기 
	//glColor3f(1.0, 0.0, 0.0);
	//if (collision_count % 2)
	//	Modeling_Circle(radius1, fixed_ball);

	// 충돌 처리 부분
	Collision_Detection_Between_Balls();		// 공과 공의 충돌 함수 
	Collision_Detection_to_Walls();			// 공과 벽의 충돌 함수 
	Collision_Detection_With_Stick();		// 공과 스틱의 충돌 함수 

	// 움직이는 공의 위치 변화 
	moving_ball.x += velocity.x;
	moving_ball.y += velocity.y;

	// 움직이는 공 그리기 
	glColor3f(0.0, 0.0, 1.0);
	Modeling_Circle(moving_ball_radius, moving_ball);
}

void stick(void) {
	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_POLYGON);
	glVertex2f(stick_x, stick_y);
	glVertex2f(stick_x + 95.0, stick_y);
	glVertex2f(stick_x + 95.0, stick_y + 25.0);
	glVertex2f(stick_x, stick_y + 25.0);
	glEnd();
}

void MySpecial(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		stick_x -= move;
		break;
	case GLUT_KEY_RIGHT:
		stick_x += move;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void RenderScene(void) {
	// 화면 배경색 설정
	frame_reset();

	// 공 그리기
	ball();

	// 막대 그리기
	stick();
	glutSwapBuffers();
	glFlush();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Bouncing Ball & Wall");
	init();
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(MySpecial);
	glutMainLoop();
}
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

#define	WIDTH 			1200
#define	HEIGHT			600
#define	PI				3.1415
#define	POLYGON_num		50

int		left = 0;
int		bottom = 0;

float	moving_ball_radius;				// 움직이는 공의 반지름 

float	stick_x, stick_y;				// 스틱의 가로,세로
float	stick_velocity = 20;			// 스틱 움직이는 속도

const int	num_blocks = 8;				// 블럭 개수 설정

// 공의 위치 정보를 저장할 구조체
typedef struct _Point {
	float	x;
	float	y;
} Point;

typedef struct _Block {
	float	x;
	float	y;
	float	width;
	float	height;
	bool	visible;
} Block;

Point	moving_ball, ball_velocity;		// 움직이는 공, 속도 관련
Point	brick_center;
Point	dist, unit_dist;
Block	blocks[num_blocks];

// 공을 그리는 함수
void Modeling_Circle(float radius, Point CC);

// 스틱 그리는 함수
void Modeling_Stick(void);

// 블럭 그리기 함수
void Modeling_Block(const Block& block);

// 공과 벽의 충돌을 검사하는 함수
void Collision_Detection_to_Walls(void);

// 공과 스틱의 충돌을 검사하는 함수
void Collision_Detection_With_Stick(void);

// 공과 블럭의 충돌을 검사하는 함수
void Collision_Detection_to_Brick(Block& block);

// 초기화 함수
void init(void);

// 블럭 생성 및 초기화 함수
void init_blocks(void);

// 윈도우 크기 변경 시 호출되는 함수
void MyReshape(int w, int h);

// 화면을 그리는 함수
void RenderScene(void);

// 화면 배경색 설정 함수
void frame_reset(void);



void init(void) {
	// 움직이는 공의 반지름과 초기 위치, 속도 설정
	moving_ball_radius = 10.0;
	moving_ball.x = WIDTH / 2;
	moving_ball.y = HEIGHT / 4;

	ball_velocity.x = 0.3;
	ball_velocity.y = 0.3;

	// 스틱 초기위치 
	stick_x = WIDTH / 2 - 95.0 / 2;
	stick_y = 10.0;
}


void init_blocks(void) {
	float	block_width		= 60;
	float	block_height	= 30;
	
	for (int i = 0; i < num_blocks; i++) {
		blocks[i].x = 200 + i * (block_width + 50);
		blocks[i].y = 500;

		blocks[i].width = block_width;
		blocks[i].height = block_height;
		blocks[i].visible = true;
	}
}


void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 마우스 왼쪽 버튼 클릭 시 좌표계 원점 이동
	gluOrtho2D(left, left + WIDTH, bottom, bottom + HEIGHT); // mouse2()
}


void	Modeling_Circle(float radius, Point CC) {
	float	delta;

	delta = 2 * PI / POLYGON_num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < POLYGON_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
	glEnd();
}

void Modeling_Stick(void) {
	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_POLYGON);
	glVertex2f(stick_x, stick_y);
	glVertex2f(stick_x + 95.0, stick_y);
	glVertex2f(stick_x + 95.0, stick_y + 25.0);
	glVertex2f(stick_x, stick_y + 25.0);
	glEnd();
}

void Modeling_Block(const Block &block) {
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	glVertex2f(block.x, block.y);
	glVertex2f(block.x + block.width, block.y);
	glVertex2f(block.x + block.width, block.y + block.height);
	glVertex2f(block.x, block.y + block.height);
	glEnd();
}

//void Collision_Detection_Between_Balls(void) {
//	float	distance;
//
//	float	delta_x;
//	float	delta_y;
//
//	delta_x = fixed_ball.x - moving_ball.x;
//	delta_y = fixed_ball.y - moving_ball.y;
//
//	distance = sqrt(delta_x * delta_x + delta_y * delta_y);
//
//	if (distance <= radius1 + moving_ball_radius) {
//		printf("fixed 공 충돌함\n");
//		velocity.x = velocity.x * -1;
//		velocity.y = velocity.y * -1;
//		collision_count++;
//	}
//}

void Collision_Detection_to_Walls(void) {
	float ball_left = moving_ball.x - moving_ball_radius;
	float ball_right = moving_ball.x + moving_ball_radius;
	float ball_top = moving_ball.y + moving_ball_radius;
	float ball_bottom = moving_ball.y - moving_ball_radius;


	if (left >= ball_left) {
		printf("왼쪽 충돌함\n");
		ball_velocity.x = ball_velocity.x * -1;
	}
	else if (WIDTH <= ball_right) {
		printf("오른쪽 충돌함\n");
		ball_velocity.x = ball_velocity.x * -1;
	}
	else if (HEIGHT <= ball_top) {
		printf("위 충돌함\n");
		ball_velocity.y = ball_velocity.y * -1;
	}
	else if (bottom >= ball_bottom) {
		printf("아래 충돌함\n");
		ball_velocity.y = ball_velocity.y * -1;

	}
}

void Collision_Detection_With_Stick(void) {
	//if (moving_ball.y - moving_ball_radius <= stick_y + 25.0 &&
	//	moving_ball.x > stick_x && moving_ball.x < stick_x + 95.0) {
	//	printf("스틱에 충돌함\n");
	//	ball_velocity.y = ball_velocity.y * -1;
	//}

	const float min_speed = 0.1f;
	const float max_speed = 0.5f;

	float ball_left = moving_ball.x - moving_ball_radius;
	float ball_right = moving_ball.x + moving_ball_radius;
	float ball_top = moving_ball.y + moving_ball_radius;
	float ball_bottom = moving_ball.y - moving_ball_radius;

	float stick_left = stick_x;
	float stick_right = stick_x + 95;
	float stick_top = stick_y + 25;
	float stick_bottom = stick_y;

	if (ball_left > stick_right || ball_right < stick_left || ball_top < stick_bottom || ball_bottom > stick_top) {
		return;
	}

	float mid_stick = stick_left + 95 / 2;

	// 공이 스틱의 중앙에서 왼쪽에 충돌
	if (moving_ball.x < mid_stick) {
		ball_velocity.x -= fabs(moving_ball.x - mid_stick) / 1000;
	}
	// 공이 스틱의 중앙에서 오른쪽에 충돌
	else {
		ball_velocity.x += fabs(moving_ball.x - mid_stick) / 1000;
	}

	// 공이 스틱에 닿았으면 위 또는 아래쪽으로 속도를 변경 (반사)
	if (ball_bottom == stick_top || ball_top == stick_bottom) {
		ball_velocity.y = -ball_velocity.y;
	}

	float speed = sqrt(ball_velocity.x * ball_velocity.x + ball_velocity.y * ball_velocity.y);
	if (speed < min_speed) {
		float multiplier = min_speed / speed;
		ball_velocity.x *= multiplier;
		ball_velocity.y *= multiplier;
	}
	else if (speed > max_speed) {
		float multiplier = max_speed / speed;
		ball_velocity.x *= multiplier;
		ball_velocity.y *= multiplier;
	}
}

void Collision_Detection_to_Brick(Block& block) {

	float dotProduct, norm;

	// 블럭의 중심 좌표 가져옴
	brick_center.x = block.x + block.width / 2;
	brick_center.y = block.y + block.height / 2;

	// 공과 블록 중심 사이의 거리 백터 계산
	dist.x = moving_ball.x - brick_center.x;
	dist.y = moving_ball.y - brick_center.y;

	// 거리 백터의 크기 계산
	norm = sqrt(pow(dist.x, 2) + pow(dist.y, 2));

	//공과 블럭이 충돌 했는지 확인
	if (norm < (moving_ball_radius + fmin(block.width / 2, block.height / 2))) {
		// 법선 백터 계산
		unit_dist.x = dist.x / norm;
		unit_dist.y = dist.y / norm;

		// 공 속도 백터와 법선 백터의 내적 계산
		dotProduct = ball_velocity.x * unit_dist.x + ball_velocity.y * unit_dist.y;

		// 법선 백터를 사용하여 공의 속도 백터 -> 반사 백터
		ball_velocity.x -= 2.0f * dotProduct * unit_dist.x;
		ball_velocity.y -= 2.0f * dotProduct * unit_dist.y;
	}
}

void frame_reset(void) {
	glClearColor(1.0, 1.0, 0.0, 0.0); // Set display-window color to Yellow
	glClear(GL_COLOR_BUFFER_BIT);
}

void ball(void) {
	// 충돌 처리 부분
	Collision_Detection_to_Walls();			// 공과 벽의 충돌 함수 
	Collision_Detection_With_Stick();		// 공과 스틱의 충돌 함수 
	for (int i = 0; i < num_blocks; i++) {
		Collision_Detection_to_Brick(blocks[i]);
	}

	// 움직이는 공의 위치 변화 
	moving_ball.x += ball_velocity.x;
	moving_ball.y += ball_velocity.y;

	// 움직이는 공 그리기 
	glColor3f(0.0, 0.0, 1.0);
	Modeling_Circle(moving_ball_radius, moving_ball);
}


void MySpecial(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		stick_x -= stick_velocity;
		break;
	case GLUT_KEY_RIGHT:
		stick_x += stick_velocity;
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

	// 스틱 그리기
	Modeling_Stick();

	// 블럭 그리기
	for (int i = 0; i < num_blocks; i++) {
		Modeling_Block(blocks[i]);
		//Collision_Detection_to_Brick(blocks[i]);
	}

	glutSwapBuffers();
	glFlush();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Bouncing Ball & Wall");
	init();
	init_blocks();
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(MySpecial);
	glutMainLoop();
}
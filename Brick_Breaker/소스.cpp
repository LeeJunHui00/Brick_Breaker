#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

#define	WIDTH 			1200
#define	HEIGHT			800
#define	PI				3.1415
#define	POLYGON_num		50

int		left = 0;
int		bottom = 0;

float	moving_ball_radius;				// 움직이는 공의 반지름 
float	wall_radius;
float	stick_x, stick_y;				// 스틱의 가로,세로
float	stick_velocity = 20;			// 스틱 움직이는 속도

const int	num_blocks = 8;				// 블럭 개수 설정

float	block_radius = 100;
float	block_rotation = 0.0;
float	block_rotation_speed = 0.02;

float	stick_radius = 300;
float	stick_rotation = 0.0;

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

typedef struct _POSITION {
	float	left;
	float	right;
	float	top;
	float	bottom;
} POSITION;

Point	moving_ball, ball_velocity;		// 움직이는 공, 속도 관련
Point	brick_center, stick_center;
Point	sdist, unit_sdist;
Point	closet_block, closet_stick;
Point	wall;
Block	blocks[num_blocks];
POSITION pos_stick, pos_ball;


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

// 공과 원형벽의 충돌을 검사하는 함수
void Collision_Detection_to_Sphere_Walls(void);

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

// 텍스트 출력 함수
void displayText(float x, float y, float r, float g, float b, const char* str);

// 벽 그리는 함수
void Modeling_Wall(void);

// 원운동 하는 블럭 그리는 함수
void draw_blocks_with_circular_motion(void);

void ball(void);

// 원운동 하는 스틱 그리는 함수
void draw_stick_with_circular_motion(void);

void init(void) {
	// 움직이는 공의 반지름과 초기 위치, 속도 설정
	moving_ball_radius = 10.0;
	moving_ball.x = WIDTH / 2;
	moving_ball.y = HEIGHT / 2 -300;

	ball_velocity.x = 0.3;
	ball_velocity.y = 0.3;

	// 스틱 초기위치 
	//stick_x = WIDTH / 2 - 95.0 / 2;
	//stick_y = 10.0;

	//stick_x = WIDTH / 2;
	//stick_y = HEIGHT /2;


	// 벽 초기 위치
	wall_radius = 400;
	wall.x = WIDTH / 2;
	wall.y = HEIGHT / 2;
	
	// 블럭 회전
	block_rotation += block_rotation_speed;

	// 스틱 회전
	//stick_roation += 0.02;


}


void init_blocks(void) {
	float	block_width		= 60;
	float	block_height	= 30;
	
	for (int i = 0; i < num_blocks; i++) {
		//blocks[i].x = 200 + i * (block_width + 50);
		//blocks[i].y = 500;

		//blocks[i].x = 200;
		//blocks[i].y = 200;

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
	glColor3f(0.0, 0.6, 0.6);
	glBegin(GL_POLYGON);
	// 가로
	//glVertex2f(stick_x, stick_y);
	//glVertex2f(stick_x + 95.0, stick_y);
	//glVertex2f(stick_x + 95.0, stick_y + 25.0);
	//glVertex2f(stick_x, stick_y + 25.0);

	// 세로
	glVertex2f(stick_x, stick_y);
	glVertex2f(stick_x + 25.0, stick_y);
	glVertex2f(stick_x + 25.0, stick_y + 95.0);
	glVertex2f(stick_x, stick_y + 95.0);

	
	glEnd();
}

void Modeling_Block(const Block &block) {
	if (block.visible) {
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_POLYGON);
		glVertex2f(block.x, block.y);
		glVertex2f(block.x + block.width, block.y);
		glVertex2f(block.x + block.width, block.y + block.height);
		glVertex2f(block.x, block.y + block.height);
		glEnd();
	}
}

void Modeling_Wall(void) {

	float radius = 400;
	float x = WIDTH / 2;
	float y = HEIGHT / 2;

	glColor3f(1, 1, 1);
	//glBegin(GL_LINE_LOOP);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++) {
		float theta = i * (PI / 180);
		float cx = x + radius * cos(theta);
		float cy = y + radius * sin(theta);
		glVertex2f(cx, cy);
	}
	glEnd();
}

void Collision_Detection_to_Sphere_Walls(void) {
	float distance;
	float dotProduct;
	float length;

	Point norm;	// 충돌 지점과 벽의 중심을 연결한 벡터

	// 공과 벽과의 거리 계산
	distance = sqrt(pow(moving_ball.x - wall.x, 2) + sqrt(pow(moving_ball.y - wall.y,2)));

	if (distance <= (wall_radius + moving_ball_radius)) {
		// 충돌 지점과 벽 중심 사이의 법선 백터 구하기
		norm.x = moving_ball.x - wall.x;
		norm.y = moving_ball.y - wall.y;		

		length = sqrt(pow(norm.x, 2) + pow(norm.y,2));

		norm.x /= length;
		norm.y /= length;

		if (length >= wall_radius) {
			// 충돌 지점에서의 공의 속도와 법선 벡터의 내적 계산
			dotProduct = ball_velocity.x * norm.x + ball_velocity.y * norm.y;

			// 법선 방향으로 반사된 속도 계산
			ball_velocity.x -= 2 * dotProduct * norm.x;
			ball_velocity.y -= 2 * dotProduct * norm.y;
		}
	}
}

void Collision_Detection_to_Walls(void) {
	float ball_left = moving_ball.x - moving_ball_radius;
	float ball_right = moving_ball.x + moving_ball_radius;
	float ball_top = moving_ball.y + moving_ball_radius;
	float ball_bottom = moving_ball.y - moving_ball_radius;

	if (left >= ball_left) {
		//printf("왼쪽 충돌함\n");
		ball_velocity.x = ball_velocity.x * -1;
	}
	else if (WIDTH <= ball_right) {
		//printf("오른쪽 충돌함\n");
		ball_velocity.x = ball_velocity.x * -1;
	}
	else if (HEIGHT <= ball_top) {
		//printf("위 충돌함\n");
		ball_velocity.y = ball_velocity.y * -1;
	}
	else if (bottom >= ball_bottom) {
		//printf("아래 충돌함\n");
		ball_velocity.y = ball_velocity.y * -1;
	}
}

void Collision_Detection_With_Stick(void) {
	//if (moving_ball.y - moving_ball_radius <= stick_y + 25.0 &&
	//	moving_ball.x > stick_x && moving_ball.x < stick_x + 95.0) {
	//	printf("스틱에 충돌함\n");
	//	ball_velocity.y = ball_velocity.y * -1;
	//}

	// -------- 안 돌아가는 스틱 -----
	//float dotProduct, norm;

	//stick_center.x = stick_x + 95 / 2;
	//stick_center.y = stick_y + 25 / 2;

	//closet_stick.x = moving_ball.x;
	//closet_stick.y = moving_ball.y;

	//// 가장 가까운 위치
	//if (moving_ball.x < stick_x) {
	//	closet_stick.x = stick_x;
	//}
	//else if (moving_ball.x > stick_x + 95) {
	//	closet_stick.x = stick_x + 95;
	//}
	//if (moving_ball.y < stick_y) {
	//	closet_stick.y = stick_y;
	//}
	//else if (moving_ball.y > stick_y + 25) {
	//	closet_stick.y = stick_y + 25;
	//}

	//sdist.x = moving_ball.x - closet_stick.x;
	//sdist.y = moving_ball.y - closet_stick.y;

	//// 거리 백터의 크기 계산
	//norm = sqrt(pow(sdist.x, 2) + pow(sdist.y, 2));

	////공과 블럭이 충돌 했는지 확인
	//if (norm < (moving_ball_radius + fmin(95 / 2, 25 / 2))) {
	//	// 법선 백터 계산
	//	unit_sdist.x = sdist.x / norm;
	//	unit_sdist.y = sdist.y / norm;

	//	// 공 속도 백터와 법선 백터의 내적 계산
	//	dotProduct = ball_velocity.x * unit_sdist.x + ball_velocity.y * unit_sdist.y;

	//	// 법선 백터를 사용하여 공의 속도 백터 -> 반사 백터
	//	ball_velocity.x -= 2 * dotProduct * unit_sdist.x;
	//	ball_velocity.y -= 2 * dotProduct * unit_sdist.y;
	//}

	// -------- 안 돌아가는 스틱 -----

	Point rotated_stick;
	Point dist, unit_dist;

	float dotProduct, norm;
	float distance_from_center = 350;

	rotated_stick.x = wall.x + distance_from_center * cos(stick_rotation * PI / 180);
	rotated_stick.y = wall.y + distance_from_center * sin(stick_rotation * PI / 180);
	
	printf("%f\n", rotated_stick.x);
}

void Collision_Detection_to_Brick(Block& block) {
	if (block.visible) {
		Point rotated_block;
		Point dist, unit_dist;

		float dotProduct, norm;
		float distance_from_center = 100;

		int block_num = &block - blocks;  // 현재 블록 번호 계산
		float block_angle = block_rotation + (block_num * (360.0f / num_blocks));  // 블록 회전 각도 계산

		// 회전한 블록의 좌표를 계산한다. 중심으로부터 일정한 거리에 블록이 위치한다.
		rotated_block.x = wall.x + distance_from_center * cos(block_angle * PI / 180);
		rotated_block.y = wall.y + distance_from_center * sin(block_angle * PI / 180);

		// 블럭의 중심 좌표 가져옴
		brick_center.x = rotated_block.x + block.width / 2;
		brick_center.y = rotated_block.y + block.height / 2;
		//printf("%d 의 x 위치 : %f, y 위치 : %f\n", block_num, rotated_block.x, rotated_block.y);

		closet_block.x = moving_ball.x;
		closet_block.y = moving_ball.y;

		// 공의 x 좌표가 블록의 왼쪽 경계보다 작은 경우 가장 가까운 위치를 왼쪽 경계로 설정한다.
		if (moving_ball.x < rotated_block.x) {
			closet_block.x = rotated_block.x;
		}
		// 공의 x 좌표가 블록의 오른쪽 경계보다 큰 경우 가장 가까운 위치를 오른쪽 경계로 설정한다.
		else if (moving_ball.x > rotated_block.x + block.width) {
			closet_block.x = rotated_block.x + block.width;
		}
		// 공의 y 좌표가 블록의 위쪽 경계보다 작은 경우 가장 가까운 위치를 위쪽 경계로 설정한다.
		if (moving_ball.y < rotated_block.y) {
			closet_block.y = rotated_block.y;
		}
		// 공의 y 좌표가 블록의 아래쪽 경계보다 큰 경우 가장 가까운 위치를 아래쪽 경계로 설정한다.
		else if (moving_ball.y > rotated_block.y + block.height) {
			closet_block.y = rotated_block.y + block.height;
		}

		// 공과 블록 중심 사이의 거리 백터 계산
		dist.x = moving_ball.x - closet_block.x;
		dist.y = moving_ball.y - closet_block.y;

		//printf("%f\n", rotated_block.x);

		// 거리 백터의 크기 계산
		norm = sqrt(pow(dist.x, 2) + pow(dist.y, 2));

		//공과 블럭이 충돌 했는지 확인
		if (norm < (moving_ball_radius)) {
			// 법선 백터 계산
			unit_dist.x = dist.x / norm;
			unit_dist.y = dist.y / norm;

			// 공 속도 백터와 법선 백터의 내적 계산
			dotProduct = ball_velocity.x * unit_dist.x + ball_velocity.y * unit_dist.y;

			// 법선 백터를 사용하여 공의 속도 백터 -> 반사 백터
			ball_velocity.x -= 2 * dotProduct * unit_dist.x;
			ball_velocity.y -= 2 * dotProduct * unit_dist.y;
			printf("블록과 충돌함\n");
			block.visible = false;
		}
	}
}

void frame_reset(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0); // 검은색 배경
	glClear(GL_COLOR_BUFFER_BIT);
}

void ball(void) {
	// 충돌 처리 부분
	Collision_Detection_to_Walls();			// 공과 벽의 충돌 함수 
	Collision_Detection_With_Stick();		// 공과 스틱의 충돌 함수 
	for (int i = 0; i < num_blocks; i++) {
		Collision_Detection_to_Brick(blocks[i]);
	}
	Collision_Detection_to_Sphere_Walls();
	// 움직이는 공의 위치 변화 
	moving_ball.x += ball_velocity.x;
	moving_ball.y += ball_velocity.y;

	// 움직이는 공 그리기 
	glColor3f(1.0, 0.0, 0.0);
	Modeling_Circle(moving_ball_radius, moving_ball);
}

void draw_blocks_with_circular_motion(void) {

	block_rotation += block_rotation_speed;

	for (int i = 0; i < num_blocks; i++) {

		glPushMatrix();

		// 원의 중심으로 이동
		glTranslatef(WIDTH/2, HEIGHT/2, 0.0f);

		// 각도에 따라 회전
		glRotatef(block_rotation + (i * 360.0f / num_blocks), 0.0f, 0.0f, 1.0f);

		// 원의 경계선까지 이동
		glTranslatef(100, 0.0f, 0.0f);

		// 블록 그리기
		Modeling_Block(blocks[i]);
		glPopMatrix();

	}
}

void draw_stick_with_circular_motion(void) {

	//stick_roation -= 0.02;

	glPushMatrix();

	glTranslatef(WIDTH / 2, HEIGHT / 2, 0.0f);
	glRotatef(stick_rotation + 360.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(350, 0.0f, 0.0f);
	Modeling_Stick();

	glPopMatrix();

}


void displayText(float x, float y, float r, float g, float b, const char* str) {
	glColor3f(r, g, b);
	glRasterPos2f(x, y);

	for (const char* c = str; *c != '\0'; ++c) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
	}
}


void MySpecial(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		//stick_x -= stick_velocity;
		stick_rotation -= 2;
		break;
	case GLUT_KEY_RIGHT:
		//stick_x += stick_velocity;
		stick_rotation += 2;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void RenderScene(void) {
	// 화면 배경색 설정
	frame_reset();

	// 벽 그리기
	Modeling_Wall();

	// 공 그리기
	ball();

	// 스틱 그리기
	//Modeling_Stick();

	// 블럭 그리기
	//for (int i = 0; i < num_blocks; i++) {
	//	Modeling_Block(blocks[i]);
	//}

	draw_blocks_with_circular_motion();

	draw_stick_with_circular_motion();

	// 글씨 출력해보기
	char formattedText_x[255];
	char formattedText_y[255];

	sprintf(formattedText_x, "x speed : %f", ball_velocity.x);
	sprintf(formattedText_y, "y speed : %f", ball_velocity.y);

	displayText(0.0f, 13.0f, 1.0f, 1.0f, 1.0f, formattedText_x);
	displayText(0.0f, 3.0f, 1.0f, 1.0f, 1.0f, formattedText_y);
	//

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
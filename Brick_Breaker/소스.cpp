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
//float	stick_x, stick_y;				// 스틱의 가로,세로
float	stick_velocity = 30;			// 스틱 움직이는 속도

const int	num_blocks = 8;				// 움직이는 블럭 개수 설정
const int	num_fixed_blocks = 24;		// 고정된 블럭 개수 설정

float	block_radius = 100;
float	block_rotation = 0.0;
float	block_rotation_speed = 0.02;

float	stick_radius = 300;
float	stick_rotation = -110.0;

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
Block	blocks[num_blocks], fixed_blocks[num_fixed_blocks];
POSITION pos_stick, pos_ball;

Block	stick;


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

// 공 그리기
void ball(void);

// 원운동 하는 스틱 그리는 함수
void draw_stick_with_circular_motion(void);

// 고정된 블럭 생성 및 초기화 함수
void init_Fixed_blocks(void);

// 고정된 블럭 그리는 함수
void Modeling_Fixed_blocks(const Block& fixed_block);

// 고정된 블럭 반복 함수
void draw_blocks_with_fixed(void);

// 고정된 블럭 충돌 체크 함수
void Collision_Detection_to_Fixed_Brick(const Block& fixed_block);

void init(void) {
	// 움직이는 공의 반지름과 초기 위치, 속도 설정
	moving_ball_radius = 10.0;
	moving_ball.x = WIDTH / 2;
	moving_ball.y = HEIGHT / 2 -300;

	ball_velocity.x = 0.3;
	ball_velocity.y = 0.5;

	// 벽 초기 위치
	wall_radius = 400;
	wall.x = WIDTH / 2;
	wall.y = HEIGHT / 2;
	
	// 블럭 회전
	block_rotation += block_rotation_speed;

	stick.width = 25.0;
	stick.height = 95.0;

}


void init_blocks(void) {
	float	block_width		= 60;
	float	block_height	= 30;
	float	distance_from_center = 100;
	float	block_angle;

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
	glVertex2f(stick.x, stick.y);
	glVertex2f(stick.x + stick.width, stick.y);
	glVertex2f(stick.x + stick.width, stick.y + stick.height);
	glVertex2f(stick.x, stick.y + stick.height);

	glEnd();
}

void init_Fixed_blocks(void) {
	float fixed_block_width = 60;
	float fixed_block_height = 30;
	
	//for (int i = 0; i < num_fixed_blocks; i++) {
	//	
	//	fixed_blocks[i].x = 400 + i * (fixed_block_width + 50);
	//	fixed_blocks[i].y = 700;
	//	fixed_blocks[i].width = fixed_block_width;
	//	fixed_blocks[i].height = fixed_block_height;
	//	fixed_blocks[i].visible = true;
	//}
	int rows = 5;
	int index = 0;

	for (int row = 0; row < rows; row++) {
		int numBlocksInRow = row + 1;
		float y = 750 - row * (fixed_block_height + 15);
		float margin_x = (WIDTH - numBlocksInRow * (fixed_block_width + 70)) / 2;
		for (int j = 0; j < numBlocksInRow; j++) {
			fixed_blocks[index].x = 35 + margin_x + j * (fixed_block_width + 70);
			fixed_blocks[index].y = y;

			fixed_blocks[index].width = fixed_block_width;
			fixed_blocks[index].height = fixed_block_height;
			fixed_blocks[index].visible = true;

			index++;
		}
	}

}

void draw_blocks_with_fixed(void) {

	for (int i = 0; i < num_fixed_blocks; i++) {
		//glPushMatrix();
		Modeling_Fixed_blocks(fixed_blocks[i]);
		//glPopMatrix();
	}
}

void Modeling_Fixed_blocks(const Block &fixed_block) {
	if (fixed_block.visible) {
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_POLYGON);
		glVertex2f(fixed_block.x , fixed_block.y);
		glVertex2f(fixed_block.x + fixed_block.width, fixed_block.y);
		glVertex2f(fixed_block.x + fixed_block.width, fixed_block.y + fixed_block.height);
		glVertex2f(fixed_block.x, fixed_block.y + fixed_block.height);
		glEnd();
	}
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

	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	for (int i = 0; i < 360; i++) {
		float theta = i * (PI / 180);
		float cx = x + radius * cos(theta);
		float cy = y + radius * sin(theta);
		glVertex2f(cx, cy);
	}
	glEnd();

	glLineWidth(10.0f);
	glBegin(GL_LINE_LOOP);

	// 데드라인 설정
	for (int i = 0; i < 360; i++) {
		if (i > 240 && i <300) {
			glColor3f(1.0, 0.0, 0.0);
		}
		else {
			glColor3f(1.0, 1.0, 1.0);
		}
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
	float length, theta;

	Point norm;	// 충돌 지점과 벽의 중심을 연결한 벡터

	// 공과 벽과의 거리 계산
	//distance = sqrt(pow(moving_ball.x - wall.x, 2) + sqrt(pow(moving_ball.y - wall.y,2)));
	distance = sqrt(pow(moving_ball.x - wall.x, 2) + pow(moving_ball.y - wall.y, 2));

	if (distance <= (wall_radius + moving_ball_radius)) {
		// 충돌 지점과 벽 중심 사이의 법선 백터 구하기
		norm.x = moving_ball.x - wall.x;
		norm.y = moving_ball.y - wall.y;		

		length = sqrt(pow(norm.x, 2) + pow(norm.y,2));

		norm.x /= length;
		norm.y /= length;

		if (length >= wall_radius) {
			theta = atan2(moving_ball.y - wall.y, moving_ball.x - wall.x) * (180 / PI);
			printf("%f\n", theta);

			if (theta >= -130 && theta < -50) {
				printf("데드라인에 충돌되었습니다\n");
				ball_velocity.x = 0;
				ball_velocity.y = 0;
			}

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

//* 안돌아가는 스틱*/
//void Collision_Detection_With_Stick(void) {
//	//if (moving_ball.y - moving_ball_radius <= stick_y + 25.0 &&
//	//	moving_ball.x > stick_x && moving_ball.x < stick_x + 95.0) {
//	//	printf("스틱에 충돌함\n");
//	//	ball_velocity.y = ball_velocity.y * -1;
//	//}
//
//	// -------- 안 돌아가는 스틱 -----
//	//float dotProduct, norm;
//
//	//stick_center.x = stick_x + 95 / 2;
//	//stick_center.y = stick_y + 25 / 2;
//
//	//closet_stick.x = moving_ball.x;
//	//closet_stick.y = moving_ball.y;
//
//	//// 가장 가까운 위치
//	//if (moving_ball.x < stick_x) {
//	//	closet_stick.x = stick_x;
//	//}
//	//else if (moving_ball.x > stick_x + 95) {
//	//	closet_stick.x = stick_x + 95;
//	//}
//	//if (moving_ball.y < stick_y) {
//	//	closet_stick.y = stick_y;
//	//}
//	//else if (moving_ball.y > stick_y + 25) {
//	//	closet_stick.y = stick_y + 25;
//	//}
//
//	//sdist.x = moving_ball.x - closet_stick.x;
//	//sdist.y = moving_ball.y - closet_stick.y;
//
//	//// 거리 백터의 크기 계산
//	//norm = sqrt(pow(sdist.x, 2) + pow(sdist.y, 2));
//
//	////공과 블럭이 충돌 했는지 확인
//	//if (norm < (moving_ball_radius + fmin(95 / 2, 25 / 2))) {
//	//	// 법선 백터 계산
//	//	unit_sdist.x = sdist.x / norm;
//	//	unit_sdist.y = sdist.y / norm;
//
//	//	// 공 속도 백터와 법선 백터의 내적 계산
//	//	dotProduct = ball_velocity.x * unit_sdist.x + ball_velocity.y * unit_sdist.y;
//
//	//	// 법선 백터를 사용하여 공의 속도 백터 -> 반사 백터
//	//	ball_velocity.x -= 2 * dotProduct * unit_sdist.x;
//	//	ball_velocity.y -= 2 * dotProduct * unit_sdist.y;
//	//}
//
//	// -------- 안 돌아가는 스틱 -----
//}

//void Collision_Detection_With_Stick(void) {
//	Point rotated_stick;
//	Point local_ball, rotated_ball;
//	Point dist, unit_dist;
//
//	float dotProduct, norm;
//	float distance_from_center = 350;
//
//	rotated_stick.x = wall.x + distance_from_center * cos(stick_rotation * PI / 180);
//	rotated_stick.y = wall.y + distance_from_center * sin(stick_rotation * PI / 180);
//	
//	// 공의 로컬 좌표 계산
//	stick_center.x = rotated_stick.x + stick.width / 2;
//	stick_center.y = rotated_stick.y + stick.height / 2;
//
//	closet_stick.x = moving_ball.x;
//	closet_stick.y = moving_ball.y;
//
//	if (moving_ball.x < rotated_stick.x) {
//		closet_stick.x = rotated_stick.x;
//	}
//	else if (moving_ball.x > rotated_stick.x + stick.width) {
//		closet_stick.x = rotated_stick.x + stick.width;
//	}
//	if (moving_ball.y < rotated_stick.y) {
//		closet_stick.y = rotated_stick.y;
//	}
//	else if (moving_ball.y > rotated_stick.y + stick.height) {
//		closet_stick.y = rotated_stick.y + stick.height;
//	}
//
//	dist.x = moving_ball.x - closet_stick.x;
//	dist.y = moving_ball.y - closet_stick.y;
//
//	// 거리 벡터의 크기(길이)를 계산한다. 거리 계산을 위해 제곱근 함수를 사용한다.
//	norm = sqrt(pow(dist.x, 2) + pow(dist.y, 2));
//
//	if (norm < (moving_ball_radius)) {
//		// 충돌이 발생한 경우, 법선 벡터(단위 거리 벡터)를 계산한다.
//		unit_dist.x = dist.x / norm;
//		unit_dist.y = dist.y / norm;
//
//		// 공 속도 벡터와 법선 벡터의 내적을 계산한다. 
//		dotProduct = ball_velocity.x * unit_dist.x + ball_velocity.y * unit_dist.y;
//
//		// 법선 벡터와 관련된 내적 값을 이용하여 공의 속도 벡터를 업데이트한다.   
//		ball_velocity.x -= 2 * dotProduct * unit_dist.x;
//		ball_velocity.y -= 2 * dotProduct * unit_dist.y;
//	}
//}


void Collision_Detection_With_Stick(void) {
	Point rotated_stick;
	Point local_ball, rotated_ball;
	Point dist, unit_dist, overlap, rotated_velocity;

	float dotProduct, norm;
	float distance_from_center = 350;

	rotated_stick.x = wall.x + distance_from_center * cos(stick_rotation * PI / 180);
	rotated_stick.y = wall.y + distance_from_center * sin(stick_rotation * PI / 180);

	// 공의 로컬 좌표 계산
	local_ball.x = moving_ball.x - rotated_stick.x;
	local_ball.y = moving_ball.y - rotated_stick.y;

	float radian_angle = -(stick_rotation * PI / 180);

	rotated_ball.x = local_ball.x * cos(radian_angle) - local_ball.y * sin(radian_angle);
	rotated_ball.y = local_ball.x * sin(radian_angle) + local_ball.y * cos(radian_angle);

	if (rotated_ball.x >= 0 && rotated_ball.x <= stick.width && rotated_ball.y >= 0 && rotated_ball.y <= stick.height) {
		overlap.x = (stick.width / 2 + moving_ball_radius) - fabs(rotated_ball.x - stick.width / 2);
		overlap.y = (stick.height / 2 + moving_ball_radius) - fabs(rotated_ball.y - stick.height / 2);

		rotated_velocity.x = ball_velocity.x * cos(radian_angle) - ball_velocity.y * sin(radian_angle);
		rotated_velocity.y = ball_velocity.x * sin(radian_angle) + ball_velocity.y * cos(radian_angle);

		// 충돌이 일어난 축에 따라 속도를 음수로 바꾸어 공을 반사시키도록 합니다
		//if (overlap.x >= overlap.y) {
		//	rotated_velocity.y = -rotated_velocity.y; // Y 축 속도 변경
		//}
		//else {
		//	rotated_velocity.x = -rotated_velocity.x; // X 축 속도 변경
		//}
		if (overlap.x > 0 && overlap.y > 0) {
			if (overlap.x >= overlap.y) {
				rotated_velocity.y = -rotated_velocity.y; // Y 축 속도 변경
			}
			else {
				rotated_velocity.x = -rotated_velocity.x; // X 축 속도 변경
			}
		}
		else if (overlap.x > 0 && overlap.y <= 0) {
			rotated_velocity.x = -rotated_velocity.x; // X 축 속도 변경
		}
		else if (overlap.y > 0 && overlap.x <= 0) {
			rotated_velocity.y = -rotated_velocity.y; // Y 축 속도 변경
		}

		// 공의 속도 회전 원래 축으로 되돌리기
		ball_velocity.x = rotated_velocity.x * cos(-radian_angle) - rotated_velocity.y * sin(-radian_angle);
		ball_velocity.y = rotated_velocity.x * sin(-radian_angle) + rotated_velocity.y * cos(-radian_angle);

		printf("스틱과 충돌함\n");
	}
}

// 성공
void Collision_Detection_to_Brick(Block& block) {
	if (block.visible) {
		Point rotated_block;
		Point local_ball, rotated_ball;
		Point dist, unit_dist;

		float dotProduct, norm;
		float distance_from_center = 100;

		int block_num = &block - blocks;
		float block_angle = block_rotation + (block_num * (360.0f / num_blocks));

		// 회전된 블록 좌표 계산
		rotated_block.x = wall.x + distance_from_center * cos(block_angle * PI / 180);
		rotated_block.y = wall.y + distance_from_center * sin(block_angle * PI / 180);

		// 공의 로컬 좌표 계산
		local_ball.x = moving_ball.x - rotated_block.x;
		local_ball.y = moving_ball.y - rotated_block.y;

		// 회전 각도 계산
		float radian_angle = -(block_angle * PI / 180);

		// 공을 회전시키고 변환된 좌표 구하기
		rotated_ball.x = local_ball.x * cos(radian_angle) - local_ball.y * sin(radian_angle);
		rotated_ball.y = local_ball.x * sin(radian_angle) + local_ball.y * cos(radian_angle);

		// 충돌이 블록 내에서 발생하는지 확인 및 처리
		if (rotated_ball.x >= 0 && rotated_ball.x <= block.width && rotated_ball.y >= 0 && rotated_ball.y <= block.height) {
			Point overlap;
			overlap.x = (block.width / 2 + moving_ball_radius) - fabs(rotated_ball.x - block.width / 2);
			overlap.y = (block.height / 2 + moving_ball_radius) - fabs(rotated_ball.y - block.height / 2);

			// 로컬 벡터를 회전시키기 전에 업데이트
			Point rotated_velocity;
			rotated_velocity.x = ball_velocity.x * cos(radian_angle) - ball_velocity.y * sin(radian_angle);
			rotated_velocity.y = ball_velocity.x * sin(radian_angle) + ball_velocity.y * cos(radian_angle);

			// 충돌이 일어난 축에 따라 속도를 음수로 바꾸어 공을 반사시키도록 합니다
			//if (overlap.x >= overlap.y) {
			//	rotated_velocity.y = -rotated_velocity.y; // Y 축 속도 변경
			//}
			//else {
			//	rotated_velocity.x = -rotated_velocity.x; // X 축 속도 변경
			//}
			if (overlap.x > 0 && overlap.y > 0) {
				if (overlap.x >= overlap.y) {
					rotated_velocity.y = -rotated_velocity.y; // Y 축 속도 변경
				}
				else {
					rotated_velocity.x = -rotated_velocity.x; // X 축 속도 변경
				}
			}
			else if (overlap.x > 0 && overlap.y <= 0) {
				rotated_velocity.x = -rotated_velocity.x; // X 축 속도 변경
			}
			else if (overlap.y > 0 && overlap.x <= 0) {
				rotated_velocity.y = -rotated_velocity.y; // Y 축 속도 변경
			}

			// 공의 속도 회전 원래 축으로 되돌리기
			ball_velocity.x = rotated_velocity.x * cos(-radian_angle) - rotated_velocity.y * sin(-radian_angle);
			ball_velocity.y = rotated_velocity.x * sin(-radian_angle) + rotated_velocity.y * cos(-radian_angle);

			printf("블록과 충돌함\n");
			block.visible = false;
		}
	}
}

void Collision_Detection_to_Fixed_Brick(Block& fixed_block) {

	if (fixed_block.visible) {
		//float left_bound = moving_ball.x - moving_ball_radius;
		//float right_bound = moving_ball.x + moving_ball_radius;
		//float top_bound = moving_ball.y - moving_ball_radius;
		//float bottom_bound = moving_ball.y + moving_ball_radius;

		//float block_left = fixed_block.x;
		//float block_right = fixed_block.x + fixed_block.width;
		//float block_top = fixed_block.y;
		//float block_bottom = fixed_block.y + fixed_block.height;

		//if (left_bound <= block_right && right_bound >= block_left && top_bound <= block_bottom && bottom_bound >= block_top) {
		//	float dx = moving_ball.x - (fixed_block.x + fixed_block.width / 2);
		//	float dy = moving_ball.y - (fixed_block.y + fixed_block.height / 2);

		//	if (fabs(dx) > fabs(dy)) {
		//		ball_velocity.x = -ball_velocity.x;
		//	}
		//	else {
		//		ball_velocity.y = -ball_velocity.y;
		//	}

		//	// 대각선 충돌 처리
		//	if (fabs(dx) < fixed_block.width / 2 && fabs(dy) < fixed_block.height / 2) {
		//		ball_velocity.x = -ball_velocity.x;
		//		ball_velocity.y = -ball_velocity.y;
		//	}

		//	printf("고정된 블럭과 충돌\n");
		//	fixed_block.visible = false;

		//}
		float dotProduct, norm;

		closet_stick.x = moving_ball.x;
		closet_stick.y = moving_ball.y;

		// 가장 가까운 위치
		if (moving_ball.x < fixed_block.x) {
			closet_stick.x = fixed_block.x;
		}
		else if (moving_ball.x > fixed_block.x + fixed_block.width) {
			closet_stick.x = fixed_block.x + fixed_block.width;
		}
		if (moving_ball.y < fixed_block.y) {
			closet_stick.y = fixed_block.y;
		}
		else if (moving_ball.y > fixed_block.y + fixed_block.height) {
			closet_stick.y = fixed_block.y + fixed_block.height;
		}

		sdist.x = moving_ball.x - closet_stick.x;
		sdist.y = moving_ball.y - closet_stick.y;

		// 거리 백터의 크기 계산
		norm = sqrt(pow(sdist.x, 2) + pow(sdist.y, 2));

		//공과 블럭이 충돌 했는지 확인
		if (norm < (moving_ball_radius + fmin(95 / 2, 25 / 2))) {
			// 법선 백터 계산
			unit_sdist.x = sdist.x / norm;
			unit_sdist.y = sdist.y / norm;

			// 공 속도 백터와 법선 백터의 내적 계산
			dotProduct = ball_velocity.x * unit_sdist.x + ball_velocity.y * unit_sdist.y;

			// 법선 백터를 사용하여 공의 속도 백터 -> 반사 백터
			ball_velocity.x -= 2 * dotProduct * unit_sdist.x;
			ball_velocity.y -= 2 * dotProduct * unit_sdist.y;

			fixed_block.visible = false;
		}
	}
}

void frame_reset(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0); // 검은색 배경
	glClear(GL_COLOR_BUFFER_BIT);
}

void ball(void) {
	// 충돌 처리 부분
	//Collision_Detection_to_Walls();			// 공과 벽의 충돌 함수 
	Collision_Detection_With_Stick();		// 공과 스틱의 충돌 함수 
	for (int i = 0; i < num_blocks; i++) {
		Collision_Detection_to_Brick(blocks[i]);
	}
	for (int i = 0; i < num_fixed_blocks; i++) {
		Collision_Detection_to_Fixed_Brick(fixed_blocks[i]);
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

	glPushMatrix();

	glTranslatef(WIDTH / 2, HEIGHT / 2, 0.0f);
	glRotatef(stick_rotation + 360.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(350, 0.0f, 0.0f);
	Modeling_Stick();
	if (stick_rotation <= -180.0) {
		stick_rotation = -180.0;
	}
	else if (stick_rotation > -10.0) {
		stick_rotation = -10.0;
	}

	// printf("%f\n", stick_rotation);
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
		stick_rotation -= 2;
		break;
	case GLUT_KEY_RIGHT:
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

	draw_blocks_with_circular_motion();

	draw_stick_with_circular_motion();

	draw_blocks_with_fixed();

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
	init_Fixed_blocks();
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(MySpecial);
	glutMainLoop();
}
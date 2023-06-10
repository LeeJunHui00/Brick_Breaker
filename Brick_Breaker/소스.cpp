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
bool	start_check;

float	moving_ball_radius;				// 움직이는 공의 반지름 
float	wall_radius;

const int	num_blocks = 18;				// 움직이는 블럭 개수 설정
const int	num_fixed_blocks = 24;		// 고정된 블럭 개수 설정

float	block_radius = 100;
float	block_rotation = 0.0;
float	block_rotation_speed;

float	stick_radius = 300;
float	stick_velocity = 30;			// 스틱 움직이는 속도
float	stick_rotation;

float	game_page = 0;
int		score;

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
void displayText(float x, float y, float r, float g, float b, const char* str, int num);

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

// 키보드 입력 함수
void Mykey(unsigned char key, int x, int y);

// 다음 충돌체크 두번하기
bool Next_Ball_Collision_Detection_With_Stick(void);

// 메인 화면
void MainPage(void);

// 사망 창
void DiePage(void);

// 클리어 창
void ClearPage(void);

void info(void);

void init(void) {
	// 움직이는 공의 반지름과 초기 위치, 속도 설정
	moving_ball_radius = 10.0;
	moving_ball.x = WIDTH / 2;
	moving_ball.y = HEIGHT / 2 -350;

	ball_velocity.x = 0.0;
	ball_velocity.y = 0.0;

	// 벽 초기 위치
	wall_radius = 400;
	wall.x = WIDTH / 2;
	wall.y = HEIGHT / 2;
	
	// 블럭 회전
	block_rotation += block_rotation_speed;

	stick.width = 25.0;
	stick.height = 100.0;
	stick_rotation = -90.0;

	score = 0;

	start_check = FALSE;
}


void init_blocks(void) {
	float	block_width		= 60;
	float	block_height	= 30;
	float	distance_from_center = 100;
	float	block_angle;

	block_rotation_speed = 0.02;
	for (int i = 0; i < num_blocks; i++) {
		blocks[i].width = block_width;
		blocks[i].height = block_height;
		blocks[i].visible = true;
	}
}


void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
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
	glShadeModel(GL_SMOOTH);//그라데이션으로 다각형을 채움
	glBegin(GL_POLYGON);

	// 세로
	glColor3f(0.161, 0.196, 0.235);
	glVertex2f(stick.x, stick.y);

	glColor3f(0.282, 0.333, 0.388);
	glVertex2f(stick.x + stick.width, stick.y);
	
	glColor3f(0.282, 0.333, 0.388);
	glVertex2f(stick.x + stick.width, stick.y + stick.height);

	glColor3f(0.161, 0.196, 0.235);
	glVertex2f(stick.x, stick.y + stick.height);

	glEnd();
}

void init_Fixed_blocks(void) {
	float fixed_block_width = 60;
	float fixed_block_height = 30;
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
		Modeling_Fixed_blocks(fixed_blocks[i]);
	}
}

void Modeling_Fixed_blocks(const Block &fixed_block) {
	if (fixed_block.visible) {
		glShadeModel(GL_SMOOTH);	//그라데이션으로 다각형을 채움
		glBegin(GL_POLYGON);
		glColor3f(0.961 , 0.969, 0.980);
		glVertex2f(fixed_block.x, fixed_block.y);

		glColor3f(0.765, 0.812, 0.886);
		glVertex2f(fixed_block.x + fixed_block.width, fixed_block.y);

		glColor3f(0.765, 0.812, 0.886);
		glVertex2f(fixed_block.x + fixed_block.width, fixed_block.y + fixed_block.height);

		glColor3f(0.961, 0.969, 0.980);
		glVertex2f(fixed_block.x, fixed_block.y + fixed_block.height);
		glEnd();
	}
}

void Modeling_Block(const Block &block) {
	if (block.visible) {
		glShadeModel(GL_SMOOTH);//그라데이션으로 다각형을 채움
		glBegin(GL_POLYGON);
		glColor3f(0.949, .949, 0.949);
		glVertex2f(block.x, block.y);

		glColor3f(0.839, 0.886, 0.961);
		glVertex2f(block.x + block.width, block.y);

		glColor3f(0.839, 0.886, 0.961);
		glVertex2f(block.x + block.width, block.y + block.height);

		glColor3f(0.949, 0.949, 0.949);
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
		// safe 라인
		else if (i > 180) {
			glColor3f(0.0, 1.0, 0.0);
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
	float distance;				// 공과 원형 벽 사이의 거리
	float dotProduct;			// 공의 속도 벡터와 법선 벡터의 내적
	float length, theta;		// 법선 벡터의 길이, 공의 극좌표계 각도

	Point norm; // 충돌 지점과 벽의 중심을 연결한 벡터(법선 벡터)

	// 공과 벽과의 거리 계산
	distance = sqrt(pow(moving_ball.x - wall.x, 2) + pow(moving_ball.y - wall.y, 2));

	// 공의 반지름과 벽의 반지름의 합보다 거리가 작거나 같으면 충돌이 발생
	if (distance <= (wall_radius + moving_ball_radius)) {
		// 충돌 지점과 벽 중심 사이의 법선 백터 구하기
		norm.x = moving_ball.x - wall.x;
		norm.y = moving_ball.y - wall.y;		

		// 법선 벡터의 길이(크기) 계산
		length = sqrt(pow(norm.x, 2) + pow(norm.y,2));

		// 법선 벡터를 정규화(단위 벡터로 만들기)
		norm.x /= length;
		norm.y /= length;

		// 벽에 설정된 데드라인 각도 범위에 따라 충돌 여부 판단
		if (length >= wall_radius) {

			theta = atan2(moving_ball.y - wall.y, moving_ball.x - wall.x) * (180 / PI);

			// 데드라인에 충돌한 경우 공의 속도를 0으로 설정함, DiePage() 실행
			if (theta >= -120 && theta < -60) {
				//printf("데드라인에 충돌되었습니다\n");
				ball_velocity.x = 0;
				ball_velocity.y = 0;
				DiePage();
			}

			// 충돌 지점에서의 공의 속도와 법선 벡터의 내적 계산
			dotProduct = ball_velocity.x * norm.x + ball_velocity.y * norm.y;

			// 법선 방향으로 반사된 속도 계산
			ball_velocity.x -= 2 * dotProduct * norm.x;
			ball_velocity.y -= 2 * dotProduct * norm.y;
		}
	}
}


bool Next_Ball_Collision_Detection_With_Stick(void) {
	Point rotated_stick;
	Point local_ball, rotated_ball;
	Point dist, unit_dist, overlap, rotated_velocity;

	Point next_ball_position;

	float dotProduct, norm;
	float distance_from_center = 350;

	next_ball_position.x = moving_ball.x + ball_velocity.x;
	next_ball_position.y = moving_ball.y + ball_velocity.y;

	rotated_stick.x = wall.x + distance_from_center * cos(stick_rotation * PI / 180);
	rotated_stick.y = wall.y + distance_from_center * sin(stick_rotation * PI / 180);

	// 공의 로컬 좌표 계산
	local_ball.x = next_ball_position.x - rotated_stick.x;
	local_ball.y = next_ball_position.y - rotated_stick.y;

	float radian_angle = -(stick_rotation * PI / 180);

	rotated_ball.x = local_ball.x * cos(radian_angle) - local_ball.y * sin(radian_angle);
	rotated_ball.y = local_ball.x * sin(radian_angle) + local_ball.y * cos(radian_angle) + 50;

	if (rotated_ball.x >= 0 && rotated_ball.x <= stick.width && rotated_ball.y >= 0 && rotated_ball.y <= stick.height) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void Collision_Detection_With_Stick(void) {

	if (Next_Ball_Collision_Detection_With_Stick()) {
		Point rotated_stick;
		Point local_ball, rotated_ball;
		Point dist, unit_dist, overlap, rotated_velocity;

		float dotProduct, norm;
		float distance_from_center = 350;

		// 회전된 스틱 위치 계산
		rotated_stick.x = wall.x + distance_from_center * cos(stick_rotation * PI / 180);
		rotated_stick.y = wall.y + distance_from_center * sin(stick_rotation * PI / 180);

		// 공의 로컬 좌표 계산
		local_ball.x = moving_ball.x - rotated_stick.x;
		local_ball.y = moving_ball.y - rotated_stick.y;

		float radian_angle = -(stick_rotation * PI / 180);

		// 회전된 좌표에서 공의 위치로 회전
		rotated_ball.x = local_ball.x * cos(radian_angle) - local_ball.y * sin(radian_angle);
		rotated_ball.y = local_ball.x * sin(radian_angle) + local_ball.y * cos(radian_angle) + 50;

		// 공이 스틱 내부에 들어왔는지 확인
		if (rotated_ball.x >= 0 && rotated_ball.x <= stick.width && rotated_ball.y >= 0 && rotated_ball.y <= stick.height) {
			// 겹치는 부분 길이 계산
			overlap.x = (stick.width / 2 + moving_ball_radius) - fabs(rotated_ball.x - stick.width / 2);
			overlap.y = (stick.height / 2 + moving_ball_radius) - fabs(rotated_ball.y - stick.height / 2);

			// 공의 속도를 회전 좌표계로 변환
			rotated_velocity.x = ball_velocity.x * cos(radian_angle) - ball_velocity.y * sin(radian_angle);
			rotated_velocity.y = ball_velocity.x * sin(radian_angle) + ball_velocity.y * cos(radian_angle);


			// 충돌이 일어난 축에 따라 속도를 음수로 바꾸어 공을 반사시키도록 합니다
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

			//printf("스틱과 충돌함\n");
		}
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

		//printf("%d 번 블록 x : %f , y : %f\n", block_num, rotated_ball.x, rotated_ball.y);
		// 충돌이 블록 내에서 발생하는지 확인 및 처리
		if (rotated_ball.x >= 0 && rotated_ball.x <= block.width && rotated_ball.y >= 0 && rotated_ball.y <= block.height) {
			// 
			Point overlap;
			overlap.x = (block.width / 2 + moving_ball_radius) - fabs(rotated_ball.x - block.width / 2);
			overlap.y = (block.height / 2 + moving_ball_radius) - fabs(rotated_ball.y - block.height / 2);

			// 로컬 벡터를 회전시키기 전에 업데이트
			Point rotated_velocity;
			rotated_velocity.x = ball_velocity.x * cos(radian_angle) - ball_velocity.y * sin(radian_angle);
			rotated_velocity.y = ball_velocity.x * sin(radian_angle) + ball_velocity.y * cos(radian_angle);

			// 충돌이 일어난 축에 따라 속도를 음수로 바꾸어 공을 반사시키도록 합니다
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



			//printf("블록과 충돌함\n");
			block.visible = false;
			score += 10;
		}
	}
}

void Collision_Detection_to_Fixed_Brick(Block& fixed_block) {
	// 블록이 화면에 표시되어 있으면 충돌 검사 수행
	if (fixed_block.visible) {
		float dotProduct, norm;

		closet_block.x = moving_ball.x;
		closet_block.y = moving_ball.y;

		// 가장 가까운 위치
		if (moving_ball.x < fixed_block.x) {
			closet_block.x = fixed_block.x;
		}
		else if (moving_ball.x > fixed_block.x + fixed_block.width) {
			closet_block.x = fixed_block.x + fixed_block.width;
		}
		if (moving_ball.y < fixed_block.y) {
			closet_block.y = fixed_block.y;
		}
		else if (moving_ball.y > fixed_block.y + fixed_block.height) {
			closet_block.y = fixed_block.y + fixed_block.height;
		}

		// 공과 가장 가까운 위치와의 거리 벡터 계산
		sdist.x = moving_ball.x - closet_block.x;
		sdist.y = moving_ball.y - closet_block.y;

		// 거리 백터의 크기 계산
		norm = sqrt(pow(sdist.x, 2) + pow(sdist.y, 2));

		//공과 블럭이 충돌 했는지 확인
		if (norm < (moving_ball_radius + fixed_block.width/2)) {
			// 법선 백터 계산
			unit_sdist.x = sdist.x / norm;
			unit_sdist.y = sdist.y / norm;

			// 공 속도 백터와 법선 백터의 내적 계산
			dotProduct = ball_velocity.x * unit_sdist.x + ball_velocity.y * unit_sdist.y;

			// 법선 백터를 사용하여 공의 속도 백터 -> 반사 백터
			ball_velocity.x -= 2 * dotProduct * unit_sdist.x;
			ball_velocity.y -= 2 * dotProduct * unit_sdist.y;

			fixed_block.visible = false;
			score += 10;
		}
	}
}

void frame_reset(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0); // 검은색 배경
	glClear(GL_COLOR_BUFFER_BIT);
}

void ball(void) {
	// 충돌 처리 부분
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
	glTranslatef(360, -50.0f, 0.0f);
	Modeling_Stick();

	// 스틱을 움직일 수 있는 최대 범위
	if (stick_rotation <= -180.0) {
		stick_rotation = -180.0;
	}
	else if (stick_rotation > 0.0) {
		stick_rotation = 0.0;
	}
	glPopMatrix();
}


void displayText(float x, float y, float r, float g, float b, const char* str, int num) {
	glColor3f(r, g, b);
	glRasterPos2f(x, y);

	if (num == 0) {
		for (const char* c = str; *c != '\0'; ++c) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		}
	}
	else if (num == 1) {
		for (const char* c = str; *c != '\0'; ++c) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
		}
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
	if (game_page == 0) {
		MainPage();
	}

	else if (game_page == 1) {

		// 벽 그리기
		Modeling_Wall();

		draw_blocks_with_circular_motion();

		draw_stick_with_circular_motion();

		draw_blocks_with_fixed();

		// 공 그리기
		ball();

		// 글씨 출력해보기
		char formattedText_x[255];

		sprintf(formattedText_x, "score : %d", score);

		displayText(20.0f, HEIGHT-50, 1.0f, 1.0f, 1.0f, formattedText_x, 0);
		
		if (start_check == FALSE) {
			info();
		}

	}
	if (score == 330) {
		ball_velocity.x = 0;
		ball_velocity.y = 0;
		ClearPage();
	}

	glutSwapBuffers();
	glFlush();
}

void Mykey(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'r':
		init();
		init_blocks();
		init_Fixed_blocks();
		break;
	case 's':
		start_check = TRUE;
		ball_velocity.x = 0.1;
		ball_velocity.y = 0.3;
		break;
	case 32:
		game_page = 1;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

// game_mode == 0 일 때 실행
void MainPage(void) {
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, HEIGHT);
	glVertex2f(WIDTH, HEIGHT);
	glVertex2f(WIDTH, 0);
	glEnd();


	// 글씨 출력해보기
	char start_text[255];

	sprintf(start_text, "Press SpaceBar to Play");

	displayText(WIDTH / 2 - 100, HEIGHT / 2, 1.0f, 1.0f, 1.0f, start_text, 0);
}

void DiePage(void) {
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
    glVertex2f(400, 400);
    glVertex2f(800, 400);
    glVertex2f(800, 600);
    glVertex2f(400, 600);
	glEnd();

	block_rotation_speed = 0;

	char die_text[255];
	char die_text_1[255];

	sprintf(die_text, "Game Over");
	sprintf(die_text_1, "Press 'R' to Restart");

	displayText(WIDTH / 2 - 60, HEIGHT / 2 + 100, 1.0f, 1.0f, 1.0f, die_text, 0);
	displayText(WIDTH / 2 - 60, HEIGHT / 2 + 70, 1.0f, 1.0f, 1.0f, die_text_1, 0);
}

void ClearPage(void) {
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex2f(400, 400);
	glVertex2f(800, 400);
	glVertex2f(800, 600);
	glVertex2f(400, 600);
	glEnd();

	block_rotation_speed = 0;

	char clear_text[255];
	char clear_text_1[255];

	sprintf(clear_text, "Stage Clear!");
	sprintf(clear_text_1, "Congratulations!");

	displayText(WIDTH / 2 - 60, HEIGHT / 2 + 100, 1.0f, 1.0f, 1.0f, clear_text, 0);
	displayText(WIDTH / 2 - 60, HEIGHT / 2 + 70, 1.0f, 1.0f, 1.0f, clear_text_1, 0);
}

void info(void) {
	glColor3f(1.0f, 1.0f, 1.0f);

	char info_text[255];

	sprintf(info_text, "Press 'S' to start");

	displayText(WIDTH / 2 - 80, HEIGHT / 2 - 300, 0.0f, 0.0f, 0.0f, info_text, 0);
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
	glutKeyboardFunc(Mykey);
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(MySpecial);
	glutMainLoop();
}
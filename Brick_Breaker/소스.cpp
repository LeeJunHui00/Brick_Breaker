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

float	moving_ball_radius;				// �����̴� ���� ������ 
float	wall_radius;

const int	num_blocks = 18;				// �����̴� �� ���� ����
const int	num_fixed_blocks = 24;		// ������ �� ���� ����

float	block_radius = 100;
float	block_rotation = 0.0;
float	block_rotation_speed;

float	stick_radius = 300;
float	stick_velocity = 30;			// ��ƽ �����̴� �ӵ�
float	stick_rotation;

float	game_page = 0;
int		score;

// ���� ��ġ ������ ������ ����ü
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

Point	moving_ball, ball_velocity;		// �����̴� ��, �ӵ� ����
Point	brick_center, stick_center;
Point	sdist, unit_sdist;
Point	closet_block, closet_stick;
Point	wall;
Block	blocks[num_blocks], fixed_blocks[num_fixed_blocks];
POSITION pos_stick, pos_ball;

Block	stick;


// ���� �׸��� �Լ�
void Modeling_Circle(float radius, Point CC);

// ��ƽ �׸��� �Լ�
void Modeling_Stick(void);

// �� �׸��� �Լ�
void Modeling_Block(const Block& block);

// ���� ���� �浹�� �˻��ϴ� �Լ�
void Collision_Detection_to_Walls(void);

// ���� ��ƽ�� �浹�� �˻��ϴ� �Լ�
void Collision_Detection_With_Stick(void);

// ���� ���� �浹�� �˻��ϴ� �Լ�
void Collision_Detection_to_Brick(Block& block);

// ���� �������� �浹�� �˻��ϴ� �Լ�
void Collision_Detection_to_Sphere_Walls(void);

// �ʱ�ȭ �Լ�
void init(void);

// �� ���� �� �ʱ�ȭ �Լ�
void init_blocks(void);

// ������ ũ�� ���� �� ȣ��Ǵ� �Լ�
void MyReshape(int w, int h);

// ȭ���� �׸��� �Լ�
void RenderScene(void);

// ȭ�� ���� ���� �Լ�
void frame_reset(void);

// �ؽ�Ʈ ��� �Լ�
void displayText(float x, float y, float r, float g, float b, const char* str, int num);

// �� �׸��� �Լ�
void Modeling_Wall(void);

// ��� �ϴ� �� �׸��� �Լ�
void draw_blocks_with_circular_motion(void);

// �� �׸���
void ball(void);

// ��� �ϴ� ��ƽ �׸��� �Լ�
void draw_stick_with_circular_motion(void);

// ������ �� ���� �� �ʱ�ȭ �Լ�
void init_Fixed_blocks(void);

// ������ �� �׸��� �Լ�
void Modeling_Fixed_blocks(const Block& fixed_block);

// ������ �� �ݺ� �Լ�
void draw_blocks_with_fixed(void);

// ������ �� �浹 üũ �Լ�
void Collision_Detection_to_Fixed_Brick(const Block& fixed_block);

// Ű���� �Է� �Լ�
void Mykey(unsigned char key, int x, int y);

// ���� �浹üũ �ι��ϱ�
bool Next_Ball_Collision_Detection_With_Stick(void);

// ���� ȭ��
void MainPage(void);

// ��� â
void DiePage(void);

// Ŭ���� â
void ClearPage(void);

void info(void);

void init(void) {
	// �����̴� ���� �������� �ʱ� ��ġ, �ӵ� ����
	moving_ball_radius = 10.0;
	moving_ball.x = WIDTH / 2;
	moving_ball.y = HEIGHT / 2 -350;

	ball_velocity.x = 0.0;
	ball_velocity.y = 0.0;

	// �� �ʱ� ��ġ
	wall_radius = 400;
	wall.x = WIDTH / 2;
	wall.y = HEIGHT / 2;
	
	// �� ȸ��
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
	glShadeModel(GL_SMOOTH);//�׶��̼����� �ٰ����� ä��
	glBegin(GL_POLYGON);

	// ����
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
		glShadeModel(GL_SMOOTH);	//�׶��̼����� �ٰ����� ä��
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
		glShadeModel(GL_SMOOTH);//�׶��̼����� �ٰ����� ä��
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

	// ������� ����
	for (int i = 0; i < 360; i++) {
		if (i > 240 && i <300) {
			glColor3f(1.0, 0.0, 0.0);
		}
		// safe ����
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
	float distance;				// ���� ���� �� ������ �Ÿ�
	float dotProduct;			// ���� �ӵ� ���Ϳ� ���� ������ ����
	float length, theta;		// ���� ������ ����, ���� ����ǥ�� ����

	Point norm; // �浹 ������ ���� �߽��� ������ ����(���� ����)

	// ���� ������ �Ÿ� ���
	distance = sqrt(pow(moving_ball.x - wall.x, 2) + pow(moving_ball.y - wall.y, 2));

	// ���� �������� ���� �������� �պ��� �Ÿ��� �۰ų� ������ �浹�� �߻�
	if (distance <= (wall_radius + moving_ball_radius)) {
		// �浹 ������ �� �߽� ������ ���� ���� ���ϱ�
		norm.x = moving_ball.x - wall.x;
		norm.y = moving_ball.y - wall.y;		

		// ���� ������ ����(ũ��) ���
		length = sqrt(pow(norm.x, 2) + pow(norm.y,2));

		// ���� ���͸� ����ȭ(���� ���ͷ� �����)
		norm.x /= length;
		norm.y /= length;

		// ���� ������ ������� ���� ������ ���� �浹 ���� �Ǵ�
		if (length >= wall_radius) {

			theta = atan2(moving_ball.y - wall.y, moving_ball.x - wall.x) * (180 / PI);

			// ������ο� �浹�� ��� ���� �ӵ��� 0���� ������, DiePage() ����
			if (theta >= -120 && theta < -60) {
				//printf("������ο� �浹�Ǿ����ϴ�\n");
				ball_velocity.x = 0;
				ball_velocity.y = 0;
				DiePage();
			}

			// �浹 ���������� ���� �ӵ��� ���� ������ ���� ���
			dotProduct = ball_velocity.x * norm.x + ball_velocity.y * norm.y;

			// ���� �������� �ݻ�� �ӵ� ���
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

	// ���� ���� ��ǥ ���
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

		// ȸ���� ��ƽ ��ġ ���
		rotated_stick.x = wall.x + distance_from_center * cos(stick_rotation * PI / 180);
		rotated_stick.y = wall.y + distance_from_center * sin(stick_rotation * PI / 180);

		// ���� ���� ��ǥ ���
		local_ball.x = moving_ball.x - rotated_stick.x;
		local_ball.y = moving_ball.y - rotated_stick.y;

		float radian_angle = -(stick_rotation * PI / 180);

		// ȸ���� ��ǥ���� ���� ��ġ�� ȸ��
		rotated_ball.x = local_ball.x * cos(radian_angle) - local_ball.y * sin(radian_angle);
		rotated_ball.y = local_ball.x * sin(radian_angle) + local_ball.y * cos(radian_angle) + 50;

		// ���� ��ƽ ���ο� ���Դ��� Ȯ��
		if (rotated_ball.x >= 0 && rotated_ball.x <= stick.width && rotated_ball.y >= 0 && rotated_ball.y <= stick.height) {
			// ��ġ�� �κ� ���� ���
			overlap.x = (stick.width / 2 + moving_ball_radius) - fabs(rotated_ball.x - stick.width / 2);
			overlap.y = (stick.height / 2 + moving_ball_radius) - fabs(rotated_ball.y - stick.height / 2);

			// ���� �ӵ��� ȸ�� ��ǥ��� ��ȯ
			rotated_velocity.x = ball_velocity.x * cos(radian_angle) - ball_velocity.y * sin(radian_angle);
			rotated_velocity.y = ball_velocity.x * sin(radian_angle) + ball_velocity.y * cos(radian_angle);


			// �浹�� �Ͼ �࿡ ���� �ӵ��� ������ �ٲپ� ���� �ݻ��Ű���� �մϴ�
			if (overlap.x > 0 && overlap.y > 0) {
				if (overlap.x >= overlap.y) {
					rotated_velocity.y = -rotated_velocity.y; // Y �� �ӵ� ����
				}
				else {
					rotated_velocity.x = -rotated_velocity.x; // X �� �ӵ� ����
				}
			}
			else if (overlap.x > 0 && overlap.y <= 0) {
				rotated_velocity.x = -rotated_velocity.x; // X �� �ӵ� ����
			}
			else if (overlap.y > 0 && overlap.x <= 0) {
				rotated_velocity.y = -rotated_velocity.y; // Y �� �ӵ� ����
			}

			// ���� �ӵ� ȸ�� ���� ������ �ǵ�����
			ball_velocity.x = rotated_velocity.x * cos(-radian_angle) - rotated_velocity.y * sin(-radian_angle);
			ball_velocity.y = rotated_velocity.x * sin(-radian_angle) + rotated_velocity.y * cos(-radian_angle);

			//printf("��ƽ�� �浹��\n");
		}
	}
}

// ����
void Collision_Detection_to_Brick(Block& block) {
	if (block.visible) {
		Point rotated_block;
		Point local_ball, rotated_ball;
		Point dist, unit_dist;

		float dotProduct, norm;
		float distance_from_center = 100;

		int block_num = &block - blocks;
		float block_angle = block_rotation + (block_num * (360.0f / num_blocks));

		// ȸ���� ��� ��ǥ ���
		rotated_block.x = wall.x + distance_from_center * cos(block_angle * PI / 180);
		rotated_block.y = wall.y + distance_from_center * sin(block_angle * PI / 180);

		// ���� ���� ��ǥ ���
		local_ball.x = moving_ball.x - rotated_block.x;
		local_ball.y = moving_ball.y - rotated_block.y;

		// ȸ�� ���� ���
		float radian_angle = -(block_angle * PI / 180);

		// ���� ȸ����Ű�� ��ȯ�� ��ǥ ���ϱ�
		rotated_ball.x = local_ball.x * cos(radian_angle) - local_ball.y * sin(radian_angle);
		rotated_ball.y = local_ball.x * sin(radian_angle) + local_ball.y * cos(radian_angle);

		//printf("%d �� ��� x : %f , y : %f\n", block_num, rotated_ball.x, rotated_ball.y);
		// �浹�� ��� ������ �߻��ϴ��� Ȯ�� �� ó��
		if (rotated_ball.x >= 0 && rotated_ball.x <= block.width && rotated_ball.y >= 0 && rotated_ball.y <= block.height) {
			// 
			Point overlap;
			overlap.x = (block.width / 2 + moving_ball_radius) - fabs(rotated_ball.x - block.width / 2);
			overlap.y = (block.height / 2 + moving_ball_radius) - fabs(rotated_ball.y - block.height / 2);

			// ���� ���͸� ȸ����Ű�� ���� ������Ʈ
			Point rotated_velocity;
			rotated_velocity.x = ball_velocity.x * cos(radian_angle) - ball_velocity.y * sin(radian_angle);
			rotated_velocity.y = ball_velocity.x * sin(radian_angle) + ball_velocity.y * cos(radian_angle);

			// �浹�� �Ͼ �࿡ ���� �ӵ��� ������ �ٲپ� ���� �ݻ��Ű���� �մϴ�
			if (overlap.x > 0 && overlap.y > 0) {
				if (overlap.x >= overlap.y) {
					rotated_velocity.y = -rotated_velocity.y; // Y �� �ӵ� ����
				}
				else {
					rotated_velocity.x = -rotated_velocity.x; // X �� �ӵ� ����
				}
			}
			else if (overlap.x > 0 && overlap.y <= 0) {
				rotated_velocity.x = -rotated_velocity.x; // X �� �ӵ� ����
			}
			else if (overlap.y > 0 && overlap.x <= 0) {
				rotated_velocity.y = -rotated_velocity.y; // Y �� �ӵ� ����
			}

			// ���� �ӵ� ȸ�� ���� ������ �ǵ�����
			ball_velocity.x = rotated_velocity.x * cos(-radian_angle) - rotated_velocity.y * sin(-radian_angle);
			ball_velocity.y = rotated_velocity.x * sin(-radian_angle) + rotated_velocity.y * cos(-radian_angle);



			//printf("��ϰ� �浹��\n");
			block.visible = false;
			score += 10;
		}
	}
}

void Collision_Detection_to_Fixed_Brick(Block& fixed_block) {
	// ����� ȭ�鿡 ǥ�õǾ� ������ �浹 �˻� ����
	if (fixed_block.visible) {
		float dotProduct, norm;

		closet_block.x = moving_ball.x;
		closet_block.y = moving_ball.y;

		// ���� ����� ��ġ
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

		// ���� ���� ����� ��ġ���� �Ÿ� ���� ���
		sdist.x = moving_ball.x - closet_block.x;
		sdist.y = moving_ball.y - closet_block.y;

		// �Ÿ� ������ ũ�� ���
		norm = sqrt(pow(sdist.x, 2) + pow(sdist.y, 2));

		//���� ���� �浹 �ߴ��� Ȯ��
		if (norm < (moving_ball_radius + fixed_block.width/2)) {
			// ���� ���� ���
			unit_sdist.x = sdist.x / norm;
			unit_sdist.y = sdist.y / norm;

			// �� �ӵ� ���Ϳ� ���� ������ ���� ���
			dotProduct = ball_velocity.x * unit_sdist.x + ball_velocity.y * unit_sdist.y;

			// ���� ���͸� ����Ͽ� ���� �ӵ� ���� -> �ݻ� ����
			ball_velocity.x -= 2 * dotProduct * unit_sdist.x;
			ball_velocity.y -= 2 * dotProduct * unit_sdist.y;

			fixed_block.visible = false;
			score += 10;
		}
	}
}

void frame_reset(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0); // ������ ���
	glClear(GL_COLOR_BUFFER_BIT);
}

void ball(void) {
	// �浹 ó�� �κ�
	Collision_Detection_With_Stick();		// ���� ��ƽ�� �浹 �Լ� 
	for (int i = 0; i < num_blocks; i++) {
		Collision_Detection_to_Brick(blocks[i]);
	}
	for (int i = 0; i < num_fixed_blocks; i++) {
		Collision_Detection_to_Fixed_Brick(fixed_blocks[i]);
	}

	Collision_Detection_to_Sphere_Walls();

	// �����̴� ���� ��ġ ��ȭ 
	moving_ball.x += ball_velocity.x;
	moving_ball.y += ball_velocity.y;

	// �����̴� �� �׸��� 
	glColor3f(1.0, 0.0, 0.0);
	Modeling_Circle(moving_ball_radius, moving_ball);
}

void draw_blocks_with_circular_motion(void) {

	block_rotation += block_rotation_speed;

	for (int i = 0; i < num_blocks; i++) {

		glPushMatrix();

		// ���� �߽����� �̵�
		glTranslatef(WIDTH/2, HEIGHT/2, 0.0f);

		// ������ ���� ȸ��
		glRotatef(block_rotation + (i * 360.0f / num_blocks), 0.0f, 0.0f, 1.0f);

		// ���� ��輱���� �̵�
		glTranslatef(100, 0.0f, 0.0f);

		// ��� �׸���
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

	// ��ƽ�� ������ �� �ִ� �ִ� ����
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
	// ȭ�� ���� ����
	frame_reset();
	if (game_page == 0) {
		MainPage();
	}

	else if (game_page == 1) {

		// �� �׸���
		Modeling_Wall();

		draw_blocks_with_circular_motion();

		draw_stick_with_circular_motion();

		draw_blocks_with_fixed();

		// �� �׸���
		ball();

		// �۾� ����غ���
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

// game_mode == 0 �� �� ����
void MainPage(void) {
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, HEIGHT);
	glVertex2f(WIDTH, HEIGHT);
	glVertex2f(WIDTH, 0);
	glEnd();


	// �۾� ����غ���
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
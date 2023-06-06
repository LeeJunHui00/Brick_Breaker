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

float	moving_ball_radius;				// �����̴� ���� ������ 
float	wall_radius;
//float	stick_x, stick_y;				// ��ƽ�� ����,����
float	stick_velocity = 30;			// ��ƽ �����̴� �ӵ�

const int	num_blocks = 8;				// �����̴� �� ���� ����
const int	num_fixed_blocks = 24;		// ������ �� ���� ����

float	block_radius = 100;
float	block_rotation = 0.0;
float	block_rotation_speed = 0.02;

float	stick_radius = 300;
float	stick_rotation = -110.0;

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
void displayText(float x, float y, float r, float g, float b, const char* str);

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

void init(void) {
	// �����̴� ���� �������� �ʱ� ��ġ, �ӵ� ����
	moving_ball_radius = 10.0;
	moving_ball.x = WIDTH / 2;
	moving_ball.y = HEIGHT / 2 -300;

	ball_velocity.x = 0.3;
	ball_velocity.y = 0.5;

	// �� �ʱ� ��ġ
	wall_radius = 400;
	wall.x = WIDTH / 2;
	wall.y = HEIGHT / 2;
	
	// �� ȸ��
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
	// ���콺 ���� ��ư Ŭ�� �� ��ǥ�� ���� �̵�
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
	// ����
	//glVertex2f(stick_x, stick_y);
	//glVertex2f(stick_x + 95.0, stick_y);
	//glVertex2f(stick_x + 95.0, stick_y + 25.0);
	//glVertex2f(stick_x, stick_y + 25.0);

	// ����
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

	// ������� ����
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

	Point norm;	// �浹 ������ ���� �߽��� ������ ����

	// ���� ������ �Ÿ� ���
	//distance = sqrt(pow(moving_ball.x - wall.x, 2) + sqrt(pow(moving_ball.y - wall.y,2)));
	distance = sqrt(pow(moving_ball.x - wall.x, 2) + pow(moving_ball.y - wall.y, 2));

	if (distance <= (wall_radius + moving_ball_radius)) {
		// �浹 ������ �� �߽� ������ ���� ���� ���ϱ�
		norm.x = moving_ball.x - wall.x;
		norm.y = moving_ball.y - wall.y;		

		length = sqrt(pow(norm.x, 2) + pow(norm.y,2));

		norm.x /= length;
		norm.y /= length;

		if (length >= wall_radius) {
			theta = atan2(moving_ball.y - wall.y, moving_ball.x - wall.x) * (180 / PI);
			printf("%f\n", theta);

			if (theta >= -130 && theta < -50) {
				printf("������ο� �浹�Ǿ����ϴ�\n");
				ball_velocity.x = 0;
				ball_velocity.y = 0;
			}

			// �浹 ���������� ���� �ӵ��� ���� ������ ���� ���
			dotProduct = ball_velocity.x * norm.x + ball_velocity.y * norm.y;

			// ���� �������� �ݻ�� �ӵ� ���
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
		//printf("���� �浹��\n");
		ball_velocity.x = ball_velocity.x * -1;
	}
	else if (WIDTH <= ball_right) {
		//printf("������ �浹��\n");
		ball_velocity.x = ball_velocity.x * -1;
	}
	else if (HEIGHT <= ball_top) {
		//printf("�� �浹��\n");
		ball_velocity.y = ball_velocity.y * -1;
	}
	else if (bottom >= ball_bottom) {
		//printf("�Ʒ� �浹��\n");
		ball_velocity.y = ball_velocity.y * -1;
	}
}

//* �ȵ��ư��� ��ƽ*/
//void Collision_Detection_With_Stick(void) {
//	//if (moving_ball.y - moving_ball_radius <= stick_y + 25.0 &&
//	//	moving_ball.x > stick_x && moving_ball.x < stick_x + 95.0) {
//	//	printf("��ƽ�� �浹��\n");
//	//	ball_velocity.y = ball_velocity.y * -1;
//	//}
//
//	// -------- �� ���ư��� ��ƽ -----
//	//float dotProduct, norm;
//
//	//stick_center.x = stick_x + 95 / 2;
//	//stick_center.y = stick_y + 25 / 2;
//
//	//closet_stick.x = moving_ball.x;
//	//closet_stick.y = moving_ball.y;
//
//	//// ���� ����� ��ġ
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
//	//// �Ÿ� ������ ũ�� ���
//	//norm = sqrt(pow(sdist.x, 2) + pow(sdist.y, 2));
//
//	////���� ���� �浹 �ߴ��� Ȯ��
//	//if (norm < (moving_ball_radius + fmin(95 / 2, 25 / 2))) {
//	//	// ���� ���� ���
//	//	unit_sdist.x = sdist.x / norm;
//	//	unit_sdist.y = sdist.y / norm;
//
//	//	// �� �ӵ� ���Ϳ� ���� ������ ���� ���
//	//	dotProduct = ball_velocity.x * unit_sdist.x + ball_velocity.y * unit_sdist.y;
//
//	//	// ���� ���͸� ����Ͽ� ���� �ӵ� ���� -> �ݻ� ����
//	//	ball_velocity.x -= 2 * dotProduct * unit_sdist.x;
//	//	ball_velocity.y -= 2 * dotProduct * unit_sdist.y;
//	//}
//
//	// -------- �� ���ư��� ��ƽ -----
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
//	// ���� ���� ��ǥ ���
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
//	// �Ÿ� ������ ũ��(����)�� ����Ѵ�. �Ÿ� ����� ���� ������ �Լ��� ����Ѵ�.
//	norm = sqrt(pow(dist.x, 2) + pow(dist.y, 2));
//
//	if (norm < (moving_ball_radius)) {
//		// �浹�� �߻��� ���, ���� ����(���� �Ÿ� ����)�� ����Ѵ�.
//		unit_dist.x = dist.x / norm;
//		unit_dist.y = dist.y / norm;
//
//		// �� �ӵ� ���Ϳ� ���� ������ ������ ����Ѵ�. 
//		dotProduct = ball_velocity.x * unit_dist.x + ball_velocity.y * unit_dist.y;
//
//		// ���� ���Ϳ� ���õ� ���� ���� �̿��Ͽ� ���� �ӵ� ���͸� ������Ʈ�Ѵ�.   
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

	// ���� ���� ��ǥ ���
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

		// �浹�� �Ͼ �࿡ ���� �ӵ��� ������ �ٲپ� ���� �ݻ��Ű���� �մϴ�
		//if (overlap.x >= overlap.y) {
		//	rotated_velocity.y = -rotated_velocity.y; // Y �� �ӵ� ����
		//}
		//else {
		//	rotated_velocity.x = -rotated_velocity.x; // X �� �ӵ� ����
		//}
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

		printf("��ƽ�� �浹��\n");
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

		// �浹�� ��� ������ �߻��ϴ��� Ȯ�� �� ó��
		if (rotated_ball.x >= 0 && rotated_ball.x <= block.width && rotated_ball.y >= 0 && rotated_ball.y <= block.height) {
			Point overlap;
			overlap.x = (block.width / 2 + moving_ball_radius) - fabs(rotated_ball.x - block.width / 2);
			overlap.y = (block.height / 2 + moving_ball_radius) - fabs(rotated_ball.y - block.height / 2);

			// ���� ���͸� ȸ����Ű�� ���� ������Ʈ
			Point rotated_velocity;
			rotated_velocity.x = ball_velocity.x * cos(radian_angle) - ball_velocity.y * sin(radian_angle);
			rotated_velocity.y = ball_velocity.x * sin(radian_angle) + ball_velocity.y * cos(radian_angle);

			// �浹�� �Ͼ �࿡ ���� �ӵ��� ������ �ٲپ� ���� �ݻ��Ű���� �մϴ�
			//if (overlap.x >= overlap.y) {
			//	rotated_velocity.y = -rotated_velocity.y; // Y �� �ӵ� ����
			//}
			//else {
			//	rotated_velocity.x = -rotated_velocity.x; // X �� �ӵ� ����
			//}
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

			printf("��ϰ� �浹��\n");
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

		//	// �밢�� �浹 ó��
		//	if (fabs(dx) < fixed_block.width / 2 && fabs(dy) < fixed_block.height / 2) {
		//		ball_velocity.x = -ball_velocity.x;
		//		ball_velocity.y = -ball_velocity.y;
		//	}

		//	printf("������ ���� �浹\n");
		//	fixed_block.visible = false;

		//}
		float dotProduct, norm;

		closet_stick.x = moving_ball.x;
		closet_stick.y = moving_ball.y;

		// ���� ����� ��ġ
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

		// �Ÿ� ������ ũ�� ���
		norm = sqrt(pow(sdist.x, 2) + pow(sdist.y, 2));

		//���� ���� �浹 �ߴ��� Ȯ��
		if (norm < (moving_ball_radius + fmin(95 / 2, 25 / 2))) {
			// ���� ���� ���
			unit_sdist.x = sdist.x / norm;
			unit_sdist.y = sdist.y / norm;

			// �� �ӵ� ���Ϳ� ���� ������ ���� ���
			dotProduct = ball_velocity.x * unit_sdist.x + ball_velocity.y * unit_sdist.y;

			// ���� ���͸� ����Ͽ� ���� �ӵ� ���� -> �ݻ� ����
			ball_velocity.x -= 2 * dotProduct * unit_sdist.x;
			ball_velocity.y -= 2 * dotProduct * unit_sdist.y;

			fixed_block.visible = false;
		}
	}
}

void frame_reset(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0); // ������ ���
	glClear(GL_COLOR_BUFFER_BIT);
}

void ball(void) {
	// �浹 ó�� �κ�
	//Collision_Detection_to_Walls();			// ���� ���� �浹 �Լ� 
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
	// ȭ�� ���� ����
	frame_reset();

	// �� �׸���
	Modeling_Wall();

	// �� �׸���
	ball();

	// ��ƽ �׸���
	//Modeling_Stick();

	draw_blocks_with_circular_motion();

	draw_stick_with_circular_motion();

	draw_blocks_with_fixed();

	// �۾� ����غ���
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
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

float	stick_x, stick_y;				// ��ƽ�� ����,����
float	stick_velocity = 20;			// ��ƽ �����̴� �ӵ�

const int	num_blocks = 8;				// �� ���� ����

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
Point	dist, unit_dist, sdist, unit_sdist;
Point	closet_block, closet_stick;
Block	blocks[num_blocks];
POSITION pos_stick, pos_ball;

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


void init(void) {
	// �����̴� ���� �������� �ʱ� ��ġ, �ӵ� ����
	moving_ball_radius = 10.0;
	moving_ball.x = WIDTH / 2;
	moving_ball.y = HEIGHT / 2;

	ball_velocity.x = 0.3;
	ball_velocity.y = 0.3;

	// ��ƽ �ʱ���ġ 
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
	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_POLYGON);
	glVertex2f(stick_x, stick_y);
	glVertex2f(stick_x + 95.0, stick_y);
	glVertex2f(stick_x + 95.0, stick_y + 25.0);
	glVertex2f(stick_x, stick_y + 25.0);
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

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 360; i++) {
		float theta = i * (PI / 180);
		float cx = x + radius * cos(theta);
		float cy = y + radius * sin(theta);
		glVertex2f(cx, cy);
	}
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
//		printf("fixed �� �浹��\n");
//		velocity.x = velocity.x * -1;
//		velocity.y = velocity.y * -1;
//		collision_count++;
//	}
//}

void Collision_Detection_to_Sphere_Walls(void) {
	float wall_radius = 400;
	float wall_x = WIDTH / 2;
	float wall_y = HEIGHT / 2;
	float distance, d;
	float correction;
	float dot;
	float normal_x, normal_y;
	float next_moving_ball_x, next_moving_ball_y;
	float reflected_vx, reflected_vy;

	next_moving_ball_x = moving_ball.x + ball_velocity.x;
	next_moving_ball_y = moving_ball.y + ball_velocity.y;

	distance = sqrt(pow(moving_ball.x - wall_x, 2) + sqrt(pow(moving_ball.y - wall_y,2)));

	if (distance < (wall_radius + moving_ball_radius)) {
		normal_x = moving_ball.x - wall_x;
		normal_y = moving_ball.y - wall_y;
		float length;
		
		length = sqrt(normal_x * normal_x + normal_y * normal_y);
		if (length > 0) {
			normal_x /= length;
			normal_y /= length;
		}
		if (length >= wall_radius) {
			// �ݻ� ���� ���
			dot = ball_velocity.x * normal_x + ball_velocity.y * normal_y;

			// ���� �ӵ� ������Ʈ
			reflected_vx = ball_velocity.x - 2 * dot * normal_x;
			reflected_vy = ball_velocity.y - 2 * dot * normal_y;

			ball_velocity.x = reflected_vx;
			ball_velocity.y = reflected_vy;

		}
	}
}

void Collision_Detection_to_Walls(void) {
	float ball_left = moving_ball.x - moving_ball_radius;
	float ball_right = moving_ball.x + moving_ball_radius;
	float ball_top = moving_ball.y + moving_ball_radius;
	float ball_bottom = moving_ball.y - moving_ball_radius;

	if (left >= ball_left) {
		printf("���� �浹��\n");
		ball_velocity.x = ball_velocity.x * -1;
	}
	else if (WIDTH <= ball_right) {
		printf("������ �浹��\n");
		ball_velocity.x = ball_velocity.x * -1;
	}
	else if (HEIGHT <= ball_top) {
		printf("�� �浹��\n");
		ball_velocity.y = ball_velocity.y * -1;
	}
	else if (bottom >= ball_bottom) {
		printf("�Ʒ� �浹��\n");
		ball_velocity.y = ball_velocity.y * -1;
	}
}

void Collision_Detection_With_Stick(void) {
	//if (moving_ball.y - moving_ball_radius <= stick_y + 25.0 &&
	//	moving_ball.x > stick_x && moving_ball.x < stick_x + 95.0) {
	//	printf("��ƽ�� �浹��\n");
	//	ball_velocity.y = ball_velocity.y * -1;
	//}
	float dotProduct, norm;

	stick_center.x = stick_x + 95 / 2;
	stick_center.y = stick_y + 25 / 2;

	closet_stick.x = moving_ball.x;
	closet_stick.y = moving_ball.y;

	// ���� ����� ��ġ
	if (moving_ball.x < stick_x) {
		closet_stick.x = stick_x;
	}
	else if (moving_ball.x > stick_x + 95) {
		closet_stick.x = stick_x + 95;
	}
	if (moving_ball.y < stick_y) {
		closet_stick.y = stick_y;
	}
	else if (moving_ball.y > stick_y + 25) {
		closet_stick.y = stick_y + 25;
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
	}

}

void Collision_Detection_to_Brick(Block& block) {
	if (block.visible) {
		float dotProduct, norm;
		closet_block.x = moving_ball.x;
		closet_block.y = moving_ball.y;

		// ���� �߽� ��ǥ ������
		brick_center.x = block.x + block.width / 2;
		brick_center.y = block.y + block.height / 2;

		// ���� ����� ��ġ
		if (moving_ball.x < block.x) {
			closet_block.x = block.x;
		}
		else if (moving_ball.x > block.x + block.width) {
			closet_block.x = block.x + block.width;
		}
		if (moving_ball.y < block.y) {
			closet_block.y = block.y;
		}
		else if (moving_ball.y > block.y + block.height) {
			closet_block.y = block.y + block.height;
		}

		// ���� ��� �߽� ������ �Ÿ� ���� ���
		dist.x = moving_ball.x - closet_block.x;
		dist.y = moving_ball.y - closet_block.y;

		// �Ÿ� ������ ũ�� ���
		norm = sqrt(pow(dist.x, 2) + pow(dist.y, 2));

		//���� ���� �浹 �ߴ��� Ȯ��
		if (norm < (moving_ball_radius)) {
			// ���� ���� ���
			unit_dist.x = dist.x / norm;
			unit_dist.y = dist.y / norm;

			// �� �ӵ� ���Ϳ� ���� ������ ���� ���
			dotProduct = ball_velocity.x * unit_dist.x + ball_velocity.y * unit_dist.y;

			// ���� ���͸� ����Ͽ� ���� �ӵ� ���� -> �ݻ� ����
			ball_velocity.x -= 2 * dotProduct * unit_dist.x;
			ball_velocity.y -= 2 * dotProduct * unit_dist.y;
		
			block.visible = false;
		}
	}
}

void frame_reset(void) {
	glClearColor(1.0, 1.0, 0.0, 0.0); // Set display-window color to Yellow
	glClear(GL_COLOR_BUFFER_BIT);
}

void ball(void) {
	// �浹 ó�� �κ�
	Collision_Detection_to_Walls();			// ���� ���� �浹 �Լ� 
	Collision_Detection_With_Stick();		// ���� ��ƽ�� �浹 �Լ� 
	for (int i = 0; i < num_blocks; i++) {
		Collision_Detection_to_Brick(blocks[i]);
	}
	Collision_Detection_to_Sphere_Walls();
	// �����̴� ���� ��ġ ��ȭ 
	moving_ball.x += ball_velocity.x;
	moving_ball.y += ball_velocity.y;

	// �����̴� �� �׸��� 
	glColor3f(0.0, 0.0, 1.0);
	Modeling_Circle(moving_ball_radius, moving_ball);
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
	// ȭ�� ���� ����
	frame_reset();

	// �� �׸���
	Modeling_Wall();

	// �� �׸���
	ball();

	// ��ƽ �׸���
	Modeling_Stick();

	// �� �׸���
	for (int i = 0; i < num_blocks; i++) {
		Modeling_Block(blocks[i]);
		//Collision_Detection_to_Brick(blocks[i]);
	}
	// �۾� ����غ���
	char formattedText_x[255];
	char formattedText_y[255];

	sprintf(formattedText_x, "x speed : %f", ball_velocity.x);
	sprintf(formattedText_y, "y speed : %f", ball_velocity.y);

	displayText(0.0f, 13.0f, 0.0f, 0.0f, 0.0f, formattedText_x);
	displayText(0.0f, 3.0f, 0.0f, 0.0f, 0.0f, formattedText_y);
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
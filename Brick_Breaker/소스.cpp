#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>
#include <stdio.h>

#define	WIDTH 			1200
#define	HEIGHT			600
#define	PI				3.1415
#define	POLYGON_num		50

int		left = 0;
int		bottom = 0;

float	moving_ball_radius;				// �����̴� ���� ������ 

float	stick_x, stick_y;				// ��ƽ�� ����,����
float	stick_velocity = 10;			// ��ƽ �����̴� �ӵ�

const int	num_blocks = 10;				// �� ���� ����

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

Point	moving_ball, ball_velocity;		// �����̴� ��, �ӵ� ����
Point	brick_center;
Point	dist, unit_dist;
Block	blocks[num_blocks];

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



void init(void) {
	// �����̴� ���� �������� �ʱ� ��ġ, �ӵ� ����
	moving_ball_radius = 10.0;
	moving_ball.x = WIDTH / 2;
	moving_ball.y = HEIGHT / 4;

	ball_velocity.x = 0.0;
	ball_velocity.y = 0.3;

	// ��ƽ �ʱ���ġ 
	stick_x = WIDTH / 2 - 95.0 / 2;
	stick_y = 10.0;
}


void init_blocks(void) {
	float	block_width		= 60;
	float	block_height	= 30;
	
	for (int i = 0; i < num_blocks; i++) {
		blocks[i].x = 200 + i * (block_width + 10);
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
//		printf("fixed �� �浹��\n");
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
	if (moving_ball.y - moving_ball_radius <= stick_y + 25.0 &&
		moving_ball.x > stick_x && moving_ball.x < stick_x + 95.0) {
		printf("��ƽ�� �浹��\n");
		ball_velocity.y = ball_velocity.y * -1;
	}
}

void Collision_Detection_to_Brick(Block& block) {
	//float ball_left = moving_ball.x - moving_ball_radius;
	//float ball_right = moving_ball.x + moving_ball_radius;
	//float ball_top = moving_ball.y + moving_ball_radius;
	//float ball_bottom = moving_ball.y - moving_ball_radius;

	//float brick_center_x = block.x + block.width / 2;
	//float brick_center_y = block.y + block.height / 2;

	//float dist_x = moving_ball.x - brick_center_x;
	//float dist_y = moving_ball.y - brick_center_y;
	//float norm = sqrt(pow(dist_x, 2) + pow(dist_y, 2));
	//float unit_dist_x = dist_x / norm;
	//float unit_dist_y = dist_y / norm;

	//float angle_in = atan2(unit_dist_y, unit_dist_x);
	//float angle_out = -angle_in;

	//float velocity_norm = sqrt(ball_velocity.x * ball_velocity.x + ball_velocity.y * ball_velocity.y);

	//ball_velocity.x = velocity_norm * cos(angle_out);
	//ball_velocity.y = velocity_norm * sin(angle_out);

	float dotProduct, norm;

	// ���� �߽� ��ǥ ������
	brick_center.x = block.x + block.width / 2;
	brick_center.y = block.y + block.height / 2;

	// ���� ��� �߽� ������ �Ÿ� ���� ���
	dist.x = moving_ball.x - brick_center.x;
	dist.y = moving_ball.y - brick_center.y;

	// �Ÿ� ������ ũ�� ���
	norm = sqrt(pow(dist.x, 2) + pow(dist.y, 2));

	//���� ���� �浹 �ߴ��� Ȯ��
	if (norm <= (moving_ball_radius + fmin(block.width / 2, block.height / 2))) {
		// ���� ���� ���
		unit_dist.x = dist.x / norm;
		unit_dist.y = dist.y / norm;

		// �� �ӵ� ���Ϳ� ���� ������ ���� ���
		dotProduct = ball_velocity.x * unit_dist.x + ball_velocity.y * unit_dist.y;

		// ���� ���͸� ����Ͽ� ���� �ӵ� ���� -> �ݻ� ����
		ball_velocity.x -= 2.0f * dotProduct * unit_dist.x;
		ball_velocity.y -= 2.0f * dotProduct * unit_dist.y;
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

	// �����̴� ���� ��ġ ��ȭ 
	moving_ball.x += ball_velocity.x;
	moving_ball.y += ball_velocity.y;

	// �����̴� �� �׸��� 
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
	// ȭ�� ���� ����
	frame_reset();


	// �� �׸���
	ball();

	// ��ƽ �׸���
	Modeling_Stick();

	// �� �׸���
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
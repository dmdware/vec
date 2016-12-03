
#ifndef CAMERA_H
#define CAMERA_H

#include "vec3f.h"
#include "quaternion.h"
#include "fixmath.h"

#define MOUSE_SENSITIVITY	0.0001f	//radians over pixels

struct Camera 
{
	Vec3i pos;					
	Vec3i view;			
	Vec3i up;		
	Vec3i strafe;		
	Vec3i vel;
	Vec3i orientv;	// pitch, yaw, roll
	Quaternion orientq;	//orientation quaternion
	ecbool grounded;
};

typedef struct Camera Camera;

void Cam_init(Camera *c);
Vec3i Cam_up2(Camera *c);
void Cam_rotateview(Camera *c, int angle, int x, int y, int z);
void Cam_lookat(Camera *c, Vec3i at);
void Cam_rotatebymouse(Camera *c, int dx, int dy);
Vec3i Cam_lookpos(Camera *c);
void Cam_grounded(Camera *c, ecbool ground);
void Cam_rotateabout(Camera *c, Vec3i center, int rad, int x, int y, int z);
void Cam_strafe(Camera *c, int speed);
void Cam_move(Camera *c, int speed);
void Cam_accelerate(Camera *c, int speed);
void Cam_accelstrafe(Camera *c, int speed);
void Cam_accelrise(Camera *c, int speed);
void Cam_rise(Camera *c, int speed);
void Cam_move(Camera *c, Vec3i delta);
void Cam_moveto(Camera *c, Vec3i newpos);
void Cam_limithvel(Camera *c, int limit);
void Cam_stop(Camera *c);
void Cam_calcstrafe(Camera *c);
void Cam_frameupd(Camera *c);
void Cam_friction(Camera *c);
void Cam_friction2(Camera *c);
int Cam_yaw(Camera *c);
int Cam_pitch(Camera *c);

extern Camera g_cam;

#endif
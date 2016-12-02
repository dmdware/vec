
#ifndef CAMERA_H
#define CAMERA_H

#include "vec3f.h"
#include "quaternion.h"

#define MOUSE_SENSITIVITY	0.0001f	//radians over pixels

struct Camera 
{
public:
	Vec3f pos;					
	Vec3f view;			
	Vec3f up;		
	Vec3f strafe;		
	Vec3f vel;
	Vec3f orientv;	// pitch, yaw, roll
	Quaternion orientq;	//orientation quaternion
	ecbool grounded;

	Camera();
	Camera(float posx, float posy, float posz, float viewx, float viewy, float viewz, float upx, float upy, float upz);

	Vec3f up2();
	void position(float posx, float posy, float posz, float viewx, float viewy, float viewz, float upx, float upy, float upz);
	void rotateview(float angle, float x, float y, float z);
	void lookat(Vec3f at);
	void rotatebymouse(int dx, int dy);
	Vec3f lookpos();
	void grounded(ecbool ground);
	void rotateabout(Vec3f center, float rad, float x, float y, float z);
	void strafe(float speed);
	void move(float speed);
	void accelerate(float speed);
	void accelstrafe(float speed);
	void accelrise(float speed);
	void rise(float speed);
	void move(Vec3f delta);
	void moveto(Vec3f newpos);
	void limithvel(float limit);
	void stop();
	void calcstrafe();
	void calcyaw();
	void calcpitch();
	void calcroll();
	void frameupd();
	void friction();
	void friction2();
	float yaw();
	float pitch();
};									

extern Camera g_cam;

#endif
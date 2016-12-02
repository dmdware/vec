
#include "camera.h"
#include "3dmath.h"
#include "physics.h"

Camera g_cam;

Vec3f Camera::up2()		
{
	return Normalize( Cross( strafe, view - pos ) );
}

float Camera::yaw()
{
	return orientv.y;
}

float Camera::pitch()
{
	return orientv.x;
}

void Camera::calcyaw()
{
	Vec3f d = view - pos;
	orientv.y = GetYaw(d.x, d.z);
}

void Camera::calcpitch()
{
	Vec3f d = view - pos;
	float lateral = MAG_VEC3F(Vec3f(d.x, 0, d.z));
	orientv.x = RADTODEG( atan2(d.y, lateral) );
}

void Camera::calcroll()
{
	Vec3f d = view - pos;
	float lateral = MAG_VEC3F(Vec3f(d.x, 0, d.z));
	orientv.x = RADTODEG( atan2(d.y, lateral) );
}

void Camera::frameupd()
{
	pos = pos + vel;
	view = view + vel;

	//Vec3f vNew = pos + vel * g_drawfrinterval;
	//MoveTo(vNew);
}

Vec3f Camera::lookpos()
{
	return pos;

	/*
	if(g_viewmode == FIRSTPERSON)
		return pos;
	else
	{
		Vec3f d = Normalize(view - pos);
		Vec3f l[2];
		l[0] = pos;
		l[1] = pos - d * 64.0f;

		l[1] = g_edmap.TraceRay(pos, l[1]);

		CEntity* e;
		int localE = g_py[g_localP].entity;
		for(int i=0; i<ENTITIES; i++)
		{
			e = &g_entity[i];

			if(!e->on)
				continue;

			if(localE == i)
				continue;

			l[1] = e->TraceRay(l);
		}

		return l[1];
	}*/
}

Camera::Camera()
{
	pos	= Vec3f(0.0, 0.0, 0.0);
	view	= Vec3f(0.0, -1.0, 1.0);
	up	= Vec3f(0.0, 1.0, 0.0);

	grounded = ecfalse;

	orientv.x = 0;
	orientv.y = 0;

	calcstrafe();
}

Camera::Camera(float posx, float posy, float posz, float viewx, float viewy, float viewz, float upx, float upy, float upz)
{
	position(posx, posy, posz, viewx, viewy, viewz, upx, upy, upz);
}

void Camera::position(float posx, float posy, float posz, float viewx, float viewy, float viewz, float upx, float upy, float upz)
{
	pos = Vec3f(posx, posy, posz);
	view  = Vec3f(viewx, viewy, viewz);
	up = Vec3f(upx, upy, upz);

	calcstrafe();
	calcyaw();
	calcpitch();
}

void Camera::rotatebymouse(int dx, int dy)
{
	if( (dx == 0) && (dy == 0) ) return;

	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	float angleY = (float)( -dx ) * MOUSE_SENSITIVITY;
	float angleZ = (float)( -dy ) * MOUSE_SENSITIVITY;

	static float lastRotX = 0.0f; 
 	lastRotX = orientv.x; // We store off the currentRotX and will use it in when the angle is capped
	
	// Here we keep track of the current rotation (for up and down) so that
	// we can restrict the camera from doing a full 360 loop.
	orientv.x += angleZ;
 
	// If the current rotation (in radians) is greater than 1.0, we want to cap it.
	if(orientv.x > 1.0f)     
	{
		orientv.x = 1.0f;
		
		// Rotate by remaining angle if there is any
		if(lastRotX != 1.0f) 
		{
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			Vec3f vAxis = Cross(view - pos, up);
			vAxis = Normalize(vAxis);
				
			// rotate the camera by the remaining angle (1.0f - lastRotX)
			rotateview( 1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
	else if(orientv.x < -1.0f)
	{
		orientv.x = -1.0f;
		
		// Rotate by the remaining angle if there is any
		if(lastRotX != -1.0f)
		{
			// To find the axis we need to rotate around for up and down
			// movements, we need to get a perpendicular vector from the
			// camera's view vector and up vector.  This will be the axis.
			// Before using the axis, it's a good idea to normalize it first.
			Vec3f vAxis = Cross(view - pos, up);
			vAxis = Normalize(vAxis);
			
			// rotate the camera by ( -1.0f - lastRotX)
			rotateview( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Otherwise, we can rotate the view around our position
	else 
	{	
		// To find the axis we need to rotate around for up and down
		// movements, we need to get a perpendicular vector from the
		// camera's view vector and up vector.  This will be the axis.
		// Before using the axis, it's a good idea to normalize it first.
		Vec3f vAxis = Cross(view - pos, up);
		vAxis = Normalize(vAxis);
	
		// Rotate around our perpendicular axis
		rotateview(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	// Always rotate the camera around the y-axis
	rotateview(angleY, 0, 1, 0);
	calcstrafe();
	calcyaw();
	//calcpitch();
}

void Camera::rotateabout(Vec3f center, float rad, float x, float y, float z)
{
	view = RotateAround(view, center, rad, x, y, z);
	pos = RotateAround(pos, center, rad, x, y, z);
	up = Rotate(up, rad, x, y, z);
	
	calcstrafe();
	calcyaw();
}

void Camera::grounded(ecbool ground)
{
	grounded = ground;

	if(grounded && vel.y < 0.0f)
		vel.y = 0.0f;
}

void Camera::limithvel(float vLimit)
{
	Vec3f hVel = Vec3f(vel.x, 0, vel.z);
	float vVel = MAG_VEC3F( hVel );

	if(vVel <= vLimit)
		return;

	hVel = hVel * vLimit / vVel;

	vel.x = hVel.x;
	vel.z = hVel.z;
}

void Camera::rotateview(float angle, float x, float y, float z)
{
	Vec3f vNewView;

	Vec3f vView = view - pos;		

	float costheta = (float)cos(angle);
	float sintheta = (float)sin(angle);

	vNewView.x  = (costheta + (1 - costheta) * x * x)		* vView.x;
	vNewView.x += ((1 - costheta) * x * y - z * sintheta)	* vView.y;
	vNewView.x += ((1 - costheta) * x * z + y * sintheta)	* vView.z;

	vNewView.y  = ((1 - costheta) * x * y + z * sintheta)	* vView.x;
	vNewView.y += (costheta + (1 - costheta) * y * y)		* vView.y;
	vNewView.y += ((1 - costheta) * y * z - x * sintheta)	* vView.z;

	vNewView.z  = ((1 - costheta) * x * z - y * sintheta)	* vView.x;
	vNewView.z += ((1 - costheta) * y * z + x * sintheta)	* vView.y;
	vNewView.z += (costheta + (1 - costheta) * z * z)		* vView.z;

	view = pos + vNewView;
	calcyaw();
	calcstrafe();
}

void Camera::lookat(Vec3f at)
{
	view = at;
	calcyaw();
	calcstrafe();
}

/*
void Camera::Strafe(float speed)
{
	vel.x += strafe.x * speed;
	vel.z += strafe.z * speed;
}

void Camera::Move(float speed)
{
	Vec3f v = view - pos;

	v = Normalize(v);

	vel.x += v.x * speed;
	vel.z += v.z * speed;
}
*/

void Camera::strafe(float speed)
{
	pos.x += strafe.x * speed;
	pos.z += strafe.z * speed;

	view.x += strafe.x * speed;
	view.z += strafe.z * speed;
}

void Camera::move(float speed)
{
	Vec3f v = view - pos;

	v = Normalize(v);

	pos.x += v.x * speed;
	pos.z += v.z * speed;
	view.x += v.x * speed;
	view.z += v.z * speed;
}

//ecbool g_debug2 = ecfalse;

void Camera::accelerate(float speed)
{
	Vec3f v = view - pos;

	v = Normalize(v);
	
	vel.x += v.x * speed;
	vel.z += v.z * speed;
}

void Camera::accelstrafe(float speed)
{
	Vec3f v = Normalize(strafe);
	
	vel.x += v.x * speed;
	vel.z += v.z * speed;
}

void Camera::accelrise(float speed)
{
	Vec3f v = Normalize(up);
	
	vel.y += v.y * speed;
}

void Camera::friction()
{
	vel.x *= INVFRICTION;
	vel.z *= INVFRICTION;
}

void Camera::friction2()
{
	vel.x *= INVFRICTION;
	vel.y *= INVFRICTION;
	vel.z *= INVFRICTION;
}

/*
void Camera::Rise(float speed)
{
	Vec3f v = up;

	v = Normalize(v);

	vel.y += v.y * speed;
}		
*/

void Camera::rise(float speed)
{
	Vec3f up = Normalize(up);

	pos.y += up.y * speed;
	view.y += up.y * speed; 
}	

void Camera::move(Vec3f delta)
{
	pos = pos + delta;
	view = view + delta;
}

void Camera::moveto(Vec3f newpos)
{
	Vec3f delta = newpos - pos;
	//Move(delta);
	view = view + delta;
	pos = newpos;
}

void Camera::stop()
{
	vel = Vec3f(0, 0, 0);
}

void Camera::calcstrafe() 
{
	Vec3f vCross = Cross(Normalize(view - pos), up);
	strafe = Normalize(vCross);
}

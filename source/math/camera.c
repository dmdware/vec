
#include "camera.h"
#include "3dmath.h"
#include "physics.h"

Camera g_cam;



void Cam_init(Camera *c)
{
	pos	= Vec3f(0.0, 0.0, 0.0);
	view	= Vec3f(0.0, -1.0, 1.0);
	up	= Vec3f(0.0, 1.0, 0.0);

	grounded = ecfalse;

	orientv.x = 0;
	orientv.y = 0;

	Cam_calcstrafe(c);
}

Vec3i Cam_up2(Camera *c)		
{
	return Normalize( Cross( strafe, view - pos ) );
}

void Cam_rotateview(Camera *c, int angle, int x, int y, int z)
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

void Cam_lookat(Camera *c, Vec3i at)
{
	view = at;
	calcyaw();
	calcstrafe();
}

void Cam_rotatebymouse(Camera *c, int dx, int dy)
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

Vec3i Cam_lookpos(Camera *c)
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

void Cam_grounded(Camera *c, ecbool ground)
{
	grounded = ground;

	if(grounded && vel.y < 0.0f)
		vel.y = 0.0f;
}

void Cam_rotateabout(Camera *c, Vec3i center, int rad, int x, int y, int z)
{
	view = RotateAround(view, center, rad, x, y, z);
	pos = RotateAround(pos, center, rad, x, y, z);
	up = Rotate(up, rad, x, y, z);
	
	calcstrafe();
	calcyaw();
}

void Cam_strafe(Camera *c, int speed)
{
	pos.x += strafe.x * speed;
	pos.z += strafe.z * speed;

	view.x += strafe.x * speed;
	view.z += strafe.z * speed;
}

void Cam_move(Camera *c, int speed)
{
	Vec3f v = view - pos;

	v = Normalize(v);

	pos.x += v.x * speed;
	pos.z += v.z * speed;
	view.x += v.x * speed;
	view.z += v.z * speed;
}

void Cam_accelerate(Camera *c, int speed)
{
	Vec3f v = view - pos;

	v = Normalize(v);
	
	vel.x += v.x * speed;
	vel.z += v.z * speed;
}

void Cam_accelstrafe(Camera *c, int speed)
{
	Vec3f v = Normalize(strafe);
	
	vel.x += v.x * speed;
	vel.z += v.z * speed;
}

void Cam_accelrise(Camera *c, int speed)
{
	Vec3f v = Normalize(up);
	
	vel.y += v.y * speed;
}

void Cam_rise(Camera *c, int speed)
{
	Vec3f up = Normalize(up);

	pos.y += up.y * speed;
	view.y += up.y * speed; 
}	

void Cam_move(Camera *c, Vec3i delta)
{
	pos = pos + delta;
	view = view + delta;
}

void Cam_moveto(Camera *c, Vec3i newpos)
{
	Vec3f delta = newpos - pos;
	//Move(delta);
	view = view + delta;
	pos = newpos;
}

void Cam_limithvel(Camera *c, int limit)
{
	Vec3f hVel = Vec3f(vel.x, 0, vel.z);
	float vVel = MAG_VEC3F( hVel );

	if(vVel <= vLimit)
		return;

	hVel = hVel * vLimit / vVel;

	vel.x = hVel.x;
	vel.z = hVel.z;
}

void Cam_stop(Camera *c)
{
	vel = Vec3f(0, 0, 0);
}

void Cam_calcstrafe(Camera *c)
{
	Vec3f vCross = Cross(Normalize(view - pos), up);
	strafe = Normalize(vCross);
}

void Cam_frameupd(Camera *c)
{
	pos = pos + vel;
	view = view + vel;
}

void Cam_friction(Camera *c)
{
	vel.x *= INVFRICTION;
	vel.z *= INVFRICTION;
}

void Cam_friction2(Camera *c)
{
	vel.x *= INVFRICTION;
	vel.y *= INVFRICTION;
	vel.z *= INVFRICTION;
}

int Cam_yaw(Camera *c)
{
	return orientv.y;
}

int Cam_pitch(Camera *c)
{
	return orientv.x;
}

/*

Created Date: 10 Mar 2020

*/

#include "precompiled.h"

#define Determinant(a, b, c) ((a)[2] * ((b)[0] * (c)[1] - (b)[1] * (c)[0]) + (a)[1] * ((b)[2] * (c)[0] - (b)[0] * (c)[2]) + (a)[0] * ((b)[1] * (c)[2] - (b)[2] * (c)[1]))

bool SolveLSE(Vector v0, Vector v1, Vector v2, Vector v3, float* x, float* y, float* z)
{
	float d = Determinant(v1, v2, v3);

	if (d == 0.0f)
		return false;

	if (x)
		*x = Determinant(v0, v2, v3) / d;

	if (y)
		*y = Determinant(v1, v0, v3) / d;

	if (z)
		*z = Determinant(v1, v2, v0) / d;

	return true;
}

bool GetPointBetweenLines(Vector& p, Vector a1, Vector m1, Vector a2, Vector m2)
{
	float x, z;

	Vector t1 = CrossProduct(m1, m2);
	Vector t2 = a2 - a1;

	if (!SolveLSE(t2, m1, t1, m2, &x, NULL, &z))
		return false;

	t1 = a1 + x * m1;
	t2 = a2 + (-z) * m2;
	p = (t1 + t2) / 2.0f;

	return true;
}

#define BernsteinPolynom20(u) ((1.0f - u) * (1.0f - u))
#define BernsteinPolynom21(u) (2.0f * u * (1.0f - u))
#define BernsteinPolynom22(u) (u * u)

CInterpolation::CInterpolation(void)
{
}

CInterpolation::~CInterpolation(void)
{
	m_SmoothStart = m_SmoothEnd = false;
}

void CInterpolation::SetViewAngles(Vector start, Vector end)
{
	m_StartAngle = start;
	m_EndAngle = end;

	NormalizeAngles(m_StartAngle);
	NormalizeAngles(m_EndAngle);
}

void CInterpolation::SetFOVs(float start, float end)
{
	m_StartFov = start;
	m_EndFov = end;
}

void CInterpolation::SetWaypoints(Vector* prev, Vector start, Vector end, Vector* next)
{
	m_StartPoint = start;
	m_EndPoint = end;

	Vector a, b, c, d;

	if (!prev && !next)
	{
		m_Center = (m_StartPoint + m_EndPoint) / 2.0f;
	}
	else if (!prev)
	{
		a = start - end;
		float dist = a.Length() / 2.0f;
		a = a.Normalize();
		b = *next - end;
		b = b.Normalize();
		c = a - b;
		c = c.Normalize();
		m_Center = end + c * dist;
	}
	else if (!next)
	{
		a = *prev - start;
		a = a.Normalize();
		b = end - start;
		float dist = b.Length() / 2.0f;
		b = b.Normalize();
		c = b - a;
		c = c.Normalize();
		m_Center = start + c * dist;
	}
	else
	{
		a = *prev - start;
		a = a.Normalize();
		b = end - start;
		b = b.Normalize();
		c = b - a;

		a = start - end;
		a = a.Normalize();
		b = *next - end;
		b = b.Normalize();
		d = a - b;

		GetPointBetweenLines(m_Center, start, c, end, d);
	}
}

void CInterpolation::Interpolate(float t, Vector& point, Vector& angle, float* fov)
{
	if (m_SmoothStart && m_SmoothEnd)
	{
		t = (1.0f - t) * (t * t) + t * (1.0f - ((t - 1.0f) * (t - 1.0f)));
	}
	else if (m_SmoothStart)
	{
		t = t * t;
	}
	else if (m_SmoothEnd)
	{
		t = t - 1.0f;
		t = -(t * t) + 1;
	}

	if (point)
		BezierInterpolatePoint(t, point);

	if (angle)
		InterpolateAngle(t, angle);

	if (fov)
		*fov = m_StartFov + (t * (m_EndFov - m_StartFov));
}

void CInterpolation::BezierInterpolatePoint(float t, Vector& point)
{
	point = m_StartPoint * BernsteinPolynom20(t);
	point = point + m_Center * BernsteinPolynom21(t);
	point = point + m_EndPoint * BernsteinPolynom22(t);
}

void CInterpolation::SetSmoothing(bool start, bool end)
{
	m_SmoothStart = start;
	m_SmoothEnd = end;
}

void CInterpolation::InterpolateAngle(float t, Vector& angle)
{
	int i;
	float ang1, ang2;
	float d;

	for (i = 0; i < 3; i++)
	{
		ang1 = m_StartAngle[i];
		ang2 = m_EndAngle[i];

		d = ang2 - ang1;

		if (d > 180)
		{
			d -= 360;
		}
		else if (d < -180)
		{
			d += 360;
		}

		angle[i] = ang1 + d * t;
	}

	NormalizeAngles(angle);
}

#include <windows.h>
#include "C_vector.h"

float DotProduct(Vector3f a,Vector3f b)
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}
void CrossVector( Vector3f a, Vector3f b, Vector3f r )
{
    r[0] = (a[1] * b[2]) - (a[2] * b[1]);
    r[1] = (a[2] * b[0]) - (a[0] * b[2]);
    r[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

//-----------------------------------------------------------------------------
// Desc: Converts a normalized axis and angle to a unit quaternion.
//-----------------------------------------------------------------------------
void QuaternionFromRotation( Vector4f quat,Vector3f v, float fTheta )
{
	FLOAT sinValue = (FLOAT)sin(fTheta/2);
    quat[0] = sinValue * v[0];
    quat[1] = sinValue * v[1];
    quat[2] = sinValue * v[2];
    quat[3] = (FLOAT)cos(fTheta/2);
}
//기준벡터는 {0,1,0}
void GetQuaternionFromVector(Vector4f quat,Vector3f vec)
{
	//축과 각도를 계산해볼까나...
	Vector3f b={0,1,0},cross,temp;
	float angle;

	Vector3fCopy(vec,temp);
	Normalize(temp);
	Vector3fCross(temp,b,cross);	//이것이 축.
	Normalize(cross);
	angle = (float)acos(Vector3fDot(temp,b));	//각도 구하고.

	QuaternionFromRotation(quat,cross,angle);
}
//활경우 기준벡터는 {0,0,-1}맥스에서 프론트의경우.
//피라든지 그런거는 {0,1,0}맥스에서 top의경우 
void GetQuaternionFromVector(Vector4f quat,Vector3f vec,Vector3f axis)
{
	//축과 각도를 계산해볼까나...
	Vector3f cross,temp;
	float angle;

	Vector3fCopy(vec,temp);
	Normalize(temp);
	Vector3fCross(temp,axis,cross);	//이것이 축.
	Normalize(cross);
	angle = (float)acos(Vector3fDot(temp,axis));	//각도 구하고.

	QuaternionFromRotation(quat,cross,angle);
}

void MatrixFromQuaternion( float mat[4][4], float x, float y, float z,
                                   float w )
{
    float xx = x*x; float yy = y*y; float zz = z*z;
    float xy = x*y; float xz = x*z; float yz = y*z;
    float wx = w*x; float wy = w*y; float wz = w*z;

    mat[0][0] = 1 - 2 * ( yy + zz );
    mat[0][1] =     2 * ( xy - wz );
    mat[0][2] =     2 * ( xz + wy );

    mat[1][0] =     2 * ( xy + wz );
    mat[1][1] = 1 - 2 * ( xx + zz );
    mat[1][2] =     2 * ( yz - wx );

    mat[2][0] =     2 * ( xz - wy );
    mat[2][1] =     2 * ( yz + wx );
    mat[2][2] = 1 - 2 * ( xx + yy );

    mat[0][3] = mat[1][3] = mat[2][3] = 0.0f;
    mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
}
#define g_PI	3.141592

void QuaternionSlerp( float& Qx, float& Qy, float& Qz, float& Qw,
                              float Ax, float Ay, float Az, float Aw,
                              float Bx, float By, float Bz, float Bw,
                              float fAlpha )
{
    float fScale1;
    float fScale2;

    // Compute dot product, aka cos(theta):
    float fCosTheta = Ax*Bx + Ay*By + Az*Bz + Aw*Bw;

    if( fCosTheta < 0.0f )
    {
        // Flip start quaternion
        Ax = -Ax; Ay = -Ay; Az = -Az; Aw = -Aw;
        fCosTheta = -fCosTheta;
    }

    if( fCosTheta + 1.0f > 0.05f )
    {
        // If the quaternions are close, use linear interploation
        if( 1.0f - fCosTheta < 0.05f )
        {
            fScale1 = 1.0f - fAlpha;
            fScale2 = fAlpha;
        }
        else // Otherwise, do spherical interpolation
        {
            float fTheta    = (float)acos( fCosTheta );
            float fSinTheta = (float)sin( fTheta );
            
            fScale1 = (float)sin( fTheta * (1.0f-fAlpha) ) / fSinTheta;
            fScale2 = (float)sin( fTheta * fAlpha ) / fSinTheta;
        }
    }
    else
    {
        Bx = -Ay;
        By =  Ax;
        Bz = -Aw;
        Bw =  Az;
        fScale1 = (float)sin( g_PI * (0.5f - fAlpha) );
        fScale2 = (float)sin( g_PI * fAlpha );
    }

    Qx = fScale1 * Ax + fScale2 * Bx;
    Qy = fScale1 * Ay + fScale2 * By;
    Qz = fScale1 * Az + fScale2 * Bz;
    Qw = fScale1 * Aw + fScale2 * Bw;
}


BOOL MatrixInvert( float get[4][4], float a[4][4] )
{
    if( fabs(a[3][3] - 1.0f) > .001f)
        return FALSE;
    if( fabs(a[0][3]) > .001f || fabs(a[1][3]) > .001f || fabs(a[2][3]) > .001f )
        return FALSE;

    float fDetInv = 1.0f / ( a[0][0] * ( a[1][1] * a[2][2] - a[1][2] * a[2][1] ) -
                             a[0][1] * ( a[1][0] * a[2][2] - a[1][2] * a[2][0] ) +
                             a[0][2] * ( a[1][0] * a[2][1] - a[1][1] * a[2][0] ) );

    get[0][0] =  fDetInv * ( a[1][1] * a[2][2] - a[1][2] * a[2][1] );
    get[0][1] = -fDetInv * ( a[0][1] * a[2][2] - a[0][2] * a[2][1] );
    get[0][2] =  fDetInv * ( a[0][1] * a[1][2] - a[0][2] * a[1][1] );
    get[0][3] = 0.0f;

    get[1][0] = -fDetInv * ( a[1][0] * a[2][2] - a[1][2] * a[2][0] );
    get[1][1] =  fDetInv * ( a[0][0] * a[2][2] - a[0][2] * a[2][0] );
    get[1][2] = -fDetInv * ( a[0][0] * a[1][2] - a[0][2] * a[1][0] );
    get[1][3] = 0.0f;

    get[2][0] =  fDetInv * ( a[1][0] * a[2][1] - a[1][1] * a[2][0] );
    get[2][1] = -fDetInv * ( a[0][0] * a[2][1] - a[0][1] * a[2][0] );
    get[2][2] =  fDetInv * ( a[0][0] * a[1][1] - a[0][1] * a[1][0] );
    get[2][3] = 0.0f;

    get[3][0] = -( a[3][0] * get[0][0] + a[3][1] * get[1][0] + a[3][2] * get[2][0] );
    get[3][1] = -( a[3][0] * get[0][1] + a[3][1] * get[1][1] + a[3][2] * get[2][1] );
    get[3][2] = -( a[3][0] * get[0][2] + a[3][1] * get[1][2] + a[3][2] * get[2][2] );
    get[3][3] = 1.0f;

    return TRUE;
}

void GetMatrixFrom3DSMAXMatrix( float mat[4][4] )
{
	float imsi;
	for(int k=0;k<4;k++)
	{
		imsi=mat[k][1];
		mat[k][1]=mat[k][2];
		mat[k][2]=imsi;
	}
}
/*
void MatrixMultiply( float get[4][4],float a[4][4], float b[4][4] )
{
    float* pA = (float*)&a[0][0];
    float* pB = (float*)&b[0][0];
    float  pM[16];

    memset( pM,0, sizeof(float)*4*4 );

    for( WORD i=0; i<4; i++ ) 
        for( WORD j=0; j<4; j++ ) 
            for( WORD k=0; k<4; k++ ) 
                pM[4*i+j] += pA[4*k+j] * pB[4*i+k];

    memcpy( get, pM, sizeof(float)*4*4 );
}
*/

float stpfResults[4][4];

void MatrixMultiply( float get[4][4],float pfM1[4][4], float pfM2[4][4])
//float *pfResults, const float *pfM1, const float *pfM2 )
{
	float *pfResults = &stpfResults[0][0];
    // assertions dont show up in release builds, so it incurs no performance loss.
//    _ASSERT( pfResults != pfM2 );        

    __asm
    {
        push        ebx
        push        edx

        mov         eax, pfM2
        mov         ebx, pfM1
        mov         edx, pfResults

        
        fld         dword ptr[ eax ]
        fld         dword ptr[ eax + 4 ]
        fld         dword ptr[ eax + 8 ]
        fld         dword ptr[ eax + 12 ]
        
        fld         dword ptr[ ebx ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 4 ]
        fmul        st(0), st(5)
        fld         dword ptr[ ebx + 16 ]         
        fld         dword ptr[ ebx + 20 ]
        fmul        st(0), st(6)
        faddp       st(2), st(0)
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 32 ]         
        fld         dword ptr[ ebx + 36 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 48 ]         
        fld         dword ptr[ ebx + 52 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fstp        dword ptr[ edx + 4 ]
        fstp        dword ptr[ edx ]


        fld         dword ptr[ ebx + 12 ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 8 ]
        fmulp       st(5), st(0)
        fld         dword ptr[ ebx + 24 ]
        fld         dword ptr[ ebx + 28 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmulp       st(4), st(0)
        fld         dword ptr[ ebx + 40 ]
        fld         dword ptr[ ebx + 44 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmulp       st(3), st(0)
        fld         dword ptr[ ebx + 56 ]
        fld         dword ptr[ ebx + 60 ]         
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fmulp       st(2), st(0)
        fstp        dword ptr[ edx + 12 ]
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        fstp        dword ptr[ edx + 8 ]





        fld         dword ptr[ eax + 16 ]
        fld         dword ptr[ eax + 20 ]
        fld         dword ptr[ eax + 24 ]
        fld         dword ptr[ eax + 28 ]
        
        fld         dword ptr[ ebx ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 4 ]
        fmul        st(0), st(5)
        fld         dword ptr[ ebx + 16 ]         
        fld         dword ptr[ ebx + 20 ]
        fmul        st(0), st(6)
        faddp       st(2), st(0)
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 32 ]         
        fld         dword ptr[ ebx + 36 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 48 ]         
        fld         dword ptr[ ebx + 52 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fstp        dword ptr[ edx + 20 ]
        fstp        dword ptr[ edx + 16 ]


        fld         dword ptr[ ebx + 12 ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 8 ]
        fmulp       st(5), st(0)
        fld         dword ptr[ ebx + 24 ]
        fld         dword ptr[ ebx + 28 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmulp       st(4), st(0)
        fld         dword ptr[ ebx + 40 ]
        fld         dword ptr[ ebx + 44 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmulp       st(3), st(0)
        fld         dword ptr[ ebx + 56 ]
        fld         dword ptr[ ebx + 60 ]         
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fmulp       st(2), st(0)
        fstp        dword ptr[ edx + 28 ]
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        fstp        dword ptr[ edx + 24 ]





        fld         dword ptr[ eax + 32 ]
        fld         dword ptr[ eax + 36 ]
        fld         dword ptr[ eax + 40 ]
        fld         dword ptr[ eax + 44 ]
        
        fld         dword ptr[ ebx ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 4 ]
        fmul        st(0), st(5)
        fld         dword ptr[ ebx + 16 ]         
        fld         dword ptr[ ebx + 20 ]
        fmul        st(0), st(6)
        faddp       st(2), st(0)
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 32 ]         
        fld         dword ptr[ ebx + 36 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 48 ]         
        fld         dword ptr[ ebx + 52 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fstp        dword ptr[ edx + 36 ]
        fstp        dword ptr[ edx + 32 ]


        fld         dword ptr[ ebx + 12 ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 8 ]
        fmulp       st(5), st(0)
        fld         dword ptr[ ebx + 24 ]
        fld         dword ptr[ ebx + 28 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmulp       st(4), st(0)
        fld         dword ptr[ ebx + 40 ]
        fld         dword ptr[ ebx + 44 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmulp       st(3), st(0)
        fld         dword ptr[ ebx + 56 ]
        fld         dword ptr[ ebx + 60 ]         
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fmulp       st(2), st(0)
        fstp        dword ptr[ edx + 44 ]
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        fstp        dword ptr[ edx + 40 ]





        fld         dword ptr[ eax + 48 ]
        fld         dword ptr[ eax + 52 ]
        fld         dword ptr[ eax + 56 ]
        fld         dword ptr[ eax + 60 ]
        
        fld         dword ptr[ ebx ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 4 ]
        fmul        st(0), st(5)
        fld         dword ptr[ ebx + 16 ]         
        fld         dword ptr[ ebx + 20 ]
        fmul        st(0), st(6)
        faddp       st(2), st(0)
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 32 ]         
        fld         dword ptr[ ebx + 36 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 48 ]         
        fld         dword ptr[ ebx + 52 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fstp        dword ptr[ edx + 52 ]
        fstp        dword ptr[ edx + 48 ]


        fld         dword ptr[ ebx + 12 ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 8 ]
        fmulp       st(5), st(0)
        fld         dword ptr[ ebx + 24 ]
        fld         dword ptr[ ebx + 28 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmulp       st(4), st(0)
        fld         dword ptr[ ebx + 40 ]
        fld         dword ptr[ ebx + 44 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmulp       st(3), st(0)
        fld         dword ptr[ ebx + 56 ]
        fld         dword ptr[ ebx + 60 ]         
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fmulp       st(2), st(0)
        fstp        dword ptr[ edx + 60 ]
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        fstp        dword ptr[ edx + 56 ]



        pop         edx
        pop         ebx
    }
    memcpy( get, stpfResults, sizeof(float)*4*4 );
}


void MatrixIdentity( float get[4][4] )
{
    memset( get,0, sizeof(float)*4*4 );
	get[0][0]=1;
	get[1][1]=1;
	get[2][2]=1;
	get[3][3]=1;
}

void MatrixScale( float get_mat[4][4],float xscale,float yscale,float zscale )
{
	get_mat[0][0] = xscale; get_mat[0][1] = 0; get_mat[0][2] = 0; get_mat[0][3] = 0;
	get_mat[1][0] = 0; get_mat[1][1] = yscale; get_mat[1][2] = 0; get_mat[1][3] = 0;
	get_mat[2][0] = 0; get_mat[2][1] = 0; get_mat[2][2] = zscale; get_mat[2][3] = 0;
    get_mat[3][0] = 0; get_mat[3][1] = 0; get_mat[3][2] = 0;  get_mat[3][3] = 1.0f;
}

void MatrixCopy( float get[4][4], float source[4][4])
{
    memcpy( &get[0][0], &source[0][0], sizeof(float)*4*4 );
}

void MatrixRotate(float get_mat[4][4],float x_rot,float y_rot,float z_rot)//단위는 360도
{
    float mat1[4][4];
    float Ymat[4][4];
    float Xmat[4][4];
    float Zmat[4][4];
    double fax,fay,faz;

    fax=2.0*g_PI*x_rot/360.0f;
    fay=2.0*g_PI*y_rot/360.0f;
    faz=2.0*g_PI*z_rot/360.0f;

    Ymat[0][0]=(float)cos(fay); Ymat[0][1]=0;   Ymat[0][2]=(float)-sin(fay); Ymat[0][3]=0;
    Ymat[1][0]=0;       Ymat[1][1]=1.0f;   Ymat[1][2]=0;        Ymat[1][3]=0;
    Ymat[2][0]=(float)sin(fay); Ymat[2][1]=0;   Ymat[2][2]=(float)cos(fay);  Ymat[2][3]=0;
    Ymat[3][0]=0;       Ymat[3][1]=0;   Ymat[3][2]=0;        Ymat[3][3]=1.0f;

    Xmat[0][0]=1.0f;    Xmat[0][1]=0;        Xmat[0][2]=0;       Xmat[0][3]=0;
    Xmat[1][0]=0;    Xmat[1][1]=(float)cos(fax);  Xmat[1][2]=(float)sin(fax); Xmat[1][3]=0;
    Xmat[2][0]=0;    Xmat[2][1]=(float)-sin(fax); Xmat[2][2]=(float)cos(fax); Xmat[2][3]=0;
    Xmat[3][0]=0;    Xmat[3][1]=0;        Xmat[3][2]=0;       Xmat[3][3]=1.0f;

    MatrixMultiply(mat1,Ymat,Xmat);

    Zmat[0][0]=(float)cos(faz);  Zmat[0][1]=(float)sin(faz);  Zmat[0][2]=0;    Zmat[0][3]=0;
    Zmat[1][0]=(float)-sin(faz); Zmat[1][1]=(float)cos(faz);  Zmat[1][2]=0;    Zmat[1][3]=0;
    Zmat[2][0]=0;        Zmat[2][1]=0;        Zmat[2][2]=1.0f;    Zmat[2][3]=0;
    Zmat[3][0]=0;        Zmat[3][1]=0;        Zmat[3][2]=0;    Zmat[3][3]=1.0f;

    MatrixMultiply(get_mat,Zmat,mat1);
}
void MatrixRotateX(float get_mat[4][4],float x_rot)
{
    double fax;

    fax=2.0*g_PI*x_rot/360.0f;
    get_mat[0][0]=1.0f;    get_mat[0][1]=0;        get_mat[0][2]=0;       get_mat[0][3]=0;
    get_mat[1][0]=0;    get_mat[1][1]=(float)cos(fax);  get_mat[1][2]=(float)sin(fax); get_mat[1][3]=0;
    get_mat[2][0]=0;    get_mat[2][1]=(float)-sin(fax); get_mat[2][2]=(float)cos(fax); get_mat[2][3]=0;
    get_mat[3][0]=0;    get_mat[3][1]=0;        get_mat[3][2]=0;       get_mat[3][3]=1.0f;
}
void MatrixRotateY(float get_mat[4][4],float y_rot)
{
    double fay=2.0*g_PI*y_rot/360.0f;

    get_mat[0][0]=(float)cos(fay); get_mat[0][1]=0;   get_mat[0][2]=(float)-sin(fay); get_mat[0][3]=0;
    get_mat[1][0]=0;       get_mat[1][1]=1.0f;   get_mat[1][2]=0;        get_mat[1][3]=0;
    get_mat[2][0]=(float)sin(fay); get_mat[2][1]=0;   get_mat[2][2]=(float)cos(fay);  get_mat[2][3]=0;
    get_mat[3][0]=0;       get_mat[3][1]=0;   get_mat[3][2]=0;        get_mat[3][3]=1.0f;
}
void MatrixRotateZ(float get_mat[4][4],float z_rot)
{
    double faz=2.0*g_PI*z_rot/360.0f;

    get_mat[0][0]=(float)cos(faz);  get_mat[0][1]=(float)sin(faz);  get_mat[0][2]=0;    get_mat[0][3]=0;
    get_mat[1][0]=(float)-sin(faz); get_mat[1][1]=(float)cos(faz);  get_mat[1][2]=0;    get_mat[1][3]=0;
    get_mat[2][0]=0;        get_mat[2][1]=0;        get_mat[2][2]=1.0f;    get_mat[2][3]=0;
    get_mat[3][0]=0;        get_mat[3][1]=0;        get_mat[3][2]=0;    get_mat[3][3]=1.0f;
}

void GetNormal(Vector4f get,Vector3f vv0,Vector3f vv1,Vector3f vv2)
{
//----------------------- 평면의 방정식 생성
	Vector3f v0,v1,v2;

	v0[0] = 0;	v0[1] = 0;	v0[2] = 0;
	v1[0] = vv1[0] - vv0[0];	v1[1] = vv1[1] - vv0[1];	v1[2] = vv1[2] - vv0[2];
	v2[0] = vv2[0] - vv0[0];	v2[1] = vv2[1] - vv0[1];	v2[2] = vv2[2] - vv0[2];

    get[0]=(v0[1]*(v1[2]-v2[2])+v1[1]*(v2[2]-v0[2])+v2[1]*(v0[2]-v1[2]));
    get[1]=(v0[2]*(v1[0]-v2[0])+v1[2]*(v2[0]-v0[0])+v2[2]*(v0[0]-v1[0]));
    get[2]=(v0[0]*(v1[1]-v2[1])+v1[0]*(v2[1]-v0[1])+v2[0]*(v0[1]-v1[1]));

	Normalize(get);	//정규화 

	get[3] = get[0]*vv0[0] +get[1]*vv0[1] + get[2]*vv0[2]; 
}

static float stClipHalfWidth,stClipHalfHeight,stXCenter,stYCenter,stYadd;
void SetTransformClipInfo(float clip_width,float clip_height, float x_center, float y_center )//아래함수들 호줄 이전에 거를 먼저해주자..
{
	stClipHalfWidth=clip_width/2;
	stClipHalfHeight=clip_height/2;
	stXCenter = x_center;
	stYCenter = y_center;
	stYadd = y_center-stClipHalfHeight;
}

int TransformVertex( float xyzw[4], float f_matrix[4][4],float pos[3])
{

	float wp = f_matrix[0][3]*pos[0] + f_matrix[1][3]*pos[1] + f_matrix[2][3]*pos[2] + f_matrix[3][3];
	if( wp < 5 )
		return -8;
	float xp = f_matrix[0][0]*pos[0] + f_matrix[1][0]*pos[1] + f_matrix[2][0]*pos[2] + f_matrix[3][0];
	float yp = f_matrix[0][1]*pos[0] + f_matrix[1][1]*pos[1] + f_matrix[2][1]*pos[2] + f_matrix[3][1];
	float zp = f_matrix[0][2]*pos[0] + f_matrix[1][2]*pos[1] + f_matrix[2][2]*pos[2] + f_matrix[3][2];

	xp/=wp; yp/=wp; zp/=wp;

	xyzw[0]  =  xp * stClipHalfWidth+stXCenter;
//	xyzw[1]  =  yp * clip_height+y_center;
	xyzw[1]  = ( 1.0f - yp ) * stClipHalfHeight+stYadd;//-y_center;
	xyzw[2]  = zp;
	xyzw[3] = 1/wp;

	if( xp < -1 || yp < -1 || xp > 1 || yp > 1 )
		return FALSE;

	return TRUE;
}
int TransformVertex( float xyzw[4], float f_matrix[4][4],float pos[3],float add[3])
{

	float f_pos[3];

	f_pos[0] = pos[0];
	f_pos[1] = pos[1];
	f_pos[2] = pos[2];

	float wp = f_matrix[0][3]*f_pos[0] + f_matrix[1][3]*f_pos[1] + f_matrix[2][3]*f_pos[2] + f_matrix[3][3];
	if( wp < 5 )
		return FALSE;
	float xp = f_matrix[0][0]*f_pos[0] + f_matrix[1][0]*f_pos[1] + f_matrix[2][0]*f_pos[2] + f_matrix[3][0]+add[0];
	float yp = f_matrix[0][1]*f_pos[0] + f_matrix[1][1]*f_pos[1] + f_matrix[2][1]*f_pos[2] + f_matrix[3][1]+add[1];
	float zp = f_matrix[0][2]*f_pos[0] + f_matrix[1][2]*f_pos[1] + f_matrix[2][2]*f_pos[2] + f_matrix[3][2]+add[2];

	xp/=wp; yp/=wp; zp/=wp;

	xyzw[0]  =  xp * stClipHalfWidth+stXCenter;
//	xyzw[1]  =  yp * clip_height+y_center;
	xyzw[1]  = ( 1.0f - yp ) * stClipHalfHeight+stYadd;//-y_center;
	xyzw[2]  = zp;
	xyzw[3] = 1/wp;

	return TRUE;
}
void Vector3fTransform( float get[3],float pos[3],float f_matrix[4][4])
{
	Vector3f temp;

	Vector3fCopy(pos,temp);
	get[0] = f_matrix[0][0]*temp[0] + f_matrix[1][0]*temp[1] + f_matrix[2][0]*temp[2] + f_matrix[3][0];
	get[1] = f_matrix[0][1]*temp[0] + f_matrix[1][1]*temp[1] + f_matrix[2][1]*temp[2] + f_matrix[3][1];
	get[2] = f_matrix[0][2]*temp[0] + f_matrix[1][2]*temp[1] + f_matrix[2][2]*temp[2] + f_matrix[3][2];
}




//#define MAXPOINT  10
//#define MAXDIVIDE 100
 
 
float CalcBi_n(int n, int i, double u)	//베지어 함수
{
	int    j;
	double Bin, nCi, Pn, Pi, Pni;

	if(n == 0 || n == 1)
		Pn  = 1;
	else
	{
		Pn  = (double)n;

		for(j = 1; j < n; j++)
			Pn = Pn * (double)(n - j);	
	}
	
	if(i == 0 || i == 1)
		Pi = 1;
	else
	{
		Pi = (double)i;

		for(j = 1; j < i; j++)
			Pi = Pi * (double)(i - j);
	}

	if((n-i) == 0 || (n-i) == 1)
		Pni = 1;
	else
	{
		Pni = (double)(n - i);

		for(j = 1; j < n-i; j++)
			Pni = Pni * (double)((n-i) - j);
	}

	nCi = Pn / (Pi * Pni);
	Bin = nCi * pow(u, i) * pow((1-u), n-i);

	return (float)Bin;
}
 
//get_pos,원하는 위치, pos 컨트롤포인트점들 ,컨트롤 포인트 수, 0 ~ 1 해당되는 값
void GetBezierPoint(Vector3f *get_pos,Vector3f *pos,DWORD ctrl_cnt,float alpha)// 0-1까지.
{
	DWORD i;
	DWORD calc_cnt = 4;	

	get_pos[0][0] = 0;
	get_pos[0][1] = 0;
	get_pos[0][2] = 0;
	for(i = 0; i < ctrl_cnt; i++)
	{
		float temp = CalcBi_n(ctrl_cnt-1, i, alpha);

		get_pos[0][0] += temp * pos[i][0];
		get_pos[0][1] += temp * pos[i][1];
		get_pos[0][2] += temp * pos[i][2];
	}
}

void CalcCubicCurve(Vector4f *get_abcd,int n, float *vptr)
{
	float gamma[1000];
	float delta[1000];
	float D[1000];
	int i;
	typedef struct{
		float r;
//		float no_use[2];
	}_X;
	_X *x=(_X *)vptr;

	gamma[0] = 1.0f/2.0f;
	for(i=1; i<n; i++)
	{
		gamma[i] = 1/(4-gamma[i-1]);
	}
	gamma[n] = 1/(2-gamma[n-1]);
	
	delta[0] = 3*(x[1].r-x[0].r)*gamma[0];
	for(i=1; i<n; i++)
	{
		delta[i] = (3*(x[i+1].r-x[i-1].r)-delta[i-1])*gamma[i];
	}
	delta[n] = (3*(x[n].r-x[n-1].r)-delta[n-1])*gamma[n];
	
	D[n] = delta[n];
	for(i=n-1; i>=0; i--)
	{
		D[i] = delta[i] - gamma[i]*D[i+1];
	}

	for(i=0; i<n; i++)
	{
		get_abcd[i][0] = (float)x[i].r;
		get_abcd[i][1] = (float)D[i];
		get_abcd[i][2] = (float)3*(x[i+1].r - x[i].r) - 2*D[i] - D[i+1];
		get_abcd[i][3] = (2.0f*(x[i].r - x[i+1].r) + D[i] + D[i+1]);
	}
}

float CalcEvalCubicCurve(Vector4f v,float u)
{
	return ((((v[3]*u) + v[2])*u + v[1])*u + v[0]);
}

/* 
void main()
 
{
         FILE   *fp;
         int    i, N, u;
         double x, y, z, temp;
         double X[MAXPOINT], Y[MAXPOINT], Z[MAXPOINT];
  
        for(i = 0; i < MAXPOINT; i++)
                 X[i] = Y[i] = Z[i] = 0;
 
        X[0] = 0;       Y[0] = 0;       Z[0] = 0;  
        X[1] = 0;       Y[1] = 4;       Z[1] = 0;  
        X[2] = 4;       Y[2] = 4;       Z[2] = 0;  
        X[3] = 4;       Y[3] = 0;       Z[3] = 0;  
 
        N = 3;
        fp = fopen("Result1.Dat", "w");
        for(u = 0; u <= MAXDIVIDE; u++)
        {
                x = 0;
                y = 0;
                z = 0;
                for(i = 0; i < N+1; i++)
                {
                        temp = CalcBi_n(N, i, (u * 0.01));
                        x = x + temp * X[i];
                        y = y + temp * Y[i];
                        z = z + temp * Z[i];
                } 
                fprintf(fp, "%lf \t %lf \t %lf \t %lf\n", (u * 0.01), x, y, z);
         }
         fclose(fp);
 
}
 
*/


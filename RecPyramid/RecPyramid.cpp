/********************************************************
*
*     Author: Tianhao Wang
*     Date: 2015-03-08
*     Desc: 绘制了一个简单的四棱锥，并尝试使用了HLSL。
*     W、S键控制摄影机距离，移动鼠标以改变观察视角。
*
*********************************************************/ 


#include "d3d9App.h"
#include "DirectInput.h"
#include "Vertex.h"
#include <crtdbg.h>

class RecPyramid : public d3d9App {
public:
	RecPyramid(HINSTANCE hInstance, std::string winTitle, D3DDEVTYPE devType, DWORD vertexProc);
	~RecPyramid();

	void DrawScene();           // 渲染场景
	void UpdateScene(float dt); // 更新绘制方式
	void SetViewMatrix();       // 设置观察矩阵
	void SetProjMatrix();       // 设置投影矩阵

	void BuildShaderFile();     // 设置顶点着色器
	void BuildRecPyramid();     // 设置四棱锥的顶点缓存和索引缓存
private:
	IDirect3DVertexBuffer9* m_vertexBuffer;
	IDirect3DIndexBuffer9 * m_indexBuffer;
	DWORD m_numVertics;
	DWORD m_numTriangles;

	ID3DXEffect           * m_shaderFile;
	D3DXHANDLE              m_hTech;
	D3DXHANDLE              m_hWVP;

	float m_CameraDist;      // 摄影机和原点的距离
	float m_CameraRotationY; // 摄影机和Y轴的夹角
	float m_CameraHeight;    // 摄影机的高度
	
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projMatrix;
};

// 主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int)
{
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	RecPyramid app(hInstance, "RecPyramid", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	g_pd3d9App = &app;

	DirectInput dxInput(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND,
						DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	g_pDxInput = &dxInput;

	return g_pd3d9App->MsgLoop();
}

// 类的初始化
RecPyramid::RecPyramid(HINSTANCE hInstance, std::string winTitle, D3DDEVTYPE devType, DWORD vertexProc)
: d3d9App(hInstance, winTitle, devType, vertexProc)
{

	m_CameraDist = 5.0f;
	m_CameraRotationY = 0.0f;
	m_CameraHeight = 5.0f;

	BuildShaderFile();
	BuildRecPyramid();

	SetProjMatrix(); // 投影矩阵只需设置一次
	InitCVertex();
}

// 清理接口
RecPyramid::~RecPyramid()
{
	ReleaseCOM(m_vertexBuffer);
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_shaderFile);
	DeleCVertex();
}

// 设置四棱锥的顶点缓存和索引缓存
void RecPyramid::BuildRecPyramid()
{
	HR(g_pd3dDevice->CreateVertexBuffer(5 * sizeof(CVertex),
										D3DUSAGE_WRITEONLY,
										0,
										D3DPOOL_MANAGED,
										&m_vertexBuffer,
										0));
	CVertex* p_vertex;
	HR(m_vertexBuffer->Lock(0, 0, (void**)&p_vertex, 0));

	p_vertex[0] = CVertex( 1.0f, -1.0f,  1.0f);
	p_vertex[1] = CVertex( 1.0f, -1.0f, -1.0f);
	p_vertex[2] = CVertex(-1.0f, -1.0f, -1.0f);
	p_vertex[3] = CVertex(-1.0f, -1.0f,  1.0f);
	p_vertex[4] = CVertex( 0.0f,  1.0f,  0.0f);

	HR(m_vertexBuffer->Unlock());

	HR(g_pd3dDevice->CreateIndexBuffer(18 * sizeof(WORD),
									   D3DUSAGE_WRITEONLY,
									   D3DFMT_INDEX16,
									   D3DPOOL_MANAGED,
									   &m_indexBuffer,
									   0));
	WORD* p_index;
	HR(m_indexBuffer->Lock(0, 0, (void**)&p_index, 0));

	// Direct3D认为顶点排列顺序为顺时针（观察坐标系中）的三角元是正面朝向
	// 顶点排列顺序为逆时针的三角形为背面朝向
	// Direct3D会遮蔽背面朝向的多边形，这称为背面消隐
	// 底面
	p_index[0]  = 0; p_index[1]  = 2; p_index[2]  = 1;
	p_index[3]  = 2; p_index[4]  = 0; p_index[5]  = 3;

	// 前、后、左、右
	p_index[6]  = 3; p_index[7]  = 0; p_index[8]  = 4;
	p_index[9]  = 1; p_index[10] = 2; p_index[11] = 4;
	p_index[12] = 0; p_index[13] = 1; p_index[14] = 4;
	p_index[15] = 2; p_index[16] = 3; p_index[17] = 4;

	HR(m_indexBuffer->Unlock());
}

// 设置顶点着色器
void RecPyramid::BuildShaderFile()
{
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(g_pd3dDevice,
								"transform.fx",
								0,
								0,
								D3DXSHADER_DEBUG,
								0,
								&m_shaderFile,
								&errors));
	if (errors)
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	m_hTech = m_shaderFile->GetTechniqueByName("TransformTech");
	m_hWVP  = m_shaderFile->GetParameterByName(0, "gWVP");
}

// 设置投影矩阵
void RecPyramid::SetProjMatrix()
{
	float width	 = (float)m_d3dPP.BackBufferWidth;
	float height = (float)m_d3dPP.BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&m_projMatrix,
							   D3DX_PI * 0.25f,
							   width / height,
							   1.0f,
							   5000.0f);
}

// 设置观察矩阵
void RecPyramid::SetViewMatrix()
{
	// 我本来试过把Camera相关的三个类成员替换为x、y、z坐标
	// 然后发现，计算Camera距离时需要进行三次平方，然后再开根号
	// 于是沿用了FRANK D.LUNA的做法
	float x = m_CameraDist * cosf(m_CameraRotationY);
	float z = m_CameraDist * sinf(m_CameraRotationY);
	D3DXVECTOR3 pos(x, m_CameraHeight, z); // 摄影机位置
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);  // 摄影机观察点
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);      // 世界朝向（指向上方）
	D3DXMatrixLookAtLH(&m_viewMatrix, &pos, &target, &up);
}

// 更新绘制方式，这次只更新摄影机位置
void RecPyramid::UpdateScene(float dt)
{
	g_pDxInput->GetCurState();

	if (g_pDxInput->KeyPress(DIK_W))
		m_CameraDist -= 10.0f * dt;
	if (g_pDxInput->KeyPress(DIK_S))
		m_CameraDist += 10.0f * dt;

	// 使摄影机不会进入几何体内部
	if (m_CameraDist <= 4.0f)
		m_CameraDist = 4.0f;

	m_CameraHeight += g_pDxInput->MouseY() / 25.0f;
	m_CameraRotationY += g_pDxInput->MouseX() / 50.0f;

	// 将角度限制在360度范围内
	if (fabsf(m_CameraRotationY) >= 2.0f * D3DX_PI) 
		m_CameraRotationY = 0.0f;

	// 每次更新摄影机位置之后，都需要重新设置投影矩阵
	SetViewMatrix();
}

// 开始渲染
void RecPyramid::DrawScene()
{
	HR(g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						   D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0));
	HR(g_pd3dDevice->BeginScene());
	
	// 将顶点缓存绑定到流
	HR(g_pd3dDevice->SetStreamSource(0, m_vertexBuffer, 0, sizeof(CVertex)));
	HR(g_pd3dDevice->SetIndices(m_indexBuffer));
	// 声明顶点的组成结构
	HR(g_pd3dDevice->SetVertexDeclaration(CVertex::m_verDecl));

	HR(m_shaderFile->SetTechnique(m_hTech));
	HR(m_shaderFile->SetMatrix(m_hWVP, &(m_viewMatrix * m_projMatrix)));

	UINT numPasses = 0;
	HR(m_shaderFile->Begin(&numPasses, 0));
	for (UINT i = 0; i < numPasses; ++i) {
		HR(m_shaderFile->BeginPass(i));
		HR(g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 5, 0, 6));
		HR(m_shaderFile->EndPass());
	}
	HR(m_shaderFile->End());
	HR(g_pd3dDevice->EndScene());
	// 提交后台缓存
	HR(g_pd3dDevice->Present(0, 0, 0, 0));
}
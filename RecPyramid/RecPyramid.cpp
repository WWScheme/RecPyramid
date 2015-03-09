/********************************************************
*
*     Author: Tianhao Wang
*     Date: 2015-03-08
*     Desc: ������һ���򵥵�����׶��������ʹ����HLSL��
*     W��S��������Ӱ�����룬�ƶ�����Ըı�۲��ӽǡ�
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

	void DrawScene();           // ��Ⱦ����
	void UpdateScene(float dt); // ���»��Ʒ�ʽ
	void SetViewMatrix();       // ���ù۲����
	void SetProjMatrix();       // ����ͶӰ����

	void BuildShaderFile();     // ���ö�����ɫ��
	void BuildRecPyramid();     // ��������׶�Ķ��㻺�����������
private:
	IDirect3DVertexBuffer9* m_vertexBuffer;
	IDirect3DIndexBuffer9 * m_indexBuffer;
	DWORD m_numVertics;
	DWORD m_numTriangles;

	ID3DXEffect           * m_shaderFile;
	D3DXHANDLE              m_hTech;
	D3DXHANDLE              m_hWVP;

	float m_CameraDist;      // ��Ӱ����ԭ��ľ���
	float m_CameraRotationY; // ��Ӱ����Y��ļн�
	float m_CameraHeight;    // ��Ӱ���ĸ߶�
	
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projMatrix;
};

// ������
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

// ��ĳ�ʼ��
RecPyramid::RecPyramid(HINSTANCE hInstance, std::string winTitle, D3DDEVTYPE devType, DWORD vertexProc)
: d3d9App(hInstance, winTitle, devType, vertexProc)
{

	m_CameraDist = 5.0f;
	m_CameraRotationY = 0.0f;
	m_CameraHeight = 5.0f;

	BuildShaderFile();
	BuildRecPyramid();

	SetProjMatrix(); // ͶӰ����ֻ������һ��
	InitCVertex();
}

// ����ӿ�
RecPyramid::~RecPyramid()
{
	ReleaseCOM(m_vertexBuffer);
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_shaderFile);
	DeleCVertex();
}

// ��������׶�Ķ��㻺�����������
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

	// Direct3D��Ϊ��������˳��Ϊ˳ʱ�루�۲�����ϵ�У�������Ԫ�����泯��
	// ��������˳��Ϊ��ʱ���������Ϊ���泯��
	// Direct3D���ڱα��泯��Ķ���Σ����Ϊ��������
	// ����
	p_index[0]  = 0; p_index[1]  = 2; p_index[2]  = 1;
	p_index[3]  = 2; p_index[4]  = 0; p_index[5]  = 3;

	// ǰ��������
	p_index[6]  = 3; p_index[7]  = 0; p_index[8]  = 4;
	p_index[9]  = 1; p_index[10] = 2; p_index[11] = 4;
	p_index[12] = 0; p_index[13] = 1; p_index[14] = 4;
	p_index[15] = 2; p_index[16] = 3; p_index[17] = 4;

	HR(m_indexBuffer->Unlock());
}

// ���ö�����ɫ��
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

// ����ͶӰ����
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

// ���ù۲����
void RecPyramid::SetViewMatrix()
{
	// �ұ����Թ���Camera��ص��������Ա�滻Ϊx��y��z����
	// Ȼ���֣�����Camera����ʱ��Ҫ��������ƽ����Ȼ���ٿ�����
	// ����������FRANK D.LUNA������
	float x = m_CameraDist * cosf(m_CameraRotationY);
	float z = m_CameraDist * sinf(m_CameraRotationY);
	D3DXVECTOR3 pos(x, m_CameraHeight, z); // ��Ӱ��λ��
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);  // ��Ӱ���۲��
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);      // ���糯��ָ���Ϸ���
	D3DXMatrixLookAtLH(&m_viewMatrix, &pos, &target, &up);
}

// ���»��Ʒ�ʽ�����ֻ������Ӱ��λ��
void RecPyramid::UpdateScene(float dt)
{
	g_pDxInput->GetCurState();

	if (g_pDxInput->KeyPress(DIK_W))
		m_CameraDist -= 10.0f * dt;
	if (g_pDxInput->KeyPress(DIK_S))
		m_CameraDist += 10.0f * dt;

	// ʹ��Ӱ��������뼸�����ڲ�
	if (m_CameraDist <= 4.0f)
		m_CameraDist = 4.0f;

	m_CameraHeight += g_pDxInput->MouseY() / 25.0f;
	m_CameraRotationY += g_pDxInput->MouseX() / 50.0f;

	// ���Ƕ�������360�ȷ�Χ��
	if (fabsf(m_CameraRotationY) >= 2.0f * D3DX_PI) 
		m_CameraRotationY = 0.0f;

	// ÿ�θ�����Ӱ��λ��֮�󣬶���Ҫ��������ͶӰ����
	SetViewMatrix();
}

// ��ʼ��Ⱦ
void RecPyramid::DrawScene()
{
	HR(g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						   D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0));
	HR(g_pd3dDevice->BeginScene());
	
	// �����㻺��󶨵���
	HR(g_pd3dDevice->SetStreamSource(0, m_vertexBuffer, 0, sizeof(CVertex)));
	HR(g_pd3dDevice->SetIndices(m_indexBuffer));
	// �����������ɽṹ
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
	// �ύ��̨����
	HR(g_pd3dDevice->Present(0, 0, 0, 0));
}
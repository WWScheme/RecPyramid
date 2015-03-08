/*****************************************
*
*     ����ṹ��Ķ��塢��ʼ�������
*     �����հ�FRANK D.LUNA������ڶ���
*
******************************************/
#ifndef _VERTEX_H
#define _VERTEX_H

#include "d3dUtil.h"

struct CVertex {
    CVertex(float x, float y, float z): m_verPos(x, y, z) { }

    D3DXVECTOR3 m_verPos;
    static IDirect3DVertexDeclaration9* m_verDecl;
};
// CVertex�͵Ķ�����ܻ�������������̬��Աm_verDeclֻ��һ��
// ��m_verDecl��Ա��ʼ��֮��
// ����������Ϊ�ýṹ�����͵Ķ�����ṹҲ�͹̶���
IDirect3DVertexDeclaration9* CVertex::m_verDecl = 0;

void InitCVertex()
{
    D3DVERTEXELEMENT9 a_VertexElements[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,
        D3DDECLUSAGE_POSITION, 0},
        D3DDECL_END()
    };
    HR(g_pd3dDevice->CreateVertexDeclaration(a_VertexElements,
                                             &CVertex::m_verDecl));
}

void DeleCVertex()
{
    ReleaseCOM(CVertex::m_verDecl);
}

#endif
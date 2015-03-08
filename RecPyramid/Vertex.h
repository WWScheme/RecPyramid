/*****************************************
*
*     顶点结构体的定义、初始化和清除
*     基本照搬FRANK D.LUNA的龙书第二版
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
// CVertex型的对象可能会有许多个，但静态成员m_verDecl只有一个
// 当m_verDecl成员初始化之后
// 对于所有身为该结构体类型的对象，其结构也就固定了
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
#pragma once

#include <windows.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <DirectXMath.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imguizmo/ImGuizmo.h"

struct dxshared 
{
public:
	static ID3D11Device* m_pDevice;
	static ID3D11DeviceContext* m_pImmediateContext;

	static UINT m_renderWidth;
	static UINT m_renderHeight;

	static DirectX::XMMATRIX mView;
	static DirectX::XMMATRIX mProjection;

	static DirectX::XMFLOAT4 ambientLightColour;

	static bool drawBoundingBoxes;

	static ImGuizmo::OPERATION mCurrentGizmoOperation;
	static ImGuizmo::MODE mCurrentGizmoMode;
};
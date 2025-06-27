#pragma once
#include "OnlyForTestScene.h"
#include "Camera2D.h"
#include <list>
#include <vector>
#include <wrl/client.h>
#include <d2d1_1.h>

class Object;
class TransformPracticeScene : public OnlyForTestScene
{
public:
    TransformPracticeScene() = default;
    virtual ~TransformPracticeScene();
    
    void SetUp(HWND hWnd) override;

    void Tick(float deltaTime) override;

    void OnResize(int width, int height) override;

private:

    void ProcessKeyboardEvents();
    void OnMouseLButtonUp(D2D1_POINT_2F pos) override;
    void OnMouseRButtonUp(D2D1_POINT_2F point) override;

    std::vector<Object*> m_Objects;
   
    UnityCamera m_UnityCamera;

    ComPtr<ID2D1Bitmap1> bitmapSun;
    ComPtr<ID2D1Bitmap1> bitmapMercury;
    ComPtr<ID2D1Bitmap1> bitmapVenus;
    ComPtr<ID2D1Bitmap1> bitmapEarth;
    ComPtr<ID2D1Bitmap1> bitmapMoon;
    ComPtr<ID2D1Bitmap1> bitmapMars;
    ComPtr<ID2D1Bitmap1> bitmapJupiter;
    ComPtr<ID2D1Bitmap1> bitmapSaturn;
    ComPtr<ID2D1Bitmap1> bitmapUranus;
    ComPtr<ID2D1Bitmap1> bitmapNeptune;

    float m_width, m_height;

    MAT3X2F finalTM;
    MAT3X2F renderTM; // 카메라 위치 렌더링 매트릭스
    MAT3X2F cameraTM;
};


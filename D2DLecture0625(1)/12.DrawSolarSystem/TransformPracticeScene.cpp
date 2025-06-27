#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "TransformPracticeScene.h"

using TestRenderer = myspace::D2DRenderer;
using Vec2 = MYHelper::Vector2F;

class Object
{
    Object() = delete;
    Object(const Object&) = delete;
    void operator=(const Object&) = delete;

public:
    Object(ComPtr<ID2D1Bitmap1>& bitmap)
    {
        m_BitmapPtr = bitmap;

        m_renderTM = MYTM::MakeRenderMatrix(true);

        D2D1_SIZE_F size = { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top };

        //m_transform.SetPivotPreset(D2DTM::PivotPreset::TopLeft, size);
        //m_transform.SetPivotPreset(D2DTM::PivotPreset::BottomRight, size);
        m_transform.SetPivotPreset(D2DTM::PivotPreset::Center, size);
        m_degree = 36.f;
    }

    ~Object() = default;

    void Update(float deltaTime)
    {
        if (m_isSelfRotation)
        {
            m_transform.Rotate(deltaTime * m_degree); // 자기 회전
        }
    }

    void Draw(TestRenderer& testRender, D2D1::Matrix3x2F viewTM)
    {
        static  D2D1_RECT_F s_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        testRender.SetTransform(finalTM);

        if (m_isShow)
        {
            D2D1_RECT_F dest = D2D1::RectF(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom);
            testRender.DrawBitmap(m_BitmapPtr.Get(), dest);
        }      
    }

    void SetPosition(const Vec2& pos)
    {
        m_transform.SetPosition(pos);
    }

    void Move(const Vec2& offset)
    {
        m_transform.Translate(offset);
    }

    void Rotate(float angle)
    {
        m_transform.Rotate(angle);
    }

    void ToggleSelfRotation()
    {
        m_isSelfRotation = !m_isSelfRotation;
    }

    void ToggleShow()
    {
        m_isShow = !m_isShow;
    }

    D2DTM::Transform* GetTransform()
    {
        return &m_transform;
    }

    void SetParent(Object* parent)
    {
        assert(parent != nullptr);

        if (nullptr != m_transform.GetParent())
        {
            // 이미 부모가 있다면 부모 관계를 해제합니다.
            m_transform.DetachFromParent();
        }

        m_transform.SetParent(parent->GetTransform());
    }

    void DetachFromParent()
    {
        m_transform.DetachFromParent();
    }

    void SetRotation(float newDegree)
    {
        m_degree = newDegree;
    }

private:
    D2DTM::Transform m_transform;

    MAT3X2F m_renderTM; // 렌더링 변환 행렬

    D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

    bool m_isSelfRotation = false; // 자기 회전 여부

    bool m_isShow = true; // 이미지 렌더 여부

    float m_degree;

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;

};

TransformPracticeScene::~TransformPracticeScene()
{
    for (auto& object : m_Objects)
    {
        delete object;
    }
}

void TransformPracticeScene::SetUp(HWND hWnd)
{
    constexpr int defaultGameObjectCount = 100;

    m_Objects.reserve(defaultGameObjectCount);

    m_hWnd = hWnd;

    SetWindowText(m_hWnd, 
    L"가상의 태양계를 만들어 주세요. 물리 법칙은 무시 합니다. ^^;;");

    std::cout << "태양은 자전을 해야 합니다." << std::endl;
    std::cout << "행성들은 자전을 하며 동시에 태영의 자전에 영향을 받아 공전하는 것처럼 보입니다."<< std::endl;
    std::cout << "달은 자전을 하면서 동시에 지구의 자전에 영향을 받아 공전하는 것처럼 보입니다." << std::endl;
    std::cout << "회전 속도는 자유롭게 설정하세요." << std::endl;

    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Sun.png", *bitmapSun.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Mercury.png", *bitmapMercury.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Venus.png", *bitmapVenus.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Earth.png", *bitmapEarth.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Moon.png", *bitmapMoon.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Mars.png", *bitmapMars.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Jupiter.png", *bitmapJupiter.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Saturn.png", *bitmapSaturn.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Uranus.png", *bitmapUranus.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Neptune.png", *bitmapNeptune.GetAddressOf());
   
    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        m_width = static_cast<float>(rc.right - rc.left);
        m_height = static_cast<float>(rc.bottom - rc.top);

        m_UnityCamera.SetScreenSize(m_width, m_height);
    }

    Object* tmp = new Object(bitmapSun);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 0, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapSun); // 안보이는 태양(1) for 금성
    tmp->ToggleSelfRotation();
    tmp->ToggleShow();
    tmp->GetTransform()->SetPosition({ 0, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->SetRotation(45.f);
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapSun); // 안보이는 태양(2) for 지구
    tmp->ToggleSelfRotation();
    tmp->ToggleShow();
    tmp->GetTransform()->SetPosition({ 0, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->SetRotation(25.f);
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapSun); // 안보이는 태양(3) for 목성
    tmp->ToggleSelfRotation();
    tmp->ToggleShow();
    tmp->GetTransform()->SetPosition({ 0, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->SetRotation(30.f);
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapSun); // 안보이는 태양(4) for 토성
    tmp->ToggleSelfRotation();
    tmp->ToggleShow();
    tmp->GetTransform()->SetPosition({ 0, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->SetRotation(50.f);
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapSun); // 안보이는 태양(5) for 천왕성
    tmp->ToggleSelfRotation();
    tmp->ToggleShow();
    tmp->GetTransform()->SetPosition({ 0, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->SetRotation(75.f);
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapVenus);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 80, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->GetTransform()->SetParent(m_Objects[0]->GetTransform());
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapMercury);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 150, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->GetTransform()->SetParent(m_Objects[1]->GetTransform());
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapEarth);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 240, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->GetTransform()->SetParent(m_Objects[2]->GetTransform());
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapMoon);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 300, 0 });
    tmp->GetTransform()->SetScale({ 0.3f, 0.3f });
    tmp->GetTransform()->SetParent(m_Objects[8]->GetTransform());
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapMars);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 380, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->GetTransform()->SetParent(m_Objects[0]->GetTransform());
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapJupiter);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 460, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->GetTransform()->SetParent(m_Objects[3]->GetTransform());
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapSaturn);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 540, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->GetTransform()->SetParent(m_Objects[4]->GetTransform());
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapUranus);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 620, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->GetTransform()->SetParent(m_Objects[5]->GetTransform());
    m_Objects.push_back(tmp);

    tmp = new Object(bitmapNeptune);
    tmp->ToggleSelfRotation();
    tmp->GetTransform()->SetPosition({ 700, 0 });
    tmp->GetTransform()->SetScale({ 0.5f, 0.5f });
    tmp->GetTransform()->SetParent(m_Objects[0]->GetTransform());
    m_Objects.push_back(tmp);

}

void TransformPracticeScene::Tick(float deltaTime)
{
    ProcessKeyboardEvents();
    ProcessMouseEvents();

    renderTM = MYTM::MakeRenderMatrix(true); // 카메라 위치 렌더링 매트릭스
    cameraTM = m_UnityCamera.GetViewMatrix();
    finalTM = renderTM * cameraTM;

    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();
    globalRenderer.SetTransform(finalTM);

    globalRenderer.RenderBegin();

    for (auto i : m_Objects)
    {
        i->Update(deltaTime);
    }

    for (auto i : m_Objects)
    {
        i->Draw(globalRenderer, cameraTM);
    }

    globalRenderer.RenderEnd();
}

void TransformPracticeScene::OnResize(int width, int height)
{ 
}

void TransformPracticeScene::ProcessKeyboardEvents()
{
    // 카메라 이동 처리, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17부터는 structured binding을 사용하여 더 간결하게 표현할 수 있습니다.
    for (auto& [vk, dir] : kCameraMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
            m_UnityCamera.Move(dir.x, dir.y);
        }
    }
}

void TransformPracticeScene::OnMouseLButtonUp(D2D1_POINT_2F pos)
{
    float differ = 0.5f;
    m_UnityCamera.SetZoom(differ);
}

void TransformPracticeScene::OnMouseRButtonUp(D2D1_POINT_2F point)
{
    float differ = -0.3f;
    m_UnityCamera.SetZoom(differ);
}

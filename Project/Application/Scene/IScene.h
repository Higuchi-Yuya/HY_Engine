#pragma once
class IScene
{
public:
	virtual ~IScene() {}
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void ImguiUpdate() = 0;
	virtual void Draw3D() = 0;
	virtual void Draw2DFront() = 0;
	virtual void DrawBloomObject() = 0;
	virtual void DrawHighLumiObj() = 0;
	virtual void Reset() = 0;
	virtual bool GetIsSceneFinsh() = 0;
	virtual bool GetIsGameClear() = 0;
};


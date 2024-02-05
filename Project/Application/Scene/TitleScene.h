#pragma once
#include "Sprite.h"
#include "Object3d.h"
#include "LightGroup.h"
#include "TextureManager.h"
#include "GameCamera.h"
#include "GameCollider.h"
#include "IScene.h"

class TitleScene:public IScene
{
public:
	enum ObjsType {
		Normal,
		Doors,
		Ranterns,
		HiguLumiRanterns
	};
public:// メンバ関数

	~TitleScene();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 背面スプライトの描画処理
	/// </summary>
	void Draw2DBack();

	/// <summary>
	/// 3Dオブジェクトの描画処理
	/// </summary>
	void Draw3D()override;

	/// <summary>
	/// パーティクルの描画処理
	/// </summary>
	void DrawParticle();

	/// <summary>
	/// 前面スプライトの描画処理
	/// </summary>
	void Draw2DFront()override;

	/// <summary>
	/// ブルームを掛けるオブジェクトの描画
	/// </summary>
	void DrawBloomObject()override;

	/// <summary>
	/// 高輝度抽出するオブジェクトの描画
	/// </summary>
	void DrawHighLumiObj()override;

	/// <summary>
	/// リセット関数
	/// </summary>
	void Reset()override;


	void ImguiUpdate()override;
	bool GetIsGameClear()override;

public:// 静的メンバ関数

public:// セッター

	/// <summary>
	/// ゲームカメラを設定
	/// </summary>
	/// <param name="gameCamera"></param>
	void SetGameCamera(GameCamera* gameCamera) { gameCamera_ = gameCamera; }

	/// <summary>
	/// オブジェクト配列を設定
	/// </summary>
	/// <param name="objs">オブジェクトの配列</param>
	/// <param name="objType">配列のタイプ</param>
	void SetObjs(std::vector<Object3d*> objs,ObjsType objType);

	/// <summary>
	/// ライトグループを設定
	/// </summary>
	/// <param name="lightGroup">ライトグループ</param>
	void SetLightGroup(LightGroup* lightGroup) { lightGroup_ = lightGroup; }

	/// <summary>
	/// ポイントライトの情報を設定
	/// </summary>
	/// <param name="pointLightsInfo">ポイントライトの情報が格納されている配列</param>
	void SetPointInfo(std::vector<WorldTransform*> pointLightsInfo) { pointLightsInfo_ = pointLightsInfo; }

public:// ゲッター

	/// <summary>
	/// シーン終了しているかを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsSceneFinsh()override { return IsSceneFinsh_; }

private:// プレイべーとメンバ関数

private:// メンバ変数

	// 借りてくるゲームカメラ
	GameCamera* gameCamera_ = nullptr;

	//	借りてくるライトグループ
	LightGroup* lightGroup_;
	// ポイントライト情報の配列
	std::vector<WorldTransform*> pointLightsInfo_;

	// テクスチャハンドル
	std::unique_ptr<Texture> titleFontTexHandle;
	
	std::unique_ptr<Texture> titleButtonTexHandle;

	// スプライト
	std::unique_ptr<Sprite> titleKariFont = nullptr;
	std::unique_ptr<Sprite> titleKariPressA = nullptr;

	// スプライトを消すフラグ
	bool IsTitleAlpha_ = false;
	float titleAlpha_ = 1;

	// オブジェクトの配列(借りてくる)
	std::vector<Object3d*> objects;

	// お墓のドアの配列(借りてくる)
	std::vector<Object3d*> latticeDoors_;

	// ランタンの配列(借りてくる)
	std::vector<Object3d*> ranterns_;

	// ランタンの高輝度用配列
	std::vector<Object3d*> highLumiRanterns_;

	// シーン終了フラグ
	bool IsSceneFinsh_;
};


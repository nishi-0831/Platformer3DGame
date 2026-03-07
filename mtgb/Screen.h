#pragma once
#include "ISystem.h"
#include "Vector2Int.h"
#include "Vector2F.h"
namespace mtgb
{
	class Screen : public ISystem
	{
	  public:
		Screen();
		~Screen();

		void Initialize() override;
		void Update() override;

		Vector2Int GetSize() const;
		Vector2F GetSizeF() const;
		/// <summary>
		/// 初期値を1としたときのスクリーンサイズの比率を返す
		/// </summary>
		/// <returns></returns>
		Vector2F GetSizeRatio() const;
		mtgb::Vector2Int GetInitialSize() const;
		/// <summary>
		/// <para> スクリーンのサイズを保持する</para>
		/// </summary>
		/// <param name="_width"></param>
		/// <param name="_height"></param>
		void SetSize(int _width, int _height);
		int GetFPS() const;

	  private:
		Vector2F sizeRatio_;
		Vector2Int initialSize_;
		Vector2Int size_;
		int fpsLimit_; // FPS制限(1フレームあたりの更新フレーム数)
	};
} // namespace mtgb

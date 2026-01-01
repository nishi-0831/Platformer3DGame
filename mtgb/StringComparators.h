#pragma once
#pragma once
#include <string>
#include <string_view>

namespace mtgb
{
	// MEMO:
	// std::stringをキーとするunorderer_mapなどに、std::string_viewで検索ができるようになる
	// std::unordered_map<std::string, std::type_index,TransparentStringHash,TransparentStringEq> nameToTypeIndex_;
	// ↑のように書く

	/// <summary>
	/// <para> 透過的な検索を有効にするハッシュ関数オブジェクト型 </para>
	/// REF: https://cpprefjp.github.io/reference/functional/hash.html
	/// </summary>
	struct TransparentStringHash
	{
		using is_transparent = void;
		size_t operator()(std::string_view _s) const noexcept
		{
			return std::hash<std::string_view>{}(_s);
		}
		size_t operator()(const std::string& _s) const noexcept
		{
			return std::hash<std::string_view>{}(_s);
		}
		size_t operator()(const char* _s) const noexcept
		{
			return std::hash<std::string_view>{}(_s);
		}
	};

	/// <summary>
	/// 透過的な検索を有効にする比較関数オブジェクト型
	/// </summary>
	struct TransparentStringEq
	{
		using is_transparent = void;
		bool operator()(std::string_view _a, std::string_view _b) const noexcept
		{
			return _a == _b;
		}
		bool operator()(const std::string& _a, std::string_view _b) const noexcept
		{
			return _a == _b;
		}
		bool operator()(std::string_view _a, const std::string& _b) const noexcept
		{
			return _a == _b;
		}
		bool operator()(const std::string& _a, const std::string& _b) const noexcept
		{
			return _a == _b;
		}
	};
} // namespace mtgb
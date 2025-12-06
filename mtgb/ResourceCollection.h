#pragma once
#include <map>
#include <vector>
#include <typeindex>

#include "ReleaseUtility.h"

namespace mtgb
{
    class WindowContextResource;
    /// <summary>
    /// リソースを一纏めにした構造体
    /// </summary>
    struct ResourceCollection
    {
        std::map<std::type_index, WindowContextResource*> resourceCollection_;//リソース群
        ResourceCollection() = default;
        ~ResourceCollection();

        WindowContextResource*& operator[](const std::type_index& key);
        const WindowContextResource* operator[](const std::type_index& key) const;

        void Insert(const std::type_index& _key,WindowContextResource* _pResource);
        void Swap(ResourceCollection& other);
        void Release();

        /// <summary>
        /// <para> 指定されたリソースの参照を返す </para>
        /// <para> 見つからない場合はassertでプログラム中止 </para>
        /// </summary>
        /// <typeparam name="Type">リソースの型</typeparam>
        /// <returns>指定されたリソースの参照</returns>
        template<typename Type>
        Type& Get()
        {
            auto itr = resourceCollection_.find(typeid(Type));
            assert(itr != resourceCollection_.end() && "指定された型のリソースが見つかりません");
            return *dynamic_cast<Type*>(itr->second);
        }

        // 非constバージョン
        auto begin() { return resourceCollection_.begin(); }
        auto end() { return resourceCollection_.end(); }

        // constバージョン
        auto begin() const { return resourceCollection_.begin(); }
        auto end() const { return resourceCollection_.end(); }

        
    };
}
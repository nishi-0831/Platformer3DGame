#pragma once
#include <typeindex>
#include <unordered_map>
#include <string>
#include <optional>
#include "ISystem.h"
#include "StringComparators.h"
namespace mtgb
{
	class GameObjectTypeRegistry : public ISystem
	{
	public:
        void Initialize() override;
        void Update() override;
        
        /// <summary>
        /// Œ^‚ğ“o˜^‚µA–¼‘O‚Æ•R•t‚¯‚é
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="_typeName"></param>
        template<typename T>
        void Register(std::string_view _typeName)
        {
            typeToName_.emplace(typeid(T), std::string(_typeName));
            nameToType_.emplace(std::string(_typeName), typeid(T));
        }

        /// <summary>
        /// Œ^î•ñ‚©‚çŒ^–¼‚ğæ“¾
        /// </summary>
        /// <param name="_typeIndex"></param>
        /// <returns></returns>
        std::string GetNameFromType(const std::type_index& _typeIndex) const
        {
            auto it = typeToName_.find(_typeIndex);
            if (it == typeToName_.end())
                return "Unknown";
            return it->second;
        }

        /// <summary>
        /// “o˜^‚³‚ê‚Ä‚¢‚é‘S‚Ä‚ÌŒ^‚Ì–¼‘O‚ğ•Ô‚·
        /// </summary>
        /// <returns></returns>
        std::vector<std::string> GetRegisteredNames() const;


        /// <summary>
        /// Œ^–¼‚©‚çŒ^î•ñ‚ğæ“¾
        /// </summary>
        /// <param name="_typeName"></param>
        /// <returns></returns>
        std::optional<std::type_index> GetTypeFromName(std::string_view _typeName) const
        {
            auto it = nameToType_.find(_typeName);
            if (it == nameToType_.end())
                return std::nullopt;
            return it->second;
        }

        /// <summary>
        /// Œ^‚ª“o˜^‚³‚ê‚Ä‚¢‚é‚©Šm”F
        /// </summary>
        /// <param name="_typeIndex"></param>
        /// <returns></returns>
        bool IsRegistered(const std::type_index& _typeIndex) const
        {
            return typeToName_.find(_typeIndex) != typeToName_.end();
        }
        bool IsRegistered(std::string_view _typeName);
	private:
        std::unordered_map<std::type_index, std::string> typeToName_;
        std::unordered_map<std::string, std::type_index,TransparentStringHash,TransparentStringEq> nameToType_;
	};
}

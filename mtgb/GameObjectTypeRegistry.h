#pragma once
#include <typeindex>
#include <unordered_map>
#include <string>
#include "ISystem.h"
namespace mtgb
{
	class GameObjectTypeRegistry : public ISystem
	{
	public:
        void Initialize() override;
        void Update() override;
        /// Œ^‚ğ“o˜^i–¼‘O‚ğ•R•t‚¯‚éj
        template<typename T>
        void Register(std::string_view _typeName)
        {
            typeToName_.emplace(typeid(T), std::string(_typeName));
            nameToType_.emplace(std::string(_typeName), typeid(T));
        }

        /// Œ^î•ñ‚©‚çŒ^–¼‚ğæ“¾
        std::string GetNameFromType(const std::type_index& _typeIndex) const
        {
            auto it = typeToName_.find(_typeIndex);
            if (it == typeToName_.end())
                return "Unknown";
            return it->second;
        }

        /// Œ^–¼‚©‚çŒ^î•ñ‚ğæ“¾
        std::type_index GetTypeFromName(std::string_view _typeName) const
        {
            auto it = nameToType_.find(std::string(_typeName));
            if (it == nameToType_.end())
                throw std::runtime_error("Unknown type: " + std::string(_typeName));
            return it->second;
        }

        /// Œ^‚ª“o˜^‚³‚ê‚Ä‚¢‚é‚©Šm”F
        bool IsRegistered(const std::type_index& _typeIndex) const
        {
            return typeToName_.find(_typeIndex) != typeToName_.end();
        }
	private:
        std::unordered_map<std::type_index, std::string> typeToName_;
        std::unordered_map<std::string, std::type_index> nameToType_;
	};
}

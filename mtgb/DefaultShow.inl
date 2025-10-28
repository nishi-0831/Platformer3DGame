#include "Command.h"
#include "ImGuiInputCommand.h"
// DefaultShow.inl - �e���v���[�g�����t�@�C��
namespace mtgb {

template<typename T>
Command* DefaultShow(T* value, const char* name)
{
    bool changed = false;

    using Type = std::remove_cvref_t<T>;
    if constexpr (std::is_array_v<Type>)
    {
        if (ImGui::CollapsingHeader(name))
        {
            ImGui::Indent();

            //�v�f�^�擾
            using ElemType = std::remove_extent_t<Type>;
            //0�����ڂ̗v�f���擾
            constexpr size_t N = std::extent_v<Type>;

            //�z��v�f���ƂɍċA���A�ŏ��ɓ���ꂽ�R�}���h��Ԃ�
            for (size_t i = 0; i < N; ++i)
            {
                std::string elemName = std::string(name) + "[" + std::to_string(i) + "]";
                Command* cmd = DefaultShow(static_cast<ElemType*>(&(*value)[i]), elemName.c_str());
                if (cmd)
                {
                    ImGui::Unindent();
                    return cmd;
                }
            }

            ImGui::Unindent();
        }
    }
    else
    {
        T oldValue{ *value };
        if constexpr (std::is_same_v<Type, bool>)
        {
            changed = ImGui::Checkbox(name, reinterpret_cast<bool*>(value));
        }
        else if constexpr (std::is_same_v<Type, float>)
        {
            changed = ImGui::InputFloat(name, reinterpret_cast<float*>(value));
        }
        else if constexpr (std::is_same_v<Type, float*>)
        {
            changed = ImGui::InputFloat(name, *reinterpret_cast<float**>(value));
        }
        else if constexpr (std::is_same_v<Type, int>)
        {
            changed = ImGui::InputInt(name, reinterpret_cast<int*>(value));
        }
        else if constexpr (std::is_same_v<Type, int*>)
        {
            changed = ImGui::InputInt(name, *reinterpret_cast<int**>(value));
        }
        else if constexpr (std::is_same_v<Type, long>)
        {
            ImGui::Text("%s : %4.2ld", name, *value);
        }
        else if constexpr (std::is_same_v<Type, long*>)
        {
            ImGui::Text("%s : %4.2ld", name, **reinterpret_cast<long**>(value));
        }
        else if constexpr (std::is_same_v<Type, unsigned long>)
        {
            ImGui::Text("%s : %4.2lo", name, *value);
        }
        else if constexpr (std::is_same_v<Type, unsigned long*>)
        {
            ImGui::Text("%s : %4.2lo", name, **reinterpret_cast<unsigned long**>(value));
        }
        else if constexpr (std::is_same_v<Type, unsigned char>)
        {
            ImGui::Text("%s : %4.2hhu", name, *value);
        }
        else if constexpr (std::is_enum_v<Type>)
        {
            // TODO: enum handling
        }
        else if constexpr (std::is_same_v<Type, std::string>)
        {
            std::string str = mtgb::MultiToUTF8(*value);
            ImGui::Text("%s : %s", name, str.c_str());
            /*std::vector<char> buffer(str.begin(), str.end());
            buffer.resize(256);
            if (ImGui::InputText(name,buffer.data(), buffer.size()))
            {
                *value = UTF8ToMulti(std::string(buffer.data()));
            }*/
        }
        else if constexpr (std::is_same_v<Type, std::string_view>)
        {
            std::string str(value->data(), value->size());
            str = MultiToUTF8(str);
            ImGui::Text("%s : %s", name, str.c_str());
        }
        else
        {
            ImGui::Text("%s:Unknown,%s", name, typeid(Type).name());
        }


        // �z��łȂ��Aconst�łȂ��ꍇ�������̉�������
        if constexpr (std::is_array_v<T> == false && std::is_const_v<T> == false)
        {

            if (changed != false)
            {
                // �ύX������Ă�����R�}���h��Ԃ�
                return new ImGuiInputCommand<T>(value, oldValue, *value, name);
            }
        }

    // �ύX������Ă��Ȃ��ꍇ��nullptr
    }
    return nullptr;
}

} // namespace mtgb
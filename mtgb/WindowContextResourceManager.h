#pragma once
#include <functional>
#include <map>
#include <cassert>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <initializer_list>
#include "ISystem.h"
#include "WindowContext.h"
#include "WindowContextResource.h"
#include "ReleaseUtility.h"
#include "ResourceCollection.h"
typedef struct HWND__* HWND;

namespace mtgb
{
    /// <summary>
    /// �E�B���h�E���Ƃ̃��\�[�X���Ǘ�����N���X
    /// </summary>
    class WindowContextResourceManager : public ISystem
    {
    private:
        WindowContext currentContext_;//���݂̃E�B���h�E
        ResourceCollection defResource_;//�����p�̃��\�[�X
        std::map<WindowContext, ResourceCollection> collectionMap_;//�E�B���h�E���Ƃ̃��\�[�X
    public:
        WindowContextResourceManager();
        ~WindowContextResourceManager();
        void Initialize() override;
        void Update() override;
        void Release();


        /// <summary>
        /// �E�B���h�E���ƂɕK�v�ȃ��\�[�X���쐬
        /// </summary>
        /// <param name="windowContext">�쐬����E�B���h�E�̎��ʎq</param>
        void CreateResource(WindowContext windowContext);

        /// <summary>
        /// ���\�[�X��؂�ւ���
        /// </summary>
        /// <param name="windowContext">�؂�ւ���E�B���h�E�̎��ʎq</param>
        void ChangeActiveResource(WindowContext windowContext);

        /// <summary>
        /// �S���\�[�X��OnResize���Ăт���
        /// </summary>
        /// <param name="windowContext"></param>
        /// <param name="width"></param>
        /// <param name="height"></param>
        void OnResizeAll(WindowContext windowContext, UINT width, UINT height);

        /// <summary>
        /// �w�肵����̃E�B���h�E�̎w�肵�����\�[�X����������
        /// �Ή�������͋@������ւ���ۂȂǂɎg��
        /// </summary>
        /// <typeparam name="ResourceT">�w�肷�郊�\�[�X�̌^</typeparam>
        /// <param name="context1">�f�t�H���g��First</param>
        /// <param name="context2">�f�t�H���g��Second</param>
        template<typename ResourceT>
        void SwapResource(WindowContext context1 = WindowContext::First, WindowContext context2 = WindowContext::Second);
        
       /* /// <summary>
        /// <para> �w�肵����̃E�B���h�E�̑S���\�[�X���������� </para>
        /// <para> �����܂�WindowContext���w���|�C���^���������邾���Ȃ̂ŉ�ʂ��؂�ւ������͂��Ȃ��B </para>
        /// </summary>
        /// <param name="context1">�f�t�H���g��First</param>
        /// <param name="context2">�f�t�H���g��Second</param>
        void SwapAllResource(WindowContext context1 = WindowContext::First, WindowContext context2 = WindowContext::Second);*/

        /// <summary>
        /// ���\�[�X���擾����
        /// </summary>
        /// <typeparam name="ResourceT">���\�[�X�̌^</typeparam>
        /// <param name="windowContext">�擾����E�B���h�E�̎��ʎq</param>
        /// <returns>���\�[�X�̎Q��</returns>
        template<typename ResourceT>
        ResourceT& Get(WindowContext windowContext)
        {
            auto itr = collectionMap_.find(windowContext);
            assert(itr != collectionMap_.end() && "�w�肳�ꂽWindowContext�̃��\�[�X��������܂���");

            static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
                "WindowContextResource���p�����Ă��Ȃ��N���X�̃C���X�^���X�͎擾�ł��܂���B");

            return itr->second.Get<ResourceT>();
        }

        

        /// <summary>
        /// ���ݗL���ȃE�B���h�E�̎��ʎq��Ԃ�
        /// </summary>
        /// <returns>���݂�WindowContext</returns>
        WindowContext CurrCtx()
        {
            return currentContext_;
        }

        /// <summary>
        /// �^���w�肵�ēo�^����֐�
        /// </summary>
        /// <typeparam name="...Args">�w�肵�����Ԃɏ����������</typeparam>
        template<class... Args>
        void RegisterResourceTypes()
        {
            static constexpr bool allAreBaseOfWindowContextResource =
                (std::is_base_of_v<WindowContextResource, Args> && ...);

            static_assert(allAreBaseOfWindowContextResource, "Args...��WindowContextResource�̔h���^�ł���K�v������܂�");

            (void)std::initializer_list <int>
            {
                (defResource_[typeid(Args)] = new Args, 0)...
            };
        }
    };

    template<typename ResourceT>
    inline void WindowContextResourceManager::SwapResource(WindowContext context1, WindowContext context2)
    {
        // �O�̂��ߓ����E�B���h�E���w�肵�Ă��Ȃ����m�F
        if (context1 == context2)
        {
            return;
        }

        // context1,2��ResourceCollection���o�^����Ă���m�F
        auto itr1 = collectionMap_.find(context1);
        auto itr2 = collectionMap_.find(context2);

        assert(itr1 != collectionMap_.end() && "�w�肳�ꂽWindowContext��������܂���");
        assert(itr2 != collectionMap_.end() && "�w�肳�ꂽWindowContext��������܂���");

        std::type_index typeIdx = typeid(ResourceT);

        // ���\�[�X���擾
        /*WindowContextResource* resource1 = itr1->second[typeIdx];
        WindowContextResource* resource2 = itr2->second[typeIdx];*/

        // ����ւ���
        std::swap(itr1->second[typeIdx], itr2->second[typeIdx]);
    }
}
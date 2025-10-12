#pragma once

// �|�C���^�����S�ɉ�����邽�߂̃}�N��
#define SAFE_DELETE(p)\
if (p != nullptr)      \
{                       \
	delete p;            \
	p = nullptr;          \
}

// �|�C���^�z������S�ɉ�����邽�߂̃}�N��
#define SAFE_DELETE_ARRAY(p)\
if (p != nullptr)            \
{                             \
	delete[] p;                \
	p = nullptr;                \
}

// �|�C���^�����S��Release���邽�߂̃}�N��
#define SAFE_RELEASE(p)\
if (p != nullptr)       \
{                        \
	p->Release();         \
	p = nullptr;           \
}

// �|�C���^�����S��Destroy���邽�߂̃}�N��
#define SAFE_DESTROY(p)\
if (p != nullptr)       \
{                        \
	p->Destroy();         \
	p = nullptr;           \
}

// �R���e�i�̗v�f�����S��Release���邽�߂̃}�N��
// �R���e�i�� std::vector �� std::list �Ȃǂ̂���
#define SAFE_CLEAR_CONTAINER_RELEASE(container)\
{                                                \
	for (auto& p : container)                     \
	{                                              \
		SAFE_RELEASE(p);                            \
	}                                                \
	container.clear();                                \
}

// �R���e�i�̗v�f�����S�ɉ�����邽�߂̃}�N��
// �R���e�i�� std::vector �� std::list �Ȃǂ̂���
#define SAFE_CLEAR_CONTAINER_DELETE(container)\
{                                               \
	for (auto& p : container)                    \
	{                                             \
		SAFE_DELETE(p);                            \
	}                                               \
	container.clear();                               \
}



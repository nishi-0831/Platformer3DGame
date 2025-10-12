#pragma once
template <typename Func>
struct ShowFunc : refl::attr::usage::type
{
private:
	Func func;
public:
	constexpr ShowFunc(Func _func) : func(_func)
	{
	}
	template<typename T>
	void operator()(T value, const char* name) const { func(value, name); }
};

// ShowFunc���쐬����w���p�[�֐�
template<typename F>
constexpr auto make_show_func(F&& f) {
	return ShowFunc<F>(std::forward<F>(f));
}
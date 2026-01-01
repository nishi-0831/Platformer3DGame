#pragma once

namespace mtgb
{
	class ISystem
	{
	  public:
		ISystem();
		virtual ~ISystem();

		virtual void Initialize() = 0;
		virtual void Update()	  = 0;
		/// <summary>
		/// ŠJ•úˆ—
		/// </summary>
		virtual void Release()
		{
		}
	};
} // namespace mtgb

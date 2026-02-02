#pragma once

namespace EX
{
	class DeltaTime
	{
	public:
		DeltaTime(float time = 0.0f)
			:mTime(time) {}

		operator float() const { return mTime; }

		float GetTimeSeconds() { return mTime; }
		float GetTimeMilliSeconds() { return mTime * 1000.0f; }

		static float GetTime();
	private:
		float mTime;
	};
}
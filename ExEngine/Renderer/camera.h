#pragma once

#include "glm/glm.hpp"

//only Persperctive Supported as of now 
namespace EX
{

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			:mProjection(projection)
		{}
			
		const glm::mat4& GetProjection()const { return mProjection; }
		const glm::mat4& GetView()const { return mView; }
		const glm::mat4& GetViewProjection() { return mProjection * mView; }
		const glm::vec3& GetForwardVector()const { return mForwardDirection; }
		const glm::vec3& GetRightVector()const { return mRightDirection; }
		const glm::vec3& GetUpVector()const { return mUpDirection; }
		const glm::vec3& GetPosition()const { return mPosition; }
	private:
	protected:
		enum class CameraMode { Orbit, Free };
		CameraMode mMode = CameraMode::Free;
		glm::mat4 mProjection = glm::mat4(1.0f);
		glm::mat4 mView = glm::mat4(1.0f);
		glm::vec3 mPosition{ 0.0f, 0.0f, 3.0f };
		glm::vec3 mForwardDirection{ 0.0f, 0.0f, 1.0f };
		glm::vec3 mRightDirection = glm::vec3(1, 0, 0);
		glm::vec3 mUpDirection{ 0.0f, 1.0f, 0.0f };

		float mYaw = glm::radians(90.0f);// In radians
		float mPitch = glm::radians(0.0f);
	};

}
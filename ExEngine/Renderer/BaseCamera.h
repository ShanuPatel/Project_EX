#pragma once

#include "Camera.h"
#include "Core/DeltaTime.h"
#include "Core/Event/Event.h"
#include "Core/Event/MouseEvent.h"
#include <glm/glm.hpp>


namespace EX
{
	class BaseCamera : public Camera
	{
	public:
		BaseCamera();
		BaseCamera(float fov, float aspectRatio, float nearClip, float farClip);

		bool OnUpdate(DeltaTime ts);
		void OnEvent(Event& e);

		inline float GetDistance() const { return mDistance; }
		inline void SetDistance(float distance) { mDistance = distance; }

		void SetViewportSize(float width, float height);

		glm::mat4 GetViewProjection() const { return mProjection * mView; }

		const glm::vec3& GetPosition() const { return mPosition; }
		void SetPosition(const glm::vec3& position) { mPosition = position; }
		void SetYaw(float yaw) { mYaw = yaw; }														// In radians
		void SetPitch(float pitch) { mPitch = pitch; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return mPitch; }
		float GetYaw() const { return mYaw; }
		void MouseRotate(const glm::vec2& delta);

		float GetPanSpeed() const;
		void Pan(const glm::vec2& delta);

		void AutoOrbit(float yawSpeed, float pitchSpeed, DeltaTime dt);
		void Orbit(const glm::vec2& delta);
		void SetFocusPoint(const glm::vec3& point) { mFocusPoint = point; }
		void SetFocusDistance(glm::vec3 distance) { mDistanceFromFocus = distance; }

		void SetCursorLocked(bool bLocked) { mCursorLocked = bLocked; }
		void SetMouseLastPosition(const glm::vec2& lastPos) { mLastMousePosition = lastPos; }
	private:
		void CameraStyle();
		void Rage();
		void Default();
		void RecalculateProjection();
		float GetRotationSpeed() const;

		void DefaultCameraController(DeltaTime dt);
		bool OnMouseScroll(MouseScrolledEvent& e);
		void MouseZoom(float delta);

	private:
		float mFOV = 45.0f, mAspectRatio = 1.777f, mNearClip = 0.1f, mFarClip = 2000.0f;

		float mDistance = 10.0f;
		glm::vec3 mFocusPoint = glm::vec3(0.0f);
		glm::vec3 mDistanceFromFocus = glm::vec3(0.0f, 0.0f, 10.0f);

		float mViewportWidth = 1024, mViewportHeight = 1024;
		glm::vec2 mLastMousePosition{ 0.0f, 0.0f };
		bool mFirstMouse = true;
		bool mWasMouseDownLastFrame = false;

		bool mCursorLocked = false;

		float mMoveSpeed = 10.0f;//10
		float mMouseSensitivity = 0.001f;//0.06

		float mTranslationDampening = 0.6f;
		float mRotationDampening = 0.3f;
		glm::vec3 mTranslationVelocity = glm::vec3(0.0f);
		glm::vec2 mRotationVelocity = glm::vec2(0.0f);
		glm::mat4 mCubeViewMatrix = glm::mat4(1.0f);
		glm::vec2 mGizmoPosition = glm::vec2(1.0f);

	};
}
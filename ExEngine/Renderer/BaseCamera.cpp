#include "PCH.h"
#include "BaseCamera.h"
#include "Core/Input.h"

#include <glm/gtx/quaternion.hpp>       // rotate(quat, vec3), toMat4, quat_cast, etc.

namespace EX
{
	BaseCamera::BaseCamera()
		: mFOV(45.0f), mAspectRatio(1.778f), mNearClip(0.1f), mFarClip(200.0f), Camera(glm::perspective(glm::radians(mFOV), mAspectRatio, mNearClip, mFarClip))
	{
		mFocusPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		mDistanceFromFocus.z = 3.0f;
		mPosition = glm::vec3(mDistanceFromFocus);
		//mPosition = mFocusPoint - glm::vec3(3.0f, 0.0f, 0.0f); // 6 units back along -X //Rage
		mYaw = 0.0f; // looking along +X
		mPitch = 0.0f;
	}

	BaseCamera::BaseCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: mFOV(fov), mAspectRatio(aspectRatio), mNearClip(nearClip), mFarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		mFocusPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		mPosition = mFocusPoint - glm::vec3(6.0f, 0.0f, 0.0f); // 6 units back along -X
		mYaw = 0.0f; // looking along +X
		mPitch = 0.0f;
	}

	void BaseCamera::RecalculateProjection()
	{
		mProjection = glm::perspective(glm::radians(mFOV), mAspectRatio, mNearClip, mFarClip);
	}

	void BaseCamera::SetViewportSize(float width, float height)
	{
		mViewportWidth = width;
		mViewportHeight = height;
		mAspectRatio = width / height;
		RecalculateProjection();
	}

	float BaseCamera::GetRotationSpeed() const
	{
		return 0.8f;
	}

	bool BaseCamera::OnUpdate(DeltaTime dt)
	{
		CameraStyle();

		if (Input::IsMouseButtonPressed(Mouse::Button1))
		{
			if (!mWasMouseDownLastFrame) // Just pressed this frame
			{
				mFirstMouse = true;
				mWasMouseDownLastFrame = true;
			}
			DefaultCameraController(dt);
		}
		else
		{
			mWasMouseDownLastFrame = false;
			if (mCursorLocked && Input::IsMouseButtonPressed(Mouse::Button0))
			{
				glm::vec2 currentPos = Input::GetMousePosition();
				glm::vec2 delta = currentPos - mLastMousePosition;

				// Only rotate if mouse actually moved
				if (glm::length(delta) > 0.0f)
				{
					delta *= mMouseSensitivity;
					Orbit(delta);
				}

				mLastMousePosition = currentPos;
				return true;
			}
		}
		return true;
	}

	void BaseCamera::Rage()
	{
		// Clamp pitch
		mPitch = glm::clamp(mPitch, glm::radians(-89.0f), glm::radians(89.0f));

		//// Update orientation based on current pitch/yaw
		glm::quat orientation = GetOrientation();
		////calculate direction vectors 
		mForwardDirection = glm::normalize(glm::rotate(orientation, glm::vec3(1, 0, 0)));
		mRightDirection = glm::normalize(glm::rotate(orientation, glm::vec3(0, 1, 0)));
		mUpDirection = glm::normalize(glm::rotate(orientation, glm::vec3(0, 0, 1)));

		// Update view matrix
		mView = glm::lookAt(mPosition, mPosition + mForwardDirection, mUpDirection);
	}

	void BaseCamera::Default()
	{
		mPitch = glm::clamp(mPitch, glm::radians(-89.0f), glm::radians(89.0f));

		//// Update orientation based on current pitch/yaw
		glm::quat orientation = GetOrientation();
		////calculate direction vectors 
		mForwardDirection = glm::normalize(glm::rotate(orientation, glm::vec3(0, 0, -1)));
		mRightDirection = glm::normalize(glm::rotate(orientation, glm::vec3(1, 0, 0)));
		mUpDirection = glm::normalize(glm::rotate(orientation, glm::vec3(0, 1, 0)));

		// Update view matrix
		mView = glm::lookAt(mPosition, mPosition + mForwardDirection, mUpDirection);
	}

	void BaseCamera::CameraStyle()
	{
		Default();
		//Rage();
	}

	void BaseCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(EX_BIND_FN(BaseCamera::OnMouseScroll));
	}

	void BaseCamera::MouseRotate(const glm::vec2& delta)
	{
		mYaw += delta.x * GetRotationSpeed();  // Right drag Yaw right
		mPitch += delta.y * GetRotationSpeed();  // Up drag Look up

		mPitch = glm::clamp(mPitch, glm::radians(-89.0f), glm::radians(89.0f));

	}

	glm::quat BaseCamera::GetOrientation() const
	{
		///Rage
		//return glm::angleAxis(-mYaw, glm::vec3(0, 0, 1)) *  // yaw around Z
		//	glm::angleAxis(-mPitch, glm::vec3(0, 1, 0));   // pitch around Y

		///Default
		return glm::quat(glm::vec3(mPitch, -mYaw, 0.0f));
	}

	void BaseCamera::AutoOrbit(float yawSpeed, float pitchSpeed, DeltaTime dt)
	{
		glm::vec2 delta(yawSpeed * dt, pitchSpeed * dt);
		Orbit(delta);
	}

	void BaseCamera::Pan(const glm::vec2& delta)
	{
		float panSpeed = GetPanSpeed();

		glm::vec3 right = GetRightVector();
		glm::vec3 up = GetUpVector();

		glm::vec3 translation = -delta.x * panSpeed * right + delta.y * panSpeed * up;

		mFocusPoint += translation;
		mPosition += translation;
	}

	float BaseCamera::GetPanSpeed() const
	{
		return mDistanceFromFocus.z * 0.001f;
	}

	void BaseCamera::Orbit(const glm::vec2& delta)
	{
		// Initialize orbit state if coming from free mode
		if (mMode != CameraMode::Orbit)
		{
			mDistanceFromFocus.z = glm::length(mPosition - mFocusPoint);
			mFocusPoint = mPosition + mForwardDirection * mDistanceFromFocus.z;
			mMode = CameraMode::Orbit;
		}

		float rotationSpeed = GetRotationSpeed();
		mYaw += delta.x * rotationSpeed;
		mPitch -= delta.y * rotationSpeed;
		mPitch = glm::clamp(mPitch, glm::radians(-89.0f), glm::radians(89.0f));

		glm::quat orientation = GetOrientation();
		glm::vec3 offset = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, mDistanceFromFocus.z));

		mPosition = mFocusPoint + offset;

		// Recalculate forward vector (for smooth switching)
		mForwardDirection = glm::normalize(mFocusPoint - mPosition);
		mRightDirection = glm::normalize(glm::cross(mForwardDirection, glm::vec3(0, 1, 0)));
		mUpDirection = glm::normalize(glm::cross(mRightDirection, mForwardDirection));

		// --- Sync focus point for when we switch back to orbit ---
		mFocusPoint = mPosition + mForwardDirection * mDistanceFromFocus.z;
	}

void BaseCamera::DefaultCameraController(DeltaTime ts)
{
	const float dt = ts;
	const float moveSpeed = mMoveSpeed * dt;
	const float rotateSpeed = mMouseSensitivity;
	glm::vec2 currentMousePos = Input::GetMousePosition();

	if (mFirstMouse)
	{
		mLastMousePosition = currentMousePos;
		mFirstMouse = false;
	}
	// Switch to free mode if needed
	if (mMode != CameraMode::Free)
	{
		mMode = CameraMode::Free;
		mFirstMouse = true;  // <<< Reset mouse delta initialization
	}

	if (Input::IsMouseButtonPressed(Mouse::Button1))
	{
		if (mFirstMouse)
		{
			mLastMousePosition = currentMousePos;
			mFirstMouse = false;
		}

		glm::vec2 delta = currentMousePos - mLastMousePosition;
		delta *= rotateSpeed;
		delta.y = -delta.y;

		MouseRotate(delta);
	}

	mLastMousePosition = currentMousePos;

	// WASD
	glm::vec3 movement(0.0f);
	if (Input::IsMouseButtonPressed(Mouse::Button1))
	{
		if (Input::IsKeyPressed(Key::W)) movement += mForwardDirection;
		if (Input::IsKeyPressed(Key::S)) movement -= mForwardDirection;
		if (Input::IsKeyPressed(Key::D)) movement += mRightDirection;
		if (Input::IsKeyPressed(Key::A)) movement -= mRightDirection;
		if (Input::IsKeyPressed(Key::Q)) movement -= mUpDirection;
		if (Input::IsKeyPressed(Key::E)) movement += mUpDirection;

		if (glm::length(movement) > 0.0f)
			movement = glm::normalize(movement);
	}

	mPosition += movement * moveSpeed;
}

	bool BaseCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		mMoveSpeed += e.GetYOffset() * 0.25f; // Tune the multiplier as needed
		mMoveSpeed = glm::clamp(mMoveSpeed, 0.1f, 20.0f); // Prevent going negative or too fast
		//GLSL_CORE_INFO("mouseScroll");
		return false;
	}

	void BaseCamera::MouseZoom(float delta)
	{

	}
}
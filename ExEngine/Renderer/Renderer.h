#ifndef RENDERER
#define RENDERER

#include "RenderCommand.h"
#include "Mesh.h"
#include "ShaderLibrary.h"
#include "Core/DeltaTime.h"
#include <glm/gtx/quaternion.hpp>

namespace EX
{
	class ShaderLibrary;
	class Shader;
	class EditorCamera;
	class UniformBuffer;
	class Texture2D;
	class TextureCubemap;
	class Framebuffer;

	struct CameraData
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 ViewProjection;
		glm::vec3 Position;
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f }; // Euler angles (radians)
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
			: Translation(translation), Rotation(rotation), Scale(scale) {
		}

		glm::mat4 GetLocalTransform() const
		{
			glm::mat4 rotationMat = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation)
				* rotationMat
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	class Renderer
	{
		struct MeshData
		{
			glm::mat4 Transform;
			MeshVA Geometry;//Mesh/Submesh
			MeshData(const glm::mat4& transform, const MeshVA& meshData)
				: Transform(transform), Geometry(meshData)
			{
			}
		};
	public:
		static constexpr uint32_t MAX_NUM_DIR_LIGHTS = 3;

		static void Init();
		static void Shutdown();

		static void Quad();
		static void Cube();

		static void SetupSamplers();
		static void SetupEnviromentMap();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void SetCameraData(const CameraData& cameraData);
		static void BeginScene(const CameraData& cameraData);
		static void SubmitMesh(const glm::mat4& transform, MeshVA& mesh);
		static void DrawMesh(const Ref<Shader>& shader);
		static void DrawCube();
		static void DrawQuad();
		static void DrawSky();
		static void Flush();
		static void EndScene();
		static void RenderPass();
		static void PostProcessPass();
		//inline static RendererAPI::API GetRenderAPI() { return RendererAPI::GetAPI(); }

		static void OrbitDLitght(DeltaTime dt);
	private:

		static void SetupMesh();
		static void SetupUBOs();
		static void SetupFBOs();
		static void LightingData();

		inline static Ref<EditorCamera> mCamera;
		inline static Ref<UniformBuffer> sUbCamera;

		//Framebuffer
		inline static Ref<Framebuffer> mFramebuffer;

		//MesheDatas
		inline static std::vector<MeshData> mMeshes;
		inline static Ref<Mesh> mModel;

		//VAO
		inline static Ref<VertexArray> mCubeVA;
		inline static Ref<VertexArray> mQuadVertexArray;
		
		//Shaders
		inline static ShaderLibrary mShaderLib;
		inline static Ref<Shader> mCubeShader;
		inline static Ref<Shader> mPostProcessShader;
		inline static Ref<Shader> mModelShader;
		inline static Ref<Shader> mQuadShader;
		inline static Ref<Shader> mCubemapShader;

		//Textures
		inline static Ref<Texture2D> mTexture;
		inline static Ref<Texture2D> mSampleTexture;
		inline static Ref<TextureCubemap> mSkyTexture;

		//Transform
		inline static TransformComponent mTransform;
		//Lighting
		inline static Ref<UniformBuffer> sUbDirectionalLights;

		inline static bool bModelType = true;
	};
}
#endif // !RENDERER

#include "PCH.h"
#include "Renderer.h"

#include "Shader.h"
#include "BaseCamera.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Material.h"
#include "Core/AssetManager.h"

namespace EX
{
	// ------------------------------------------------------------
	// Static state
	// ------------------------------------------------------------
	static glm::mat4 ModelMatrix(1.0f);
	static glm::mat4 SceneRotationMatrix(1.0f);

	// ------------------------------------------------------------
	// Init / Shutdown
	// ------------------------------------------------------------
	void Renderer::Init()
	{
		RenderCommand::SetAPI(RendererAPI::Create());
		RenderCommand::Init();

		mCubeShader = Shader::Create("bin/assets/shaders/Cube.glsl");
		mModelShader = mShaderLib.Load("PBR", "bin/assets/shaders/PBRv2.glsl");
		mCubemapShader = mShaderLib.Load("Cubemap", "bin/assets/shaders/skybox.glsl");

		SetupMesh();
		SetupUBOs();
		SetupFBOs();

		Quad();
		Cube();

		SetupSamplers();
		SetupEnviromentMap();
	}

	void Renderer::Shutdown()
	{
		// intentionally empty
	}

	// ------------------------------------------------------------
	// Samplers & Environment
	// ------------------------------------------------------------
	void Renderer::SetupSamplers()
	{
		mCubeShader->Bind();
		mCubeShader->SetInt("uTexture", 0);

		mModelShader->Bind();
		mModelShader->SetBool("bModelType", bModelType);
		mModelShader->SetInt("uAlbedoMap", 0);
		mModelShader->SetInt("uNormalMap", 1);
		mModelShader->SetInt("uMetallicRoughnessMap", 2);
		mModelShader->SetInt("uRoughnessMap", 3);
		mModelShader->SetInt("uAOMap", 4);
		mModelShader->SetInt("uEmissiveMap", 5);

		mPostProcessShader->Bind();
		mPostProcessShader->SetInt("uScreenTexture", 0);
	}

	void Renderer::SetupEnviromentMap()
	{
		//mSkyTexture = TextureCubemap::Create("bin/assets/engine/textures/HDR/Environment.hdr");
		//mSkyTexture = TextureCubemap::Create("bin/assets/engine/textures/HDR/cannon.hdr");
		mSkyTexture = TextureCubemap::Create("bin/assets/engine/textures/HDR/barcelona.hdr");
		mSkyTexture->GenerateCubeMap();
	}

	// ------------------------------------------------------------
	// Mesh setup
	// ------------------------------------------------------------
	void Renderer::SetupMesh()
	{
		const std::filesystem::path path =
			"bin/assets/engine/models/gltf/DamagedHelmet/DamagedHelmet.gltf";

		mModel = AssetManager::GetMesh(path.string());
		bModelType = mModel->GetModelType();

		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		SubmitMesh(ModelMatrix, mModel->GetMeshData(0));
	}

	// ------------------------------------------------------------
	// Uniform Buffers
	// ------------------------------------------------------------
	void Renderer::SetupUBOs()
	{
		sUbCamera = UniformBuffer::Create();
		sUbCamera->SetLayout({
			{ ShaderDataType::Mat4,  "uView" },
			{ ShaderDataType::Mat4,  "uProjection" },
			{ ShaderDataType::Mat4,  "uViewProjection" },
			{ ShaderDataType::Float4,"uCameraPosition" },
			}, 0);

		sUbDirectionalLights = UniformBuffer::Create();
		sUbDirectionalLights->SetLayout({
			{ ShaderDataType::Float4, "uPosition" },
			{ ShaderDataType::Float4, "uColor" },
			{ ShaderDataType::Float4, "uLightDir" },
			}, 1, MAX_NUM_DIR_LIGHTS + 1);
	}

	// ------------------------------------------------------------
	// Framebuffer
	// ------------------------------------------------------------
	void Renderer::SetupFBOs()
	{
		const uint32_t width = 1280;
		const uint32_t height = 720;

		FramebufferSpecification spec;
		spec.Width = width;
		spec.Height = height;
		spec.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::Depth
		};

		mFramebuffer = Framebuffer::Create(spec);
		mFramebuffer->Resize(width, height);
	}

	void Renderer::Quad()
	{
		float vertices[] = {
			 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};

		uint32_t indices[] = {
			0, 1, 2,
			0, 2, 3
		};

		mQuadVertexArray = VertexArray::Create();

		Ref<VertexBuffer> quadVertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		quadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		mQuadVertexArray->AddVertexBuffer(quadVertexBuffer);

		Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));//6
		mQuadVertexArray->SetIndexBuffer(quadIndexBuffer);
		mPostProcessShader = Shader::Create("bin/assets/shaders/PostProcess.glsl");
	}

	void Renderer::Cube()
	{
		mCubeVA = VertexArray::Create();
		float cubeVertices[] = {
			// Front face
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top right
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // top left
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom left
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // bottom right

			 // Right face
			  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top right
			  0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // top left
			  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom left
			  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // bottom right

			  // Back face
			  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top right
			   0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top left
			   0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom left
			  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // bottom right

			  // Left face
			  -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top right
			  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top left
			  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom left
			  -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // bottom right

			  // Top face
			   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top right
			  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top left
			  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // bottom left
			   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // bottom right

			   // Bottom face
				0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // top right
			   -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // top left
			   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom left
				0.5f, -0.5f, -0.5f,  1.0f, 0.0f   // bottom right
		};

		Ref<VertexBuffer> cubeVertexBuffer;;
		cubeVertexBuffer = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));

		BufferLayout cubeLayout = {
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float2, "aTexCoord" }
		};

		cubeVertexBuffer->SetLayout(cubeLayout);
		mCubeVA->AddVertexBuffer(cubeVertexBuffer);

		uint32_t cubeIndices[] = {
			0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
			4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
			8, 9,10,  10,11, 8,    // v0-v5-v6, v6-v1-v0 (top)
			12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
			16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
			20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)

		};

		Ref<IndexBuffer> cubeIndexBufffer;
		cubeIndexBufffer = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t));
		mCubeVA->SetIndexBuffer(cubeIndexBufffer);

	}

	// ------------------------------------------------------------
	// Camera & Lighting
	// ------------------------------------------------------------
	void Renderer::SetCameraData(const CameraData& cameraData)
	{
		sUbCamera->Bind();
		sUbCamera->SetData(&cameraData, sizeof(CameraData), 0);
	}

	void Renderer::LightingData()
	{
		struct DirectionalLightData
		{
			glm::vec4 Position;
			glm::vec4 Color;
			glm::vec4 LightDir;
		};

		sUbDirectionalLights->Bind();

		mTransform.Rotation = glm::vec3(glm::radians(180.0f), 90.0f, 0.0f);
		glm::mat4 world = mTransform.GetLocalTransform();

		glm::vec3 dir = glm::normalize(glm::vec3(world * glm::vec4(0, 0, 1, 0)));
		glm::vec3 pos = glm::vec3(world[3]);

		DirectionalLightData data = {
			glm::vec4(pos, 0.0f),
			glm::vec4(glm::vec3(1.0f), 1.0f),
			glm::vec4(dir, 0.0f)
		};

		sUbDirectionalLights->SetData(&data, sizeof(data), 0);
	}

	// ------------------------------------------------------------
	// Drawing
	// ------------------------------------------------------------
	void Renderer::DrawCube()
	{
		RenderCommand::DrawIndexed(mCubeVA);
	}

	void Renderer::DrawQuad()
	{
		RenderCommand::DrawIndexed(mQuadVertexArray);
	}

	void Renderer::DrawSky()
	{
		RenderCommand::SetDepthFunction(DepthFunc::LEQUAL);

		mSkyTexture->Bind(0);
		mSkyTexture->BindIrradianceMap(6);
		mSkyTexture->BindRadianceMap(7);
		mSkyTexture->BindBRDFLUT(8);

		mCubemapShader->Bind();
		DrawCube();

		RenderCommand::SetDepthFunction(DepthFunc::LESS);
	}

	// ------------------------------------------------------------
	// Scene flow
	// ------------------------------------------------------------
	void Renderer::BeginScene(const CameraData& cameraData)
	{
		SetCameraData(cameraData);
		LightingData();
	}

	void Renderer::SubmitMesh(const glm::mat4& transform, MeshVA& mesh)
	{
		mMeshes.emplace_back(transform, mesh);
	}

	void Renderer::DrawMesh(const Ref<Shader>& shader)
	{
		for (const auto& mesh : mMeshes)
		{
			shader->Bind();
			mModel->GetMeshData(0).Mat->Bind();

			shader->SetMat4("uModel", ModelMatrix);
			shader->SetMat4("uSceneRotation", SceneRotationMatrix);
			shader->SetInt("uIrradianceMap", 6);
			shader->SetInt("uRadianceMap", 7);
			shader->SetInt("uBRDFLutMap", 8);

			RenderCommand::DrawIndexed(mesh.Geometry.Geometry);
		}
	}

	//Frame Flow
	void Renderer::RenderPass()
	{
		mFramebuffer->Bind();
		RenderCommand::SetDepthTest(true);
		RenderCommand::SetBlendState(true);
		RenderCommand::Clear();

		DrawSky();
		DrawMesh(mModelShader);

		mFramebuffer->Unbind();
		RenderCommand::SetDepthTest(false);
		RenderCommand::SetBlendState(false);
	}

	void Renderer::PostProcessPass()
	{
		mPostProcessShader->Bind();
		mFramebuffer->BindColorAttachment(0, 0);
		mPostProcessShader->SetFloat2("uResolution", { 1280, 720 });
		DrawQuad();
	}

	void Renderer::Flush()
	{
		RenderPass();
		PostProcessPass();
	}

	void Renderer::EndScene()
	{
		Flush();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}
#pragma once
#include <glm/glm.hpp>
#include "Core/Core.h"

namespace EX
{
    class Texture2D;
    class Shader;
    class Material
    {
    public:
        Material()
            : mAlbedo(glm::vec3(1.0f)), mMetallic(0.0f), mRoughness(1.0f), mAO(1.0f) {
        }

        // Getters
        const glm::vec3& GetAlbedo() const { return mAlbedo; }
        float GetMetallic() const { return mMetallic; }
        float GetRoughness() const { return mRoughness; }
        float GetAO() const { return mAO; }
        float GetEmissive() const { return mEmissive; }

        Ref<Texture2D> GetAlbedoTexture() const { return mAlbedoTexture; }
        Ref<Texture2D> GetNormal()const { return mNormalTexture; }
        Ref<Texture2D> GetNormalTexture() const { return mNormalTexture; }
        Ref<Texture2D> GetMetallicRoughnessTexture() const { return mMetallicRoughnessTexture; }
        Ref<Texture2D> GetAOTexture() const { return mAOTexture; }
        Ref<Texture2D> GetEmissiveTexture() const { return mEmissiveTexture; }

        // Setters
        void SetAlbedo(const glm::vec3& color) { mAlbedo = color; }
        void SetMetallic(float value) { mMetallic = value; }
        void SetRoughness(float value) { mRoughness = value; }
        void SetAO(float value) { mAO = value; }

        void SetAlbedoTexture(Ref<Texture2D> tex) { mAlbedoTexture = tex; }
        void SetNormalTexture(Ref<Texture2D> tex) { mNormalTexture = tex; }
        void SetMetallicRoughnessTexture(Ref<Texture2D> tex) { mMetallicRoughnessTexture = tex; }
        void SetRoughnessTexture(Ref<Texture2D> tex) { mRoughnessTexture = tex; }
        void SetAOTexture(Ref<Texture2D> tex) { mAOTexture = tex; }
        void SetEmissiveTexture(Ref<Texture2D> tex) { mEmissiveTexture = tex; }

        void Bind() const;

    private:
        glm::vec3 mAlbedo;
        float mMetallic;
        float mRoughness;
        float mAO;
        float mEmissive;

        Ref<Texture2D> mAlbedoTexture;
        Ref<Texture2D> mNormalTexture;
        Ref<Texture2D> mMetallicRoughnessTexture;
        Ref<Texture2D> mRoughnessTexture;
        Ref<Texture2D> mAOTexture;
        Ref<Texture2D> mEmissiveTexture;

    };
}
#include "PCH.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"

namespace EX
{
	void Material::Bind() const
	{
		if (mAlbedoTexture)
			mAlbedoTexture->Bind();

		if (mNormalTexture)
			mNormalTexture->Bind(1);

		if (mMetallicRoughnessTexture)
			mMetallicRoughnessTexture->Bind(2);

		if (mRoughnessTexture)
			mRoughnessTexture->Bind(3);

		if (mAOTexture)
			mAOTexture->Bind(4);

		if (mEmissiveTexture)
			mEmissiveTexture->Bind(5);

	}
}
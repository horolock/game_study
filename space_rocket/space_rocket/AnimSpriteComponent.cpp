#include "AnimSpriteComponent.h"
#include "Math.h"

AnimSpriteComponent::AnimSpriteComponent(class Actor* owner, int drawOrder)
	: SpriteComponent(owner, drawOrder),
	mCurrentFrame(0.0f),
	mAnimFPS(24.0f)
{
}

void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	if (mAnimTextures.size() > 0)
	{
		/* Update current frame */
		mCurrentFrame += mAnimFPS * deltaTime;

		while (mCurrentFrame >= mAnimTextures.size())
		{
			mCurrentFrame -= mAnimTextures.size();
		}

		SetTexture(mAnimTextures[static_cast<int>(mCurrentFrame)]);
	}
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<SDL_Texture*>& textures)
{
	mAnimTextures = textures;

	if (mAnimTextures.size() > 0)
	{
		mCurrentFrame = 0.0f;
		SetTexture(mAnimTextures[0]);
	}
}

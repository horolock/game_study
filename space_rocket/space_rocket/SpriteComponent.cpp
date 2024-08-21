#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

/*
* Painter's algorithm
*/

SpriteComponent::SpriteComponent(class Actor* owner, int drawOrder) :
	Component(owner),
	mTexture(nullptr),
	mDrawOrder(drawOrder),
	mTextureWidth(0),
	mTextureHeight(0)
{
	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(SDL_Renderer* renderer)
{
	if (mTexture)
	{
		SDL_Rect r;
		r.w = static_cast<int>(mTextureWidth * mOwner->GetScale());
		r.h = static_cast<int>(mTextureHeight * mOwner->GetScale());

		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

		SDL_RenderCopyEx(
			renderer,
			mTexture,
			nullptr,
			&r,
			-Math::ToDegrees(mOwner->GetRotation()),
			nullptr,
			SDL_FLIP_NONE);
	}
}

void SpriteComponent::SetTexture(SDL_Texture* texture)
{
	mTexture = texture;

	/* Get texture width and height */
	SDL_QueryTexture(texture, nullptr, nullptr, &mTextureWidth, &mTextureHeight);
}
#include "Actor/PGCharacter.h"
#include "Actor/PGViewport.h"

APGCharacter::~APGCharacter()
{
	if (World)
	{
		World->OnGameDifficultyChanged.Remove(this);
	}
}

sf::FloatRect APGCharacter::GetCharacterBounds()
{
	sf::RectangleShape* Shape = dynamic_cast<sf::RectangleShape*>(GetActorRenderTarget().Drawable);

	sf::Vector2f Pos = GetActorRenderTarget().Transform.transformPoint(0.f, 0.f);
	sf::Vector2f Size = Shape->getSize();

	return sf::FloatRect(Pos, Size);
}

void APGCharacter::BindWorldDelegates(APGViewport* InWorld)
{
	World = InWorld;
	if (World)
	{
		World->OnGameDifficultyChanged.Bind(this, &APGCharacter::HandleDifficultyChange);
	}
}

void APGCharacter::OnGameDifficultyChanged(const FGameDifficulty& GameDifficulty)
{

}

void APGCharacter::HandleDifficultyChange(const FGameDifficulty& GameDifficulty)
{
	OnGameDifficultyChanged(GameDifficulty);
}


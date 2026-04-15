#include "Actor/PGWidget.h"
#include "Actor/PGViewport.h"

std::string APGWidget::GetWidgetText() const
{
	sf::Text* RText = dynamic_cast<sf::Text*>(RenderText.Drawable);
	if (RText)
	{
		return RText->getString().toAnsiString();;
	}
	return std::string();
}

void APGWidget::SetHovered()
{
	if (!RenderTarget.Drawable)
	{
		return;
	}

	sf::RectangleShape* Slot = dynamic_cast<sf::RectangleShape*>(RenderTarget.Drawable);
	if (Slot)
	{
		Slot->setOutlineColor(HoverOutlineColor);
		Slot->setOutlineThickness(HoverOutlineThickness);
	}
}

void APGWidget::SetUnhovered()
{
	if (!RenderTarget.Drawable)
	{
		return;
	}

	sf::RectangleShape* Slot = dynamic_cast<sf::RectangleShape*>(RenderTarget.Drawable);
	if (Slot)
	{
		Slot->setOutlineColor(DefaultOutlineColor);
		Slot->setOutlineThickness(DefaultOutlineThickness);
	}
}

void APGWidget::OnMouseEvent(EMouseInput::I MouseInput, const sf::Vector2f CachedGeometry)
{
	if (MouseInput == INDEX_NONE)
	{
		return;
	}

	if (MouseInput != EMouseInput::I::MouseButtonClick && MouseInput != EMouseInput::I::MouseButtonReleased)
	{
		return;
	}

	const sf::RectangleShape* Slot = dynamic_cast<const sf::RectangleShape*>(RenderTarget.Drawable);
	if (!Slot)
	{
		return;
	}

	sf::FloatRect LocalSize = Slot->getLocalBounds();
	sf::FloatRect Transform = RenderTarget.Transform.transformRect(LocalSize);

	if (bIsFocusable && RenderTarget.bRenderOpacity && Transform.contains(CachedGeometry))
	{
		OnMouseReleased.Broadcast(this);
		SetHovered();
	}
}


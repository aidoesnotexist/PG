#include "Actor/PGPawn.h"
#include "Core/Components/TDelegate.h"

class APGViewport;

/** Widgets should not be actors. They are independent instances that
	either are tied to actors or user viewport. Not enough time to
	implement it fully corrected, but here widgets are pawns to recieve inputs */
class APGWidget : public APGPawn
{
public:
	virtual ~APGWidget() = default;

	virtual void Tick(const float Delta) override {}

	TDelegate<APGWidget*> OnMouseReleased;

	std::string GetWidgetText() const;

	void SetHovered();
	void SetUnhovered();

	bool bIsFocusable = true;

protected:
	friend TStrongPtr<APGWidget>;
	APGWidget() = default;

	virtual void OnMouseEvent(EMouseInput::I MouseInput, const sf::Vector2f CachedGeometry) override;

	//Hardcoded hovers
	sf::Color DefaultOutlineColor = sf::Color::Black;
	sf::Color HoverOutlineColor = sf::Color::White;

	float DefaultOutlineThickness = 1.f;
	float HoverOutlineThickness = 5.f;
};
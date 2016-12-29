#include "Command.h"

bool Command::init()
{
	if (!Node::init())
		return false;

	// Unit command
	_unit_command = Sprite::create();
	_unit_command->setTextureRect(Rect(0, 0, 100, 400));
	_unit_command->setColor(Color3B::MAGENTA);
	_unit_command->setOpacity(0);
	this->addChild(_unit_command);
	for (auto name : { u8"•â‹‹", u8"ˆÚ“®", u8"UŒ‚", u8"è—Ì", u8"«”\", u8"‘Ò‹@" })
	{
		static Label* preItem = nullptr;
		auto item = Label::createWithSystemFont(name, JP_FONT, INFO_SIZE + 10);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		item->setColor(Color3B::BLACK);
		item->setPosition(_unit_command->getTextureRect().getMaxX() - 10, preItem ? preItem->getPosition().y - 60 : _unit_command->getTextureRect().getMaxY() - 50);
		item->setTag(preItem ? preItem->getTag() + 1 : 10);
		_unit_command->addChild(item);
		preItem = item;
	}

	// City command
	_city_command = Sprite::create();
	_city_command->setTextureRect(Rect(0, 0, 100, 150));
	_city_command->setColor(Color3B::MAGENTA);
	_city_command->setOpacity(0);
	this->addChild(_city_command);
	for (auto name : { u8"•â‹‹", u8"”z”õ" })
	{
		static Label* preItem = nullptr;
		auto item = Label::createWithSystemFont(name, JP_FONT, INFO_SIZE + 10);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		item->setColor(Color3B::BLACK);
		item->setPosition(_unit_command->getTextureRect().getMaxX() - 10, preItem ? preItem->getPosition().y - 60 : _unit_command->getTextureRect().getMaxY() - 50);
		item->setTag(preItem ? preItem->getTag() + 1 : 10);
		_unit_command->addChild(item);
		preItem = item;
	}
	return false;
}

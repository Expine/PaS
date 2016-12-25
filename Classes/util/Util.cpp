#include "Util.h"

std::string util::loadText(const std::string &file)
{
	return FileUtils::getInstance()->getStringFromFile(file);
}


std::vector<std::string> util::splitString(const std::string& input, char delim)
{
	std::string item;
	std::vector<std::string> result;
	for (char ch : input)
	{
		if (ch == delim)
		{
			result.push_back(item);
			item.clear();
		}
		else
		{
			item += ch;
		}
	}
	result.push_back(item);
	return result;
}

std::vector<std::string> util::splitFile(const std::string &file)
{
	// Result
	std::string item;
	std::vector<std::string> result;
	// for BOM
	bool first = true;
	bool bom = false;
	int bom_count = 0;

	for (char ch : loadText(file))
	{
		// BOM Check
		if (first)
		{
			if ((unsigned char)ch == 0xEF)
				bom = true;
			first = false;
		}
		if (bom && bom_count++ < 3)
			continue;

		// '\r' is skip
		if (ch == '\r')
			continue;

		if (ch == '\n')
		{
			result.push_back(item);
			item.clear();
		}
		else
		{
			item += ch;
		}
	}

	result.push_back(item);
	return result;
}

/*
* Checking node range contains point by point and node data.
* down_x or y is extension of left or lower.
* append_x or y is extension of right or upper.
*/
bool util::isTouchInEvent(Vec2 point, Node* t, int down_x, int down_y, int append_x, int append_y)
{
	//NULL check
	if (!t)
	{
		CCLOG("[ERROR] isInTocuh node is NULL");
		return false;
	}

	Rect targetBox;
	//if ignore anchorpoint, not multiple anchorpoint.
	if (t->isIgnoreAnchorPointForPosition())
		targetBox = Rect(t->getParent()->convertToWorldSpace(t->getPosition()).x - t->getContentSize().width - down_x,
			t->getParent()->convertToWorldSpace(t->getPosition()).y - t->getContentSize().height - down_y,
			t->getContentSize().width + append_x, t->getContentSize().height + append_y);
	else
		targetBox = Rect(t->getParent()->convertToWorldSpace(t->getPosition()).x - t->getContentSize().width * t->getAnchorPoint().x - down_x,
			t->getParent()->convertToWorldSpace(t->getPosition()).y - t->getContentSize().height * t->getAnchorPoint().y - down_y,
			t->getContentSize().width + append_x, t->getContentSize().height + append_y);
	return targetBox.containsPoint(point);
}

/*
* Checking event range contains touch point by touch and event data.
* down_x or y is extension of left or lower.
* append_x or y is extension of right or upper.
*/
bool util::isTouchInEvent(Touch* touch, Event* event, int down_x, int down_y, int append_x, int append_y)
{
	return isTouchInEvent(touch->getLocation(), event->getCurrentTarget(), down_x, down_y, append_x, append_y);
}

/*
* Checking node range contains point by point and node data.
*/
bool util::isTouchInEvent(Vec2 point, Node* t)
{
	return isTouchInEvent(point, t, 0, 0, 0, 0);
}

/*
* Checking event range contains point by point and event data.
*/
bool util::isTouchInEvent(Vec2 point, Event* event)
{
	return isTouchInEvent(point, event->getCurrentTarget());
};

/*
* Checking event range contains touch point by touch and event data.
*/
bool util::isTouchInEvent(Touch* touch, Event* event)
{
	return isTouchInEvent(touch->getLocation(), event->getCurrentTarget());
};

#include "Ai.h"

#include "entity/Entity.h"
#include "entity/Weapon.h"
#include "stage/Stage.h"
#include "stage/Tile.h"
#include "util/Util.h"

USING_NS_CC;


/*
 * Calculate center
 */
Vec2 BattleField::getCenter()
{
	if (_center != Vec2::ZERO)
		return _center;
	StageTile* preTile = nullptr;
	int count = 0;
	Vec2 center = Vec2::ZERO;
	for (auto tile : _tiles)
	{
		if (preTile && preTile->getPositionAsTile() == tile->getPositionAsTile())
			continue;
		center += tile->getPosition();
		count++;
		preTile = tile;
	}
	_center = center / count;
	return _center;
}

/*
 * Constructor
 */
PlayerAI::PlayerAI()
	: _map_max_length(-1.0f), _stage(nullptr)
	, _searce_range_correctly(100000.0f)
	, _search_range_of_battlefield(1.0f)
	, _battlefield_distance_dependence(0.1f), _battlefield_importance_dependence(0.05f)
	, _unit_distance_dependence(0.1f), _unit_importance_dependence(0.05f)
	, _city_distance_dependence(0.1f), _city_importance_dependence(0.05f)
	, _city_guard_number(1.0f), _city_guard_area(3)
	, _residual_force_dependence(1.0f), _residual_resource_dependence(1.0f), _supply_distance_dependence(1.0f), _supply_evaluation_level(1.0f)
	, _large_battle_priority(1.0f), _overwhelming_priority(1.0f), _battlefield_number_axis(0.004f), _battlefield_number_mid(0.1f)
	, _battlefield_number_coefficient(1.0f), _battlefield_situation_coefficient(1.0f), _battlefield_to_city_coefficient(1.0f)
	, _city_deploy_coefficient(1.0f), _city_weakness_coefficient(1.0f), _city_deployer_gradient_coefficient(1.0f), _city_deployer_coefficient(1.0f)
	, _city_to_battlefield_coefficient(1.0f), _city_supply_line_coefficient(1.0f), _city_deployer_min(0.0f), _city_to_capital_coefficient(1.0f)
	, _unit_weakness_coefficient(1.0f), _unit_durability_coefficient(1.0f), _unit_material_coefficient(1.0f), _unit_mobility_coefficient(1.0f)
	, _unit_searching_coefficient(1.0f), _unit_defence_coefficient(1.0f), _unit_attack_coefficient(1.0f), _unit_spec_coefficient(1.0f)
{
	for (int i = 0; i < static_cast<int>(EntityType::COUNT); i++)
		_unit_type_coefficient[static_cast<EntityType>(i)] = 1.0f;
}

/*
 * Create AI instance
 */
PlayerAI * PlayerAI::createAI(const std::string file)
{
	auto ai = PlayerAI::create();

	return ai;
}

/*
* Get route to field
*/
std::vector<StageTile*> PlayerAI::getRoute(Entity * unit, BattleField * field)
{
	return _stage->startRecursiveTileSearchByAstar(field->_tiles, unit->getPositionAsTile(), 100, unit->getType(), true, false, false, _searce_range_correctly);
}

/*
* Get route to enemy
*/
std::vector<StageTile*> PlayerAI::getRoute(Entity * unit, Entity * enemy)
{
	auto tile = _stage->getTiles(enemy->getPositionAsTile()).front();
	auto tiles = std::vector<StageTile*>();
	tiles.push_back(tile);
	return _stage->startRecursiveTileSearchByAstar(tiles, unit->getPositionAsTile(), 100, unit->getType(), true, false, false, _searce_range_correctly);
}

/*
* Get route to city
*/
std::vector<StageTile*> PlayerAI::getRoute(Entity * unit, City * city)
{
	auto tile = _stage->getTiles(city->getPositionAsTile()).front();
	auto tiles = std::vector<StageTile*>();
	tiles.push_back(tile);
	return _stage->startRecursiveTileSearchByAstar(tiles, unit->getPositionAsTile(), 100, unit->getType(), true, false, false, _searce_range_correctly);
}

/*
 * Get max length on map
 */
float PlayerAI::getMapMaxLength()
{
	if (_map_max_length > 0.0f)
		return _map_max_length;

	_map_max_length = Vec2(_stage->getMapSize().x * _stage->getChipSize().x, _stage->getMapSize().y * _stage->getChipSize().y).getLengthSq();
	return _map_max_length;
}

/*
 * Get capital
 */
City * PlayerAI::getCapital(Owner owner)
{
	if(_capital.count(owner) != 0)
		return _capital[owner];

	for (auto city : _stage->getCities(owner))
		if (util::instanceof<Capital>(city))
			_capital[owner] = city;
	return _capital[owner];
}

/*
 * Search battle field
 */
void PlayerAI::searchBattleField()
{
	auto units = _stage->getUnits(_owner);
	for (auto unit : units)
	{
		if (!unit->getReserved())
		{
			auto field = BattleField::create();
			for(auto tile : _stage->getTiles(unit->getPositionAsTile()))
				field->_tiles.push_back(tile);
			field->_units.push_back(unit);
			unit->setReserved(true);
			recursiveSearchForBattleField(unit, field);
			if (field->_units.size() > 1)
				_battle_fields.pushBack(field);
		}
	}
	for (auto unit : units)
		unit->setReserved(false);
}

/*
 * Recurssive search for battlefield
 */
void PlayerAI::recursiveSearchForBattleField(Entity * unit, BattleField* field)
{
	for (auto tile : _stage->startRecursiveTileSearch(unit->getPositionAsTile(), unit->getMobility() * _search_range_of_battlefield, unit->getType(), true, true))
	{
		// Add element to field
		if (std::find(field->_tiles.begin(), field->_tiles.end(), tile) == field->_tiles.end())
			field->_tiles.push_back(tile);
		auto enemy = _stage->getUnit(tile->getPositionAsTile());
		if (enemy && enemy->isSelectableEnemy(unit->getAffiliation()))
		{
			// Set searched
			if(enemy->getAffiliation() == _owner)
				enemy->setReserved(true);
			if (std::find(field->_units.begin(), field->_units.end(), enemy) == field->_units.end())
			{
				// Add element to field
				field->_units.push_back(enemy);
				// Recursive search
				recursiveSearchForBattleField(enemy, field);
			}
		}
	}
}

/*
 * Execute to decide action
 */
void PlayerAI::execute()
{
	// Evaluate
	evaluate();

	_executes.push_back([this] { for (auto unit : _stage->getUnits(_owner)) unit->setReserved(false); });

	//Protect city
	for (auto city : _stage->getCities(_owner))
		protectCity(city);

	//Act on battlefield
	for (auto field : _battle_fields)
		_executes.push_back(std::bind(&PlayerAI::actOnButtlefield, this, field));
//		actOnButtlefield(field);

	//Act not on battlefield
	auto units = _stage->getUnits(_owner);
	std::sort(units.begin(), units.end(), [this](const Entity* a, const Entity* b)
	{
		return a->getPosition() == b->getPosition() ? a->getMobility() < b->getMobility() : (a->getPosition() - getCapital(_owner)->getPosition()).getLengthSq() < (b->getPosition() - getCapital(_owner)->getPosition()).getLengthSq();
	});
	for (auto unit : units)
		if (!unit->getReserved())
			_executes.push_back(std::bind(&PlayerAI::actOnNotButtlefield, this, unit));
//			actOnNotButtlefield(unit);

}

/*
 * Protect city turn
 */
void PlayerAI::protectCity(City* city)
{
	// Search enemy
	auto tiles = _stage->startRecursiveTileSearch(city->getPositionAsTile(), _city_guard_area, EntityType::counter, true, true);
	auto enemies = std::vector<Entity*>();
	for (auto tile : tiles)
	{
		auto unit = _stage->getUnit(tile->getPositionAsTile());
		if (unit && unit->isSelectableEnemy(_owner))
			enemies.push_back(unit);
	}

	// Evaluate unit
	auto units = std::priority_queue<PointerEvaluation<Entity>, std::vector<PointerEvaluation<Entity>>, std::greater<PointerEvaluation<Entity>>>();
	for (auto unit : _stage->getUnits(_owner))
		units.push(PointerEvaluation<Entity>(unit, evaluateGuard(unit, city)));

	// Reserve unit or battle
	auto point = _city_eval[city] * _city_guard_number;
	while (point > 0.0f)
	{
		auto unit = units.top()._pointer;
		units.pop();
		point -= _unit_eval[unit];
		// If unit can attack enemy, do it
		for (auto enemy : enemies)
		{
			auto route = _stage->startRecursiveTileSearchByAstar(_stage->getTiles(enemy->getPositionAsTile()).front(), unit->getPositionAsTile(), unit->getMobility(), unit->getType());
			if (route.size() > 0)
			{
				// Dispatch
				if (unit->isDeployer())
					_stage->dispatchUnit(unit, city);
				_stage->moveUnit(unit, route);
				_stage->moveUnitPositionAsTile(route.back()->getPositionAsTile(), unit);
				unit->setReserved(true);
				break;
			}
		}
		// If not, reserve guard
		if (unit->getState() != EntityState::acted)
		{
			_city_protect_units[city].push_back(unit);
			unit->setReserved(true);
		}
	}
}

/*
 * Act on battlefield
 */
void PlayerAI::actOnButtlefield(BattleField * field)
{
	for (auto unit : field->_units)
	{
		if (unit->getReserved())
			continue;
		float supply = evaluateSupply(unit);
		float ad_battle = evaluateBattleFieldAdvance(unit);
		float ad_city = evaluateCityAdvance(unit);
		float ad_unit = evaluateUnitAdvance(unit);

		if (supply > ad_battle && supply > ad_city && supply > ad_unit)
			goToCity(unit, _supply_city[unit], true, false);
		else if (ad_city > ad_battle && ad_city > ad_unit)
			goToCity(unit, _advance_city[unit], false, true);
		else if (ad_unit > ad_battle)
			goToUnit(unit, _advance_unit[unit]);
		else if (_advance_battlefield[unit] == field)
			attackNearBy(unit, field);
		else
			goToBattleField(unit, _advance_battlefield[unit]);
	}
}

/*
 * Unit on not battlefield act
 */
void PlayerAI::actOnNotButtlefield(Entity * unit)
{
	float supply = evaluateSupply(unit);
	float ad_battle = evaluateBattleFieldAdvance(unit);
	float ad_city = evaluateCityAdvance(unit);
	float ad_unit = evaluateUnitAdvance(unit);

	if (supply > ad_battle && supply > ad_city && supply > ad_unit)
		goToCity(unit, _supply_city[unit], true, false);
	else if (ad_city > ad_battle && ad_city > ad_unit)
		goToCity(unit, _advance_city[unit], false, true);
	else if (ad_unit > ad_battle)
		goToUnit(unit, _advance_unit[unit]);
	else
		goToBattleField(unit, _advance_battlefield[unit]);

}

/*
 * Unit go to battlefield
 */
void PlayerAI::goToBattleField(Entity * unit, BattleField * field)
{
	// Dispatch
	if (unit->isDeployer())
		_stage->dispatchUnit(unit);
	auto route = _stage->startRecursiveTileSearchByAstar(field->_tiles, unit->getPositionAsTile(), unit->getMobility(), unit->getType(), true);
	_stage->moveUnit(unit, route);
	_stage->moveUnitPositionAsTile(route.back()->getPositionAsTile(), unit);
	unit->setReserved(true);
}

/*
 * Unit go to city
 */
void PlayerAI::goToCity(Entity * unit, City * city, bool isSupply, bool isOccupy)
{
	// Dispatch
	if (unit->isDeployer())
		_stage->dispatchUnit(unit);
	auto route = isSupply ? _stage->startRecursiveTileSearchByAstar(_stage->getTiles(city->getPositionAsTile()).front(), unit->getPositionAsTile(), unit->getMobility(), unit->getType(), true) : _advance_city_route[unit];
	_stage->cutRouteByMobility(unit, route);
	if (route.size() > 1)
	{
		Vector<FiniteTimeAction*> acts;
		auto i = 0;
		for (auto t : route)
			if (i++ > 0)
				acts.pushBack(Sequence::create(
					MoveTo::create(0.5f, _stage->getPositionByTile(t->getPositionAsTile()) + Vec2(_stage->getChipSize().x / 2, 0)),
					NULL));
		unit->runAction(Sequence::create(acts));
		_stage->moveUnitPositionAsTile(route.back()->getPositionAsTile(), unit, true);
		/**/
		_stage->moveUnit(unit, route);
		_stage->moveView(unit);
		unit->setReserved(true);
	}
}

/*
 * Unit go to unit
 */
void PlayerAI::goToUnit(Entity * unit, Entity * target)
{
	// Dispatch
	if (unit->isDeployer())
		_stage->dispatchUnit(unit);
	auto route = _stage->startRecursiveTileSearchByAstar(_stage->getTiles(target->getPositionAsTile()).front(), unit->getPositionAsTile(), unit->getMobility(), unit->getType(), true);
	_stage->moveUnit(unit, route);
	_stage->moveUnitPositionAsTile(route.back()->getPositionAsTile(), unit);
	unit->setReserved(true);
}

/*
 * Unit attack near enemy
 */
//TODO UNIMPLEMENT
void PlayerAI::attackNearBy(Entity * unit, BattleField * field)
{
}

/*
 * Start to evaluate
 */
void PlayerAI::evaluate()
{
	// Search battle field
	searchBattleField();

	// Calculate unit evaluation
	for(auto pair : _stage->getUnits())
		for(auto unit : pair.second)
			_unit_eval[unit] = evaluateUnit(unit);

	// Calculate city evaluation
	for(auto pair : _stage->getCities())
		for (auto city : pair.second)
			_city_eval[city] = evaluateCity(city);

	// Calculate battlefield situation
	for (auto field : _battle_fields)
		_battlefield_situation[field] = evaluateBattleFieldSituation(field);

	// Calculate battlefield evaluation
	for (auto field : _battle_fields)
		_battlefield_eval[field] = evaluateBattleField(field);

	/*
	for (auto pair : _unit_eval)
	{
		auto label = Label::createWithSystemFont(StringUtils::format("%f", pair.second), "Arial", 10);
		label->setColor(Color3B::BLACK);
		label->setPosition(pair.first->getPosition());
		_stage->addChild(label);
	}
	*/
	/*
	for (auto pair : _city_eval)
	{
		auto label = Label::createWithSystemFont(StringUtils::format("%f", pair.second), "Arial", 10);
		label->setColor(Color3B::BLUE);
		label->setPosition(pair.first->getPosition());
		_stage->addChild(label);
	}

	auto count = 0;
	for (auto pair : _battlefield_eval)
	{
		Color3B color = Color3B((count * 30 + 10)%255, (count * 40 + 80) % 255, (count * 50 + 150) % 255);
		for (auto tile : pair.first->_tiles)
			_stage->blinkTile(tile, color);
		pair.first->_color = color;

		auto label = Label::createWithSystemFont(StringUtils::format("%f", pair.second), "Arial", 10);
		label->setColor(color);
		label->setPosition(pair.first->getCenter());
		_stage->addChild(label);
		count++;
	}
	*/
}

/*
 * Evaluate supply point
 * [0, 1]
 * Target	: each unit
 * Variable	: residual force, residual resources, distance from it to near city
 * Requirement
 *			: There is negative correlation with the residual force.
 *			: There is negative correlation with the residual resources.
 *			: There is positive correlation with the distance.
 *			: If force and resources are both full, supply point is zero.
 *			: If distance is zero, supply point is zero.
 */
//TODO RE-THINKIN
float PlayerAI::evaluateSupply(Entity * unit)
{
	auto chipSize = _stage->getChipSize();
	float force = _residual_force_dependence * (1.0f - (float)unit->getDurability() / unit->getMaxDurability());
	float resource = _residual_resource_dependence * (1.0f - (float)unit->getMaterial() / unit->getMaxMaterial());
	float distance = getMapMaxLength();
	for (auto city : _stage->getCities(_owner))
	{
		auto diff = (city->getPosition() - unit->getPosition() + Vec2(chipSize.x / 2, 0)).getLengthSq();
		if (diff > distance)
		{
			distance = diff;
			_supply_city[unit] = city;
		}
	}
	distance /= getMapMaxLength();
	distance = pow(distance, 0.05f * _supply_distance_dependence);

	return (force > resource ? force : resource) * distance * _supply_evaluation_level;
}

/*
 * Evaluate battlefield advance point
 * [0, 1]
 * Target	: each unit
 * Variable	: importance of battlefield, distance from this unit to battlefield
 * Requirement
 *			: There is positive correlation with the importance.
 *			: There is negative correlation with the distance.
 */
float PlayerAI::evaluateBattleFieldAdvance(Entity * unit)
{
	// Near fields. range is about 316 pixcel
	auto near_fields = std::vector<PointerEvaluation<BattleField>>();
	auto point = 0.0f;

	// Add list and sort by length. And filter by length
	for (auto field : _battle_fields)
		near_fields.push_back(PointerEvaluation<BattleField>(field, (field->getCenter() - unit->getPosition()).getLengthSq()));
	std::sort(near_fields.begin(), near_fields.end());
	while (near_fields.size() > 3 && (near_fields.size() > 5 ||  near_fields.back()._eval > _searce_range_correctly))
		near_fields.pop_back();

	auto max = _stage->getMapSize().x + _stage->getMapSize().y;
	for (auto field : near_fields)
	{
		auto route = getRoute(unit, field._pointer);
		auto x = route.size() / max;
		float n = log(_battlefield_eval[field._pointer]) / log(0.5);
		auto comp = pow((1.0f - pow(x, _battlefield_distance_dependence)), n) * pow(_battlefield_eval[field._pointer], _battlefield_importance_dependence);
		if (comp > point)
		{
			point = comp;
			_advance_battlefield[unit] = field._pointer;
			_advance_battlefield_route[unit] = route;
		}
	}
	return point;
}

/*
 * Evaluate unit advance point (not on battlefield)
 * [0, 1]
 * Target	: each unit
 * Variable	: importance of enemy unit, distance from this unit to enemy unit
 * Requirement
 *			: There is positive correlation with the importance.
 *			: There is negative correlation with the distance.
 */
float PlayerAI::evaluateUnitAdvance(Entity * unit)
{
	// Near units. range is about 316 pixcel
	auto near_units = std::vector<PointerEvaluation<Entity>>();
	auto point = 0.0f;

	// Add list and sort by length. And filter by length
	for (auto pair : _stage->getUnits())
		for(auto enemy : pair.second)
			if (enemy->isSelectableEnemy(_owner))
				near_units.push_back(PointerEvaluation<Entity>(enemy, (enemy->getPosition() - unit->getPosition()).getLengthSq()));
	std::sort(near_units.begin(), near_units.end());
	while (near_units.size() > 1 && (near_units.size() > 3 || near_units.back()._eval > _searce_range_correctly))
		near_units.pop_back();

	auto max = _stage->getMapSize().x + _stage->getMapSize().y;
	for (auto enemy : near_units)
	{
		auto route = getRoute(unit, enemy._pointer);
		auto x = route.size() / max;
		float n = log(_unit_eval[enemy._pointer]) / log(0.5);
		auto comp = pow((1.0f - pow(x, _unit_distance_dependence)), n) * pow(_unit_eval[enemy._pointer], _unit_importance_dependence);
		if (comp > point)
		{
			point = comp;
			_advance_unit[unit] = enemy._pointer;
			_advance_unit_route[unit] = route;
		}
	}
	return point;
}

/*
 * Evaluate city advance point
 * [0, 1]
 * Target	: each unit
 * Variable	: importance of city, distance from this unit to city
 * Requirement
 *			: There is positive correlation with the importance.
 *			: There is negative correlation with the distance.
 */
float PlayerAI::evaluateCityAdvance(Entity * unit)
{
	// Near cities. range is about 316 pixcel
	auto near_cities = std::vector<PointerEvaluation<City>>();
	auto point = 0.0f;

	// Add list and sort by length. And filter by length
	for (auto pair : _stage->getCities())
		for (auto city : pair.second)
			if (!OwnerInformation::getInstance()->isSameGroup(city->getOwner(), _owner))
				near_cities.push_back(PointerEvaluation<City>(city, (city->getPosition() - unit->getPosition()).getLengthSq()));
	std::sort(near_cities.begin(), near_cities.end());
	while (near_cities.size() > 1 && (near_cities.size() > 3 || near_cities.back()._eval > _searce_range_correctly))
		near_cities.pop_back();

	auto max = _stage->getMapSize().x + _stage->getMapSize().y;
	for (auto city : near_cities)
	{
		auto route = getRoute(unit, city._pointer);
		auto x = route.size() / max;
		float n = log(_city_eval[city._pointer]) / log(0.5);
		auto comp = pow((1.0f - pow(x, _city_distance_dependence)), n) * pow(_city_eval[city._pointer], _city_importance_dependence);

		if (comp > point)
		{
			point = comp;
			_advance_city[unit] = city._pointer;
			_advance_city_route[unit] = route;
		}
	}

	return point;
}

/*
 * Evaluate guard point
 * Target : unit
 * Variable : distance to city
 * Requirement
 *			: There is negative correlation with the distance
 */
float PlayerAI::evaluateGuard(Entity * unit, City* city)
{
	auto distance = (unit->getPosition() - city->getPosition()).getLengthSq();

	return 1.0f - distance / getMapMaxLength();
}

/*
 * Evaluate battlefield importance
 * [0, 1]
 * Target	: each battlefield
 * Variable	: all unit number, enemy number, ally number,
 *			: all unit importance, enemy unit importance, ally unit importance
 *			: battlefield situation, battlefield situation transition grade
 *			: distance to city and it's importance, distance to other battlefield and it's importance
 * Requirement
 *			: There is positive correlation with the all unit / enemy / ally number , as long as the number is large enough
 *			: There is negative correlation with the all unit / enemy / ally number , as long as the number is small enough
 *				: This is a convex downward function.
 *			: There is positive correlation with the importance.
 *			: There is positive correlation with the battlefield situation (transition), as long as the situation or transition is little bettter.
 *			: There is negative correlation with the battlefield situation (transition), as long as the situation or transition is little worse.
 *				: This is a convex upward function.
 *			: Thres is negative correlation with the distance and positive correlation with the it's importance.
 */
constexpr int GRADIENT = 100000;
float PlayerAI::evaluateBattleField(BattleField * field)
{
	auto sum = _battlefield_number_coefficient + _battlefield_situation_coefficient + _battlefield_to_city_coefficient;

	auto unitProprtion = 0.0f;
	for (auto unit : field->_units)
		unitProprtion += _unit_eval[unit];
	unitProprtion /= _stage->getUnitNumber();
	if (unitProprtion < _battlefield_number_axis)
	{
		auto x = (unitProprtion - _battlefield_number_axis);
		unitProprtion = (_overwhelming_priority - _battlefield_number_mid) / (_battlefield_number_axis * _battlefield_number_axis) * x * x + _battlefield_number_mid;
	}
	else
	{
		auto e = exp(_large_battle_priority - _battlefield_number_mid);
		unitProprtion = log(GRADIENT * (unitProprtion - _battlefield_number_axis) * (e - 1) / (1 - _battlefield_number_axis) + 1) / log(GRADIENT * (e - 1) + 1) * (_large_battle_priority - _battlefield_number_mid) + _battlefield_number_mid;
	}
	unitProprtion *= _battlefield_number_coefficient;

	auto situation = _battlefield_situation_coefficient * abs(_battlefield_situation[field]);

	auto city_distance = 0;
	for (auto city : _stage->getCities(_owner))
	{
		auto diff = (getMapMaxLength() - (city->getPosition() - field->getCenter()).getLengthSq()) * _city_eval[city];
		city_distance = (city_distance < diff) ? diff : city_distance;
	}
	city_distance *= _city_to_battlefield_coefficient;
	city_distance /= getMapMaxLength();

	return (unitProprtion + situation + city_distance) / sum;
}

/*
 * Evaluate battlefield situation
 * [-1, 1] positive is good, nagative is bad
 * Target : each battlefield
 * Variable : unit number and spec, enemy ones vs ally ones
 *			: unit balance of anemy or ally
 */
float PlayerAI::evaluateBattleFieldSituation(BattleField * field)
{
	auto ally = 0.0f;
	auto enemy = 0.0f;
	for (auto unit : field->_units)
		if (OwnerInformation::getInstance()->isSameGroup(unit->getAffiliation(), _owner))
			ally += _unit_eval[unit];
		else
			enemy += _unit_eval[unit];
	auto force_diff = (ally - enemy) / _stage->getUnitNumber();
	if(force_diff > 0.0)
		force_diff = pow(force_diff, 0.12);
	else
		force_diff = -pow(-force_diff, 0.12);

	return force_diff;
}

/*
 * Evaluate city importance
 * [0, 1]
 * Target	: each city
 * Variable	: city spec, city weakness
 *			: deployers of city
 *			: the minimum of the function of each battlefield importance and perpendicular drawn to line from each battlefield to capital
 *			: distance to battlefield
 *			: distance to own capital
 * Requirement
 *			: There is positive correlation with the spec.
 *			: There is positive correlation with the weakness.
 *			: There is posi/nagative correlation with the deployers.
 *			: There is negative correlation with the perpendicular.
 *			: There is negative correlation with the battlefield.
 *			: There is negative correlation with the capital.
 */
float PlayerAI::evaluateCity(City * city)
{
	auto sum = _city_deployer_coefficient + _city_deploy_coefficient + _city_weakness_coefficient + _city_supply_line_coefficient;

	auto deploy_spec = _city_deploy_coefficient * city->getMaxDeployer() / CITY_DEPLOY_MAX;
	auto weakness = _city_weakness_coefficient * city->getDurability() / city->getMaxDurability();

	auto proportion = 0.0f;
	for (auto unit : city->getDeployersByRef())
		proportion += evaluateUnitSpec(unit);
	proportion /= city->getMaxDeployer();
	float deployers;
	if(_city_deployer_gradient_coefficient > 0.0)
		deployers = pow(proportion, _city_deployer_gradient_coefficient) * (1 - _city_deployer_min) + _city_deployer_min;
	else
		deployers = pow(proportion, -_city_deployer_gradient_coefficient) * (_city_deployer_min - 1) + 1;

	auto perpendicular = getMapMaxLength();
	for (auto field : _battle_fields)
	{
		auto av = getCapital(city->getOwner())->getPosition() - city->getPosition();
		auto bv = field->getCenter() - city->getPosition();
		auto lv = getCapital(city->getOwner())->getPosition() - field->getCenter();
		auto a = av.dot(lv);
		auto b = bv.dot(lv);
		if (a >= 0 && b <= 0)
		{
			auto tv = (b * av + a * bv) / (a - b);
			auto t = tv.getLengthSq();
			perpendicular = (perpendicular > t) ? t : perpendicular;
		}
	}
	perpendicular *= _city_supply_line_coefficient;
	perpendicular /= getMapMaxLength();
	perpendicular = 1.0f - perpendicular;

	return (deploy_spec + weakness + deployers + perpendicular) / sum;
}

/*
 * Evaluate city guard
 * [0, 1]
 * Target : each city
 * Variable : unit on city, unit near city
 * Requirement
 *			: There is positive correlation with the unit spec
 *			: There is negative correlation with the distance of unit
 */
float PlayerAI::evaluateCityGuard(City * city)
{
	return 0.0f;
}

/*
 * Evaluate unit importance
 * [0, 1]
 * Target	: each unit
 * Variable	: unit spec, unit weakness
 * Requirement
 *			: There is positive correlation with the spec.
 *			: There is positive correlation with the weakness.
 */
float PlayerAI::evaluateUnit(Entity * unit)
{
	auto sum = _unit_weakness_coefficient + _unit_spec_coefficient;

	auto weakness = _unit_weakness_coefficient * (1.0f - unit->getDurability() / unit->getMaxDurability());
	auto spec = _unit_spec_coefficient * evaluateUnitSpec(unit);

	return (spec + weakness) / sum;
}

float PlayerAI::evaluateUnitSpec(Entity * unit)
{
	auto sum = _unit_durability_coefficient + _unit_material_coefficient + _unit_mobility_coefficient
		+ _unit_defence_coefficient + _unit_searching_coefficient + _unit_attack_coefficient;
	auto durability = _unit_durability_coefficient * unit->getMaxDurability() / UNIT_MAX_DURABILITY;
	auto material = _unit_material_coefficient * unit->getMaterial() / UNIT_MAX_MATERIAL;
	auto mobility = _unit_mobility_coefficient * unit->getMobility() / UNIT_MAX_MOBILITY;
	auto defence = _unit_defence_coefficient * unit->getDefence() / UNIT_MAX_DEFENCE;
	auto searching = _unit_searching_coefficient * unit->getSearchingAbility() / UNIT_MAX_SEARCHING;
	float attack = 0.0f;
	for (auto weapon : unit->getWeaponsByRef())
	{
		auto atk = (float)weapon->getMaxAttack() / WEAPON_MAX_ATTACK;
		attack = (attack > atk) ? attack : atk;
	}
	return _unit_type_coefficient[unit->getType()] * (durability + material + mobility + defence + searching + attack) / sum;
}


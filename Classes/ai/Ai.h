#ifndef __AI_H__
#define __AI_H__

#include "cocos2d.h"

class City;
class Entity;
class StageTile;
class Stage;
enum class Owner;
enum class EntityType;

class BattleField : public cocos2d::Ref
{
private:
	cocos2d::Vec2 _center;
protected:
	BattleField() : _center(cocos2d::Vec2::ZERO) {};
	virtual bool init() { return true; };
public:
	CREATE_FUNC(BattleField);
	std::vector<StageTile*> _tiles;
	std::vector<Entity*> _units;
	cocos2d::Color3B _color;
	cocos2d::Vec2 getCenter();
};

template<typename T>
class PointerEvaluation
{
public:
	PointerEvaluation(T* field, float eval) { _pointer = field; _eval = eval; };
	T* _pointer;
	float _eval;
	inline bool operator < (const PointerEvaluation<T>& comp) const
	{
		return _eval < comp._eval;
	};
	inline bool operator > (const PointerEvaluation<T>& comp) const
	{
		return _eval > comp._eval;
	};
};

class PlayerAI : public cocos2d::Ref
{
private:
	Stage* _stage;
	cocos2d::Vector<BattleField*> _battle_fields;
	float _map_max_length;
	std::map<Owner, City*> _capital;
	std::map<Entity*, float> _unit_eval;
	std::map<City*, float> _city_eval;
	std::map<BattleField*, float> _battlefield_situation;
	std::map<BattleField*, float> _battlefield_eval;
	std::map<Entity*, City*> _supply_city;
	std::map<Entity*, BattleField*> _advance_battlefield;
	std::map<Entity*, std::vector<StageTile*>> _advance_battlefield_route;
	std::map<Entity*, City*> _advance_city;
	std::map<Entity*, std::vector<StageTile*>> _advance_city_route;
	std::map<Entity*, Entity*> _advance_unit;
	std::map<Entity*, std::vector<StageTile*>> _advance_unit_route;

	std::map<City*, std::vector<Entity*>> _city_protect_units;

	std::vector<std::function<void()>> _executes;

public:
	BattleField* getAdvanceBattleField(Entity* unit) { return _advance_battlefield[unit]; };
	City* getAdvanceCity(Entity* unit) { return _advance_city[unit]; };
	Entity* getAdvanceUnit(Entity* unit) { return _advance_unit[unit]; };
	float getCityEval(City* city) { return _city_eval[city]; };
	float getBattleFieldEval(BattleField* field) { return _battlefield_eval[field]; };
	int getExecutesNumber() { return _executes.size(); };
protected:
	PlayerAI();
	~PlayerAI()
	{};
	virtual bool init() { return true; };
public:
	CREATE_FUNC(PlayerAI);
	CC_SYNTHESIZE(Owner, _owner, Owner);

	/** [0, ��] search range correclty area. Default value is 100000.0f.*/
	CC_SYNTHESIZE(float, _searce_range_correctly, SearceRangeCorrectly);
	/** Battlefield Decision */
	/** [0, ��] ally search coefficient. Default value is 1.0f. It means movable area equals battlefields*/
	CC_SYNTHESIZE(float, _search_range_of_battlefield, SearchRangeOfBattlefield);

	/** Supply Evaluation */
	/** [0, 1] residual force coefficient. Default value is 1.0f.*/
	CC_SYNTHESIZE(float, _residual_force_dependence, ResidualForceDependence);
	/** [0, 1] residual resource coefficient. Default value is 1.0f.*/
	CC_SYNTHESIZE(float, _residual_resource_dependence, ResidualResourceDependence);
	/** [0, 1] distance from this unit to near city coefficient. Default value is 1.0f.*/
	CC_SYNTHESIZE(float, _supply_distance_dependence, SupplyDistanceDependence);
	/** [0, 1] All supply evaluation coefficient. Default value is 1.0f.*/
	CC_SYNTHESIZE(float, _supply_evaluation_level, SupplyEvaluationLevel);

	/** Battlefield Advance Evaluation */
	/** [0, 1] dependence of distance. Default value is 0.1.*/
	CC_SYNTHESIZE(float, _battlefield_distance_dependence, BattleFieldDistanceDependence);
	/** [0, 1] dependence of important. Default value is 0.05.*/
	CC_SYNTHESIZE(float, _battlefield_importance_dependence, BattleFieldImportantDependence);
	CC_SYNTHESIZE(float, _battlefield_evaluation_level, BattlefieldEvaluationLevel);

	/** City Advance Evaluation */
	/** [0, 1] dependence of distance. Default value is 0.1.*/
	CC_SYNTHESIZE(float, _city_distance_dependence, CityDistanceDependence);
	/** [0, 1] dependence of important. Default value is 0.05.*/
	CC_SYNTHESIZE(float, _city_importance_dependence, CityImportantDependence);
	CC_SYNTHESIZE(float, _city_evaluation_level, CityEvaluationLevel);

	/** Unit Advance Evaluation */
	/** [0, 1] dependence of distance. Default value is 0.1.*/
	CC_SYNTHESIZE(float, _unit_distance_dependence, UnitDistanceDependence);
	/** [0, 1] dependence of important. Default value is 0.05.*/
	CC_SYNTHESIZE(float, _unit_importance_dependence, UnitImportantDependence);
	CC_SYNTHESIZE(float, _unit_evaluation_level, UnitEvaluationLevel);

	/** Guard Evaluation */
	/** [0, ��) number of unit for protecting city. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _city_guard_number, CityGuardNumber);
	/** [0, ��) area for protecting city. Default value is 3.*/
	CC_SYNTHESIZE(int, _city_guard_area, CityGuardArea);

	/** Battlefield Importance */
	/** [mid, 1] Priority of battlefield having large number unit. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _large_battle_priority, LargeBattlePriority);
	/** [mid, 1] Priority of battlefield having small number unit. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _overwhelming_priority, OverwhelmingPriority);
	/** [0, 1) Value of battlefield having mid number unit. Default value is 0.1.*/
	CC_SYNTHESIZE(float, _battlefield_number_mid, BattleFieldNumberMid);
	/** [0, 1] Threshold of whether large or small. It depends unit number and spec. Default value is 0.004.*/
	CC_SYNTHESIZE(float, _battlefield_number_axis, BattleFieldNumberAxis);
	/** [0, 1] number of unit coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _battlefield_number_coefficient, BattleFieldNumberCoefficient);
	/** [0, 1] situation coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _battlefield_situation_coefficient, BattleFieldSituationCoefficient);
	CC_SYNTHESIZE(float, _tenacity, Tenacity);
	CC_SYNTHESIZE(float, _annihilation, Annihilation);
	/** [0, 1] distance to city coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _battlefield_to_city_coefficient, BattleFieldToCityCoefficient);
	CC_SYNTHESIZE(float, _battlefield_importance_priority, BattlefieldImportancePriority);

	/** City Importance */
	/** [0, 1] deploy of city coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _city_deploy_coefficient, CityDeployCoefficient);
	/** [0, 1] weakness of city coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _city_weakness_coefficient, CityWeaknessCoefficient);
	/** [0, 1] deployer of city coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _city_deployer_coefficient, CityDeployerGrkadientCoefficient);
	/** [-1, 1] deployer of city gradient coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _city_deployer_gradient_coefficient, CityDeployerGradientCoefficient);
	/** [0, 1) deployer of city min. Default value is 0.0.*/
	CC_SYNTHESIZE(float, _city_deployer_min, CityDeployerMin);
	CC_SYNTHESIZE(float, _city_to_battlefield_coefficient, CityToBattlefieldCoefficient);
	/** [0, 1] deployer of supply line coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _city_supply_line_coefficient, CitySupplyLineCoefficient);
	/** [0, 1] deployer of distance to capital coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _city_to_capital_coefficient, CityToCapitalCoefficient);

	/** Unit Importance */
	/** [0, 1] weakness of unit coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _unit_weakness_coefficient, UnitWeaknessCoefficient);
	/** [0, 1] spec of unit coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _unit_spec_coefficient, UnitSpecCoefficient);
	/** [0, 1] durability of unit for calculating spec coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _unit_durability_coefficient, UnitDurabilityCoefficient);
	/** [0, 1] material of unit for calculating spec coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _unit_material_coefficient, UnitMaterialCoefficient);
	/** [0, 1] mobility of unit for calculating spec coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _unit_mobility_coefficient, UnitMobilityCoefficient);
	/** [0, 1] searching of unit for calculating spec coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _unit_searching_coefficient, UnitSearchingCoefficient);
	/** [0, 1] defence of unit for calculating spec coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _unit_defence_coefficient, UnitDefenceCoefficient);
	/** [0, 1] attack of unit for calculating spec coefficient. Default value is 1.0.*/
	CC_SYNTHESIZE(float, _unit_attack_coefficient, UnitAttackCoefficient);
	/** [0, 1] type of unit for calculating spec coefficient. Default value is 1.0.*/
	std::map<EntityType, float> _unit_type_coefficient;
public:
	static PlayerAI* createAI(const std::string file);
	std::vector<StageTile*> getRoute(Entity* unit, BattleField* field);
	std::vector<StageTile*> getRoute(Entity* unit, Entity* enemy);
	std::vector<StageTile*> getRoute(Entity* unit, City* city);
	float getMapMaxLength();
	City* getCapital(Owner owner);
	inline void nextExecute() { _executes.back()(); _executes.pop_back(); };

	void initialize(Stage* stage, Owner owner) { _stage = stage; _owner = owner; };
	void searchBattleField();
	void recursiveSearchForBattleField(Entity* unit, BattleField* field);

	void execute();
	void protectCity(City* city);
	void actOnButtlefield(BattleField* field);
	void actOnNotButtlefield(Entity* unit);
	void goToBattleField(Entity* unit, BattleField* field);
	void goToCity(Entity* unit, City* city, bool isSupply, bool isOccupy);
	void goToUnit(Entity* unit, Entity* target);
	void attackNearBy(Entity* unit, BattleField* field);
	void evaluate();
	float evaluateSupply(Entity* unit);
	float evaluateBattleFieldAdvance(Entity* unit);
	float evaluateUnitAdvance(Entity* unit);
	float evaluateCityAdvance(Entity* unit);
	float evaluateGuard(Entity* unit, City* city);
	float evaluateBattleField(BattleField* field);
	float evaluateBattleFieldSituation(BattleField* field);
	float evaluateCity(City* city);
	float evaluateCityGuard(City* city);
	float evaluateUnit(Entity* unit);
	float evaluateUnitSpec(Entity* unit);
private:

};

#endif // __AI_H__

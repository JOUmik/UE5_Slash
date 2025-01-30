#pragma once
#define SLASH_CHARACTER_TAG FName("SlashCharacter")
#define ENEMY_TAG FName("Enemy")
#define Dead_TAG FName("Dead")

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon"),
	ECS_Disarm UMETA(DisplayName = "Disarm"),
	ECS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Equipping UMETA(DisplayName = "Equipping"),
	EAS_HitReacting UMETA(DisplayName = "HitReacting"),
	EAS_Dodging UMETA(DisplayName = "Dodging")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),
	EDP_Death6 UMETA(DisplayName = "Death6"),

	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),
	EES_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EPatrolMethod : uint8
{
	EPM_Circle UMETA(DisplayName = "Circle"),
	EPM_BackAndForth UMETA(DisplayName = "BackAndForth"),
	EPM_OneWay UMETA(DisplayName = "OneWay"),
	EPM_Random UMETA(DisplayName = "Random"),
};
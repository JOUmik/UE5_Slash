#include "AttackState.h"
#include "Characters/Player/SlashCharacter.h"
#include "InputActionValue.h"

AttackState::AttackState()
{
}

AttackState::~AttackState()
{
}

void AttackState::HandleInput(ASlashCharacter* Character, const FInputActionValue& Value)
{
	Character->Attack();
}

void AttackState::EnterState(ASlashCharacter* Character)
{
}

void AttackState::ExitState(ASlashCharacter* Character)
{
}

void AttackState::UpdateState(ASlashCharacter* Character, float DeltaTime)
{
}

#include "IdleState.h"
#include "Characters/Player/SlashCharacter.h"
#include "InputActionValue.h"

IdleState::IdleState()
{
}

IdleState::~IdleState()
{
}

void IdleState::HandleInput(ASlashCharacter* Character, const FInputActionValue& Value)
{
}

void IdleState::EnterState(ASlashCharacter* Character)
{
}

void IdleState::ExitState(ASlashCharacter* Character)
{
}

void IdleState::UpdateState(ASlashCharacter* Character, float DeltaTime)
{
	if (Character->GetGroundSpeed() != 0)
	{
		Character->ChangeState(Character->GetMoveState());
	}
}

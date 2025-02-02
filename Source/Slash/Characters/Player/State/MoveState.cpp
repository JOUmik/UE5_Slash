#include "MoveState.h"
#include "Characters/Player/SlashCharacter.h"
#include "InputActionValue.h"

MoveState::MoveState()
{
}

MoveState::~MoveState()
{
}

void MoveState::HandleInput(ASlashCharacter* Character, const FInputActionValue& Value)
{
}

void MoveState::EnterState(ASlashCharacter* Character)
{
}

void MoveState::ExitState(ASlashCharacter* Character)
{
}

void MoveState::UpdateState(ASlashCharacter* Character, float DeltaTime)
{
	if (Character->GetGroundSpeed() == 0)
	{
		Character->ChangeState(Character->GetIdleState());
	}
}

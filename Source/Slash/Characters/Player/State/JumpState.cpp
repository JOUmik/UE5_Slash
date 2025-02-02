#include "JumpState.h"
#include "Characters/Player/SlashCharacter.h"
#include "InputActionValue.h"

JumpState::JumpState()
{
}

JumpState::~JumpState()
{
}

void JumpState::HandleInput(ASlashCharacter* Character, const FInputActionValue& Value)
{
	Character->StartJump();
}

void JumpState::EnterState(ASlashCharacter* Character)
{
	
}

void JumpState::ExitState(ASlashCharacter* Character)
{
	Character->StopJump();
}

void JumpState::UpdateState(ASlashCharacter* Character, float DeltaTime)
{
}

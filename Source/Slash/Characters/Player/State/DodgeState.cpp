#include "DodgeState.h"
#include "Characters/Player/SlashCharacter.h"
#include "InputActionValue.h"

DodgeState::DodgeState()
{
}

DodgeState::~DodgeState()
{
}

void DodgeState::HandleInput(ASlashCharacter* Character, const FInputActionValue& Value)
{
	Character->Dodge();
}

void DodgeState::EnterState(ASlashCharacter* Character)
{
	
}

void DodgeState::ExitState(ASlashCharacter* Character)
{
}

void DodgeState::UpdateState(ASlashCharacter* Character, float DeltaTime)
{
}

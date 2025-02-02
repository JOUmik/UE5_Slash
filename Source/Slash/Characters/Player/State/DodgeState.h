#pragma once

#include "Interfaces/ICharacterState.h"

class SLASH_API DodgeState : public ICharacterState
{
public:
	DodgeState();
	virtual ~DodgeState() override;

	virtual void HandleInput(ASlashCharacter* Character, const FInputActionValue& Value) override;
	virtual void EnterState(ASlashCharacter* Character) override;
	virtual void ExitState(ASlashCharacter* Character) override;
	virtual void UpdateState(ASlashCharacter* Character, float DeltaTime) override;
};
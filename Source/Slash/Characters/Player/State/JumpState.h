#pragma once

#include "Interfaces/ICharacterState.h"

class SLASH_API JumpState : public ICharacterState
{
public:
	JumpState();
	virtual ~JumpState() override;

	virtual void HandleInput(const FInputActionValue& Value) override;
	virtual void EnterState(ASlashCharacter* Character) override;
	virtual void ExitState(ASlashCharacter* Character) override;
	virtual void UpdateState(ASlashCharacter* Character, float DeltaTime) override;
};

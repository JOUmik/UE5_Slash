#pragma once

#include "Interfaces/ICharacterState.h"

class SLASH_API IdleState : public ICharacterState
{
public:
	IdleState();
	virtual ~IdleState() override;

	virtual void HandleInput(const FInputActionValue& Value) override;
	virtual void EnterState(ASlashCharacter* Character) override;
	virtual void ExitState(ASlashCharacter* Character) override;
	virtual void UpdateState(ASlashCharacter* Character, float DeltaTime) override;
};

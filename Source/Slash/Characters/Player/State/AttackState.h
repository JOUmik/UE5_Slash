#pragma once

#include "Interfaces/ICharacterState.h"

class AttackState : public ICharacterState
{
public:
	AttackState();
	virtual ~AttackState() override;

	virtual void HandleInput(const FInputActionValue& Value) override;
	virtual void EnterState(ASlashCharacter* Character) override;
	virtual void ExitState(ASlashCharacter* Character) override;
	virtual void UpdateState(ASlashCharacter* Character, float DeltaTime) override;
};

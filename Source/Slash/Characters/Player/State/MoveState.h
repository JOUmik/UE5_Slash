#pragma once

#include "Interfaces/ICharacterState.h"

class SLASH_API MoveState : public ICharacterState
{
public:
	MoveState();
	virtual ~MoveState() override;

	virtual void HandleInput(const FInputActionValue& Value) override;
	virtual void EnterState(ASlashCharacter* Character) override;
	virtual void ExitState(ASlashCharacter* Character) override;
	virtual void UpdateState(ASlashCharacter* Character, float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	// 动画初始化
	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, CateGory = Movement, meta = (AllowPrivateAccess = "true"))
	/*射击角色类*/
	class AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, CateGory = Movement, meta = (AllowPrivateAccess = "true"))
	/* 角色速度 */
	float speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, CateGory = Movement, meta = (AllowPrivateAccess = "true"))
	/*是否在空中 */
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, CateGory = Movement, meta = (AllowPrivateAccess = "true"))
	/*是否加速中*/
	bool bIsAccelerating;
};

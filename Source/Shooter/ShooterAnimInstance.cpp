// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFrameWork\CharacterMovementComponent.h"
void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		// 速度向量
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		// 不要高度 也就是说 高度不影响动画速度
		Velocity.Z = 0;
		// 求向量速度
		speed = Velocity.Size();

		// 判断角色是否在空中
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
		// 判断是否在加速   GetCurrentAcceleration这个加速度竟然没考虑Z轴
		bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

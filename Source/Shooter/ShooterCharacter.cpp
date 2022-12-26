// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 创建一个 弹簧组件
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// 设置父级
	CameraBoom->SetupAttachment(RootComponent);
	// 设置摄像机距离角色300单位
	CameraBoom->TargetArmLength = 300.0f;
	// 使用控制器的旋转值
	CameraBoom->bUsePawnControlRotation = true;
	// 给相机弹簧增加一个偏移值
	CameraBoom->SocketOffset = FVector(0.f, 50.0f, 10.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = true; 

	// 不让character 跟随controller  让controller 只影响摄像机
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;   //第三章14节又给改为true 了
	bUseControllerRotationRoll = false;
	// 角色沿着输入方向移动
	GetCharacterMovement()->bOrientRotationToMovement = false;  // 第三章14节又给改为false 了
	// 设置旋转速率
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	// 设置跳跃高度
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	// 空中控制量
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("ShooterCharacter BeginPlay"));
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(FName("FireButton"), IE_Released, this, &AShooterCharacter::FireWeapon);
}

void AShooterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0, Rotation.Yaw, 0};
		const FVector Dir{FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)};
		AddMovementInput(Dir, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		// 取控制器方向
		const FRotator Rotation{Controller->GetControlRotation()};
		// 只要Y轴旋转
		const FRotator YawRotation{0, Rotation.Yaw, 0};
		// 怎么把角变成方向的 ?
		const FVector Dir{FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)};
		AddMovementInput(Dir, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * GetWorld()->DeltaTimeSeconds * BaseTurnRate);
}

void AShooterCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * GetWorld()->DeltaTimeSeconds * BaseLookUpRate);
}

void AShooterCharacter::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire Weapon"));
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName(FName("BarrelSocket"));
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}
		FHitResult HitResult;
		const FVector Start{SocketTransform.GetLocation()};
		const FQuat Rotation{SocketTransform.GetRotation()};
		const FVector RotationAxisX{Rotation.GetAxisX()};
		const FVector End{Start + RotationAxisX * 50000.0f};
		FVector BeamEnd{End};
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (HitResult.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red, false, 2);
			DrawDebugPoint(GetWorld(), HitResult.Location, 5, FColor::Red, false, 2);
			BeamEnd = HitResult.Location;
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* BeamParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), BeamParticles, SocketTransform);
			BeamParticleComponent->SetVectorParameter(FName("Target"), End);
		}
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location);
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
	// 简单的模拟一下后坐力
	//AddControllerPitchInput(-0.2f);
}

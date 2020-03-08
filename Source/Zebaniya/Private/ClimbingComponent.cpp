// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
//#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClimbAnimInstance.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UClimbingComponent::UClimbingComponent() : bCanTrace{false}, bIsClimbingLedge {false}, ForwardTraceResult {}, DownwardTraceResult {}, bIsHanging {false}, bCanMoveLeft {false}, bCanMoveRight {false}
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel12, ECollisionResponse::ECR_Block);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	OnComponentBeginOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapEnd);
	
	// ...
}


// Called when the game starts
void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = dynamic_cast<ACharacter*>(GetOwner());
	if (Owner) {
		Animation = Cast<UClimbAnimInstance>(Owner->GetMesh()->GetAnimInstance());
		if (GetWorld()->GetFirstPlayerController() == Owner->Controller) {
			GetWorld()->GetFirstPlayerController()->PushInputComponent(SetUpClimbingControllerForPlayer());
		}
		SetUpSideClimbingMovementDetection(LeftSphere, FVector{ 45,-60,90.f }, TEXT("ClimbLeftDetection"));
		SetUpSideClimbingMovementDetection(RightSphere, FVector{ 45,60,90.f }, TEXT("ClimbRightDetection"));
		LeftSphere->OnComponentBeginOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapBeginLeft);
		LeftSphere->OnComponentEndOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapEndLeft);
		RightSphere->OnComponentBeginOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapBeginRight);
		RightSphere->OnComponentEndOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapEndRight);
		auto DownwardTracePoint{ (Owner->GetMesh()->GetBoneLocation("head")) + (Owner->GetActorForwardVector() * ForwardTraceDistance) };
		DownwardTraceOffset = DownwardTracePoint - Owner->GetActorLocation();
	}
}

void UClimbingComponent::SetUpSideClimbingMovementDetection(USphereComponent*& Sphere, FVector LocalOffset, const FName Name)
{
	Sphere = NewObject<USphereComponent>(Owner, Name);
	Sphere->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules{ EAttachmentRule::KeepRelative, false });
	Sphere->RegisterComponent();
	Sphere->SetSphereRadius(SideSphereRadius, false);
	Sphere->AddLocalOffset(LocalOffset);
	Sphere->SetGenerateOverlapEvents(true);
	Sphere->SetCollisionProfileName(TEXT("Custom"));
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Sphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel12, ECollisionResponse::ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
}

// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Owner) { return; }
	if (bCanTrace && Owner->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling && (LetGoOfActor == nullptr || LetGoOfActor != ForwardTraceResult.Actor.Get()))
	{
		if (!DownwardSphereTrace()) {return; }
		if (!ForwardSphereTrace()) { return; }
		AttemptToGrabLedge();
	}
}

void UClimbingComponent::AttemptToGrabLedge()
{
	auto HeadLocation{ Owner->GetMesh()->GetBoneLocation("head") };
	auto JumpDistance{ DownwardTraceResult.ImpactPoint.Z - HeadLocation.Z };

	if (JumpDistance > -30.0f && JumpDistance < 60.0f)
	{
		auto StartVerticleNormal{ ForwardTraceResult.ImpactNormal };
		auto LedgeHight{ DownwardTraceResult.ImpactPoint };
		if (ClimbingInputController) {
			ClimbingInputController->bBlockInput = true;
		}
		bIsClimbingLedge = true;
		if (!Animation) { return; }
		Animation->GrabLedge(true);
		Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		bIsHanging = true;
		FLatentActionInfo Info{};
		Info.Linkage = 0;
		Info.ExecutionFunction = "StopMovementImmediately";
		Info.CallbackTarget = Owner->GetCharacterMovement();
		auto Rotation{ UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), ForwardTraceResult.ImpactPoint) };
		Rotation.Pitch = 0;
		UKismetSystemLibrary::MoveComponentTo(
			Owner->GetCapsuleComponent(),
				FVector{
					ForwardTraceResult.ImpactPoint.X + (ForwardTraceResult.ImpactNormal.X * (Owner->GetCapsuleComponent()->GetScaledCapsuleRadius())),
					ForwardTraceResult.ImpactPoint.Y + (ForwardTraceResult.ImpactNormal.Y * (Owner->GetCapsuleComponent()->GetScaledCapsuleRadius())),
					(DownwardTraceResult.ImpactPoint.Z - HeightOffset) },
				Rotation,
				false,
				false,
				0.2f,
				false,
				EMoveComponentAction::Move,
				Info);
	}
}


bool UClimbingComponent::DownwardSphereTrace() {
	auto StartPoint{ Owner->GetActorLocation() + Owner->GetActorRotation().RotateVector(DownwardTraceOffset) };
	auto EndPoint { StartPoint };
	EndPoint.Z -= HeightTrace;
	StartPoint.Z += HeightTrace;
	
	return UKismetSystemLibrary::SphereTraceSingle(
		GetOwner(),
		StartPoint, 
		EndPoint,
		SphereTraceRadius,
		ETraceTypeQuery::TraceTypeQuery16,
		true, TArray<AActor*>{},
		EDrawDebugTrace::ForOneFrame,
		DownwardTraceResult,
		true);
}

bool UClimbingComponent::ForwardSphereTrace()
{
	return UKismetSystemLibrary::SphereTraceSingle(
		GetOwner(),
		FVector{ DownwardTraceResult.ImpactPoint - (Owner->GetActorForwardVector() * ForwardTraceDistance) }, //start point
		DownwardTraceResult.ImpactPoint, //end point
		SphereTraceRadius,
		ETraceTypeQuery::TraceTypeQuery16,
		true,
		TArray<AActor*>{},
		EDrawDebugTrace::ForOneFrame,
		ForwardTraceResult,
		true);	
}

UInputComponent* UClimbingComponent::SetUpClimbingControllerForPlayer()
{	
	if (!Owner) { return nullptr;}
	static const FName InputComponentName(TEXT("PawnInputComponent1"));
	ClimbingInputController = NewObject<UInputComponent>(Owner, InputComponentName);
	ClimbingInputController->Priority = 10;
	ClimbingInputController->BindAction("CrouchAndLetGo", EInputEvent::IE_Released, this, &UClimbingComponent::LetGoOfLedge);
	ClimbingInputController->BindAction("Jump", EInputEvent::IE_Pressed, this, &UClimbingComponent::EnableLedgeGrabing);
	ClimbingInputController->BindAction("ClimbLeft", EInputEvent::IE_Pressed, this, &UClimbingComponent::ClimbLeft);
	ClimbingInputController->BindAction("ClimbRight", EInputEvent::IE_Pressed, this, &UClimbingComponent::ClimbRight);
	return ClimbingInputController;
	
}

void UClimbingComponent::FinishClimbInteractions()
{
	if (!Owner) { return; }
	Animation->GrabLedge(false);
	bIsHanging = false;
	bIsClimbingLedge = false;
	if (ClimbingInputController) {
		ClimbingInputController->bBlockInput = false;
	}
}

void UClimbingComponent::LetGoOfLedge()
{
	if (bIsHanging)
	{
		LetGoOfActor = DownwardTraceResult.Actor.Get();
		FinishClimbInteractions();
		Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	}
}

void UClimbingComponent::EnableLedgeGrabing()
{
	if (!bIsClimbingLedge) {
		if(!Owner) { return; }
		Owner->Jump();
		if(bCanTrace) { LetGoOfActor = nullptr;}
	}
	else {
		Animation->ClimbUp();
	}
}

void UClimbingComponent::ClimbUp()
{
	if (bIsHanging)
	{
		Animation->ClimbUp();
	}
}

void UClimbingComponent::FinishClimbUP()
{
	FinishClimbInteractions();
	Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UClimbingComponent::ClimbLeft()
{
	if (bIsHanging && bCanMoveLeft && Animation)
	{
		Animation->ClimbLeft();
	}
}

void UClimbingComponent::ClimbRight()
{
	if (bIsHanging && bCanMoveRight && Animation)
	{
		Animation->ClimbRight();
	}
}

void UClimbingComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bCanTrace = true;
}

void UClimbingComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsClimbingLedge = false;
	bCanTrace = false;
	ForwardTraceResult = FHitResult{};
	DownwardTraceResult = FHitResult{};
	LetGoOfActor = nullptr; 
}

void UClimbingComponent::OnOverlapBeginLeft(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bCanMoveLeft = true;
}

void UClimbingComponent::OnOverlapBeginRight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bCanMoveRight = true;
}

void UClimbingComponent::OnOverlapEndLeft(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCanMoveLeft = false;
}

void UClimbingComponent::OnOverlapEndRight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCanMoveRight = false;
}
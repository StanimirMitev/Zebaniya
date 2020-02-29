// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClimbAnimInstance.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UClimbingComponent::UClimbingComponent() : bCanTrace{false}, bIsClimbingLedge {false}, ForwardTraceResult {}, DownwardTraceResult {}, bIsHanging {false}
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	this->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel12, ECollisionResponse::ECR_Block);
	this->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	this->OnComponentBeginOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapBegin);
	this->OnComponentEndOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapEnd);
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
	}
}


// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Owner) { return; }
	if (bIsClimbingLedge && bCanTrace && Owner->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		if (!ForwardSphereTrace()) { return; }
		if (!TopSphereTrace()) {return; }
		Rest();
	}
}

void UClimbingComponent::Rest()
{
	auto HeadLocation{ Owner->GetMesh()->GetBoneLocation("head") };
	auto JumpDistance{ DownwardTraceResult.ImpactPoint.Z - HeadLocation.Z };

	if (JumpDistance > 0.0f && JumpDistance < 80.0f)
	{
		auto StartVerticleNormal{ ForwardTraceResult.ImpactNormal };
		auto LedgeHight{ DownwardTraceResult.ImpactPoint };
		if (ClimbingInputController) {
			//ClimbingInputController->Priority = 100;
			ClimbingInputController->bBlockInput = true;
		}
		bIsClimbingLedge = false;
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

bool UClimbingComponent::TopSphereTrace() {
	auto StartVerticle{ ForwardTraceResult.ImpactPoint };
	StartVerticle.Z += HeightTrace;
	//downwards trace
	return UKismetSystemLibrary::SphereTraceSingle(GetOwner(), StartVerticle, ForwardTraceResult.ImpactPoint, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery16, true, TArray<AActor*>{}, EDrawDebugTrace::ForOneFrame, DownwardTraceResult, true);
}

bool UClimbingComponent::ForwardSphereTrace()
{
	auto Start {Owner->GetActorLocation()};
	auto End{ (Owner->GetActorForwardVector()) * ForwardTraceDistance + Start};

	return UKismetSystemLibrary::SphereTraceSingle(GetOwner(), Start, End, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery16, true, TArray<AActor*>{}, EDrawDebugTrace::ForOneFrame, ForwardTraceResult, true);	
}

UInputComponent* UClimbingComponent::SetUpClimbingControllerForPlayer()
{	
	if (!Owner) { return nullptr;}
	static const FName InputComponentName(TEXT("PawnInputComponent1"));
	ClimbingInputController = NewObject<UInputComponent>(Owner, InputComponentName);
	//ClimbingInputController->bBlockInput = false;
	ClimbingInputController->Priority = 1;
	ClimbingInputController->BindAction("CrouchAndLetGo", EInputEvent::IE_Released, this, &UClimbingComponent::LetGoOfLedge);
	ClimbingInputController->BindAction("Jump", EInputEvent::IE_Pressed, this, &UClimbingComponent::EnableLedgeGrabing);
	return ClimbingInputController;
	
}

void UClimbingComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bCanTrace = true;
}

void UClimbingComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsClimbingLedge = false;
	bCanTrace = false;
}

void UClimbingComponent::LetGoOfLedge()
{
	if (!Owner) { return; }
	if (bIsHanging)
	{
		Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		Animation->GrabLedge(false);
		bIsHanging = false;
		bIsClimbingLedge = false;
		if (ClimbingInputController) {
			//ClimbingInputController->Priority = -100;
			ClimbingInputController->bBlockInput = false;
		}
	}
}

void UClimbingComponent::EnableLedgeGrabing()
{
	if(!Owner) { return; }
	Owner->Jump();
	bIsClimbingLedge = true;
}

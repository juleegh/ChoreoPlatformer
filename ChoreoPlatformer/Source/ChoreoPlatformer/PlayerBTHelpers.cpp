#include "PlayerBTHelpers.h"
#include "ComponentGetters.h"
#include "Kismet/GameplayStatics.h"
#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"

FVector UPlayerBTHelpersFunctionLibrary::GetPlayerInput(AActor* Player)
{
	auto DanceCharacter = Cast<ADanceCharacter>(Player);
	return DanceCharacter->GetCurrentInput();
}

bool UPlayerBTHelpersFunctionLibrary::CanMove(AActor* Player)
{
	auto PlayerController = ComponentGetters::GetController(Player->GetWorld());
	return PlayerController->CanMove();
}

bool UPlayerBTHelpersFunctionLibrary::IsOnTempo(AActor* Player)
{
	auto PlayerController = ComponentGetters::GetController(Player->GetWorld());
	return PlayerController->IsOnTempo();
}

AChoreoPlayerController* UPlayerBTHelpersFunctionLibrary::GetChoreoPlayerController(AActor* WorldContextActor)
{
	return ComponentGetters::GetController(WorldContextActor->GetWorld());
}
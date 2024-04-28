// Fill out your copyright notice in the Description page of Project Settings.


#include "ChoreoGameInstance.h"
#include "MoviePlayer.h"
#include "DancerUIComponent.h"
#include "TilemapLevelManager.h"
#include "ChoreoPlayerController.h"
#include "ComponentGetters.h"

void UChoreoGameInstance::Init()
{
    Super::Init();

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UChoreoGameInstance::BeginLoadingScreen);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UChoreoGameInstance::EndLoadingScreen);
}

void UChoreoGameInstance::BeginLoadingScreen(const FString& InMapName)
{
    /*
    if (!IsRunningDedicatedServer())
    {
        FLoadingScreenAttributes LoadingScreen;
        LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
        LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

        GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
    }
    */
}

void UChoreoGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
    if (!InLoadedWorld->GetName().Equals("GameIntro"))
    {
        ComponentGetters::GetDancerUIComponent(GetWorld())->GetGameUI()->LoadGame();
        if (auto SectionLevelManager = ComponentGetters::GetSectionLevelManager(GetWorld()))
        {
            SectionLevelManager->StartFromSection(CurrentLevel);
        }
        ComponentGetters::GetController(GetWorld())->CheckForCalibration();
    }
    else
    {
        ComponentGetters::GetDancerUIComponent(GetWorld())->GetGameUI()->LoadMenu();
    }
}


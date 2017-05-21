// Fill out your copyright notice in the Description page of Project Settings.

#include "CppProject5.h"
#include "MyActor.h"
#include "Engine/Engine.h"
#include "IMediaPlayer.h"
#include "FileMediaSource.h"
#include "IMediaModule.h"
#include "IMediaPlayerFactory.h"

#include "Runtime/MediaAssets/Public/MediaTexture.h"
#include "IMediaOutput.h"
#include "IMediaControls.h"
#include "IMediaTracks.h"

namespace
{
    struct MyMediaOptions
        : IMediaOptions
    {
        FName GetDesiredPlayerName() const override { return EName::NAME_None;  }
        bool GetMediaOption(const FName& Key, bool DefaultValue) const override { return DefaultValue; }
        double GetMediaOption(const FName& Key, double DefaultValue) const override { return DefaultValue; }
        int64 GetMediaOption(const FName& Key, int64 DefaultValue) const override { return DefaultValue; }
        FString GetMediaOption(const FName& Key, const FString& DefaultValue) const override { return DefaultValue; }
        FText GetMediaOption(const FName& Key, const FText& DefaultValue) const override { return DefaultValue; }
        bool HasMediaOption(const FName& Key) const override { return false; }
    };

}



// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ConstructorHelpers::FObjectFinder<UMediaTexture> finder(TEXT("/Game/FirstPerson/Media/MyNewMediaPlayer_Video"));
    texture_sink_ = finder.Object;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();

    //NotifyAboutPlayer();

    FString filename = TEXT("D:/vasya/sample.mp4");
    if (!FFileHelper::LoadFileToArray(video_bytes_, *filename))
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("File not found!"));
        return;
    }

    IMediaModule* MediaModule = FModuleManager::LoadModulePtr<IMediaModule>("Media");
    if (!MediaModule)
    {
        UE_LOG(LogTemp, Error, TEXT("No media module"));
        return;
    }

    FName factory_name(TEXT("WmfMedia"), FNAME_Find);
    auto factory = MediaModule->GetPlayerFactory(factory_name);

    if (!factory)
    {
        UE_LOG(LogTemp, Error, TEXT("No media factory"));
        return;
    }

    player_ = factory->CreatePlayer();

    if (!player_.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Can't create player"));
        return;
    }

    if (!texture_sink_)
    {
        UE_LOG(LogTemp, Error, TEXT("No texture sink"));
        return;
    }

    IMediaOutput &output = player_->GetOutput();
    output.SetVideoSink(texture_sink_);

    TSharedRef<FArchive, ESPMode::ThreadSafe> arch(new FMemoryReader(video_bytes_));
    auto size = arch->TotalSize();

    MyMediaOptions opts;

    player_->OnMediaEvent().AddLambda([this](EMediaEvent event)
    {
        if (event == EMediaEvent::TracksChanged)
        {
            auto &tracks = player_->GetTracks();
            tracks.SelectTrack(EMediaTrackType::Video, 0);
            int aaa = 5;

        }
        if (event == EMediaEvent::MediaOpened)
        {
            IMediaControls &controls = player_->GetControls();
            auto state = controls.GetState();

            if (!controls.Play())
            {
                UE_LOG(LogTemp, Error, TEXT("Play failed"));
                return;
            }        
        }
    });

    if (!player_->Open(arch, TEXT("sample.mp4"), opts))
    {
        UE_LOG(LogTemp, Error, TEXT("Open failed"));
        return;
    }





    
    //
//    
//    TSharedRef<FArchive, ESPMode::ThreadSafe> arch(new FMemoryReader(video_bytes_));
//
//    TSharedPtr<IMediaPlayer> native_player = mp_->GetBasePlayer().GetNativePlayer();
//
//    MyMediaOptions opts;
//    //native_player->Open(arch, filename, opts);
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (player_.IsValid())
        player_->TickPlayer(DeltaTime);
}

void AMyActor::NotifyAboutPlayer() const
{
//    FString msg;
//    FColor color;
//    if (mp_)
//    {
//        msg = "Found!";
//        color = FColor::Green;
//    }
//    else
//    {
//        msg = "Not found!";
//        color = FColor::Red;
//    }
//
//    GEngine->AddOnScreenDebugMessage(-1, 3.0f, color, msg);
}

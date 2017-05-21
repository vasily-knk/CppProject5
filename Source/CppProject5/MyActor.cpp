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


    class MyConsumerArchive : public FMemoryArchive
    {
    public:
        /**
        * Returns the name of the Archive.  Useful for getting the name of the package a struct or object
        * is in when a loading error occurs.
        *
        * This is overridden for the specific Archive Types
        **/
        FString GetArchiveName() const override { return TEXT("FMemoryReader"); }

        int64 TotalSize() override
        {
            return FMath::Min((int64)Bytes.Num(), LimitSize);
        }

        void Serialize(void* Data, int64 Num) override
        {
            if (!slept_)
            {
                //FPlatformProcess::Sleep(20.0f);
                slept_ = true;
            }

            if (Num && !ArIsError)
            {
                // Only serialize if we have the requested amount of data
                if (Offset + Num <= TotalSize())
                {
                    FMemory::Memcpy(Data, &Bytes[Offset], Num);
                    Offset += Num;
                }
                else
                {
                    ArIsError = true;
                }
            }
        }

        explicit MyConsumerArchive(const TArray<uint8>& InBytes, bool bIsPersistent = false)
            : FMemoryArchive()
            , Bytes(InBytes)
            , LimitSize(INT64_MAX)
        {
            ArIsLoading = true;
            ArIsPersistent = bIsPersistent;
        }

        /** With this method it's possible to attach data behind some serialized data. */
        void SetLimitSize(int64 NewLimitSize)
        {
            LimitSize = NewLimitSize;
        }

    protected:

        const TArray<uint8>& Bytes;
        int64 LimitSize;
        bool slept_ = false;
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

AMyActor::~AMyActor()
{

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();

    //NotifyAboutPlayer();

    FString filename = TEXT("D:/vasya/sample.mp4");
    if (!FFileHelper::LoadFileToArray(video_bytes_, *filename))
    {
        UE_LOG(LogTemp, Error, TEXT("File not found"));
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

    TSharedRef<FArchive, ESPMode::ThreadSafe> arch(new MyConsumerArchive(video_bytes_));
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

}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    if (player_.IsValid())
        player_->TickPlayer(0.01);
}


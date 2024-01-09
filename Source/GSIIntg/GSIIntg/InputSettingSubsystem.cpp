// Copyright (C) 2024 owoDra

#include "InputSettingSubsystem.h"

#include "GSIIntgLogs.h"

#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "PlayerMappableInputConfig.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InputSettingSubsystem)


void UInputSettingSubsystem::SetToDefaults()
{
    ControllerPlatform = NAME_None;
    ControllerPreset = FName(TEXT("Default"));

    InputConfigName = FName(TEXT("Default"));
    CustomKeyboardConfig = TMap<FName, FKey>();
}

void UInputSettingSubsystem::ApplySettings()
{
    auto* World{ GetWorld() };
    if (GEngine && World)
    {
        auto Players{ GEngine->GetGamePlayers(World) };
        for (const auto& Player : Players)
        {
            ApplyControllerPlatform(Player);
            ApplyCustomKeyboardBindings(Player);
        }
    }

    Super::ApplySettings();
}


void UInputSettingSubsystem::ApplyControllerPlatform(ULocalPlayer* LocalPlayer)
{
    if (auto* InputSubsystem{ UCommonInputSubsystem::Get(LocalPlayer) })
    {
        InputSubsystem->SetGamepadInputType(ControllerPlatform);
    }
}


void UInputSettingSubsystem::RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig, const bool bIsActive)
{
    if (NewConfig)
    {
        const auto ExistingConfigIdx
        {
            RegisteredInputConfigs.IndexOfByPredicate(
                [&NewConfig](const FLoadedMappableConfigPair& Pair)
                {
                    return Pair.Config == NewConfig;
                }
            )
        };
        
        if (ExistingConfigIdx == INDEX_NONE)
        {
            const auto NumAdded{ RegisteredInputConfigs.Add(FLoadedMappableConfigPair(NewConfig, Type, bIsActive)) };

            if (NumAdded != INDEX_NONE)
            {
                OnInputConfigRegistered.Broadcast(RegisteredInputConfigs[NumAdded]);
            }
        }
    }
}

int32 UInputSettingSubsystem::UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove)
{
    if (ConfigToRemove)
    {
        const auto Index
        {
            RegisteredInputConfigs.IndexOfByPredicate(
                [&ConfigToRemove](const FLoadedMappableConfigPair& Pair) 
                { 
                    return Pair.Config == ConfigToRemove; 
                }
            )
        };

        if (Index != INDEX_NONE)
        {
            RegisteredInputConfigs.RemoveAt(Index);

            return 1;
        }

    }

    return INDEX_NONE;
}

void UInputSettingSubsystem::GetRegisteredInputConfigsOfType(ECommonInputType Type, TArray<FLoadedMappableConfigPair>& OutArray) const
{
    OutArray.Empty();

    // If "Count" is passed in then 

    if (Type == ECommonInputType::Count)
    {
        OutArray = RegisteredInputConfigs;

        return;
    }

    for (const auto& Pair : RegisteredInputConfigs)
    {
        if (Pair.Type == Type)
        {
            OutArray.Emplace(Pair);
        }
    }
}

void UInputSettingSubsystem::GetAllMappingNamesFromKey(const FKey InKey, TArray<FName>& OutActionNames)
{
    if (InKey == EKeys::Invalid)
    {
        return;
    }

    // adding any names of actions that are bound to that key

    for (const auto& Pair : RegisteredInputConfigs)
    {
        if (Pair.Type == ECommonInputType::MouseAndKeyboard)
        {
            for (const auto& Mapping : Pair.Config->GetPlayerMappableKeys())
            {
                const auto MappingName{ FName(Mapping.PlayerMappableOptions.DisplayName.ToString()) };
                const auto ActionName{ Mapping.PlayerMappableOptions.Name };

                // make sure it isn't custom bound as well

                if (const auto* MappingKey{ CustomKeyboardConfig.Find(ActionName) })
                {
                    if (*MappingKey == InKey)
                    {
                        OutActionNames.Add(MappingName);
                    }
                }
                else
                {
                    if (Mapping.Key == InKey)
                    {
                        OutActionNames.Add(MappingName);
                    }
                }
            }
        }
    }
}

void UInputSettingSubsystem::AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey, ULocalPlayer* LocalPlayer)
{
    if (MappingName == NAME_None)
    {
        return;
    }

    static const auto NAME_CustomInputConfig{ FName(TEXT("Custom")) };

    if (InputConfigName != NAME_CustomInputConfig)
    {
        static const auto NAME_DefaultInputConfig{ FName(TEXT("Default")) };

        // Copy Presets.

        if (const auto* DefaultConfig{ GetInputConfigByName(NAME_DefaultInputConfig) })
        {
            for (const auto& Mapping : DefaultConfig->GetPlayerMappableKeys())
            {
                // Make sure that the mapping has a valid name, its possible to have an empty name
                // if someone has marked a mapping as "Player Mappabe" but deleted the default field value

                if (Mapping.PlayerMappableOptions.Name != NAME_None)
                {
                    CustomKeyboardConfig.Add(Mapping.PlayerMappableOptions.Name, Mapping.Key);
                }
            }
        }

        InputConfigName = NAME_CustomInputConfig;
    }

    // Change the key to the new one

    if (CustomKeyboardConfig.Contains(MappingName))
    {
        CustomKeyboardConfig[MappingName] = NewKey;
    }
    else
    {
        CustomKeyboardConfig.Add(MappingName, NewKey);
    }

    // Tell the enhanced input subsystem for this local player that we should remap some input

    if (auto* Subsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer) })
    {
        Subsystem->AddPlayerMappedKey(MappingName, NewKey);
    }

    MarkDirty();
}

void UInputSettingSubsystem::ResetKeybindingToDefault(const FName MappingName, ULocalPlayer* LocalPlayer)
{
    if (auto* Subsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer) })
    {
        Subsystem->RemovePlayerMappedKey(MappingName);
    }
}

void UInputSettingSubsystem::ResetKeybindingsToDefault(ULocalPlayer* LocalPlayer)
{
    if (auto* Subsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer) })
    {
        Subsystem->RemoveAllPlayerMappedKeys();
    }
}

void UInputSettingSubsystem::ApplyCustomKeyboardBindings(ULocalPlayer* LocalPlayer)
{
    if (auto* Subsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer) })
    {
        for (const auto& KVP : GetCustomPlayerInputConfig())
        {
            Subsystem->AddPlayerMappedKey(KVP.Key, KVP.Value);
        }
    }
}


const UPlayerMappableInputConfig* UInputSettingSubsystem::GetInputConfigByName(FName ConfigName) const
{
    for (const auto& Pair : RegisteredInputConfigs)
    {
        if (Pair.Config->GetConfigName() == ConfigName)
        {
            return Pair.Config;
        }
    }

    return nullptr;
}

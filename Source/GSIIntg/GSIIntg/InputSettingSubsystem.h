// Copyright (C) 2023 owoDra

#pragma once

#include "GSCSubsystem.h"

#include "MappableConfigPair.h"

#include "InputSettingSubsystem.generated.h"

class ULocalPlayer;


/**
 * Class that holds user key bindings and other settings
 */
UCLASS(configdonotcheckdefaults)
class GSIINTG_API UInputSettingSubsystem : public UGSCSubsystem
{
	GENERATED_BODY()
public:
	UInputSettingSubsystem() {}

protected:
	virtual FString GetCustomConfigIniName() const override { return FString(TEXT("UserInputSettings")); }

protected:
	virtual void SetToDefaults() override;
	virtual void ApplySettings() override;


public:
	DECLARE_EVENT_OneParam(UInputSettingSubsystem, FInputConfigDelegate, const FLoadedMappableConfigPair& /*Config*/);

	//
	// Delegate called when a new input config has been registered
	//
	FInputConfigDelegate OnInputConfigRegistered;

	//
	// Delegate called when a registered input config has been activated 
	//
	FInputConfigDelegate OnInputConfigActivated;

	//
	// Delegate called when a registered input config has been deactivate
	//
	FInputConfigDelegate OnInputConfigDeactivated;


	// =========================================
	//  Controller Settings
	// =========================================
private:
	UPROPERTY(config) 
	FName ControllerPlatform{ NAME_None };

	UPROPERTY(config) 
	FName ControllerPreset{ FName(TEXT("Default")) };

public:
	UFUNCTION() 
	void SetControllerPlatform(const FName InControllerPlatform) { ChangeValueAndDirty(ControllerPlatform, InControllerPlatform); }

	UFUNCTION() 
	FName GetControllerPlatform() const { return ControllerPlatform; }

public:
	/**
	 * Apply ControllerPlatform to subsystem
	 */
	void ApplyControllerPlatform(ULocalPlayer* LocalPlayer);


	// =========================================
	//  Keybind Settings
	// =========================================
private:
	UPROPERTY() 
	TArray<FLoadedMappableConfigPair> RegisteredInputConfigs;

	UPROPERTY(Config) 
	FName InputConfigName{ FName(TEXT("Default")) };

	UPROPERTY(Config) 
	TMap<FName, FKey> CustomKeyboardConfig;

public:
	const TArray<FLoadedMappableConfigPair>& GetAllRegisteredInputConfigs() const { return RegisteredInputConfigs; }
	const TMap<FName, FKey>& GetCustomPlayerInputConfig() const { return CustomKeyboardConfig; }


public:
	/** 
	 * Register the given input config with the settings to make it available to the player. 
	 */
	void RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig, const bool bIsActive);

	/** 
	 * Unregister the given input config. Returns the number of configs removed. 
	 */
	int32 UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove);

	/**
	 * Get all registered input configs that match the input type.
	 */
	void GetRegisteredInputConfigsOfType(ECommonInputType Type, TArray<FLoadedMappableConfigPair>& OutArray) const;

	/**
	 * Returns the display name of any actions with that key bound to it
	 */
	void GetAllMappingNamesFromKey(const FKey InKey, TArray<FName>& OutActionNames);

	/**
	 * Maps the given keyboard setting to the new key
	 */
	void AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey, ULocalPlayer* LocalPlayer);

	/**
	 * Resets keybinding to its default value in its input mapping context
	 */
	void ResetKeybindingToDefault(const FName MappingName, ULocalPlayer* LocalPlayer);

	/** 
	 * Resets all keybindings to their default value in their input mapping context
	 */
	void ResetKeybindingsToDefault(ULocalPlayer* LocalPlayer);

	/**
	 * Apply CustomKeyboardBindings to subsystem
	 */
	void ApplyCustomKeyboardBindings(ULocalPlayer* LocalPlayer);

	/**
	 * Get InputConfig from name. If the configuration does not exist, nullptr is returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "GameSettings")
	const UPlayerMappableInputConfig* GetInputConfigByName(FName ConfigName) const;

};

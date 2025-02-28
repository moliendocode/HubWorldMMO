// Copyright 2022 Sabre Dart Studios


#include "./Input/HWInputComponent.h"
#include "Containers/Map.h"
#include "EnhancedInputSubsystems.h"
#include "./Input/HWMappableConfigPair.h"
#include "./Settings/HWSettingsLocal.h"
#include "InputCoreTypes.h"
#include "UObject/NameTypes.h"
#include "UObject/UnrealNames.h"
#include "EnhancedInput/Public/InputAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HWInputComponent)

class UHWInputConfig;

UHWInputComponent::UHWInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UHWInputComponent::AddInputMappings(const UHWInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
    check(InputConfig);
    check(InputSubsystem);

    ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULocalPlayer>();
    check(LocalPlayer);

    // Add any registered input mappings from the settings!
    if (UHWSettingsLocal* LocalSettings = UHWSettingsLocal::Get())
    {
        const TMap<FName, FKey>& CustomMappings = LocalSettings->GetCustomPlayerInputConfig();
        if (!CustomMappings.IsEmpty())
        {
            CustomInputContext = NewObject<UInputMappingContext>(LocalPlayer, FName("CustomInputMappingContext"));

            for (const TPair<FName, FKey>& Pair : CustomMappings)
            {
                if (Pair.Key != NAME_None && Pair.Value.IsValid())
                {
                    UInputAction* CustomAction = NewObject<UInputAction>(CustomInputContext, Pair.Key);
                    CustomAction->ActionDescription = FText::FromName(Pair.Key);

                    CustomInputContext->MapKey(CustomAction, Pair.Value);
                }
            }

            FModifyContextOptions Options = {};
            Options.bIgnoreAllPressedKeysUntilRelease = false;
            InputSubsystem->AddMappingContext(CustomInputContext, 0, Options);
        }
    }
}

void UHWInputComponent::RemoveInputMappings(const UHWInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
    check(InputConfig);
    check(InputSubsystem);

    ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULocalPlayer>();
    check(LocalPlayer);

    if (CustomInputContext)
    {
        InputSubsystem->RemoveMappingContext(CustomInputContext);
        CustomInputContext = nullptr;
    }
}

void UHWInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
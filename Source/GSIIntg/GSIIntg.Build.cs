// Copyright (C) 2023 owoDra

using UnrealBuildTool;

public class GSIIntg : ModuleRules
{
	public GSIIntg(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
           new string[]
           {
                ModuleDirectory,
                ModuleDirectory + "/GSIIntg",
           }
       );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "CommonInput",
                "CommonUI",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GIExt",
                "GSCore",
            }
        );
    }
}

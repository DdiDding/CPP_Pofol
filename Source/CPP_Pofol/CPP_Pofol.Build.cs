// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CPP_Pofol : ModuleRules
{
	public CPP_Pofol(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        //루트기반으로 상대경로 추가
        PublicIncludePaths.Add(ModuleDirectory);
	}
}

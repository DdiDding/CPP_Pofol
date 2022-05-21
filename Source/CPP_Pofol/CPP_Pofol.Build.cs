// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CPP_Pofol : ModuleRules
{
	public CPP_Pofol(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        //��Ʈ������� ����� �߰�
        PublicIncludePaths.Add(ModuleDirectory);
	}
}

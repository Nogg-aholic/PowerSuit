// Some copyright should be here...

using UnrealBuildTool;
using System.IO;
using System.Text;
using Tools.DotNETCommon;

public class PowerSuit : ModuleRules
{
	public PowerSuit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add other public dependencies that you statically link with here ...
			}
			);

        var factoryGamePchPath = new DirectoryReference(Path.Combine(Target.ProjectFile.Directory.ToString(), "Source", "FactoryGame", "Public", "FactoryGame.h"));
        PrivatePCHHeaderFile = factoryGamePchPath.MakeRelativeTo(new DirectoryReference(ModuleDirectory));

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core", "CoreUObject","Engine","InputCore","UMG",
                "SML",
                "FactoryGame"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}

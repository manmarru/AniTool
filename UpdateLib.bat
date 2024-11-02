

xcopy	/y		.\..\Bin\Engine.lib					.\..\..\EngineSDK\Lib\
xcopy	/y		.\..\Engine\ThirdPartyLib\*.lib			.\..\..\EngineSDK\Lib\
xcopy	/y		.\..\Bin\Engine.dll					.\..\..\Client\Bin\
xcopy	/y/s		.\..\Public\*.*					.\..\..\EngineSDK\Inc\

xcopy	/y		.\..\Bin\ShaderFiles\*.*				.\..\..\Client\Bin\ShaderFiles\
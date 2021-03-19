For the dev making changes here in BindingsToCsAndCpp

** Use the BindingsToCsAndCpp sln to run the tests using Resharper

Cases:

Files generated from
Tools\UnityBindingsParser\BindingsToCsAndCpp\Tests\UnityEngineTestFixtures\Export
go to
artifacts\Tests\UnityEngineTests\ExportGenerated\<PLATFORM>\
and get compared with these expected files
Tools\UnityBindingsParser\BindingsToCsAndCpp\Tests\UnityEngineTestFixtures\Expected\<PLATFORM>\

Cases generated internally from code
go to
artifacts\Tests\UnityBindingsParser\BindingsToCsAndCpp\TestResults
and get compared with files extracted from Tools\UnityBindingsParser\BindingsToCsAndCpp\Tests\BindingsToCsAndCpp_Templates.zip
and extracted to
artifacts\Tests\UnityBindingsParser\BindingsToCsAndCpp\BindingsToCsAndCpp_Templates

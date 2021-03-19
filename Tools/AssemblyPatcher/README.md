-----------

* OK Target release? 4.5

* TomasD believes RRW will replace methods references (for instance String.Format(str, obj) with String.Format(str, params object[] obj))

# Tool

## Features

* Patching rules defined in configuration files
* Type mapping: for instance, MissingMethodException -> MissingMemberException
* Type retargeting: [mscorlib] System.Reflection.TypeCode -> [UnityScript.PlatformAdapter] Sytesm.Reflection.TypeCode
* Method retargeting: [mscorlib] System.Boolean System.Type::IsValueType() -> [UnityScript.PlatformAdapter] UnityScript.PlatformAdapter.TypeExtension::IsValueType(System.Type)
* Throw on unsuported (specified) method: For instance replace calls to System.Void System.Console.WriteLine(System.String) with a throw.
* Logging (log4net)
* Preserves debug information

## Unsupported

* Reflection base member/type access.

## Missing

* Suport for custom code rewriters (make it easier to inject/rewrite IL)
* Add support for logging upon unsupported methods / types instead of throwing (allows devs to investigate)
* Generic types
* Maybe setup tests (to be made) to be ran on Teamcity (runtests.pl).
* Warn if reflection code is found? (MethodInfo.Invoke for instance)

## Possible Improvements

* Response files

* Allow users to config whenever pacthed methods (throw) should fail the build.

* Allow custom configuration files (adictive / replacement)
    * Allow user to specify custom config files
    * Maybe check if assembly contains a resource (rely on file name convention?) (for asset store assemblies)

* Use Gendarme
    * Run if after running AssemblyPatcher on user assemblies

    * blacklist
        * Include patched methods (which call methods that will throw). For instance if we have the rule: "System.Void MyClass::Method1() ->"
          then any patched method (i.e. that invokes it) would be blacklisted.

        * Include methods replacement which will throw also, for instance, in the previous item, we would blacklist "System.Void MyClass::Method1()"" also.

    * whitelist
        * Produce a list of types / members (probably only members since to whitelist a type we'd need to make sure all of its members are supported) known to work.
          Maybe we can do something like:
            All types that don't appear as an "empty replacement" and have no members mapped as "empty replacement" (throws) also would be considered safe.

    * Failure reporting is not optimal (IMHO)
        * Throwing an exception is confusing

                Exception: Validation step FlashDotNetAPIValidationRule Failed!

                PostProcessFlashPlayer.ValidateAssemblies (.PostProcessFlashPlayerOptions options) (at C:/Projects/unity/unity/Editor/Mono/BuildPipeline/PostProcessFlashPlayer.cs:74)
                PostProcessFlashPlayer.PostProcess (.PostProcessFlashPlayerOptions options) (at C:/Projects/unity/unity/Editor/Mono/BuildPipeline/PostProcessFlashPlayer.cs:51)
                UnityEditor.PostprocessBuildPlayer.Postprocess (BuildTarget target, System.String installPath, System.String companyName, System.String productName, Int32 width, Int32 height, System.String downloadWebplayerUrl, System.String manualDownloadWebplayerUrl, BuildOptions options, UnityEditor.RuntimeClassRegistry usedClassRegistry) (at C:/Projects/unity/unity/Editor/Mono/BuildPipeline/PostprocessBuildPlayer.cs:380)
                UnityEditor.BuildPipeline:BuildPlayerInternalNoCheck(String[], String, BuildTarget, BuildOptions, Boolean, UInt32&)
                UnityEditor.BuildPlayerWindow:BuildPlayerWithDefaultSettings(Boolean, BuildOptions, Boolean) (at C:/Projects/unity/unity/Editor/Mono/BuildPlayerWindow.cs:734)
                UnityEditor.BuildPlayerWindow:BuildPlayerWithDefaultSettings(Boolean, BuildOptions) (at C:/Projects/unity/unity/Editor/Mono/BuildPlayerWindow.cs:568)
                UnityEditor.BuildPlayerWindow:GUIBuildButtons(Boolean, Boolean, Boolean, BuildPlatform) (at C:/Projects/unity/unity/Editor/Mono/BuildPlayerWindow.cs:1537)
                UnityEditor.BuildPlayerWindow:ShowBuildTargetSettings() (at C:/Projects/unity/unity/Editor/Mono/BuildPlayerWindow.cs:1501)
                UnityEditor.BuildPlayerWindow:OnGUI() (at C:/Projects/unity/unity/Editor/Mono/BuildPlayerWindow.cs:1125)
                System.Reflection.MonoMethod:InternalInvoke(Object, Object[], Exception&)
                System.Reflection.MonoMethod:Invoke(Object, BindingFlags, Binder, Object[], CultureInfo) (at /Applications/buildAgent/work/b59ae78cff80e584/mcs/class/corlib/System.Reflection/MonoMethod.cs:222)
                System.Reflection.MethodBase:Invoke(Object, Object[]) (at /Applications/buildAgent/work/b59ae78cff80e584/mcs/class/corlib/System.Reflection/MethodBase.cs:115)
                UnityEditor.HostView:Invoke(String, Object) (at C:/Projects/unity/unity/Editor/Mono/GUI/DockArea.cs:231)
                UnityEditor.HostView:Invoke(String) (at C:/Projects/unity/unity/Editor/Mono/GUI/DockArea.cs:224)
                UnityEditor.HostView:OnGUI() (at C:/Projects/unity/unity/Editor/Mono/GUI/DockArea.cs:120)

        * Individual failure messages are not very helpful if debugging info is missing

                 Usage of a type or method not supported by Unity Flash.
                * Details:   Invokes an unsupported method System.Reflection.Assembly System.Type::get_Assembly() on type System.Type
                * Source:   debugging symbols unavailable, IL offset 0x003b at line : 0

## Configuration

* Respect rules order

### Syntax

    A.B.C -> \# Replaces references to type A.B.C with references to System.Object.
             \# Any method / property reference is also replaced with a (injected) method that throws at runtime.

    A.B.C -> D.E.F \# Replaces references to type A.B.C with references to type D.E.F defined in the same assembly
                   \# as A.B.C

    A.B.C -> [assembly file name]   \# Replaces references to type A.B.C with references to a type with the same name
                                    \# but defined in the specified assembly (which must be on the same folder as the
                                    \# assembly being patched)

    A.B.C -> [assembly file name] D.E.F \# same as above but mapping to a diferent type.


    R.T A.B.C::M() ->                                       \# Replaces calls to method with one that throws at
    R.T A.B.C::M() -> [assembly file name] NR.T D.E::M2()   \# Replaces calls to A.B.C::M() with calls to D.E::M2() (signatures needs to be compatible)

    |R.T A.B::M()| R.T A.B.C::M() -> NR.T D.E::M2()         \# Replaces calls to A.B.C::M() with calls to D.E::M2(), but only within method R.T A.B::M() body
                                                            \# (signatures needs to be compatible)


## Notes

* Type.GetMethod() -> System.Reflection.RuntimeReflectionExtensions.GetRuntimeMethod(this System.Type, string, System.Type[])
* Type.GetProperty() -> System.Reflection.RuntimeReflectionExtensions.GetRuntimeProperty(this System.Type, string)


# -------------------------------------- Developing notes -----------------------------------

## Metro Runner

C:\Projects\unity\unity\PlatformDependent\MetroPlayer\MetroPlayerRunner\MetroPlayerRunner.sln

## BUILD BOO

nant set-release-configuration clean test -D:csc.define.custom=NO_SERIALIZATION_INFO,NO_SYSTEM_PROCESS,NO_ICLONEABLE,NO_SYSTEM_REFLECTION_EMIT

## ILDASM

ildasm Boo.Lang.patched.dll /ALL /TEXT /OUT=c:\temp\Boo.lang.patched.txt

## PEVERIFY

peverify Boo.Lang.patched.dll /VERBOSE /HRESULT


## Sign boo

sn -Ra Boo.lang.patched.dll ..\src\boo.snk

##Search API

a:SearchReferences.exe -c -a Boo.Lang.patched.dll -f "c:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETCore\v4.5" -r c:\projects\boo\build -v 1


## Assembly Patcher:

AssemblyPatcher.exe -a C:\Projects\boo\build\Boo.lang.dll -c c:\temp\x.txt -s c:\projects\boo\build

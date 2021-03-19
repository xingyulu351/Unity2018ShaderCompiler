Assert Generator
----------------

Assert generator is a code generator tool that generates C# code to overload Unity's generic assert methods.

Why do we need this tool?
-------------------------

Current generic assert methods work very well to support asserts on any type that has a comparator.
Unfortunately this flexibility comes with a performance knock back that could be easily solved by overloading assert methods for known types.
Using overloaded assert methods has a performance improvement of 8 to 10 times faster (Proved by MeasureAssertTime performance tests).
Rather than manually override the assert methods for known types this tool will generate the code for known types.

Why this tool is not automatized in the build system?
-----------------------------------------------------

The assert generator hasn't got any parametrization, so unless there is a code change on the assert generator the generated files output will always be the same, so the generated files are really unlikely to be modified.
This tool has been discussed with the build system pipeline team, and it has been decided that adding this tool into the build system pipeline adds some non-trivial complexity to the build system, which is not justified for this tool.

Overloaded methods
--------------------

AssertPrimitiveTypes.cs
    AreEqual
    AreNotEqual

Performance
-----------

100000 Iterations on assert check:

    Overloaded  AreEqual    [SByte]:     1.6 ms
    Generic     AreEqual    [SByte]:    17.7 ms
    Overloaded  AreNotEqual [SByte]:     1.4 ms
    Generic     AreNotEqual [SByte]:    16.4 ms

    Overloaded  AreEqual    [Byte]:      1.5 ms
    Generic     AreEqual    [Byte]:     14.8 ms
    Overloaded  AreNotEqual [Byte]:      1.4 ms
    Generic     AreNotEqual [Byte]:     16.1 ms

    Overloaded  AreEqual    [Char]:      1.5 ms
    Generic     AreEqual    [Char]:     15.8 ms
    Overloaded  AreNotEqual [Char]:      1.6 ms
    Generic     AreNotEqual [Char]:     15.9 ms

    Overloaded  AreEqual    [Int16]:     1.5 ms
    Generic     AreEqual    [Int16]:    15.9 ms
    Overloaded  AreNotEqual [Int16]:     1.4 ms
    Generic     AreNotEqual [Int16]:    16.2 ms

    Overloaded  AreEqual    [UInt16]:    1.6 ms
    Generic     AreEqual    [UInt16]:   17.4 ms
    Overloaded  AreNotEqual [UInt16]:    1.6 ms
    Generic     AreNotEqual [UInt16]:   15.8 ms

    Overloaded  AreEqual    [Int32]:     1.5 ms
    Generic     AreEqual    [Int32]:    15.7 ms
    Overloaded  AreNotEqual [Int32]:     1.4 ms
    Generic     AreNotEqual [Int32]:    17.2 ms

    Overloaded  AreEqual    [UInt32]:    2.2 ms
    Generic     AreEqual    [UInt32]:   15.9 ms
    Overloaded  AreNotEqual [UInt32]:    2.0 ms
    Generic     AreNotEqual [UInt32]:   17.2 ms

    Overloaded  AreEqual    [Int64]:     1.6 ms
    Generic     AreEqual    [Int64]:    15.0 ms
    Overloaded  AreNotEqual [Int64]:     1.6 ms
    Generic     AreNotEqual [Int64]:    13.8 ms

    Overloaded  AreEqual    [UInt64]:    2.1 ms
    Generic     AreEqual    [UInt64]:   14.9 ms
    Overloaded  AreNotEqual [UInt64]:    2.0 ms
    Generic     AreNotEqual [UInt64]:   16.6 ms

How to run
----------

Open AssertGenerator.sln with Visual Studio
Compile project
Run
Commit generated/modified assert files
    [Runtime/Export/Assertions/Assert/AssertPrimitiveTypes.cs]

Contributing
------------

This tool is meant to be expanded with more complex types, so performance could be improved in more types.

Authors
-------

Eneko Osia <eneko@unity3d.com>
Lukasz Paczkowski <lukasz@unity3d.com>

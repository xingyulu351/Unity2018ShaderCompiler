using System.Collections.Generic;
using System.Linq;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.VisualStudio
{
    public class WarningSettings_VisualStudio
    {
        public static IEnumerable<WarningAndPolicy> Policies(Language language, Compiler compiler)
        {
            var shouldBeErrors = new[]
            {
                "4002", //too many actual parameters for macro
                "4003", //not enough arguments for macro
                "4547", //'<': operator before comma has no effect; expected operator with side-effect
                //disabling this one for now, as malloc.h in the winsdk we compile against triggers it. (microsoft fixed malloc.h later at some point)
                //4548  #expression before comma has no effect

                "4549", //'==': operator before comma has no effect; did you intend '='?
                "4555", //expression has no effect; expected expression with side-effect
                "4554", // '<<': check operator precedence for possible error; use parentheses to clarify precedence

                "4578", //'abs': conversion from 'float' to 'int', possible loss of data (Did you mean to call 'fabsf' or to #include <cmath>?)

                "4263", // 'function' : member function does not override any base class virtual member function
                //figure out why this triggers all over the place, but not on clang
                //4265  #delete called on '' that has virtual functions but non-virtual destructor
                "4312", //'type cast': conversion from 'int' to 'void *' of greater size
                "4102", //unreferenced label
                "4555", //statement with no side-effect.
                "4065", //switch statement contains default but no case statements

                //temp disable to compare with -Wconditional-uninitialized
                //4701  #potentially uninitialized local variable '' used

                "4715", //not all control paths return a value
                "4805", //unsafe mix of A and B
                "4351", //new behaviour: elements of array will be default initialized
                "4005", //macro redefinition
                "4090", //different const qualifiers
                "4013", //undefined, assuming extern returning int
                "4146", //unary operator applied to unsigned type, result still unsigned
                "4521", //multiple copy constructors defined
                "4068", //unknown pragma

                "4067", //unexpected tokens following preprocessor directive - expected a newline
                "4882", //nonstandard extension used
                "4180", //qualifier applied to function type has no meaning; ignored"
                "4305", //truncation from 'int' to 'UInt8'
                "4334", // '<<' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
                "4995", //using deprecated function
                "4996", //using deprecated posix name - use iso name instead
                "4474", // too many arguments passed for format string
                "4476", // unknown type field character 'H' in format specifier
                "4193", // #pragma warning(pop): no matching '#pragma warning(push)'

                // At some point we were turning 4577 ('noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc)
                // into an error, since that generally meant that exceptions were turned off, without specifying _HAS_EXCEPTIONS=0 macro. However
                // starting with VS2017 15.7 version, MSVC STL implementation even with _HAS_EXCEPTIONS=0 results in this error in some places,
                // mostly coming from vcruntime_new.h parts that kick in when using std::stable_sort. However whole 4577 seems to be undocumented
                // these days and does not normally happen at all, so let's just not do anything about making it an error.
                //"4577",

                "4099", // type name first seen using 'class' now seen using 'struct'
                //"9025", // overriding '/TC' with '/TP'    (or any commandline wrarning like that)
            };

            var shouldBeSilent = new[]
            {
                //conversion from bla to bla, possible loss of data
                "4267",

                //disabling 4244: conversion from 'type1' to 'type2', possible loss of data. see comment about -Wconversion in clang settings
                "4244",

                //disabling 4251. requires investigation once we actually ship something that ships modules as separate .dll's.
                "4251",

                //signed/unsigned warnings. we need to decide how to deal with these in the codebase.
                "4018",

                //forcing value to bool, possible performance warning.
                "4800",

                //conversion from 'size_t' to 'UInt32', possible loss of data
                //disabling this warning until we decide to fix it everywhere, it's responsible for a lot of buildspam.
                "4267",

                //warning C4345: behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
                "4345",

                //nonstandard extension used: enum
                "4482",
            };

            foreach (var v in shouldBeErrors.Select(e => new WarningAndPolicy(e, WarningPolicy.AsError)))
                yield return v;

            foreach (var v in shouldBeSilent.Select(e => new WarningAndPolicy(e, WarningPolicy.Silent)))
                yield return v;
        }

        public static string[] UnusedWarnings_VS()
        {
            var unusedWarnings = new[]
            {
                "4189", //local variable is initialized but not referenced

                //we cannot use this yet, as we need to stop using static functions in Math headers first
                //4505      #'booya': unreferenced local function has been removed

                "4102"  //unused label
            };
            //unfortunattely we're not ready to enable this one, as vs2010 is unable to show trigger these warnings in lumped mode.
            //vs2015 is able to do it, so when we swithc to that we can turn this error on.

            //4101      #unreferenced local variable
            return unusedWarnings;
        }
    }
}

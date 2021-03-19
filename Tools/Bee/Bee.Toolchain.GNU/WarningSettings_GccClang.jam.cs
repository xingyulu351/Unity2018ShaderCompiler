using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.GNU
{
    public static class WarningSettings_GccClang
    {
        public static IEnumerable<WarningAndPolicy> Policies(Language language, Compiler compiler)
        {
            //
            // Silent warnings
            //

            //I'd love to turn on missing-braces, but there's no msvc equivalent, and it does not guarantee a bug at all, it
            // just encourages a style that is less likely to lead to bugs.
            yield return new WarningAndPolicy("missing-braces", WarningPolicy.Silent);

            //as part of c++ coding guidelines, we need to decide on how we want to tackle unsigned / signed ints.
            //datapoint: google coding guidelines say: dont use unsigned ints.   until we have made a decision here,
            //we cannot enable sign-compare, as the code changes required to make sign-compare warnings go away are
            //invasive, so we need to make sure we fix them the way we want them fixed.
            yield return new WarningAndPolicy("sign-compare", WarningPolicy.Silent);

            //While both clang and msvc have -Wint-to-pointer-cast (msvc: C4312). we should probably turn this on, when
            //we have converted all our code that uses void* for 'storage', to use intrptr_t instead, so that we can use the warning
            //to protect us from just putting some 1234 number into a pointer and then dereferencing it.
            yield return new WarningAndPolicy("int-to-pointer-cast", WarningPolicy.Silent);

            //not turning on -Wformat, because there's basically no good cross-compiler way to printf a size_t.  (clang
            // wants %zu, msvc wants %Ix)
            yield return new WarningAndPolicy("format", WarningPolicy.Silent);

            //not yet turning on -Wconversion (msvc: 4244), as it is fired in many places, so we actually have to fix at
            // least all
            //the headers it appears in before we can turn it on. looks like a valuable warning to have though.
            yield return new WarningAndPolicy("conversion", WarningPolicy.Silent);

            //we should really enable format-security, but first need to clean up existing cases of it. there's no MSVC
            // equivalent,
            //but it's bad enough that we should have it as an error.
            yield return new WarningAndPolicy("format-security", WarningPolicy.Silent);

            //cannot turn on unused-local-function yet, as we have many (math) headers that use 'static' functions that end
            // up not used in most compilation units.
            //sonny thinks we can probably stop doing that, at which point we should turn on:
            yield return new WarningAndPolicy("unused-function", WarningPolicy.Silent);

            //not particularly useful, and makes you not be able to write /dev/* in comments.
            yield return new WarningAndPolicy("comment", WarningPolicy.Silent);

            if (language == Language.Cpp)
            {
                //not turning on reorder yet, as it usually doesn't point to a bug, and there's no MSVC equivalent
                yield return new WarningAndPolicy("reorder", WarningPolicy.Silent);
            }

            if (compiler == Compiler.Clang)
            {
                //We should start requiring 'override' on all member functions that intend to override a base function.
                //fix platforms that don't support override by "#define override" or with "#define OVERRIDE override".
                yield return new WarningAndPolicy("inconsistent-missing-override", WarningPolicy.Silent);

                //I would really like to turn on logical-op-parentheses, but there's no MSVC equivalent, while it would
                //occur all the time, in situations where there is no actual bug. I think today the cost of enabling this
                //warning is too high.
                yield return new WarningAndPolicy("logical-op-parentheses", WarningPolicy.Silent);

                //lets not worry about this warning yet, it's fine in c++11, and haven't heard <c++11 platform owners
                // complain.
                yield return new WarningAndPolicy("unnamed-type-template-args", WarningPolicy.Silent);

                //this gives too many false positives. still have to investigate if we can easily get rid of the false
                // positives with UNUSED()
                //so we could keep the warning for when it's right
                yield return new WarningAndPolicy("unused-private-field", WarningPolicy.Silent);

                //not turning this on, so we can still write   "if (UNITY_EDITOR || someThing()) {}" warningfree.
                // ifdeffed alternative would be very ugly.
                yield return new WarningAndPolicy("constant-logical-operand", WarningPolicy.Silent);

                //this can be turned on when we fix Physx' PX_COMPILE_TIME_ASSERT macro.
                yield return new WarningAndPolicy("unused-local-typedef", WarningPolicy.Silent);

                //Todo: As soon as we have a clang that supports this warning we should enable it, as jam
                //requires include statements to have correct casings to pick up proper dependencies between
                //header files automatically.
                //yield return new WarningAndPolicy("no-nonportable-include-path", WarningPolicy.AsError);

                //no msvc equivalent
                yield return new WarningAndPolicy("deprecated-declarations", WarningPolicy.Silent);
            }
            else
            {
                // missing-field-initializers has false-positives in gcc 4.8:
                // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55805
                yield return new WarningAndPolicy("missing-field-initializers", WarningPolicy.Silent);

                if (language == Language.Cpp)
                {
                    yield return new WarningAndPolicy("overloaded-virtual", WarningPolicy.Silent);
                    yield return new WarningAndPolicy("invalid-offsetof", WarningPolicy.Silent); // too pedantic
                    yield return new WarningAndPolicy("multichar", WarningPolicy.Silent);        // we have a bunch of these legitimately
                    // On older gcc, pragma-based ignores don't work with some templated/inlined code
                    // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431 and friends
                    // The primary offender in our code is IsPolymorphic (TypeUtilities.h)
                    yield return new WarningAndPolicy("unused-value", WarningPolicy.Silent);

                    yield return new WarningAndPolicy("deprecated-declarations", WarningPolicy.Silent);

                    // Remove C++11 compatibility warning, because of CompilerEmulation.h
                    yield return new WarningAndPolicy("c++0x-compat", WarningPolicy.Silent);
                }
            }

            //
            // AsError warnings
            //

            //no msvc equivalent, but rare and high bug catch %
            yield return new WarningAndPolicy("switch", WarningPolicy.AsError);

            //no msvc equivalent, but near-guarantee of an actual bug
            yield return new WarningAndPolicy("enum-compare", WarningPolicy.AsError);

            //no msvc equivalent, but very rare
            yield return new WarningAndPolicy("char-subscripts", WarningPolicy.AsError);

            yield return new WarningAndPolicy("write-strings", WarningPolicy.AsError);

            yield return new WarningAndPolicy("return-type", WarningPolicy.AsError);

            // Note: on gcc -Wparentheses includes the case covered by -Wlogical-op-parentheses on clang
            yield return new WarningAndPolicy("parentheses", WarningPolicy.AsError);

            //todo: figure out why we get this warning in precompiled headers when building with tundra, but for some reason it's green on trunk.
            //if (language == NativeProgramSupport.Language.Cpp)
            //    yield return new WarningAndPolicy("delete-non-virtual-dtor", WarningPolicy.AsError);

            if (language == Language.C)
                yield return new WarningAndPolicy("implicit-function-declaration", WarningPolicy.AsError);


            // Compiler specific warnings.
            if (compiler == Compiler.Clang)
            {
                // no msvc equivalent, but rare and high bug catch % ie checking a declared array is non NULL
                yield return new WarningAndPolicy("tautological-pointer-compare", WarningPolicy.AsError);

                yield return new WarningAndPolicy("self-assign", WarningPolicy.AsError);
                yield return new WarningAndPolicy("dynamic-class-memaccess", WarningPolicy.AsError);
                yield return new WarningAndPolicy("absolute-value", WarningPolicy.AsError);
                yield return new WarningAndPolicy("sometimes-uninitialized", WarningPolicy.AsError);
                yield return new WarningAndPolicy("tautological-constant-out-of-range-compare", WarningPolicy.AsError);
                yield return new WarningAndPolicy("dangling-else", WarningPolicy.AsError);
                yield return new WarningAndPolicy("deprecated-register", WarningPolicy.AsError);
                yield return new WarningAndPolicy("unused-comparison", WarningPolicy.AsError);
                yield return new WarningAndPolicy("missing-field-initializers", WarningPolicy.AsError);
                yield return new WarningAndPolicy("shift-op-parentheses", WarningPolicy.AsError);
                yield return new WarningAndPolicy("conditional-uninitialized", WarningPolicy.AsError);
                yield return new WarningAndPolicy("incompatible-pointer-types", WarningPolicy.AsError);
                yield return new WarningAndPolicy("vexing-parse", WarningPolicy.AsError);
                yield return new WarningAndPolicy("infinite-recursion", WarningPolicy.AsError);
                yield return new WarningAndPolicy("undefined-internal", WarningPolicy.AsError);
                yield return new WarningAndPolicy("macro-redefined", WarningPolicy.AsError);
                yield return new WarningAndPolicy("overloaded-virtual", WarningPolicy.AsError);
                yield return new WarningAndPolicy("null-arithmetic", WarningPolicy.AsError);

                //rarely leads to actual bugs, and happens often when using ccache builds
                // (since it preprocesses before passing to compiler, making expressions like "if ((x==1))" occur often
                // with macros. Note; has to be passed after "parentheses" flag.
                yield return new WarningAndPolicy("parentheses-equality", WarningPolicy.Silent);

                yield return new WarningAndPolicy("shift-negative-value", WarningPolicy.AsError);

                yield return new WarningAndPolicy("mismatched-tags", WarningPolicy.AsError);

                // clang specific (it seems). MSVC errors in this situation (though error is not very informative)
                yield return new WarningAndPolicy("gnu-alignof-expression", WarningPolicy.AsError);

                //need to investigate further, seems pretty harmless, this happens in physx headers quite a bit:
                yield return new WarningAndPolicy("return-type-c-linkage", WarningPolicy.Silent);
            }
            else
            {
                //
                // Clang-Gcc common warnings
                //

                // Does not exist in g++ 4.8
                //yield return new WarningAndPolicy("incompatible-pointer-types", WarningPolicy.AsError);

                // Does not exist in g++ 4.8
                //yield return new WarningAndPolicy("dangling-else", WarningPolicy.AsError);

                // Possibly covering tautological-constant-out-of-range-compare! (does not exist in g++ 4.8)
                //yield return new WarningAndPolicy("bool-compare", WarningPolicy.AsError);

                // Problematic with gcc.
                //yield return new WarningAndPolicy("overloaded-virtual", WarningPolicy.AsError);

                //
                // GCC specific warnings
                //

                //yield return new WarningAndPolicy("strict-aliasing", WarningPolicy.AsError);
                //yield return new WarningAndPolicy("unused-local-typedefs", WarningPolicy.AsError);

                // Apparently corresponds to clang -Wsometimes-uninitialized.
                // Two problems:
                //   1. Set AsWarning for now because it has complicated errors
                //      on GCC 7 (false-positives?)
                //   2. Has to silence it because the LinuxEditorInstaller
                //      builder don't recognize this warning (seems to be using
                //      gcc 4.6 instead of 4.8), but if not explicitly defined,
                //      "uninitialized" also enables "maybe-uninitialized"
                yield return new WarningAndPolicy("maybe-uninitialized", WarningPolicy.Silent);
            }

            // On gcc, -Wuninitialized must be set after -Wmaybe-uninitialized,
            // otherwise -Wmaybe-uninitialized is considered as error.
            yield return new WarningAndPolicy("uninitialized", WarningPolicy.AsError);
        }

        public static IEnumerable<string> UnusedWarningNames(Compiler compiler, Language language)
        {
            yield return "unused-label";        //msvc equivalent: 4102
            yield return "unused-variable";     //msvc equivalent: 4189

            if (compiler == Compiler.Clang)
            {
                yield return "unused-const-variable";
                yield return "unused-comparison";
            }

            if (compiler == Compiler.Gcc)
                yield return "unused-but-set-variable";
        }
    }
}

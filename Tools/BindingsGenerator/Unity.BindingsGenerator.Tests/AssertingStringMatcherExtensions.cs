using System;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    static class AssertingStringMatcherExtensions
    {
        public static AssertingStringMatcher ExpectEmptyParens(this AssertingStringMatcher matcher)
        {
            return matcher.ExpectParensBegin().ExpectParensEnd();
        }

        public static AssertingStringMatcher ExpectInclude(this AssertingStringMatcher matcher, string fileName)
        {
            return matcher.Expect("#include").ExpectWhitespace()
                .Expect("\"").Expect(fileName).Expect("\"").OptionalWhitespace();
        }

        public static AssertingStringMatcher FindNamespaceStart(this AssertingStringMatcher matcher, string namespaceName)
        {
            return matcher
                .Find("namespace")
                .ExpectWhitespace()
                .Expect(namespaceName)
                .OptionalWhitespace()
                .ExpectBraceBegin();
        }

        public static AssertingStringMatcher FindTypedef(this AssertingStringMatcher matcher, string sourceType, string destType)
        {
            return matcher
                .Find("typedef")
                .ExpectWhitespace()
                .Expect(sourceType)
                .ExpectWhitespace()
                .Expect(destType)
                .ExpectSemiColon();
        }

        public static AssertingStringMatcher FindEmptyFunction(this AssertingStringMatcher matcher, string returnType, string methodName)
        {
            return matcher
                .FindFunctionStart(returnType, methodName).ExpectEmptyParens()
                .ExpectBraceBegin().ExpectBraceEnd();
        }

        public static AssertingStringMatcher FindEmptyFunction(this AssertingStringMatcher matcher, string returnType, string methodName, string declSpec, string callingConvention, bool friend = false)
        {
            return matcher
                .FindFunctionStart(returnType, methodName, declSpec, callingConvention, friend).ExpectEmptyParens()
                .ExpectBraceBegin().ExpectBraceEnd();
        }

        public static AssertingStringMatcher ExpectBraceBegin(this AssertingStringMatcher matcher)
        {
            return matcher.Expect("{").OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectBraceEnd(this AssertingStringMatcher matcher)
        {
            return matcher.Expect("}").OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectParensBegin(this AssertingStringMatcher matcher)
        {
            return matcher.Expect("(").OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectParensEnd(this AssertingStringMatcher matcher)
        {
            return matcher.Expect(")").OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectComma(this AssertingStringMatcher matcher)
        {
            return matcher.Expect(",").OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectSemiColon(this AssertingStringMatcher matcher)
        {
            return matcher.Expect(";").OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectColon(this AssertingStringMatcher matcher)
        {
            return matcher.Expect(":").OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectEqualSign(this AssertingStringMatcher matcher)
        {
            return matcher.Expect("=").OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectEnum(this AssertingStringMatcher matcher, string enumName)
        {
            return matcher.Expect("enum").ExpectWhitespace().ExpectIdentifier(enumName);
        }

        public static AssertingStringMatcher ExpectEnumValue(this AssertingStringMatcher matcher, string valueName, string value)
        {
            return matcher.Expect(valueName).ExpectWhitespace().ExpectEqualSign().Expect(value).OptionalWhitespace().ExpectComma();
        }

        public static AssertingStringMatcher FindFunctionStart(this AssertingStringMatcher matcher, string returnType, string functionName)
        {
            return matcher.Find(returnType).ExpectWhitespace().Find(functionName).OptionalWhitespace();
        }

        public static AssertingStringMatcher FindFunctionStart(this AssertingStringMatcher matcher, string returnType, string functionName, string declSpec, string callingConvention, bool friend = false)
        {
            var match = friend
                ? string.Format("{0}\nfriend {1}", declSpec, returnType)
                : string.Format("{0}\n{1}", declSpec, returnType);
            return matcher.Find(match).ExpectWhitespace().Expect(callingConvention).ExpectWhitespace().Find(functionName).OptionalWhitespace();
        }

        public static AssertingStringMatcher FindComment(this AssertingStringMatcher matcher, string comment)
        {
            return matcher.Find(String.Format("// {0}", comment)).ExpectWhitespace();
        }

        public static AssertingStringMatcher ExpectEtwEntryStatement(this AssertingStringMatcher matcher, string nativeFunctionName)
        {
            return matcher.Expect(Naming.EtwEntryMacro).ExpectParensBegin().Expect(nativeFunctionName).ExpectParensEnd().ExpectSemiColon();
        }

        public static AssertingStringMatcher ExpectStackCheckStatement(this AssertingStringMatcher matcher, string managedFunctionName)
        {
            return matcher.Expect(Naming.StackCheckMacro).ExpectParensBegin().Expect(managedFunctionName).ExpectParensEnd().ExpectSemiColon();
        }

        public static AssertingStringMatcher ExpectThreadSafeCheckStatement(this AssertingStringMatcher matcher, string managedFunctionName)
        {
            return matcher.Expect(Naming.ThreadSafeMacro).ExpectParensBegin().Expect(managedFunctionName).ExpectParensEnd().ExpectSemiColon();
        }

        public static AssertingStringMatcher ExpectFunctionCallStatement(this AssertingStringMatcher matcher, string callStr)
        {
            return matcher.ExpectFunctionCall(callStr).ExpectSemiColon();
        }

        public static AssertingStringMatcher ExpectKeyword(this AssertingStringMatcher matcher, string keyword)
        {
            return matcher.Expect(keyword).ExpectWhitespace();
        }

        public static AssertingStringMatcher ExpectIdentifier(this AssertingStringMatcher matcher, string identifier)
        {
            return matcher.Expect(identifier).OptionalWhitespace();
        }

        public static AssertingStringMatcher ExpectReturn(this AssertingStringMatcher matcher)
        {
            return matcher.ExpectKeyword("return");
        }

        public static AssertingStringMatcher ExpectIf(this AssertingStringMatcher matcher)
        {
            return matcher.ExpectKeyword("if");
        }

        public static AssertingStringMatcher ExpectFunctionCall(this AssertingStringMatcher matcher, string callStr)
        {
            return matcher.Expect(callStr).OptionalWhitespace().ExpectEmptyParens();
        }

        public static AssertingStringMatcher ExpectStatement(this AssertingStringMatcher matcher, string statement)
        {
            return matcher.Expect(statement).OptionalWhitespace().ExpectSemiColon();
        }

        public static AssertingStringMatcher ExpectVariableDeclaration(this AssertingStringMatcher matcher, string varType, string varName)
        {
            return matcher.Expect(varType).ExpectWhitespace().ExpectIdentifier(varName);
        }

        public static AssertingStringMatcher ExpectVariableDeclarationStatement(this AssertingStringMatcher matcher, string varType, string varName)
        {
            return matcher.ExpectVariableDeclaration(varType, varName).ExpectSemiColon();
        }

        public static AssertingStringMatcher ExpectParameterMarshalFunctionCallStatement(this AssertingStringMatcher matcher, string identifier)
        {
            return matcher.Expect("Marshalling::Marshal")
                .ExpectParensBegin()
                .Expect(Naming.AddressOf(identifier.Marshalled())).OptionalWhitespace().ExpectComma()
                .Expect(Naming.AddressOf(identifier)).OptionalWhitespace()
                .ExpectParensEnd().ExpectSemiColon();
        }

        public static AssertingStringMatcher ExpectNullCheck(this AssertingStringMatcher matcher, string variableName)
        {
            return
                matcher.ExpectIf().ExpectParensBegin().Find(")").ExpectParensEnd()
                    .ExpectBraceBegin()
                    .Expect("Scripting::RaiseArgumentNullException").ExpectParensBegin().Expect("\"" + variableName + "\"").ExpectParensEnd().ExpectSemiColon()
                    .ExpectBraceEnd();
        }

        public static AssertingStringMatcher ExpectGeneratedOutput<T>(this AssertingStringMatcher matcher, T node, int expectedIndent) where T : class
        {
            CppGenerator generator = new CppGenerator(expectedIndent);
            var generateMethod = typeof(CppGenerator).GetMethod("Generate", new[] { node.GetType() });
            Assert.IsNotNull(generateMethod, $"Could not find CppGenerator.Generate({node.GetType().Name})");
            var actualOutput = (string)generateMethod.Invoke(generator, new[] { node });
            return matcher.Expect(actualOutput);
        }
    }
}

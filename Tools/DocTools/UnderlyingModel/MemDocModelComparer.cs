namespace UnderlyingModel.MemDoc.Comparers
{
    public class MemDocModelComparer : IComparerWithError<MemDocModel>
    {
        public CompareResult Compare(MemDocModel x, MemDocModel y)
        {
            var resultNullComparison = ComparerUtils.CompareNullableElement(x, y);
            if (!resultNullComparison.Success)
                return resultNullComparison;

            if (x.NamespaceFromMember != y.NamespaceFromMember)
                return CompareResult.FailureWithExpectedAndActual("NamespaceFromMember different:", x.NamespaceFromMember, y.NamespaceFromMember);
            if (x.AssemblyKind != y.AssemblyKind)
                return CompareResult.FailureWithExpectedAndActual("AssemblyKind different:", x.AssemblyKind, y.AssemblyKind);
            if (x.ItemNameFromMember != y.ItemNameFromMember)
                return CompareResult.FailureWithExpectedAndActual("ItemNameFromMember different:", x.ItemNameFromMember, y.ItemNameFromMember);

            var compareResult = ComparerUtils.CompareLists(x.SubSections, y.SubSections, new MemberSectionComparer());
            if (!compareResult.Success)
                return compareResult;

            compareResult = ComparerUtils.CompareLists(x.ChildModels, y.ChildModels, new MemDocModelComparer());
            if (!compareResult.Success)
                return compareResult;

            return CompareResult.SuccessfulResult();
        }
    }

    class MemberSectionComparer : IComparerWithError<MemberSubSection>
    {
        public CompareResult Compare(MemberSubSection x, MemberSubSection y)
        {
            int result = x.IsUndoc.CompareTo(y.IsUndoc);
            if (result != 0)
                return CompareResult.FailureWithExpectedAndActual("IsUndoc", x.IsUndoc, y.IsUndoc);
            result = x.IsDocOnly.CompareTo(y.IsDocOnly);
            if (result != 0)
                return CompareResult.FailureWithExpectedAndActual("DocOnly", x.IsDocOnly, y.IsDocOnly);

            var compareResult = ComparerUtils.CompareLists(x.Parameters, y.Parameters, new ParameterComparer());
            if (!compareResult.Success)
                return compareResult;

            //compare return values
            compareResult = ComparerUtils.CompareNullableElement(x.ReturnDoc, y.ReturnDoc);
            if (!compareResult.Success)
                return compareResult;
            if (x.ReturnDoc != null && y.ReturnDoc != null)
            {
                var xValue = x.ReturnDoc.Doc.WithUnixLineEndings().TrimEnd();
                var yValue = y.ReturnDoc.Doc.WithUnixLineEndings().TrimEnd();
                if (xValue != yValue)
                    return CompareResult.FailureWithExpectedAndActual("Return docs are different:", xValue, yValue);
            }

            compareResult = ComparerUtils.CompareLists(x.TextBlocks, y.TextBlocks, new TextBlockComparer());
            return compareResult;
        }
    }

    class ParameterComparer : IComparerWithError<ParameterWithDoc>
    {
        public CompareResult Compare(ParameterWithDoc x, ParameterWithDoc y)
        {
            if (x.Name != y.Name)
                return CompareResult.FailureWithExpectedAndActual("parameters names different", x.Name, y.Name);
            if (x.Doc.WithUnixLineEndings().TrimEnd() != y.Doc.WithUnixLineEndings().TrimEnd())
                return CompareResult.FailureWithExpectedAndActual("parameter docs different", x.Doc, y.Doc);
            return CompareResult.SuccessfulResult();
        }
    }

    class TextBlockComparer : IComparerWithError<TextBlock>
    {
        public CompareResult Compare(TextBlock x, TextBlock y)
        {
            if (x.GetType() != y.GetType())
                return CompareResult.FailureWithExpectedAndActual("types of TextBlocks are different:", x.GetType(), y.GetType());
            if (x is DescriptionBlock)
            {
                var xText = x.Text.WithUnixLineEndings();
                var yText = y.Text.WithUnixLineEndings();
                return xText == yText
                    ? CompareResult.SuccessfulResult()
                    : CompareResult.FailureWithExpectedAndActual("Description Blocks are different:", xText, yText);
            }

            var xExample = (ExampleBlock)x;
            var yExample = (ExampleBlock)y;
            if (xExample.IsNoCheck != yExample.IsNoCheck)
                return CompareResult.FailureWithExpectedAndActual("NoCheck values are different", xExample.IsNoCheck, yExample.IsNoCheck);
            if (xExample.IsConvertExample != yExample.IsConvertExample)
                return CompareResult.FailureWithExpectedAndActual("IsConvertExample are diffent", xExample.IsConvertExample, yExample.IsConvertExample);

            string xExampleText = xExample.JsExample.WithUnixLineEndings().TrimEnd();
            string yExampleText = yExample.JsExample.WithUnixLineEndings().TrimEnd();
            if (xExampleText != yExampleText)
                return CompareResult.FailureWithExpectedAndActual("JS examples different: ", xExampleText, yExampleText);

            xExampleText = xExample.CsExample.WithUnixLineEndings().TrimEnd();
            yExampleText = yExample.CsExample.WithUnixLineEndings().TrimEnd();
            if (xExampleText != yExampleText)
                return CompareResult.FailureWithExpectedAndActual("CS examples different: ", xExampleText, yExampleText);
            return CompareResult.SuccessfulResult();
        }
    }
}

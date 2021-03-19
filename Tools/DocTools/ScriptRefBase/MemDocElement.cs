namespace ScriptRefBase
{
    public interface IMemDocElement
    {
        string PlainText { get; }
    }


    public class MemDocElementPlainText : IMemDocElement
    {
        public readonly string Text;

        public MemDocElementPlainText(string text)
        {
            Text = text;
        }

        public bool IsEmpty()
        {
            return Text == "";
        }

        public override string ToString()
        {
            return Text;
        }

        public string PlainText { get { return Text; } }
    }

    public enum LinkType
    {
        ScriptRef,
        Wiki,
        External
    }

    public class MemDocElementImage : IMemDocElement
    {
        public readonly string ImgName;

        public MemDocElementImage(string imgName)
        {
            ImgName = imgName;
        }

        public override string ToString()
        {
            return "img:" + ImgName;
        }

        public string PlainText
        {
            get { return ""; }
        }
    }

    public class MemDocElementLineBreak : IMemDocElement
    {
        public override string ToString()
        {
            return "break";
        }

        public string PlainText { get { return " "; } }
    }

    public class MemDocElementStyle : IMemDocElement
    {
        public readonly string Name;
        public readonly StyleType Style;

        public MemDocElementStyle(string name, StyleType style)
        {
            Name = name;
            Style = style;
        }

        public override string ToString()
        {
            return string.Format("name: {0}, style: {1}", Name, Style);
        }

        public string PlainText { get { return Name; } }
    }
}

namespace UnderlyingModel.MemDoc
{
    public abstract class TextBlock
    {
        public string Text { set; get; }

        protected TextBlock(string txt)
        {
            Text = txt;
        }

        public override string ToString()
        {
            return Text;
        }
    }

    public class DescriptionBlock : TextBlock
    {
        public DescriptionBlock(string txt) : base(txt) {}
    }
}

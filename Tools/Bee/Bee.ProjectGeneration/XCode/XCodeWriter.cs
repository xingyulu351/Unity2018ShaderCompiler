using System.CodeDom.Compiler;
using System.IO;

namespace Bee.ProjectGeneration.XCode
{
    class XCodeWriter
    {
        IndentedTextWriter _writer;

        public void Write(XCodeDocument doc, TextWriter writer)
        {
            _writer = new IndentedTextWriter(writer);
            _writer.WriteLine("// !$*UTF8*$!");
            WriteWithoutTerminator(doc.RootNode, Mode.Contents);
        }

        void Write(object node, Mode mode, bool oneOfMany)
        {
            WriteWithoutTerminator(node, mode);
            _writer.WriteLine(oneOfMany ? "," : ";");
        }

        void WriteWithoutTerminator(object node, Mode mode)
        {
            var many = node as System.Collections.IEnumerable;
            if (many != null && !(node is string))
            {
                _writer.WriteLine("(");
                _writer.Indent++;
                foreach (var e in many)
                {
                    Write(e, mode, true);
                }
                _writer.Indent--;
                _writer.Write(")");
                return;
            }

            var s = node as string;
            if (s != null)
            {
                _writer.Write(s);
                return;
            }

            if (mode == Mode.RefOnly || mode == Mode.RefAndContents)
            {
                var xcodeNode = (XCodeNode)node;
                _writer.Write(xcodeNode.Id);
            }
            if (mode == Mode.RefAndContents)
                _writer.Write(" = ");

            if (mode == Mode.Contents || mode == Mode.RefAndContents)
            {
                var xcodenode = (XCodeNode)node;
                _writer.WriteLine("{");
                _writer.Indent++;

                foreach (var m in xcodenode.Members)
                {
                    _writer.Write(m.Key);
                    _writer.Write(" = ");
                    if (m.Key == "objects")
                    {
                        _writer.WriteLine("{");
                        _writer.Indent++;
                        foreach (var o in (XCodeNode[])m.Value)
                        {
                            Write(o, Mode.RefAndContents, false);
                        }
                        _writer.Indent--;
                        _writer.WriteLine("};");
                    }
                    else
                        Write(m.Value, Mode.RefOnly, false);
                }

                _writer.Indent--;
                _writer.Write("}");
            }
        }

        enum Mode
        {
            RefOnly,
            RefAndContents,
            Contents
        }
    }
}

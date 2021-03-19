using System;

namespace HistoryGenerator
{
    public class VersionNumber : IComparable
    {
        public int Major { get; private set; }
        public int Minor { get; private set; }
        public int Point { get; private set; }

        public VersionNumber(string verText)
        {
            string[] fields = verText.Trim().Split('.');
            Major = fields[0] == "x" ? -1 : Int32.Parse(fields[0]);
            Minor = fields[1] == "x" ? -1 : Int32.Parse(fields[1]);

            var splitChars = new[] { 'a', 'b', 'f', 'p', 'x' };
            int index = fields[2].IndexOfAny(splitChars);

            Point = index == -1
                ? (fields[2] == "x" ? -1 : Int32.Parse(fields[2]))
                : (fields[2] == "x" ? -1 : Int32.Parse(fields[2].Substring(0, index)));
        }

        public VersionNumber(int major, int minor, int point)
        {
            Major = major;
            Minor = minor;
            Point = point;
        }

        public override string ToString()
        {
            return string.Format("{0}.{1}.{2}", NegativeMeansX(Major), NegativeMeansX(Minor), NegativeMeansX(Point));
        }

        private string NegativeMeansX(int num)
        {
            return num < 0 ? "x" : num.ToString();
        }

        public int CompareTo(object obj)
        {
            // Less than zero: This instance precedes obj in the sort order.
            // Zero: This instance occurs in the same position in the sort order as obj.
            // Greater than zero: This instance follows obj in the sort order.

            var other = obj as VersionNumber;

            if (other == null)
                return -1;

            if (Major < other.Major) return -1;
            if (Major > other.Major) return 1;

            if (Minor < other.Minor) return -1;
            if (Minor > other.Minor) return 1;

            return Point.CompareTo(other.Point);
        }
    }
}

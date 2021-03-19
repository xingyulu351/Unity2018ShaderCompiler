namespace UnderlyingModel
{
    public class ParamElement
    {
        public string Type { get; private set; }
        public string Name { get; private set; }
        public string Modifiers { get; private set; }
        public string DefaultValue { get; private set; }
        public bool Optional { get; private set; }

        public ParamElement(string name, string type, string mods = "", string defaultValue = "", bool optional = false)
        {
            Name = name;
            Type = type;
            Modifiers = mods;
            DefaultValue = defaultValue;
            Optional = optional;
        }

        private string ModifierString()
        {
            return Modifiers != "" ? string.Format("({0})", Modifiers) : "";
        }

        public string Formatted()
        {
            return string.Format("{0} :{4} {1}{2}{3}", Name, Type,
                !string.IsNullOrEmpty(DefaultValue) ? " = " : "", DefaultValue,
                ModifierString());
        }

        public string FormattedHTML()
        {
            return string.Format("<b>{0} : </b> {4} {1}{2}{3}", Name, Type,
                !string.IsNullOrEmpty(DefaultValue) ? " = " : "", DefaultValue,
                ModifierString());
        }
    }
}

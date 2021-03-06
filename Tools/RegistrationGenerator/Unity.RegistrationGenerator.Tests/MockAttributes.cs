using System;

// This is a copy of <unity repo>/Runtime/Export/Attributes.cs with some small changes
// to get rid of the dependency of UnityEngine. This is only use for test purposes.

namespace UnityEngine
{
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct, Inherited = false)]
    public sealed class NativeClassAttribute : Attribute
    {
        public string QualifiedNativeName { get; private set; }

        public NativeClassAttribute(string qualifiedCppName)
        {
            QualifiedNativeName = qualifiedCppName;
        }
    }

    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    public sealed class DisallowMultipleComponent : Attribute {}

    // The RequireComponent attribute lets automatically add required component as a dependency.
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = true)]
    public class RequireComponent : Attribute
    {
        //*undocumented*
        public Type m_Type0;
        //*undocumented*
        public Type m_Type1;
        //*undocumented*
        public Type m_Type2;

        // Require a single component
        public RequireComponent(Type requiredComponent) { m_Type0 = requiredComponent; }
        // Require a two components
        public RequireComponent(Type requiredComponent, Type requiredComponent2) { m_Type0 = requiredComponent; m_Type1 = requiredComponent2; }
        // Require three components
        public RequireComponent(Type requiredComponent, Type requiredComponent2, Type requiredComponent3) { m_Type0 = requiredComponent; m_Type1 = requiredComponent2; m_Type2 = requiredComponent3; }
    }


    // The AddComponentMenu attribute allows you to place a script anywhere in the "Component" menu, instead of just the "Component->Scripts" menu.
    public sealed class AddComponentMenu : Attribute
    {
        string m_AddComponentMenu;
        int m_Ordering;

        // The script will be placed in the component menu according to /menuName/. /menuName/ is the path to the component
        public AddComponentMenu(string menuName) { m_AddComponentMenu = menuName; m_Ordering = 0; }
        // same as above, but also specify a custom Ordering.
        public AddComponentMenu(string menuName, int order) { m_AddComponentMenu = menuName; m_Ordering = order; }

        //* undocumented
        public string componentMenu { get { return m_AddComponentMenu; } }

        //* undocumented
        public int componentOrder { get { return m_Ordering; } }
    }

    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = false)]
    public sealed class CreateAssetMenuAttribute : Attribute
    {
        public string menuName { get; set; }
        public string fileName { get; set; }
        public int order { get; set; }
    }

    // The ContextMenu attribute allows you to add commands to the context menu
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = true)]
    public sealed class ContextMenu : Attribute
    {
        // Creates a context menu item that invokes the non-static method when selected
        public ContextMenu(string itemName) : this(itemName, false) {}
        public ContextMenu(string itemName, bool isValidateFunction) : this(itemName, isValidateFunction, 1000000) {}
        public ContextMenu(string itemName, bool isValidateFunction, int priority)
        {
            menuItem = itemName;
            validate = isValidateFunction;
            this.priority = priority;
        }

        //*undocumented*
        public readonly string menuItem;
        //*undocumented*
        public readonly bool validate;
        //*undocumented*
        public readonly int priority;
    }

    // Makes a script execute in edit mode.
    public sealed class ExecuteInEditMode : Attribute {}

    // Makes a variable not show up in the inspector but be serialized.
    public sealed class HideInInspector : Attribute {}

    // Sets a custom help URL for a script.
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
    public sealed class HelpURLAttribute : Attribute
    {
        public HelpURLAttribute(string url) { m_Url = url; }

        public string URL
        {
            get { return m_Url; }
        }

        readonly string m_Url;
    }

    [AttributeUsage(AttributeTargets.Class)]
    public class DefaultExecutionOrder : Attribute
    {
        public DefaultExecutionOrder(int order)
        {
            this.order = order;
        }

        public int order { get; private set; }
    }

    [System.AttributeUsage(System.AttributeTargets.Struct)]
    internal class IL2CPPStructAlignmentAttribute : System.Attribute
    {
        public int Align;
        public IL2CPPStructAlignmentAttribute()
        {
            Align = 1;
        }
    }
}

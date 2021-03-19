using System;
using System.Collections;
using System.Reflection;
using Unity.DataContract;

namespace Unity.PackageManager.Ivy
{
    static class TypeExtensions
    {
        public static bool CanWrite(this MemberInfo member)
        {
            if (member is PropertyInfo)
                return ((PropertyInfo)member).CanWrite;
            else
                return true;
        }

        public static void SetValue(this MemberInfo member, object target, object value)
        {
            if (member is PropertyInfo)
                ((PropertyInfo)member).GetSetMethod().Invoke(target, new object[] {value});
            else
                ((FieldInfo)member).SetValue(target, value);
        }

        public static object GetValue(this MemberInfo member, object target)
        {
            if (member is PropertyInfo)
                return ((PropertyInfo)member).GetGetMethod().Invoke(target, null);
            else
                return ((FieldInfo)member).GetValue(target);
        }
    }


    public static class Cloner
    {
        static BindingFlags bindingFlags = BindingFlags.Public | BindingFlags.Instance;
        public static ToType CloneObject<ToType>(object origin) where ToType : new()
        {
            Type originType = origin.GetType();
            ToType target = new ToType();
            Type targetType = target.GetType();

            CloneProperties(origin, originType, target, targetType);
            CloneFields(origin, originType, target, targetType);

            return target;
        }

        public static object CloneObject(object origin, Type targetType)
        {
            Type originType = origin.GetType();
            object target = null;
            ConstructorInfo ctor = null;
            try
            {
                target = Activator.CreateInstance(targetType);
            }
            catch
            {
                // nope
            }

            if (target == null)
            {
                if (originType != typeof(string))
                {
                    ctor = targetType.GetConstructor(new Type[] { originType });
                    if (ctor != null)
                        target = ctor.Invoke(new object[] { origin });
                    if (target == null)
                        ctor = targetType.GetConstructor(new Type[] { typeof(string) });
                    if (ctor != null)
                        target = ctor.Invoke(new object[] { origin.ToString() });
                }
            }

            if (target == null)
                return null;

            // if we went through the default ctor, deep clone, otherwise trust the ctor to fill up things
            if (ctor == null)
            {
                CloneProperties(origin, originType, target, targetType);
                CloneFields(origin, originType, target, targetType);
            }

            return target;
        }

        private static void CloneFields(object origin, Type originType,
            object target, Type targetType)
        {
            foreach (FieldInfo originField in originType.GetFields(bindingFlags))
            {
                object originValue = originField.GetValue(origin);
                Type originValueType = originField.FieldType;
                FieldInfo targetField = targetType.GetField(originField.Name, bindingFlags);
                if (targetField != null)
                {
                    Type targetValueType = targetField.FieldType;
                    CloneThing(targetField, target, originValue, originValueType, targetValueType);
                }
                else
                {
                    PropertyInfo targetProp = targetType.GetProperty(originField.Name, bindingFlags);
                    if (targetProp != null)
                    {
                        Type targetValueType = targetProp.PropertyType;
                        CloneThing(targetProp, target, originValue, originValueType, targetValueType);
                    }
                }
            }
        }

        private static void CloneProperties(object origin, Type originType,
            object target, Type targetType)
        {
            foreach (PropertyInfo originProp in originType.GetProperties(bindingFlags))
            {
                object originValue = originProp.GetValue(origin);
                Type originValueType = originProp.PropertyType;

                PropertyInfo targetProp = targetType.GetProperty(originProp.Name, bindingFlags);
                if (targetProp != null)
                {
                    Type targetValueType = targetProp.PropertyType;
                    CloneThing(targetProp, target, originValue, originValueType, targetValueType);
                }
                else
                {
                    FieldInfo targetField = targetType.GetField(originProp.Name, bindingFlags);
                    if (targetField != null)
                    {
                        Type targetValueType = targetField.FieldType;
                        CloneThing(targetField, target, originValue, originValueType, targetValueType);
                    }
                }
            }
        }

        private static void CloneThing(MemberInfo thing, object target, object originValue, Type originValueType, Type targetValueType)
        {
            // special casing read-only properties that are of type list. we can't set them, but we can modify them
            if (!thing.CanWrite() || (originValue != null && !targetValueType.IsAssignableFrom(originValueType)))
            {
                if (typeof(IList).IsAssignableFrom(originValueType))
                {
                    IList targetList = thing.GetValue(target) as IList;
                    // if the origin property is empty but we can write to the target, then create a new list
                    if (targetList == null && thing.CanWrite())
                    {
                        targetList = Activator.CreateInstance(targetValueType) as IList;
                        thing.SetValue(target, targetList);
                    }

                    // if we have a list, it doesn't matter if the target property is readonly, we can modify it anyways
                    if (targetList != null)
                    {
                        IList originList = originValue as IList;
                        foreach (object item in originList)
                        {
                            if (item == null)
                                continue;
                            object targetValue = CloneObject(item, GetElementType(targetList.GetType()));
                            targetList.Add(targetValue);
                        }
                        return;
                    }
                }
            }

            // can't clone target readonly properties, ignoring
            if (!thing.CanWrite())
                return;

            if (originValue != null && !targetValueType.IsAssignableFrom(originValueType))
            {
                if (targetValueType.IsAssignableFrom(typeof(Uri)) &&
                    originValueType.IsAssignableFrom(typeof(string)))
                {
                    Uri uri = null;
                    if (Uri.TryCreate((string)originValue, UriKind.RelativeOrAbsolute, out uri))
                        thing.SetValue(target, uri);
                }
                else if (targetValueType.IsAssignableFrom(typeof(string)) &&
                         originValueType.IsAssignableFrom(typeof(Uri)))
                {
                    string uriString = string.Empty;
                    if (null != originValue)
                        uriString = originValue.ToString();
                    thing.SetValue(target, uriString);
                }
                else if (targetValueType.IsAssignableFrom(typeof(PackageVersion)) &&
                         originValueType.IsAssignableFrom(typeof(string)))
                {
                    try
                    {
                        PackageVersion version = new PackageVersion((string)originValue);
                        thing.SetValue(target, version);
                    }
                    catch {}
                }
                else if (targetValueType.IsAssignableFrom(typeof(string)) &&
                         originValueType.IsAssignableFrom(typeof(PackageVersion)))
                {
                    string versionString = string.Empty;
                    if (originValue != null)
                        versionString = originValue.ToString();
                    thing.SetValue(target, versionString);
                }
                else if (targetValueType.IsClass)
                {
                    object targetValue = CloneObject(originValue, targetValueType);
                    // try and get an instance created from a getter (could be private constructor/singleton)
                    if (targetValue == null)
                        targetValue = thing.GetValue(target);
                    if (targetValue != null)
                        thing.SetValue(target, targetValue);
                    else
                        thing.SetValue(target, originValue);
                }
                else if (targetValueType.IsEnum)
                    thing.SetValue(target, (int)originValue);
            }
            else
            {
                if (originValue == null)
                    thing.SetValue(target, originValue);
                else if (targetValueType.IsClass && targetValueType != typeof(string))
                {
                    object targetValue = CloneObject(originValue, targetValueType);
                    // try and get an instance created from a getter (could be private constructor/singleton)
                    if (targetValue == null)
                        targetValue = thing.GetValue(target);
                    if (targetValue != null)
                        thing.SetValue(target, targetValue);
                    else
                        thing.SetValue(target, originValue);
                }
                else
                    thing.SetValue(target, originValue);
            }
        }

        private static Type GetElementType(Type type)
        {
            Type elt = type.GetElementType();
            if (elt == null)
            {
                if (type.IsGenericType)
                {
                    Type[] typeArgs = type.GetGenericArguments();
                    if (typeArgs.Length > 0)
                        elt = typeArgs[0];
                }
                else if (type.BaseType != null)
                    elt = GetElementType(type.BaseType);
            }
            return elt;
        }
    }
}

using System;
using Mono.Cecil;

namespace Unity.BindingsGenerator.Core
{
    public class Visitor
    {
        public virtual void Visit(AssemblyDefinition assemblyDefinition)
        {
        }

        public virtual void Visit(TypeDefinition typeDefinition)
        {
        }

        public virtual void Visit(MethodDefinition methodDefinition)
        {
        }

        public virtual void Visit(PropertyDefinition propertyDefinition)
        {
        }
    }

    public class NonFilteringVisitor
    {
        protected readonly AssemblyDefinition _assembly;
        protected readonly TypeDefinition     _type;
        protected readonly MethodDefinition   _method;

        public NonFilteringVisitor(AssemblyDefinition assemblyDefinition)
        {
            _assembly = assemblyDefinition;
        }

        public NonFilteringVisitor(TypeDefinition typeDefinition)
        {
            _type = typeDefinition;
        }

        public NonFilteringVisitor(MethodDefinition methodDefinition)
        {
            _method = methodDefinition;
        }

        public NonFilteringVisitor Accept(Visitor visitor)
        {
            if (_assembly != null)
            {
                Accept(_assembly, visitor);
                return this;
            }

            if (_type != null)
            {
                Accept(_type, visitor);
                return this;
            }

            if (_method != null)
            {
                Accept(_method, visitor);
                return this;
            }

            throw new Exception("Wrong instantiation of visitor");
        }

        protected virtual void Accept(AssemblyDefinition assemblyDefinition, Visitor visitor)
        {
            visitor.Visit(assemblyDefinition);

            foreach (var typeDefinition in assemblyDefinition.MainModule.Types)
            {
                Accept(typeDefinition, visitor);
            }
        }

        protected virtual void Accept(TypeDefinition typeDefinition, Visitor visitor)
        {
            visitor.Visit(typeDefinition);

            foreach (var methodDefinition in typeDefinition.Methods)
            {
                Accept(methodDefinition, visitor);
            }

            foreach (var propertyDefinition in typeDefinition.Properties)
            {
                Accept(propertyDefinition, visitor);
            }

            foreach (var nestedTypeDefinition in typeDefinition.NestedTypes)
            {
                Accept(nestedTypeDefinition, visitor);
            }
        }

        protected virtual void Accept(MethodDefinition methodDefinition, Visitor visitor)
        {
            visitor.Visit(methodDefinition);
        }

        protected virtual void Accept(PropertyDefinition propertyDefinition, Visitor visitor)
        {
            visitor.Visit(propertyDefinition);
        }
    }

    // This class will only visit types/methods/properties that we actually have to generate something for
    public class FilteringVisitor : NonFilteringVisitor
    {
        public FilteringVisitor(AssemblyDefinition assemblyDefinition) : base(assemblyDefinition)
        {
        }

        public FilteringVisitor(TypeDefinition typeDefinition) : base(typeDefinition)
        {
        }

        public FilteringVisitor(MethodDefinition methodDefinition) : base(methodDefinition)
        {
        }

        protected override void Accept(TypeDefinition typeDefinition, Visitor visitor)
        {
            if (typeDefinition.MayRequireBindingsGeneration())
            {
                visitor.Visit(typeDefinition);

                foreach (var methodDefinition in typeDefinition.Methods)
                {
                    Accept(methodDefinition, visitor);
                }

                foreach (var propertyDefinition in typeDefinition.Properties)
                {
                    Accept(propertyDefinition, visitor);
                }
            }

            foreach (var nestedTypeDefinition in typeDefinition.NestedTypes)
            {
                Accept(nestedTypeDefinition, visitor);
            }
        }

        protected override void Accept(MethodDefinition methodDefinition, Visitor visitor)
        {
            if (methodDefinition.MayRequireBindingsGeneration())
                visitor.Visit(methodDefinition);
        }

        protected override void Accept(PropertyDefinition propertyDefinition, Visitor visitor)
        {
            if (propertyDefinition.MayRequireBindingsGeneration())
                visitor.Visit(propertyDefinition);
        }
    }
}

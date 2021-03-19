using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Unity.RegistrationGenerator.Tests
{
    class Component0 : Component
    {
    }

    class Component1 : Component
    {
    }

    [RequireComponent(typeof(Component1))]
    class Component2 : Component
    {
    }

    [RequireComponent(typeof(Component1))]
    [RequireComponent(typeof(Component0))]
    class Component2multi : Component
    {
    }

    class Component3 : Component2
    {
    }

    [RequireComponent(typeof(Component0))]
    class Component4 : Component2
    {
    }

    class Component5 : Component
    {
    }
}

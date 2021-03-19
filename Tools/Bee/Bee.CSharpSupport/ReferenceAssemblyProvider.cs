using System;
using System.Linq;
using Bee.Core;
using Bee.DotNet;
using NiceIO;

namespace Unity.BuildSystem.CSharpSupport
{
    public abstract class ReferenceAssemblyProvider
    {
        private static Lazy<ReferenceAssemblyProvider> _bestAvailable = new Lazy<ReferenceAssemblyProvider>(() =>
        {
            return DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<ReferenceAssemblyProvider>("Bee.*.dll").OrderByDescending(r => r.PreferenceScore).First();
        });
        public static ReferenceAssemblyProvider BestAvailable => _bestAvailable.Value;

        public abstract int PreferenceScore { get; }

        //These are directory listings of https://github.com/mono/reference-assemblies
        private static readonly string[] ReferenceAssemblyNamesFor46 =
        {
            "Microsoft.Build.Engine.dll",
            "Microsoft.Build.Framework.dll",
            "Microsoft.Build.Tasks.v4.0.dll",
            "Microsoft.Build.Utilities.v4.0.dll",
            "Microsoft.Build.dll",
            "Microsoft.CSharp.dll",
            "Microsoft.VisualBasic.dll",
            "Microsoft.VisualC.dll",
            "System.ComponentModel.Composition.dll",
            "System.ComponentModel.DataAnnotations.dll",
            "System.Configuration.Install.dll",
            "System.Configuration.dll",
            "System.Core.dll",
            "System.Data.DataSetExtensions.dll",
            "System.Data.Entity.dll",
            "System.Data.Linq.dll",
            "System.Data.OracleClient.dll",
            "System.Data.Services.Client.dll",
            "System.Data.Services.dll",
            "System.Data.dll",
            "System.Deployment.dll",
            "System.Design.dll",
            "System.DirectoryServices.Protocols.dll",
            "System.DirectoryServices.dll",
            "System.Drawing.Design.dll",
            "System.Drawing.dll",
            "System.Dynamic.dll",
            "System.EnterpriseServices.dll",
            "System.IO.Compression.FileSystem.dll",
            "System.IO.Compression.dll",
            "System.IdentityModel.Selectors.dll",
            "System.IdentityModel.dll",
            "System.Management.dll",
            "System.Messaging.dll",
            "System.Net.Http.WebRequest.dll",
            "System.Net.Http.dll",
            "System.Net.dll",
            "System.Numerics.dll",
            "System.Reflection.Context.dll",
            "System.Runtime.Caching.dll",
            "System.Runtime.DurableInstancing.dll",
            "System.Runtime.InteropServices.RuntimeInformation.dll",
            "System.Runtime.Remoting.dll",
            "System.Runtime.Serialization.Formatters.Soap.dll",
            "System.Runtime.Serialization.dll",
            "System.Security.dll",
            "System.ServiceModel.Activation.dll",
            "System.ServiceModel.Discovery.dll",
            "System.ServiceModel.Internals.dll",
            "System.ServiceModel.Routing.dll",
            "System.ServiceModel.Web.dll",
            "System.ServiceModel.dll",
            "System.ServiceProcess.dll",
            "System.Transactions.dll",
            "System.Web.Abstractions.dll",
            "System.Web.ApplicationServices.dll",
            "System.Web.DynamicData.dll",
            "System.Web.Extensions.Design.dll",
            "System.Web.Extensions.dll",
            "System.Web.Mobile.dll",
            "System.Web.RegularExpressions.dll",
            "System.Web.Routing.dll",
            "System.Web.Services.dll",
            "System.Web.dll",
            "System.Windows.Forms.DataVisualization.dll",
            "System.Windows.Forms.dll",
            "System.Windows.dll",
            "System.Workflow.Activities.dll",
            "System.Workflow.ComponentModel.dll",
            "System.Workflow.Runtime.dll",
            "System.Xaml.dll",
            "System.Xml.Linq.dll",
            "System.Xml.Serialization.dll",
            "System.Xml.dll",
            "System.dll",
            "WindowsBase.dll",
            "mscorlib.dll"
        };

        private static string[] ReferenceAssemblyNamesFor20 =
        {
            "Accessibility.dll",
            "CustomMarshalers.dll",
            "Microsoft.Build.Engine.dll",
            "Microsoft.Build.Framework.dll",
            "Microsoft.Build.Tasks.dll",
            "Microsoft.Build.Utilities.dll",
            "Microsoft.VisualBasic.dll",
            "Microsoft.VisualC.dll",
            "System.ComponentModel.DataAnnotations.dll",
            "System.Configuration.Install.dll",
            "System.Configuration.dll",
            "System.Core.dll",
            "System.Data.DataSetExtensions.dll",
            "System.Data.Linq.dll",
            "System.Data.OracleClient.dll",
            "System.Data.Services.Client.dll",
            "System.Data.Services.dll",
            "System.Data.dll",
            "System.Design.dll",
            "System.DirectoryServices.Protocols.dll",
            "System.DirectoryServices.dll",
            "System.Drawing.Design.dll",
            "System.Drawing.dll",
            "System.EnterpriseServices.dll",
            "System.IdentityModel.Selectors.dll",
            "System.IdentityModel.dll",
            "System.Management.dll",
            "System.Messaging.dll",
            "System.Net.dll",
            "System.Runtime.Remoting.dll",
            "System.Runtime.Serialization.Formatters.Soap.dll",
            "System.Runtime.Serialization.dll",
            "System.Security.dll",
            "System.ServiceModel.Web.dll",
            "System.ServiceModel.dll",
            "System.ServiceProcess.dll",
            "System.Transactions.dll",
            "System.Web.Abstractions.dll",
            "System.Web.DynamicData.dll",
            "System.Web.Extensions.Design.dll",
            "System.Web.Extensions.dll",
            "System.Web.Routing.dll",
            "System.Web.Services.dll",
            "System.Web.dll",
            "System.Windows.Forms.dll",
            "System.Xml.Linq.dll",
            "System.Xml.dll",
            "System.dll",
            "WindowsBase.dll",
            "mscorlib.dll"
        };

        public abstract bool TryFor(Framework framework, bool exactMatch, out NPath[] result, out string reason);


        public static string[] ReferenceAssemblyNamesFor(Framework framework)
        {
            switch (framework.Name)
            {
                case "v4.6":
                case "v4.0":  //<- hack while this file is not automatically generated yet
                    return ReferenceAssemblyNamesFor46;
                case "v3.5":
                case "v2.0":
                    return ReferenceAssemblyNamesFor20;
            }

            throw new ArgumentOutOfRangeException($"not yet supported {framework}");
        }
    }
}

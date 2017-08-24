using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Windows.Media;
using System.Diagnostics;


namespace TestLoadAndShowDCM
{
    public static class NativeMethods
    {
        [DllImport("LoadAndShowDCMDll.dll",
             EntryPoint = "LoadAndShowDCM",
             CharSet = CharSet.Unicode,
             CallingConvention = CallingConvention.StdCall)]
        internal static extern unsafe int LoadAndShowByPathAndDim(string filePath);
    }
}

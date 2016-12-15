using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Text.RegularExpressions;
using System.Xml;

namespace MtInject {
    class Program {
        static void Main(string[] args) {
            if (args.Length < 2) {
                Console.Error.Write(@"MtInject C:\Proj\PDF4Ax\Debug C:\Proj\PDF4Ax\Debug\mtinfo.txt");
                Environment.Exit(1);
            }
            String dir = args[0];
            SortedDictionary<String, Mani> dict = new SortedDictionary<string, Mani>();
            foreach (String row in File.ReadAllLines(args[1], Encoding.UTF8)) {
                String[] cols = row.Split('\t');
                dict[cols[0]] = new Mani(cols);
            }

            StringWriter wr = new StringWriter();

            foreach (KeyValuePair<string, Mani> kv in dict) {
                List<string> DLLs = new List<string>();
                {
                    ProcessStartInfo psi = new ProcessStartInfo(@"C:\Program Files\Microsoft Visual Studio 8\VC\bin\dumpbin.exe", " /imports \"" + Path.Combine(dir, kv.Key) + "\"");
                    psi.WorkingDirectory = @"C:\Program Files\Microsoft Visual Studio 8\Common7\IDE\";
                    psi.UseShellExecute = false;
                    psi.RedirectStandardOutput = true;
                    Process p = Process.Start(psi);
                    String rows = p.StandardOutput.ReadToEnd();
                    foreach (Match M in Regex.Matches(rows, "^    (?<a>.+?\\.dll)", RegexOptions.Multiline)) {
                        String dll = M.Groups["a"].Value;
                        DLLs.Add(dll);
                    }
                }

                {
                    XmlDocument xmlo = new XmlDocument();
                    XmlNamespaceManager xnm = new XmlNamespaceManager(xmlo.NameTable);
                    String Urn = "urn:schemas-microsoft-com:asm.v1";
                    XmlElement assembly = xmlo.CreateElement("assembly", Urn);
                    xmlo.AppendChild(assembly);
                    assembly.SetAttribute("manifestVersion", "1.0");
                    {
                        XmlElement assemblyIdentity = xmlo.CreateElement("assemblyIdentity", Urn);
                        assembly.AppendChild(assemblyIdentity);
                        assemblyIdentity.SetAttribute("type", "win32");
                        assemblyIdentity.SetAttribute("name", kv.Value.Name);
                        assemblyIdentity.SetAttribute("version", kv.Value.Ver);
                        assemblyIdentity.SetAttribute("processorArchitecture", "x86");
                        //assemblyIdentity.SetAttribute("publicKeyToken", "1fc8b3b9a1e18e3b");
                    }
                    foreach (String dll in DLLs) {
                        Mani refm;
                        if (dict.TryGetValue(dll, out refm)) {
                            XmlElement dependency = xmlo.CreateElement("dependency", Urn);
                            assembly.AppendChild(dependency);
                            {
                                XmlElement dependentAssembly = xmlo.CreateElement("dependentAssembly", Urn);
                                dependency.AppendChild(dependentAssembly);
                                {
                                    XmlElement assemblyIdentity = xmlo.CreateElement("assemblyIdentity", Urn);
                                    dependentAssembly.AppendChild(assemblyIdentity);

                                    assemblyIdentity.SetAttribute("type", "win32");
                                    assemblyIdentity.SetAttribute("name", refm.Name);
                                    assemblyIdentity.SetAttribute("version", refm.Ver);
                                    assemblyIdentity.SetAttribute("processorArchitecture", "x86");
                                    //assemblyIdentity.SetAttribute("publicKeyToken", "1fc8b3b9a1e18e3b");
                                }
                            }
                        }
                    }

                    String fpxml = kv.Key + ".manifest";
                    xmlo.Save(fpxml);
                    Console.Write("");

                    wr.WriteLine("mt -manifest \"{0}\" -outputresource:{1};#2"
                        , fpxml
                        , Path.Combine(dir, kv.Key)
                        );
                    wr.WriteLine("if not \"%ERRORLEVEL%\"==\"0\" pause");
                }
                Console.Write("");

            }
            File.WriteAllText("Inject.bat", wr.ToString(), Encoding.Default);
        }

        class Mani {
            String[] cols;

            public Mani(String[] cols) {
                this.cols = cols;
            }

            public String Ver {
                get {
                    Match M = Regex.Match(cols[1], "mingw32-(?<n>[^\\-]+)\\-(?<v>[^\\-]+)\\-");
                    return M.Groups["v"].Value;
                }
            }

            public String Name {
                get {
                    Match M = Regex.Match(cols[1], "mingw32-(?<n>[^\\-]+)\\-(?<v>[^\\-]+)\\-");
                    return M.Groups["n"].Value;
                }
            }
        }
    }
}

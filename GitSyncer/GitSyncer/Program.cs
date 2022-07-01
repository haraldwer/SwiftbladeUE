using System;
using System.IO;
using System.Diagnostics;

namespace GitSyncer
{
    class Program
    {
        static string FindDir(string aDirectory)
        {
            var dirs = Directory.GetDirectories(aDirectory);
            if (dirs != null)
            {
                int index = Array.FindIndex(dirs, s => s.Equals(".git"));
                if (index >= 0 && index < dirs.Length) 
                    return aDirectory;
            }
            var parentDir = Directory.GetParent(aDirectory);
            return parentDir.Exists ? FindDir(parentDir.Name) : "";
        }

        static void Cmd(string aDirectory, string aCommand)
        {
            Process process = new Process();
            ProcessStartInfo info = new ProcessStartInfo();
            info.WorkingDirectory = aDirectory;
            info.WindowStyle = ProcessWindowStyle.Hidden;
            info.FileName = "cmd.exe";
            info.Arguments = aCommand;
            process.StartInfo = info;
            if (!process.Start())
                Console.WriteLine("Failed to start process: " + aCommand);
            process.WaitForExit();
            Console.WriteLine("Process: \n" + process.StandardOutput); 
        }

        static void Main(string[] args)
        {
            var directory = FindDir(Directory.GetCurrentDirectory());
            if (directory.Equals(""))
                Console.WriteLine("Failed to find directory");

            Cmd(directory, "git config -global user.name \"Harald Wergeland\"");
            Cmd(directory, "git config -global user.name \"harwer009@gmail.com\"");

            Cmd(directory, "git add *");
            Cmd(directory, "git commit -m \"Stuff\"");
            Cmd(directory, "git pull");
            Cmd(directory, "git push -all origin");

            Console.WriteLine("Helper finished");
            Console.ReadLine();
        }
    }
}

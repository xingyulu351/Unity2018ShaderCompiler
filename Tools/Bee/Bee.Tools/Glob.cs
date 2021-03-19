using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using NiceIO;
using Unity.BuildTools;
using Unity.TinyProfiling;

namespace Bee.Tools
{
    public static class Glob
    {
        static readonly char[] Wildcards = { '*', '?' };
        static readonly char[] PathSeparators = {'/', '\\', ':'};

        public static string[] ExtensionsCpp { get; } = {"cpp", "h"};
        public static string[] ExtensionsC { get; } = {"c", "h"};

        public static NPath[] ByExtensions(NPath folder, params string[] extensions)
        {
            using (TinyProfiler.Section("Glob.ByExt", folder.ToString()))
            {
                return folder.Files(extensions);
            }
        }

        public static NPath[] ByExtensionsRecursive(NPath folder, params string[] extensions)
        {
            using (TinyProfiler.Section("Glob.ByExtRec", folder.ToString()))
            {
                return folder.Files(extensions, true);
            }
        }

        /// <summary>
        ///   Searches for files and directories by given patterns.
        /// </summary>
        /// <remarks>
        ///   Search pattern has this form: "[search patterns]".
        ///
        ///   Supported matching patterns:
        ///   <code>
        ///     ?                     matches a single character.
        ///     *                     matches 0 or more characters.
        ///     / at end of pattern   instead of file search will start directory search
        ///     **/name               search for files named "name" recursivelly
        ///     **/name/              search for directories named "name" recursivelly
        ///   </code>
        ///
        ///   Pattern expansions done automatically:
        ///   <code>
        ///     /path/**   -> /path/**/*
        ///     /path/**/  -> /path/**/*/
        ///     **name     -> **/*name
        ///     name**     -> name*/**
        ///     path**name -> path*/**/*name
        ///   </code>
        /// </remarks>
        public static IEnumerable<string> Search(string searchPattern)
        {
            using (TinyProfiler.Section("Glob.Search", searchPattern))
            {
                if (searchPattern.Length == 0)
                    return Array.Empty<string>();

                int patternStart = searchPattern.IndexOfAny(Wildcards);
                // Check if search is without any patterns and is just simple file or directory path
                if (patternStart < 0)
                {
                    var path = new NPath(searchPattern);
                    var lastChar = searchPattern.Last();
                    var searchForDirectory = lastChar == '/' || lastChar == '\\';
                    if ((searchForDirectory && path.DirectoryExists()) || (!searchForDirectory && path.FileExists()))
                        return new[] { searchPattern };

                    return Array.Empty<string>();
                }

                if (IsSimpleRecursiveSearch(searchPattern, patternStart))
                    return SearchFilesRecursively(searchPattern, patternStart);

                if (IsSimpleRecursiveSearchByExtension(searchPattern, patternStart))
                    return SearchFilesByExtensionRecursively(searchPattern, patternStart);

                var patterns = ExpandGlobs(SplitPatterns(searchPattern));

                if (patterns.Length == 0)
                    return Array.Empty<string>();

                var searchForDirectories = patterns.Last() == "/";
                var isRooted = patterns.First().Contains(":") || patterns.First() == "/";
                var startFromPattern = isRooted ? 1 : 0;
                var startingPath = isRooted ? patterns.First() : "";

                var results = new List<string>(1024);
                Search(patterns, startFromPattern, startingPath, false, searchForDirectories, results);
                return results;
            }
        }

        static bool IsSimpleRecursiveSearch(string pattern, int patternStart)
        {
            if (patternStart == pattern.Length - 2 && pattern.EndsWith("/**"))
                return true;
            if (patternStart == pattern.Length - 4 && pattern.EndsWith("/**/*"))
                return true;

            return false;
        }

        static bool IsSimpleRecursiveSearchByExtension(string pattern, int patternStart)
        {
            if (pattern.IndexOf("/**/*.", StringComparison.Ordinal) == patternStart - 1)
            {
                if (pattern.IndexOfAny(PathSeparators, patternStart + 5) == -1 &&
                    pattern.IndexOfAny(Wildcards, patternStart + 5) == -1)
                    return true;
            }

            return false;
        }

        static IEnumerable<string> SearchFilesByExtensionRecursively(string pattern, int patternStart)
        {
            var results = new List<string>(1024);
            var ext = pattern.Substring(patternStart + 3);
            var path = pattern.Substring(0, patternStart - 1).ToNPath();

            using (TinyProfiler.Section("FilesByExtensionRecursively"))
                if (path.DirectoryExists())
                    foreach (var file in path.Files(ext, true))
                        results.Add(file.ToString(SlashMode.Forward));

            return results;
        }

        static IEnumerable<string> SearchFilesRecursively(string pattern, int patternStart)
        {
            var results = new List<string>(1024);
            var path = pattern.Substring(0, patternStart - 1).ToNPath();

            using (TinyProfiler.Section("FilesRecursively"))
                if (path.DirectoryExists())
                    foreach (var file in path.Files("*", true))
                        results.Add(file.ToString(SlashMode.Forward));

            return results;
        }

        /// <summary>
        ///   Splits pattern path into tokens: "/my/path" becomes "/" "my" "/" "path".
        /// </summary>
        ///
        /// <remarks>
        ///   We could actually drop tokens like "/", ".", etc, but jam doesn't normalize paths when globing, so we need
        ///   to preserve this behavior for now.
        /// </remarks>
        static string[] SplitPatterns(string patterns)
        {
            var split = new List<string>(64);

            int current = 0;
            while (current < patterns.Length)
            {
                var start = current;
                current = patterns.IndexOfAny(new char[] {'/', '\\'}, current);
                if (current < 0)
                {
                    split.Add(patterns.Substring(start));
                    break;
                }

                if (start != current)
                    split.Add(patterns.Substring(start, current - start));
                split.Add("/");

                current++;
            }

            return split.ToArray();
        }

        /// <summary>
        ///   Expand globs:
        ///   <code>
        ///     /path/**   -> /path/**/*
        ///     /path/**/  -> /path/**/*/
        ///     **name     -> **/*name
        ///     name**     -> name*/**
        ///     path**name -> path*/**/*name
        ///   </code>
        /// </summary>
        static string[] ExpandGlobs(string[] patterns)
        {
            var list = new List<string>(patterns.Length + 8);

            for (int i = 0; i < patterns.Length; i++)
            {
                var pattern = patterns[i];
                if (pattern == "**")
                {
                    list.Add("**");
                    // ** -> **/*
                    if (i + 1 == patterns.Length)
                    {
                        list.Add("/");
                        list.Add("*");
                    }
                    // **/ -> **/*/
                    else if ((i + 2 == patterns.Length) && (patterns[i + 1] == "/"))
                    {
                        list.Add("/");
                        list.Add("*");
                    }
                }
                // **name -> **/*name
                else if (pattern.StartsWith("**"))
                {
                    list.Add("**");
                    list.Add("/");
                    list.Add(pattern.Substring(1));
                }
                // name** -> name*/**
                else if (pattern.EndsWith("**"))
                {
                    list.Add(pattern.Substring(0, pattern.Length - 1));
                    list.Add("/");
                    list.Add("**");
                }
                // path**name -> path*/**/*name
                else if (pattern.Contains("**"))
                {
                    var index = pattern.IndexOf("**");
                    list.Add(pattern.Substring(0, index + 1));
                    list.Add("/");
                    list.Add("**");
                    list.Add("/");
                    list.Add(pattern.Substring(index + 1, pattern.Length - (index + 1)));
                }
                else
                {
                    list.Add(pattern);
                }
            }
            return list.ToArray();
        }

        struct DirVisit
        {
            public string name;
            public bool goToNextPattern;
        }

        /// <summary>
        ///   Actually does all the glob searching enumerating through directories and files, and matching them to patterns.
        /// </summary>
        static void Search(string[] patterns, int index, string directory, bool recursive, bool searchForDirectories, List<string> results)
        {
            if (index == patterns.Length)
                return;

            var match = patterns[index];

            // Skip all non pattern tokens like ".", "..", "/"
            while (true)
            {
                if (IsPattern(match))
                    break;

                directory += match;
                index++;

                if (index < patterns.Length)
                    match = patterns[index];
                else
                    break;
            }

            var regex = new Regex("^" + Regex.Escape(match).Replace(@"\*", ".*").Replace(@"\?", ".") + "$", RegexOptions.IgnoreCase | RegexOptions.Singleline);
            var hasMorePatterns = HasMorePatterns(patterns, index + 1);
            var matchingDirectory = hasMorePatterns;

            // If it's a recursive search, we restart search with the next directory or file match pattern.
            if (match == "**")
            {
                Search(patterns, index + 2, directory, true, searchForDirectories, results);
                return;
            }

            // Enumerate files
            if (!searchForDirectories && !matchingDirectory)
            {
                try
                {
                    var npathFiles = directory.ToNPath().Files()
                        .Where(f => IsFileOrSymlink(f.ToString()) && regex.IsMatch(f.FileName))
                        .Select(f => f.ToString()).ToList();

                    // Need to use GetFileSystemEntries instead of GetFiles to get symlinks.
                    var tempResults = new List<string>();
                    var files = Directory.GetFileSystemEntries(directory);
                    foreach (var file in files)
                    {
                        var fileName = GetLastPathElement(file);
                        if (IsFileOrSymlink(file) && regex.IsMatch(fileName))
                            tempResults.Add(directory.ToNPath().Combine(fileName).ToString(SlashMode.Forward));
                    }

                    // Verify that both techniques found the same files
                    if (npathFiles.Except(tempResults).Any())
                        Console.WriteLine("NPath found files that globbing did not! \n\t" + string.Join("\n\t", npathFiles.Except(tempResults)));

                    if (tempResults.Except(npathFiles).Any())
                        Console.WriteLine("Globbing found files that NPath did not! \n\t" + string.Join("\n\t", tempResults.Except(npathFiles)));

                    results.AddRange(tempResults);
                }
                catch (IOException) {}
            }

            // Enumerate and/or recurse into directories
            if (matchingDirectory || recursive || searchForDirectories)
            {
                try
                {
                    var matchingAgainstPattern = match.IndexOfAny(Wildcards) >= 0;
                    var directoriesToVisit = new List<DirVisit>(16);

                    // For simple case where pattern's match is plain directory name and we're not recursing currently, we just go into it.
                    if (!matchingAgainstPattern && !recursive)
                    {
                        directoriesToVisit.Add(new DirVisit() {name = match, goToNextPattern = hasMorePatterns});
                    }
                    else
                    {
                        var directories = Directory.GetDirectories(directory);
                        // If jam detects a directory match when searching recursively, it visits the directory first,
                        // and later recurses into others. So we put the matching directory as first.
                        if (recursive && !matchingAgainstPattern && directories.Contains(match))
                            directories = MoveToFront(directories, match);
                        foreach (var dir in directories)
                        {
                            var name = GetLastPathElement(dir);
                            var nameMatchesPattern = regex.IsMatch(name);

                            if (!recursive && !nameMatchesPattern)
                                continue;

                            if (IsSymlink(dir))
                                continue;

                            if (searchForDirectories && nameMatchesPattern && !hasMorePatterns)
                                results.Add(directory + name + '/');

                            if (nameMatchesPattern)
                                directoriesToVisit.Add(new DirVisit() {name = name, goToNextPattern = hasMorePatterns});
                            else
                                directoriesToVisit.Add(new DirVisit() {name = name, goToNextPattern = false});
                        }
                    }

                    if (hasMorePatterns || recursive)
                        foreach (var dir in directoriesToVisit)
                            Search(patterns, dir.goToNextPattern ? index + 1 : index, directory + dir.name + (dir.goToNextPattern ? "" : "/"), recursive, searchForDirectories, results);
                }
                catch (DirectoryNotFoundException) {}
            }
        }

        static string[] MoveToFront(string[] directories, string match)
        {
            var index = directories.IndexOf(match);
            var info = directories[index];
            Array.Copy(directories, 0, directories, 1, index);
            directories[0] = info;
            return directories;
        }

        static bool IsFileOrSymlink(string info)
        {
            var attributes = File.GetAttributes(info);
            return !attributes.HasFlag(FileAttributes.Directory) || attributes.HasFlag(FileAttributes.ReparsePoint);
        }

        static bool IsSymlink(string info)
        {
            return File.GetAttributes(info).HasFlag(FileAttributes.ReparsePoint);
        }

        static bool HasMorePatterns(string[] patterns, int index)
        {
            for (int i = index; i < patterns.Length; i++)
                if (IsPattern(patterns[i]))
                    return true;

            return false;
        }

        static bool IsPattern(string text)
        {
            if (text == "." || text == ".." || text == "/")
                return false;
            return true;
        }

        static string GetLastPathElement(string path)
        {
            int index = path.LastIndexOfAny(PathSeparators);
            if (index >= 0)
                return path.Substring(index + 1);
            return path;
        }
    }
}

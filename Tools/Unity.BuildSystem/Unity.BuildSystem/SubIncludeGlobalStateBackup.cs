using System;
using JamSharp.Runtime;

namespace Unity.BuildSystem
{
    class SubIncludeGlobalStateBackup : IDisposable
    {
        readonly JamList SUBDIR = new JamList();
        readonly JamList SUBDIR_DOWN = new JamList();
        readonly JamList SEARCH_SOURCE = new JamList();
        readonly JamList LOCATE_SOURCE = new JamList();
        readonly JamList LOCATE_TARGET = new JamList();
        readonly JamList SOURCE_GRIST = new JamList();
        readonly JamList HDRGRIST = new JamList();
        readonly JamList ACTIVE_PROJECT = new JamList();
        readonly JamList ALL_SUBDIR_TOKENS = new JamList();

        public SubIncludeGlobalStateBackup()
        {
            ACTIVE_PROJECT.Assign(ConvertedJamFile.Vars.ACTIVE_PROJECT);
            SUBDIR.Assign(ConvertedJamFile.Vars.SUBDIR);
            SUBDIR_DOWN.Assign(ConvertedJamFile.Vars.SUBDIR_DOWN);
            SEARCH_SOURCE.Assign(ConvertedJamFile.Vars.SEARCH_SOURCE);
            ALL_SUBDIR_TOKENS.Assign(ConvertedJamFile.Vars.ALL_SUBDIR_TOKENS);
            LOCATE_SOURCE.Assign(ConvertedJamFile.Vars.LOCATE_SOURCE);
            LOCATE_TARGET.Assign(ConvertedJamFile.Vars.LOCATE_TARGET);
            SOURCE_GRIST.Assign(ConvertedJamFile.Vars.SOURCE_GRIST);
            HDRGRIST.Assign(ConvertedJamFile.Vars.HDRGRIST);
        }

        public void Dispose()
        {
            ConvertedJamFile.Vars.ACTIVE_PROJECT.Assign(ACTIVE_PROJECT);
            ConvertedJamFile.Vars.SUBDIR.Assign(SUBDIR);
            ConvertedJamFile.Vars.SUBDIR_DOWN.Assign(SUBDIR_DOWN);
            ConvertedJamFile.Vars.SEARCH_SOURCE.Assign(SEARCH_SOURCE);
            ConvertedJamFile.Vars.ALL_SUBDIR_TOKENS.Assign(ALL_SUBDIR_TOKENS);
            ConvertedJamFile.Vars.LOCATE_SOURCE.Assign(LOCATE_SOURCE);
            ConvertedJamFile.Vars.LOCATE_TARGET.Assign(LOCATE_TARGET);
            ConvertedJamFile.Vars.SOURCE_GRIST.Assign(SOURCE_GRIST);
            ConvertedJamFile.Vars.HDRGRIST.Assign(HDRGRIST);
        }
    }
}

package BuiltinShaders;

use strict;
use warnings;
use File::Basename;
use File::Find;
use File::Path qw (mkpath rmtree);
use File::Slurp;
use File::Spec;
use File::chdir;
use lib File::Spec->rel2abs(dirname($0) . "/..");
use Carp qw (croak carp);
use Tools qw (UCopy ProgressMessage);
use lib File::Spec->rel2abs(dirname($0) . "/../../External/Perl/lib");

# TODO: Implement rsync, find, zip equivalents on windows to make this cross platform

sub Register
{
    Main::RegisterTarget("BuiltinShaders", { macbuildfunction => \&BuiltinShaders });
}

sub BuiltinShaders
{
    my ($root, $targetPath, $options) = @_;

    ProgressMessage("Gathering shader files\n");
    my $shaderFolder = "$root/build/temp/builtin_shaders";
    rmtree($shaderFolder);
    mkpath($shaderFolder);

    # copy
    system("rsync", "-av", "$root/Runtime/Resources/Assets/DefaultResources/", "$shaderFolder/DefaultResources");
    system("rsync", "-av", "$root/Runtime/Resources/Assets/DefaultResourcesExtra/", "$shaderFolder/DefaultResourcesExtra");
    system("rsync", "-av", "$root/Shaders/Includes/", "$shaderFolder/CGIncludes");
    system("rsync", "-av", "$root/Shaders/license.txt", "$shaderFolder/license.txt");
    system("rsync", "-av", "$root/Editor/Mono/Inspector/StandardShaderGUI.cs", "$shaderFolder/Editor/");
    system("rsync", "-av", "$root/Editor/Mono/Inspector/StandardRoughnessShaderGUI.cs", "$shaderFolder/Editor/");
    system("rsync", "-av", "$root/Editor/Mono/Inspector/StandardParticlesShaderGUI.cs", "$shaderFolder/Editor/");
    rmtree("$shaderFolder/DefaultResources/GameSkin");
    rmtree("$shaderFolder/DefaultResources/Materials");

    # delete all files that are not relevant (*.shader, *.cginc, *.cs)
    ProgressMessage("Archiving shader files\n");
    system(
        "find '$shaderFolder' \! -name \"*.shader\" \! -name \"*.compute\" \! -name \"*.hlsl\" \! -name \"*.cginc\" \! -name \"*.glslinc\" ! -name \"*.cs\" ! -name \"*.txt\" -type f -delete"
    );

    # add license blurb to all files
    find sub
    {
        return if -d;
        return unless -e;
        my $name = $File::Find::name;
        my ($ext) = $name =~ /(\.[^.]+)$/;
        return if $ext eq ".txt";    # don't add license to txt files (e.g. license itself)

        my $text = read_file($name);
        $text = "// Unity built-in shader source. Copyright (c) 2016 Unity Technologies. MIT license (see license.txt)\n\n" . $text;
        write_file($name, $text);
    }, $shaderFolder;

    # create zip file
    local $CWD = "$shaderFolder";
    system("zip", '-9', '-T', '-r', "$targetPath/builtin_shaders.zip", ".") && croak("Could not archive builtin shaders.\n");
}

1;

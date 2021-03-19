package TestHelpers::CSProjGen;

use warnings;
use strict;

use Exporter 'import';

our @EXPORT_OK = qw [getText];

sub getText
{
    my ($assemblyName) = @_;
    my $text = qq (<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
    <PropertyGroup>
        <Configuration Condition=" '$(Configuration)' == '' ">Debug</Configuration>
        <Platform Condition=" '$(Platform)' == '' ">AnyCPU</Platform>
        <ProjectGuid>{00591A5F-21D5-4B96-A93E-A29FF452A8FD}</ProjectGuid>
        <OutputType>Library</OutputType>
        <AppDesignerFolder>Properties</AppDesignerFolder>
        <RootNamespace>$assemblyName</RootNamespace>
        <AssemblyName>$assemblyName</AssemblyName>
        <TargetFrameworkVersion>v3.5</TargetFrameworkVersion>
        <FileAlignment>512</FileAlignment>
        <ProductVersion>12.0.0</ProductVersion>
        <SchemaVersion>2.0</SchemaVersion>
    </PropertyGroup>
    <PropertyGroup Condition=" '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' ">
        <DebugSymbols>True</DebugSymbols>
        <DebugType>full</DebugType>
        <Optimize>False</Optimize>
        <OutputPath>bin/Debug</OutputPath>
        <DefineConstants>TRACE;DEBUG;UNITY_ASSERTIONS</DefineConstants>
        <ErrorReport>prompt</ErrorReport>
        <WarningLevel>4</WarningLevel>
    </PropertyGroup>
</Project>);
    return $text;
}

1;

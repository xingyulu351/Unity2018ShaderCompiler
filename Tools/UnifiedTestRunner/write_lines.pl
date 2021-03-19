sub main
{
    my $linesCount = $ARGV[0];
    my $lineLength = $ARGV[1];
    my $line = "A" x $lineLength;
    for (my $i = 0; $i < $linesCount; $i++)
    {
        print("$line\n");
    }
}

main();

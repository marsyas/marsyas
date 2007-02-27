#!/usr/bin/perl
use note;

my $PITCH_ERROR_FILE=$ARGV[0];
my $INPUT_FILE="exercises/scale.ly";
my $OUTPUT_FILE="/Users/gperciva/tmp/out.ly";

my $note;
init();
my $in_file;
my $out_file;
my @lily_file;
my @error_file;


sub init{
	open ($in_file, $INPUT_FILE) or die "can't open input";
	@lily_file = <$in_file>;
	close $in_file;
	open ($in_file, $PITCH_ERROR_FILE);
	@error_file = <$in_file>;
	close $in_file;
	$note=8;
}

sub add_note {
	$note_add = note::pitch($_[1]);

	if ($_[2]) {
		$note_color = "\\colorNote #\"Dark Red\"";
		$lily_file[ $_[0] ] =~ s/^/<< \\voiceTwo /;
		$lily_file[ $_[0] ] =~ s/$/ \\\\ \\voiceOne $note_color $note_add >>/;
	} else {
		$note_color = "\\colorNote #\"Dark Blue\"";
		$lily_file[ $_[0] ] =~ s/^/<< \\voiceOne /;
		$lily_file[ $_[0] ] =~ s/$/ \\\\ \\voiceTwo $note_color $note_add >>/;
	}
}

sub process_intonation {
	my $color="black";
	my $intonation = $_[1];
#	print "$intonation\n";

	if ($intonation < -0.3) { $color = "Medium Blue" };
	if ($intonation < -0.2) { $color = "Dodger Blue" };
	if ($intonation < -0.1) { $color = "Light Sky Blue" };

	if ($intonation > 0.1) { $color = "Light Salmon" };
	if ($intonation > 0.2) { $color = "tomato" };
	if ($intonation > 0.3) { $color = "red" };

	$lily_file[ $_[0] ] =~ s/^/\\colorNote \#"$color" /;

	if ($intonation > 0.1) {
	  $lily_file[ $_[0] ] =~ s/$/^\\markup\{ \\arrow-head #Y #LEFT ##f \}/;
	}
	if ($intonation < -0.1) {
	  $lily_file[ $_[0] ] =~ s/$/_\\markup\{ \\arrow-head #Y #RIGHT ##f \}/;
	}
}

foreach (@error_file) {
	process_intonation($note,$_);
	$note++;
}

open ($out_file, "> $OUTPUT_FILE");
foreach (@lily_file) {
	print $out_file $_;
}
close $out_file;
system("cd /Users/gperciva/tmp; rm out.preview.*; nice lilypond -dpreview out.ly");


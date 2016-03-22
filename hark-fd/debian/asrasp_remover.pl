#!/usr/bin/perl
use strict;
use warnings;
use Carp;

my $target = "./src/AudioStreamFromMic.cc";
	
# ファイルの読み書きチェック
if (!(-r $target && -w $target)) {
	croak "Can't read or write file:$target.\n";
}

open(FIN, $target);
my @lines;
my $find=0; # flowdesignerのヘッダかどうか？

while(<FIN>) {
	if ($_ =~ /[^\\]*\/\*\s*Node/) {
		# print "START";
		$find = 1;
	} elsif ($_ =~ /END\s*\*\/[^\\]*/) {
		# print "END";
		$find = 0;
	} elsif ($find == 1 and $_ =~ /\@parameter_list/ ) { # parameter_listの行か？
		my $before = $_;
		my $after;
		chomp($before);

   		if ($_ =~ s/:ASRASP// or $_ =~ s/ASRASP://) { # ASRASP削除
			$after = $_;
			chomp($after);
			print "$target: Replacing [$before] with [$after]\n";
		}
	}
	push(@lines, $_);
}
close(FIN);

# 置換されたファイルの書き込み
open(FOUT, ">$target");
foreach my $line (@lines) {
	print FOUT $line;
}
close(FOUT);

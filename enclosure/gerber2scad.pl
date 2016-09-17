#!/usr/bin/perl

#
# This perl script converts the drill file (*.drl) and the PCB edges (*Edge_Cuts.gbr)
# from the gerbers into a OpenSCAD script which creates a 3D rendering of the PCB.
#
# Then I build a 3d model of the case around it
#

use strict;

print "//\$fn = 80;\n";
print "pcb_thick = 1.8;\n\n";

# AA
my $drl_fname  = "../circuit/v4_AA/gerber/wht_v4.drl";
my $edge_fname = "../circuit/v4_AA/gerber/wht_v4-Edge.Cuts.gm1";
my $xoff = 35078000;
my $yoff = -104638500;

# AAA
#my $drl_fname  = "../circuit/v3/gerber/wht_v3.drl";
#my $edge_fname = "../circuit/v3/gerber/wht_v3-Edge.Cuts.gm1";
#my $xoff = 35078000;
#my $yoff = -100488500;

# CR123A
#my $drl_fname  = "../circuit/v2/gerber/wht_v2.drl";
#my $edge_fname = "../circuit/v2/gerber/wht_v2-Edge.Cuts.gm1";
#my $xoff = 51158000;		# the lower left corner of PCB
#my $yoff = -93178000;

my $x;
my $y;

my @drills;
my $drill_id;
my $drill_r;

my $minx;
my $miny;
my $maxx;
my $maxy;
my $rawminx = 10000000000;
my $rawminy = 10000000000;

my $minx = 10000000000;
my $miny = 10000000000;
my $maxx = -10000000000;
my $maxy = -10000000000;

# read the drill tools
open(ACFILE, "<", $drl_fname);
while (<ACFILE>)
{
	chomp;

	# if end of drill definitions
	last if /\%/;

	# if drill definition
	if (/([T\d]+)C([\d\.]+)/)
	{
		$drill_id = $1;
		$drill_r = $2;
		push(@drills, $drill_id, $drill_r * 25.4 / 2);
	}
}

# advance to the drill positions
while (<ACFILE>)
{
	chomp;

	last if /T1/;
}

# read the drills positions
my $drill_ndx = 0;
$drill_r = @drills[$drill_ndx + 1];

print "module drills()\n";
print "{\n";

while (<ACFILE>)
{
	chomp;

	if (/X(\-?[\d\.]+)Y(\-?[\d\.]+)/)
	{
		# only do the large holes
		if ($drill_r < 1)
		{
			print "//";
		}
		
		printf("    translate([%.5f, %.5f, -0.5]) cylinder(r = %.5f, h = pcb_thick+1);\n",
						$1 * 25.4 - $xoff/1000000, $2 * 25.4 - $yoff/1000000, $drill_r);
						
	} elsif ($_ = @drills[$drill_ndx]) {
		# change the current drill
		$drill_ndx = $drill_ndx + 2;
		$drill_r = @drills[$drill_ndx + 1];
	}
}

print "}\n\n";

close(ACFILE);

# read the edges
open(ACFILE, "<", $edge_fname);
my @lines;
while (<ACFILE>)
{
	chomp;

	if (/^X(\-?[\d\.]+)Y(\-?[\d\.]+)D[\d][\d]\*/)		#D([\d]+)\*$
	{
		$rawminx = ($1 < $rawminx) ? $1 : $rawminx;
		$rawminy = ($2 < $rawminy) ? $2 : $rawminy;
		
		$x = $1 - $xoff;
		$y = $2 - $yoff;

		$minx = ($x < $minx) ? $x : $minx;
		$miny = ($y < $miny) ? $y : $miny;
		$maxx = ($x > $maxx) ? $x : $maxx;
		$maxy = ($y > $maxy) ? $y : $maxy;
		
		push(@lines, $x);
		push(@lines, $y);
	}
}

printf("// rawminx = %d\n", $rawminx + 1500000);
printf("// rawminy = %d\n", $rawminy + 1500000);
print "\n";

$minx /= 1000000;
$miny /= 1000000;
$maxx /= 1000000;
$maxy /= 1000000;

# now find the lines of the edges
my $nlines;
$nlines = scalar(@lines) / 4;

my $ndx;
my $found;

$ndx = 0;

# find the edge of the PCB
while ($ndx < scalar(@lines))
{
	if ($lines[$ndx] == 0  &&  $lines[$ndx+1] == 0)
	{
		$found = 1;
		last;
	}
	
	$ndx += 4;
}

print "module pcb_edge(pcb_zoffset = 0, pw = 0, plen = 1.5)\n";
print "{\n";

# the starting point
print "    translate([0, 0, pcb_zoffset])\n";
print "        linear_extrude(height = pcb_thick)\n";
print "            polygon(points=[\n";
print "                [", $lines[$ndx] / 1000000, ", ", $lines[$ndx+1] / 1000000, "],\n";

while ($nlines > 1)
{
	$found = 0;

	# iterate through the lines
	for (my $ndxin = 0; $ndxin < scalar(@lines); $ndxin += 4)
	{
		if ($ndxin != $ndx)
		{
			if ($lines[$ndx] == $lines[$ndxin]  &&  $lines[$ndx+1] == $lines[$ndxin+1])
			{
				# swap
				($lines[$ndxin+0], $lines[$ndxin+2]) = ($lines[$ndxin+2], $lines[$ndxin+0]);
				($lines[$ndxin+1], $lines[$ndxin+3]) = ($lines[$ndxin+3], $lines[$ndxin+1]);
				
				$found = 1;
			}
			
			if ($lines[$ndx] == $lines[$ndxin+2]  &&  $lines[$ndx+1] == $lines[$ndxin+3])
			{
				$found = 1;
			}

			if ($found == 1)
			{
				print "                [", $lines[$ndxin] / 1000000, ", ", $lines[$ndxin+1] / 1000000, "],\n";

				# kill the line
				$lines[$ndx+0] = 'a';
				$lines[$ndx+1] = 'a';
				$lines[$ndx+2] = 'a';
				$lines[$ndx+3] = 'a';
				
				$ndx = $ndxin;

				last;
			}
		}
	}
	
	--$nlines;
}

print "            ]);\n";
print "}\n";
print "\n";
print "module pcb(do_drills = 1, pcb_zoffset = 0, peg_wiggle = 0, peg_length = 1.5)\n";
print "{\n";
print "    difference()\n";
print "    {\n";
print "        pcb_edge(pcb_zoffset, pw = peg_wiggle, plen = peg_length);\n";
print "        if (do_drills == 1)\n";
print "            drills();\n";
print "    };\n";
print "}\n";
print "\n";
print "pcbx = ", $maxx - $minx - 3, ";\n";
print "pcby = ", $maxy - $miny - 3, ";\n";
print "\n";
print "//pcb();\n";

close(ACFILE);

$fn = 80;

include <pcb_cr123a.scad>

wall_thick_sides = 2.75;
wall_thick_topnbot = 2;
pcb_wiggle = 0.3;
down_room = 1.8;
up_room = 6;
batt_holder_width = 19;

switch_x1 = 17;
switch_x2 = 27;
switch_z = 2;
switch_wiggle = 0.5;

originx = 51.158;		// lower left corner of PCB (on the KiCad drawing)
originy = 93.178;

module mounting_holes()
{
	translate([wall_thick_sides + pcb_wiggle, wall_thick_sides + pcb_wiggle, 0])
	{
		// under battery
		translate([61.15 - originx, originy - 83.686, -0.1]) cylinder(r = 1.67, h = 25);
		translate([85.15 - originx, originy - 83.686, -0.1]) cylinder(r = 1.67, h = 25);
		
		// screw
		translate([80.946 - originx - 0.8, originy - 69.37, -0.1]) cylinder(r = 1.67, h = 25);
	}
}

module hollow()
{
	translate([wall_thick_sides, wall_thick_sides, wall_thick_topnbot])
		cube([pcbx + pcb_wiggle*2, pcby + pcb_wiggle*2, up_room + down_room + pcb_thick]);
}

module cpegs(wiggle = 0)
{
	cpeg_width = 10;

	cpegy = 25;

	cpeg_height = wall_thick_topnbot + pcb_thick/2 + down_room;

	translate([0, cpegy - wiggle/2, 0])
		cube([wall_thick_sides, cpeg_width + wiggle, cpeg_height]);

	translate([wall_thick_sides + pcbx + pcb_wiggle*2, cpegy - wiggle/2, 0])
		cube([wall_thick_sides, cpeg_width + wiggle, cpeg_height]);

	cpegx = 28.5;

	cpeg_width1 = 5.5;
	
	translate([wall_thick_sides + pcb_wiggle + cpegx - wiggle/2, wall_thick_sides + pcby + pcb_wiggle*2, 0])
		cube([cpeg_width1 + wiggle, wall_thick_sides, cpeg_height]);
}

module bottom()
{
	difference()
	{
		union()
		{
			// lower floor
			cube([pcbx + pcb_wiggle*2 + wall_thick_sides*2, pcby + pcb_wiggle*2 + wall_thick_sides*2, pcb_thick/2 + down_room + wall_thick_topnbot]);

			// horizontal battery wall
			translate([0, 0, pcb_thick/2 + down_room + wall_thick_topnbot])
				cube([pcbx + pcb_wiggle*2 + wall_thick_sides*2, wall_thick_sides, 3]);

			// the switch
			translate([wall_thick_sides + pcb_wiggle + switch_x1, wall_thick_sides + pcby + pcb_wiggle*2, pcb_thick/2 + down_room + wall_thick_topnbot])
				cube([switch_x2 - switch_x1, wall_thick_sides, pcb_thick/2]);
		}

		cpegs(0.8);

		hollow();

		translate([wall_thick_sides + pcb_wiggle, wall_thick_sides + pcb_wiggle, wall_thick_topnbot])
			pcb(do_drills = 0, pcb_zoffset = down_room, peg_wiggle = pcb_wiggle, peg_length = wall_thick_sides + 0.5);

		mounting_holes();
	}
}

module top()
{
	ledx = 71.66;
	ledy = 67.97;

	buttonx = 73.65;
	buttony = 59.686;

	inner_wall_thick = 1.2;

	difference()
	{
		// the enture top part
		translate([0, wall_thick_sides + pcb_wiggle + batt_holder_width, wall_thick_topnbot + pcb_thick/2 + down_room])
			cube([wall_thick_sides*2 + pcbx + pcb_wiggle*2, pcby - batt_holder_width + wall_thick_sides + pcb_wiggle, up_room + pcb_thick/2 + wall_thick_topnbot]);

		// the PCB pegs
		translate([wall_thick_sides + pcb_wiggle, wall_thick_sides + pcb_wiggle, wall_thick_topnbot])
			pcb(do_drills = 0, pcb_zoffset = down_room, peg_wiggle = pcb_wiggle, peg_length = wall_thick_sides + 0.5);

		// the switch
		translate([wall_thick_sides + pcb_wiggle + switch_x1 - switch_wiggle/2, wall_thick_sides + pcb_wiggle*2 + pcby, wall_thick_topnbot + down_room + pcb_thick/2])
			cube([switch_x2 - switch_x1 + switch_wiggle, wall_thick_sides + pcb_wiggle + 0.2, pcb_thick/2 + switch_z + 0.2]);

		hollow();

		mounting_holes();

		// the led hole
		translate([ledx - originx + wall_thick_sides + pcb_wiggle, originy - ledy + wall_thick_sides + pcb_wiggle, wall_thick_topnbot + down_room + pcb_thick + up_room - 0.05])
			cylinder(r = 2.6, h = wall_thick_topnbot + 0.1);

		// the recenter button
		translate([buttonx - originx + wall_thick_sides + pcb_wiggle, originy - buttony + wall_thick_sides + pcb_wiggle, 0])
			cylinder(r = 2, h = 20);
	}

	// the inner wall
	translate([wall_thick_sides, wall_thick_sides + pcb_wiggle + batt_holder_width, wall_thick_topnbot + pcb_thick + down_room])
		difference()
		{
			cube([pcbx + pcb_wiggle*2, inner_wall_thick, up_room]);
			
			translate([16, 0, 0])
				cube([12, inner_wall_thick, 2]);
		}

	cpegs();
}

bottom();

rotate([0, 180, 0])
	translate([-(wall_thick_sides*2 + pcbx + pcb_wiggle*2), -50, -(wall_thick_topnbot*2 + down_room + up_room + pcb_thick)])
	top();

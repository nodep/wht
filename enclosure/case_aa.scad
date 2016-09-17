$fn = 80;

include <pcb_aa.scad>

wall_thick_sides = 2.75;
wall_thick_topnbot = 2;
pcb_wiggle = 0.3;
down_room = 1.8;
up_room = 6;
batt_holder_width = 17.4;

switch_x1 = 17;
switch_x2 = 27;
switch_z = 2.1;
switch_wiggle = 0.5;

originx = 35.078;
originy = 104.6385;

module mounting_holes()
{
	// mounting holes
	translate([wall_thick_sides + pcb_wiggle, wall_thick_sides + pcb_wiggle, 0])
	{
		// battery holder holes
		translate([45.128 - originx, originy - 95.9885, -0.1]) cylinder(r = 1.62, h = 25);
		translate([86.028 - originx, originy - 95.9885, -0.1]) cylinder(r = 1.62, h = 25);
		
		// screw hole(s)
		
		// MPU-9150
		//translate([49.20568, 38.84374, -0.1]) cylinder(r = 1.67, h = 25);
		//translate([49.20568, 23.84504, -0.1]) cylinder(r = 1.67, h = 25);
		
		// MPU-6050 + HMC5883L
		translate([82.083 - originx - 0.8, originy - 82.5435, -0.1]) cylinder(r = 1.65, h = 25);
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

	cpegy = 24;

	cpeg_height = wall_thick_topnbot + pcb_thick/2 + down_room;

	translate([0, cpegy - wiggle/2, 0])
		cube([wall_thick_sides, cpeg_width + wiggle, cpeg_height]);

	translate([wall_thick_sides + pcbx + pcb_wiggle*2, cpegy - wiggle/2, 0])
		cube([wall_thick_sides, cpeg_width + wiggle, cpeg_height]);

	cpegx = 35;

	translate([wall_thick_sides + pcb_wiggle + cpegx - wiggle/2, wall_thick_sides + pcby + pcb_wiggle*2, 0])
		cube([cpeg_width + wiggle, wall_thick_sides, cpeg_height]);
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
				cube([switch_x2 - switch_x1, wall_thick_sides, pcb_thick/2 - 0.3]);
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
	// MPU-9150
	//ledx = 76.48;
	//ledy = 80.2545;

	// MPU-6050 or HMC5883L
	ledx = 75.464;
	ledy = 79.683;

	flashx = 62.078 - originx;
	flashy = originy - 82.1385;

	flash_sizex = 18.4;
	flash_sizey = 9.4;

	buttonx = 75;
	buttony = 70;

	inner_wall_thick = 1.5;

	difference()
	{
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
			cylinder(r = 2.75, h = wall_thick_topnbot + 0.1);

		// the flash connector hole
		translate([flashx - flash_sizex/2 + wall_thick_sides + pcb_wiggle, flashy - flash_sizey/2 + wall_thick_sides + pcb_wiggle - 1, wall_thick_topnbot + down_room + pcb_thick + up_room - 0.05])
			cube([flash_sizex, flash_sizey + 1, wall_thick_topnbot + 0.1]);

		// the recenter button
		translate([buttonx - originx + wall_thick_sides + pcb_wiggle, originy - buttony + wall_thick_sides + pcb_wiggle, 0])
			cylinder(r = 2, h = 20);
	}

	// the inner wall
	translate([wall_thick_sides, wall_thick_sides + pcb_wiggle + batt_holder_width, wall_thick_topnbot + pcb_thick + down_room])
		cube([flashx - flash_sizex/2 + pcb_wiggle, inner_wall_thick, up_room]);

	translate([wall_thick_sides + pcb_wiggle + flashx + flash_sizex/2, wall_thick_sides + pcb_wiggle + batt_holder_width, wall_thick_topnbot + pcb_thick + down_room])
		cube([pcbx - flashx - flash_sizex/2 + pcb_wiggle, inner_wall_thick, up_room]);

	cpegs();
}

bottom();

rotate([0, 180, 0])
	translate([-(wall_thick_sides*2 + pcbx + pcb_wiggle*2), -50, -(wall_thick_topnbot*2 + down_room + up_room + pcb_thick)])
	top();

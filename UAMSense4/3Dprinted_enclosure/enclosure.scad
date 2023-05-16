// UAMSense https://github.com/GNB-UAM/UAMSense
// (C) Carlos Garcia-Saura 2023

%translate([0,0,16]) linear_extrude(height=0.8) offset(0.25) import("uamsense-Edge.Cuts.dxf"); 

$fn=20;

screw_radius = 2.8/2;

module board_pillars(r=3.2/2, h=10) {
    translate([58.039, -24.232]) cylinder(r=r, h=h);
    translate([99.695, -24.257]) cylinder(r=r, h=h);
    translate([50.576, -70.272]) cylinder(r=r, h=h);
    translate([106.756, -65.227]) cylinder(r=r, h=h);
    
    translate([110-26, -74.5-4]) cylinder(r=r, h=h-0.5);
}

module fan_pillars(r=3.2/2, h=10) {
    translate(fan_center) {
        translate([57.5/2,0,0]) cylinder(r=r, h=h);
        translate([-57.5/2,0,0]) cylinder(r=r, h=h);
    }
}

module fan_pillars_(r=3.2/2, h=10) {
    translate(fan_center)
        for(i = [0:3])
            rotate([0,0,i*90])
                translate([45.5/2,0,0]) cylinder(r=r, h=h);
}

fan_center = [78.9,-53.4,0];





module box() {
    difference() {
        union() {
            hull() {
                translate([34.5,-19,45/2-1]) rotate([0,90,0]) cylinder(r=45/2, h=84, $fn=60);
                translate([34.5,-103-16,30/2-1]) rotate([0,90,0]) cylinder(r=30/2, h=84, $fn=60);
            }
            translate([34.5+84/2,-20,-1]) hull() {
                cylinder(r=20/2,h=5);
                translate([0,20,0]) cylinder(r=20/2,h=5);
            }
        }
        hull() {
            translate([35+1,-19,45/2-1.25+0.01]) rotate([0,90,0]) cylinder(r=45/2-1.25, h=83-2, $fn=60);
            translate([35+1,-103-16,30/2-1.25+0.01]) rotate([0,90,0]) cylinder(r=30/2-1.25, h=83-2, $fn=60);
        }
        translate([34.5+84/2,0,-1-0.1]) cylinder(r=8/2,h=5.2);
    }
}

module lid() {
    elongation = 0.5;
    difference() {
        hull() {
            translate([34.5+1.5,-19+elongation,45/2-1]) rotate([0,90,0]) cylinder(r=45/2, h=84-3, $fn=60);
            translate([34.5+1.5,-103-16-elongation,30/2-1]) rotate([0,90,0]) cylinder(r=30/2, h=84-3, $fn=60);
        }
        hull() {
            translate([34,-19+elongation,45/2-1.25+0.01]) rotate([0,90,0]) cylinder(r=45/2-1.25, h=85, $fn=60);
            translate([34,-103-16-elongation,30/2-1.25+0.01]) rotate([0,90,0]) cylinder(r=30/2-1.25, h=85, $fn=60);
        }
        translate([35.9,-109.5,24]) cube([22,10.5,30]); // Hole for light sensor (top)
        translate([35.9,-140,17]) cube([22,30,9.5]); // Hole for microphone
        translate([75,-65,-15]) rotate([2,0,0]) cube([150,200,40],center=true);
    }
    translate([55,-40,40.8]) rotate([8.5,0,0]) linear_extrude(height = 1)
        text("UAM", font = "Lato Medium", size = 14, direction = "ltr", spacing = 1 );
}

difference() {
    union() {
        box();
        board_pillars(r=6.5/2, h=16);
        fan_pillars(r=8/2, h=5);
        translate([51.5+25,-117.6,0]) cube([13,6,0.5]); // dust sensor air inlet
    }
    translate([0,0,6]) board_pillars(r=screw_radius, h=16);
    translate([0,0,-2]) fan_pillars(r=screw_radius, h=9);
    translate([51.5+26,-116.6,-2]) cube([11,4,4]); // dust sensor air inlet
    
    translate([57,-70,-2]) cylinder(r=2.5/2, h=6); // Hole for fan cable
    translate([0,0,-2]) translate(fan_center) cylinder(r=40/2, h=6, $fn=60); // Hole for fan
    translate([36,-140,17]) cube([22,30,9.5]); // Hole for microphone
    translate([25,-109.5,17]) cube([21.5,10.5,9.5]); // Hole for light sensor (bottom)
    difference() { // holes for cables
        translate([30,-72,19]) cube([15,46,4]);
        translate([29,-72+3*46/9,18]) cube([17,4*46/9,6]);
    }
    hull() { // hole for lid
            translate([35+1,-19,45-1-0.6]) rotate([0,90,0]) cylinder(r=25/2, h=83-2, $fn=60);
            translate([35+1,-103-16,30-1-0.6]) rotate([0,90,0]) cylinder(r=25/2, h=83-2, $fn=60);
    }
}

%lid();
// UAMSense https://github.com/GNB-UAM/UAMSense
// (C) Carlos Garcia-Saura 2023

// Increase the resolution of default shapes
$fa = 5; // Minimum angle for fragments [degrees]
$fs = 0.5; // Minimum fragment size [mm]




module translate_mainBoard(move = true) {
    if(move)
    translate([0,74,0])
    rotate([0,0,90]) rotate([90,0,0])
    translate([-114,94,0])
    children();
    else children();
}

module pir_hole() {
    // hole for the PIR sensor
    translate_mainBoard(true)
    translate([93+12,-67,3]) rotate([30,0,0]) cylinder(r1=30/2, r2=90/2, h=100);
}

module mainBoard(drills = false, move=true, incD=0, incH=0, drillH=30, centerDrills=false) {
    translate_mainBoard(move)
    if(!drills) {
        //color("black") translate([0,0,3]) import("sensors-B.Cu.dxf");
        
        // ESP12
        color("blue") translate([80,-45,-3]) cube([16,22,3]);
        
        // TGS sensors
        color("gray") translate([44.8,-32.5,2]) cylinder(r=9/2, h=20);
        color("gray") translate([44.8,-32.5-13.9,2]) cylinder(r=9/2, h=20);
        color("gray") translate([44.8,-32.5-13.9*2,2]) cylinder(r=9/2, h=20);
        color("gray") translate([44.8,-32.5-13.9*3,2]) cylinder(r=9/2, h=20);
        
        difference() {
            hull() linear_extrude(height=2) import("sensors-Edge.Cuts.dxf");
            mainBoard(drills = true, move=false, incD=incD, incH=incH);
        }
    } else {
        translate([44.2,-88.3,3+incH]) rotate([180,0,0]) cylinder(h=drillH, r=incD+3.1/2, center=centerDrills);
        translate([61,-53,3+incH]) rotate([180,0,0]) cylinder(h=drillH, r=incD+3.1/2, center=centerDrills);
        translate([93,-67,3+incH]) rotate([180,0,0]) cylinder(h=drillH, r=incD+3.1/2, center=centerDrills);
    }
}

%mainBoard();

difference() {
    union() {
        difference() {
            translate([18,42,30]) bcube([52,160,80],cr=8,cres=30);
            translate([18,42,30]) bcube([52-3,160-3,81],cr=8,cres=30);
        }
        // Drill supports
        mainBoard(drills=true, incD=1.5, incH=-3.5, drillH=6);
        // air quality sensor
        translate([0,-5,34]) rotate([0,-90,0]) cylinder(h=7, r=2+2.5/2);
        // co2
        translate([0,95,14]) {
            rotate([0,-90,0]) cylinder(h=7, r=2+3.5/2);
            translate([0,0,39])
            rotate([0,-90,0]) cylinder(h=7, r=2+3.5/2);
        }
    }
    
    mainBoard();
    mainBoard(drills=true, centerDrills=true, drillH=150);
    pir_hole();

    // Hole for the dust sensor
    %translate([-5,-34]) cube([46.5, 32, 20]);

    // Drill for the air quality sensor
    translate([0,-5,34]) rotate([0,90,0]) cylinder(h=150, r=2/2, center=true);

    // Drills for the CO2 sensor
    translate([0,95,14]) {
        rotate([0,90,0]) cylinder(h=150, r=3/2, center=true);
        translate([0,0,39])
        rotate([0,90,0]) cylinder(h=150, r=3/2, center=true);
    }
}
    


// bcube module by Obijuan http://www.iearobotics.com/wiki/index.php?title=Objeto_3D:Cubo_Biselado
module bcube(size,cr=0,cres=0)
{
  //-- Internal cube size
  bsize = size - 2*[cr,cr,0];

  //-- Get the (x,y) coorner coordinates in the 1st cuadrant
  x = bsize[0]/2;
  y = bsize[1]/2;

  //-- A corner radius of 0 means a standar cube!
  if (cr==0)
    cube(bsize,center=true);
  else {

      
      //-- The height of minkowski object is double. So
      //-- we sould scale by 0.5
      scale([1,1,0.5])

      //-- This translation is for centering the minkowski objet
      translate([-x, -y,0])

      //-- Built the  beveled cube with minkowski
      minkowski() {

        //-- Internal cube
        cube(bsize,center=true);

        //-- Cylinder in the corner (1st cuadrant)
        translate([x,y, 0])
          cylinder(r=cr, h=bsize[2],center=true, $fn=4*(cres+1));
      }
  }

}


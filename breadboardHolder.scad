//holder for DC adapter
module dcHolder(){
    difference(){
        difference(){
            cube([15, 25, 13], center=true);
            translate([0,0-2])
            cube([14, 24, 12], center=true);
        };
        translate([0,5,0])
        rotate([90,0,0])
        cylinder(22, r=5, center=true);
    }
}

//breadboard holder
difference(){
    union(){
      cube([82, 45, 2.5], center=true);
        difference(){
            difference(){
                
                rotate([90,0,0])
                cylinder(20,41,41, center=true);
                translate([0,0,22.5])
                cube([82, 45, 43], center=true);
            };
            rotate([90,0,0])
            translate([0,-40,0])
            cylinder(25, 78/2, 78/2, center=true);
        }


        difference(){
        rotate([90,0,0])
        translate([0,-40,0])
        cylinder(20, 80/2, 80/2, center=true);
        rotate([90,0,0])
        translate([0,-40,0])
        cylinder(25, 78/2, 78/2, center=true);
        }   
      };
    translate([0,0,-75])
    cube([40,40,40], center=true);
}

translate([25,0,-76])
rotate([0,45,0])
cube([10,10,2], center=true);

translate([-25,0,-76])
rotate([0,-45,0])
cube([10,10,2], center=true);




















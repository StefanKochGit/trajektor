#include <SpaceSystem.h>
#include<stdio.h>
#include<math.h>
#include<Eigen/Dense>

void check(const char *text, double is, double must, double tolerance, int *err, int *pass)
{
	printf("%-20s : ", text);
	if (fabs(is - must) <= tolerance) {
		printf("OK");
		(*pass)++;
	} else {
		printf("FAIL (is=%E, must=%E, tolerance=%E)", is, must, tolerance);
		(*err)++;
	}
	printf("\n");
}

void t_eigen(int *err, int *pass)
{
	//-----------------------------------------------------------------
	printf("****** Test Eigen ********\n");
	Vector3d v;
	v << 1.1, 2.2, 3.3;
	printf("x = %e  y = %e  z = %e\n", v(0), v(1), v(2));
	check("Eigen x", v(0), 1.1, 0.0, err, pass);
	check("Eigen y", v(1), 2.2, 0.0, err, pass);
	check("Eigen z", v(2), 3.3, 0.0, err, pass);
}

void t_position_move(int *err, int *pass)
{
	//-----------------------------------------------------------------
	printf("****** Test Position and Move *******\n");
	Vector3d v;
	v << 1.1, 2.2, 3.3;
	printf("x = %e  y = %e  z = %e\n", v(0), v(1), v(2));
	Position p(v);
	p.report();
	printf("\n");
	check("New Eigen x", (p.get_vect())(0), 1.1, 0.0, err, pass);
	check("New Eigen y", (p.get_vect())(1), 2.2, 0.0, err, pass);
	check("New Eigen z", (p.get_vect())(2), 3.3, 0.0, err, pass);

	Vector3d base;
	Vector3d other;
	base  << 0, 0, 0;
	other << 2, 1, 0;
	Position p0(base);
	Position p1(other);
	check("distance_to", p1.distance_to(&p0), sqrt(5), 0.001, err, pass);

	Vector3d move_vect;
	move_vect << 1, 1, 0;
	Movement move(move_vect);
	p0.advance(&move, 1);
	//check
	check("move1 pos x", (p0.get_vect())(0), 1.0, 0.0, err, pass);
	check("move1 pos y", (p0.get_vect())(1), 1.0, 0.0, err, pass);
	check("move1 pos z", (p0.get_vect())(2), 0.0, 0.0, err, pass);
	Vector3d new_move;
	new_move << -1, -1, 0;
	move.add_moveVector(new_move); //stop
	move.add_moveVector(new_move); //backward
	p0.advance(&move, 1);
	//check
	check("move2 pos x", (p0.get_vect())(0), 0.0, 0.0, err, pass);
	check("move2 pos y", (p0.get_vect())(1), 0.0, 0.0, err, pass);
	check("move2 pos z", (p0.get_vect())(2), 0.0, 0.0, err, pass);
	p0.advance(&move, 1);
	//check
	check("move3 pos x", (p0.get_vect())(0), -1.0, 0.0, err, pass);
	check("move3 pos y", (p0.get_vect())(1), -1.0, 0.0, err, pass);
	check("move3 pos z", (p0.get_vect())(2), 0.0, 0.0, err, pass);
}

void t_force(int *err, int *pass)
{
	//-----------------------------------------------------------------
	printf("****** Test Force *******\n");
	Vector3d v, addf;
	v << 1.0, 2.0, 0.0;
	Force f;
	check("force_init x", (f.get_vect())(0), 0.0, 0.0, err, pass);
	check("force_init y", (f.get_vect())(1), 0.0, 0.0, err, pass);
	check("force_init z", (f.get_vect())(2), 0.0, 0.0, err, pass);
	f.add_forceVector(v);
	check("force0     x", (f.get_vect())(0), 1.0, 0.0, err, pass);
	check("force0     y", (f.get_vect())(1), 2.0, 0.0, err, pass);
	check("force0     z", (f.get_vect())(2), 0.0, 0.0, err, pass);
	addf << 0.0, -1.0, 0;
	f.add_forceVector(addf);
	check("force1     x", (f.get_vect())(0), 1.0, 0.0, err, pass);
	check("force1     y", (f.get_vect())(1), 1.0, 0.0, err, pass);
	check("force1     z", (f.get_vect())(2), 0.0, 0.0, err, pass);
	printf("gravity static not checked\n");
}

void t_spaceobject(int *err, int *pass)
{
	//-----------------------------------------------------------------
	printf("****** Test SpaceObject *******\n");
	//                                 name                     pos       move           m        r
	SpaceObject *so0 = new SpaceObject("test-earth",        1, 2, 3,   0, 0, 0,   5.972e24, 6.371e6); //earth
	check("so1 pos check x", so0->get_pos(0), 1.0, 0, err, pass);
	check("so1 pos check y", so0->get_pos(1), 2.0, 0, err, pass);
	check("so1 pos check z", so0->get_pos(2), 3.0, 0, err, pass);

	printf("**--drop test y --**\n");
	//                                     name                    pos           move          m        r
	SpaceObject *earth1 = new SpaceObject("earth",              0, 0, 0,      0, 0, 0,   5.972e24, 6.371e6); //earth
	SpaceObject *stone1 = new SpaceObject("testbody_1",   0, 6.371e6, 0,      0, 0, 0,          1,       1); //1kg, 1m, sealevel
	//earth1->report();
	stone1->report();
	for (int i=0; i < 100; i++) {
		//printf("%3.1fs stone delta=%5.2f\n", 0.01*(float)i, stone1->get_pos(1) - 6.371e6);
		stone1->clearForce();
		stone1->add_forceInteraction(earth1);
		stone1->advance(0.01);
	}
	stone1->report();
	check("drop path after 1s", stone1->get_pos(1) - 6.371e6, -4.9, 0.1, err, pass);

	printf("**--drop test y --**\n");
	//                                     name                    pos           move          m        r
	SpaceObject *earth2 = new SpaceObject("earth",              0, 0, 0,      0, 0, 0,   5.972e24, 6.371e6); //earth
	SpaceObject *stone2 = new SpaceObject("testbody_1",   6.371e6, 0, 0,      0, 0, 0,          1,       1); //1kg, 1m, sealevel
	//earth2->report();
	stone2->report();
	for (int i=0; i < 100; i++) {
		//printf("%3.1fs stone delta=%5.2f\n", 0.01*(float)i, stone2->get_pos(0) - 6.371e6);
		stone2->clearForce();
		stone2->add_forceInteraction(earth2);
		stone2->advance(0.01);
	}
	stone2->report();
	check("drop path after 1s", stone2->get_pos(0) - 6.371e6, -4.9, 0.1, err, pass);
}
	
	
int main()
{
	int err  = 0;
	int pass = 0;
	
	t_eigen(&err, &pass);
	t_position_move(&err, &pass);
	t_force(&err, &pass);
	t_spaceobject(&err, &pass);

	printf("\n-----------------------------\n");
	printf("PASS = %d\n", pass);
	printf("FAIL = %d\n", err);
}

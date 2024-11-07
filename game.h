#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

class game
{

public:

	game(int difficulte, Mat flux);
	~game();

	int get_score() { return d_score; }
	void add_score(int i) { d_score = d_score + i; }

	void afficherScore();
	void afficherCercle();
	void supprimerCercle(int index);

	bool verifCercle(int cercleIndex);
		
	void jouer();
	//bool redIsInCircle();

	int getDiff() { return d_difficulte; };
	

private:

	int d_difficulte;
	int d_score;
	Mat d_flux;
	Mat d_fluxModif;
	Mat d_red;

	int d_width;
	int d_height;

	Mat redMask;

	VideoCapture d_cap;
	int d_proba_generation;

	int d_nb_cercle_autorized;

	std::vector<Point*> d_cercle;
	std::vector<int> d_radius;

};


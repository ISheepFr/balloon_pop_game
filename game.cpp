#include "game.h"

game::game(int difficulte, Mat flux) :d_difficulte{ difficulte }, d_flux{ flux }, d_score{0}
{
	switch (difficulte)
	{

	case 1:
		d_nb_cercle_autorized = 5;
		d_proba_generation = 10;
		break;
	case 2:
		d_nb_cercle_autorized = 10;
		d_proba_generation = 30;
		break;
	case 3:
		d_nb_cercle_autorized = 30;
		d_proba_generation = 60;
		break;
	case 4:
		d_nb_cercle_autorized = 0;
		d_proba_generation = 100;
		break;

	default:
		break;
	}

	if (d_cap.open(0))
	{
		// Obtenir la taille du cadre dans le flux vidéo
		d_width = d_cap.get(CAP_PROP_FRAME_WIDTH);
		d_height = d_cap.get(CAP_PROP_FRAME_HEIGHT);
		
	}

	d_fluxModif = d_flux.clone();
}

game::~game()
{
	for (Point* p : d_cercle)
	{
		delete p;
	}
	d_cercle.clear();
}

void game::afficherScore()
{
	Point textPosition(d_width - 160, 50);
	std::string scoreStr = "Score: " + std::to_string(d_score);
	Size textSize = getTextSize(scoreStr, FONT_HERSHEY_SIMPLEX, 1, 2, nullptr);

	Point rectTopLeft(textPosition.x - 10, textPosition.y - textSize.height - 10);
	Point rectBottomRight(textPosition.x + textSize.width + 10, textPosition.y + 10);

	rectangle(d_fluxModif, rectTopLeft, rectBottomRight, Scalar(0, 255, 0), -1); // -1 remplit le rectangle

	putText(d_fluxModif, scoreStr, textPosition, FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, LINE_AA);
}

void game::supprimerCercle(int index)
{
	// Libérer la mémoire allouée pour le point
	delete d_cercle[index];

	// Supprimer le pointeur du vecteur
	d_cercle.erase(d_cercle.begin() + index);

	// Supprimer le rayon correspondant
	d_radius.erase(d_radius.begin() + index);
}

void game::afficherCercle()
{
	bool state = (d_nb_cercle_autorized != 0);
	int r = rand() % 101;
	std::cout << "proba generer : " << r << std::endl;
	if (r <= d_proba_generation)
	{
		if (d_nb_cercle_autorized != 0)
		{
			if (d_cercle.size() < d_nb_cercle_autorized)
			{
				int radius = rand() % 50 + 1;
				Point* center = new Point;
				center->x = rand() % (d_width - 2 * radius) + radius;
				center->y = rand() % (d_height - 2 * radius) + radius;

				d_cercle.push_back(center);
				std::cout <<"PUSHBACKED CERCle" << std::endl;
				d_radius.push_back(radius);
			}

		}
		else {
			int radius = rand() % 50 + 1;
			Point* center = new Point;
			center->x = rand() % (d_width - 2 * radius) + radius;
			center->y = rand() % (d_height - 2 * radius) + radius;

			d_cercle.push_back(center);
			std::cout << "PUSHBACKED CERCle" << std::endl;
			d_radius.push_back(radius);
			
		}
		

	}

	for (int i = 0; i < d_cercle.size(); i++)
	{
		if (verifCercle(i))
		{
			//add_score(1);
			if (d_radius[i] <= 10)
			{
				add_score(2);
			}
			else {
				add_score(1);
			}
			supprimerCercle(i);
			
		}
		circle(d_fluxModif, *d_cercle[i], d_radius[i], Scalar(0, 0, 255), FILLED);
		
	}
	
	
}

bool game::verifCercle(int cercleIndex)
{
	// Filtrer les pixels rouges dans l'image caméra
	Mat hsvImage;
	cvtColor(d_flux, hsvImage, cv::COLOR_BGR2HSV);

	Mat redMask;
	inRange(hsvImage, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), redMask);
	inRange(hsvImage, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), redMask);

	//image résultante
	bitwise_and(d_flux, d_flux, d_fluxModif, redMask);

	// Coordonnées du centre du cercle
	int centerX = d_cercle[cercleIndex]->x;
	int centerY = d_cercle[cercleIndex]->y;

	// Rayon du cercle
	int radius = d_radius[cercleIndex];

	// Vérifier si les pixels rouges sont à l'intérieur du cercle
	for (int y = 0; y < redMask.rows; y++)
	{
		for (int x = 0; x < redMask.cols; x++)
		{
			if (redMask.at<uchar>(y, x) > 0) // Pixel rouge détecté
			{
				// Calcul de la distance entre le pixel et le centre du cercle
				double distance = std::sqrt(std::pow(x - centerX, 2) + std::pow(y - centerY, 2));

				// Vérifie si le pixel est à l'intérieur du cercle
				if (distance <= radius)
				{
					
					d_fluxModif.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 0, 0);
					return true;
				}
			}
		}
	}

	return false;
}



void game::jouer()
{
	if (d_cap.open(0))
	{
		cv::namedWindow("Flux modifié", cv::WINDOW_AUTOSIZE);
	}

	while (true)
	{
		d_cap >> d_flux;

      
		cv::flip(d_flux, d_flux, 1);
		
		d_fluxModif = d_flux.clone();
		
		d_red = d_flux.clone();
		afficherScore();
		afficherCercle();


		if (d_cap.isOpened())
		{
		imshow("Flux modifié", d_fluxModif);
		}

		d_fluxModif = Mat();
		d_red = Mat();

		if (waitKey(30) == 27)
			break;
		
	}

	d_cap.release();
	destroyAllWindows();


}
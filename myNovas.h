#ifndef MYNOVAS_H
#define MYNOVAS_H


typedef struct {
    double ra;
    double dec;
    double promora;
    double promodec;
    double parallax;
    double radialvelocity;
} cat_entry;

typedef struct {
    double latitude;
    double longitude;
} site_info;

double julian_date(short int year, short int month, short int day, double hour);
void equ2hor(double tjd, site_info *location, double ra, double dec, double *zd, double *az);
short int app_star(double tjd, cat_entry *star, double *ra, double *dec);


#endif // MYNOVAS_H

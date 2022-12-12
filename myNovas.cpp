#include <math.h>
#include <myNovas.h>
using namespace std;

#define BARYC 0
#define HELIOC 1

const double T0 = 2451545.00000000;
const double KMAU = 1.49597870e+8;
const double MAU = 1.49597870e+11;
const double C = 173.14463348;
const double GS = 1.32712438e+20;
const double TWOPI = 6.28318530717958647692;
const double RAD2SEC = 206264.806247096355;
const double DEG2RAD = 0.017453292519943296;
const double RAD2DEG = 57.295779513082321;
const double epoch_hip = 2448349.0625;
const double epoch_fk5 = 2451545.0000;
const short int FN0 = 0;



short int vector2radec(double *pos, double *ra, double *dec) {
    double xyproj;

    xyproj = sqrt(pow(pos[0], 2.0) + pow(pos[1], 2.0));
    if ((xyproj == 0.0) && (pos[2] == 0)) {
        *ra = 0.0;
        *dec = 0.0;
        return 1;
    } else if (xyproj == 0.0) {
        *ra = 0.0;
        if (pos[2] < 0.0)
            *dec = -90.0;
        else
            *dec = 90.0;
        return 2;
    } else {
        *ra = atan2(pos[1], pos[0]) * RAD2SEC / 54000.0;
        *dec = atan2(pos[2], xyproj) * RAD2SEC / 3600.0;

        if (*ra < 0.0)
            *ra += 24.0;
    }
    return 0;
}

void fund_args(double t, double a[5]) {
    short int i;

    a[0] = 2.3555483935439407 + t * (8328.691422883896 + t * (1.517951635553957e-4 + 3.1028075591010306e-7 * t));
    a[1] = 6.240035939326023 + t * (628.3019560241842 + t * (-2.7973749400020225e-6 - 5.817764173314431e-8 * t));
    a[2] = 1.6279019339719611 + t * (8433.466158318453 + t * (-6.427174970469119e-5 + 5.332950492204896e-8 * t));
    a[3] = 5.198469513579922 + t * (7771.377146170642 + t * (-3.340851076525812e-5 + 9.211459941081184e-8 * t));
    a[4] = 2.1824386243609943 + t * (-33.75704593375351 + t * (3.614285992671591e-5 + 3.878509448876288e-8 * t));

    for (i = 0; i < 5; i++) {
        a[i] = fmod(a[i], TWOPI);
        if (a[i] < 0.0)
            a[i] += TWOPI;
    }

    return;
}

short int nutation_angles(double t, double *longnutation, double *obliqnutation) {
    double clng[106] = {1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0,
                        -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0,
                        -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0,
                        -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, -1.0,
                        1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 2.0,
                        2.0, 2.0, 2.0, 2.0, -2.0, 2.0, 2.0,
                        2.0, 3.0, -3.0, -3.0, 3.0, -3.0, 3.0,
                        -3.0, 3.0, 4.0, 4.0, -4.0, -4.0, 4.0,
                        -4.0, 5.0, 5.0, 5.0, -5.0, 6.0, 6.0,
                        6.0, -6.0, 6.0, -7.0, 7.0, 7.0, -7.0,
                        -8.0, 10.0, 11.0, 12.0, -13.0, -15.0, -16.0,
                        -16.0, 17.0, -21.0, -22.0, 26.0, 29.0, 29.0,
                        -31.0, -38.0, -46.0, 48.0, -51.0, 58.0, 59.0,
                        63.0, 63.0, -123.0, 129.0, -158.0, -217.0, -301.0,
                        -386.0, -517.0, 712.0, 1426.0, 2062.0, -2274.0,
                        -13187.0, -171996.0
                       },
                       clngx[14] = {0.1, -0.1, 0.1, 0.1, 0.1, 0.1, 0.2, -0.2, -0.4, 0.5, 1.2,
                                    -1.6, -3.4, -174.2
                                   },
                                   cobl[64] = {1.0, 1.0, 1.0, -1.0, -1.0, -1.0,
                                               1.0, 1.0, 1.0, 1.0, 1.0, -1.0,
                                               1.0, -1.0, 1.0, -1.0, -1.0, -1.0,
                                               1.0, -1.0, 1.0, 1.0, -1.0, -2.0,
                                               -2.0, -2.0, 3.0, 3.0, -3.0, 3.0,
                                               3.0, -3.0, 3.0, 3.0, -3.0, 3.0,
                                               3.0, 5.0, 6.0, 7.0, -7.0, 7.0,
                                               -8.0, 9.0, -10.0, -12.0, 13.0, 16.0,
                                               -24.0, 26.0, 27.0, 32.0, -33.0, -53.0,
                                               54.0, -70.0, -95.0, 129.0, 200.0, 224.0,
                                               -895.0, 977.0, 5736.0, 92025.0
                                              },
                                           coblx[8] = {-0.1, -0.1, 0.3, 0.5, -0.5, -0.6, -3.1, 8.9};

    short int i, ii, i1, i2, iop;
    short int nav1[10] = {0, 0, 1, 0, 2, 1, 3, 0, 4, 0},
                         nav2[10] = {0, 0, 0, 5, 1, 1, 3, 3, 4, 4},
                                    nav[183] = {2, 0, 1, 1, 5, 2, 2, 0, 2, 1, 0, 3, 2, 5, 8, 1, 17, 8,
                                                1, 18, 0, 2, 0, 8, 0, 1, 3, 2, 1, 8, 0, 17, 1, 1, 15, 1,
                                                2, 21, 1, 1, 2, 8, 2, 0, 29, 1, 21, 2, 2, 1, 29, 2, 0, 9,
                                                2, 5, 4, 2, 0, 4, 0, 1, 9, 2, 1, 4, 0, 2, 9, 2, 2, 4,
                                                1, 14, 44, 2, 0, 45, 2, 5, 44, 2, 50, 0, 1, 36, 2, 2, 5, 45,
                                                1, 37, 2, 2, 1, 45, 2, 1, 44, 2, 53, 1, 2, 8, 4, 1, 40, 3,
                                                2, 17, 4, 2, 0, 64, 1, 39, 8, 2, 27, 4, 1, 50, 18, 1, 21, 47,
                                                2, 44, 3, 2, 44, 8, 2, 45, 8, 1, 46, 8, 0, 67, 2, 1, 5, 74,
                                                1, 0, 74, 2, 50, 8, 1, 5, 78, 2, 17, 53, 2, 53, 8, 2, 0, 80,
                                                2, 0, 81, 0, 7, 79, 1, 7, 81, 2, 1, 81, 2, 24, 44, 1, 1, 79,
                                                2, 27, 44
                                               },
                                            llng[106] = {57, 25, 82, 34, 41, 66, 33, 36, 19, 88, 18, 104, 93,
                                                         84, 47, 28, 83, 86, 69, 75, 89, 30, 58, 73, 46, 77,
                                                         23, 32, 59, 72, 31, 16, 74, 22, 98, 38, 62, 96, 37,
                                                         35, 6, 76, 85, 51, 26, 10, 13, 63, 105, 52, 102, 67,
                                                         99, 15, 24, 14, 3, 100, 65, 11, 55, 68, 20, 87, 64,
                                                         95, 27, 60, 61, 80, 91, 94, 12, 43, 71, 42, 97, 70,
                                                         7, 49, 29, 2, 5, 92, 50, 78, 56, 17, 48, 40, 90,
                                                         8, 39, 54, 81, 21, 103, 53, 45, 101, 0, 1, 9, 44,
                                                         79, 4
                                                        },
                                                    llngx[14] = {81, 7, 97, 0, 39, 40, 9, 44, 45, 103, 101, 79, 1, 4},
                                                            lobl[64] = {51, 98, 17, 21, 5, 2, 63, 105, 38, 52, 102, 62, 96,
                                                                        37, 35, 76, 36, 88, 85, 104, 93, 84, 83, 67, 99, 8,
                                                                        68, 100, 60, 61, 91, 87, 64, 80, 95, 65, 55, 94, 43,
                                                                        97, 0, 71, 70, 42, 49, 92, 50, 78, 56, 90, 48, 40,
                                                                        39, 54, 1, 81, 103, 53, 45, 101, 9, 44, 79, 4
                                                                       },
                                                                    loblx[8] = {53, 1, 103, 9, 44, 101, 79, 4};

    double a[5], angle, cc, ss1, cs, sc, c[106], s[106], lng, lngx, obl,
           oblx;

    fund_args(t, a);

    i = 0;
    for (ii = 0; ii < 10; ii += 2) {
        angle = a[nav1[ii]] * (double)(nav1[1 + ii] + 1);
        c[i] = cos(angle);
        s[i] = sin(angle);
        i += 1;
    }

    i = 5;
    for (ii = 0; ii < 10; ii += 2) {
        i1 = nav2[ii];
        i2 = nav2[1 + ii];

        c[i] = c[i1] * c[i2] - s[i1] * s[i2];
        s[i] = s[i1] * c[i2] + c[i1] * s[i2];
        i += 1;
    }

    i = 10;
    for (ii = 0; ii < 183; ii += 3) {
        iop = nav[ii];
        i1 = nav[1 + ii];
        i2 = nav[2 + ii];
        switch (iop) {
        case 0:
            c[i] = c[i1] * c[i2] - s[i1] * s[i2];
            s[i] = s[i1] * c[i2] + c[i1] * s[i2];
            i += 1;
            break;
        case 1:
            c[i] = c[i1] * c[i2] + s[i1] * s[i2];
            s[i] = s[i1] * c[i2] - c[i1] * s[i2];
            i += 1;
            break;
        case 2:
            cc = c[i1] * c[i2];
            ss1 = s[i1] * s[i2];
            sc = s[i1] * c[i2];
            cs = c[i1] * s[i2];
            c[i] = cc - ss1;
            s[i] = sc + cs;
            i += 1;
            c[i] = cc + ss1;
            s[i] = sc - cs;
            i += 1;
            break;
        }
        if (iop == 3)
            break;
    }

    lng = 0.0;
    for (i = 0; i < 106; i++)
        lng += clng[i] * s[llng[i]];

    lngx = 0.0;
    for (i = 0; i < 14; i++)
        lngx += clngx[i] * s[llngx[i]];

    obl = 0.0;
    for (i = 0; i < 64; i++)
        obl += cobl[i] * c[lobl[i]];

    oblx = 0.0;
    for (i = 0; i < 8; i++)
        oblx += coblx[i] * c[loblx[i]];

    *longnutation = (lng + t * lngx) / 10000.0;
    *obliqnutation = (obl + t * oblx) / 10000.0;

    return 0;
}

void earthtilt(double tjd, double *mobl, double *tobl, double *eq, double *dpsi, double *deps) {
    static double tjd_last = 0.0;
    static double t, dp, de;
    double mean_obliq, true_obliq, eq_eq, args[5];

    t = (tjd - T0) / 36525.0;

    if (fabs(tjd - tjd_last) > 1.0e-6)
        nutation_angles(t, &dp, &de);

    mean_obliq = 84381.4480 - 46.8150 * t - 0.00059 * pow(t, 2.0) + 0.001813 * pow(t, 3.0);

    true_obliq = mean_obliq + de;

    mean_obliq /= 3600.0;
    true_obliq /= 3600.0;

    fund_args(t, args);

    eq_eq = dp * cos(mean_obliq * DEG2RAD) +
            (0.00264 * sin(args[4]) + 0.000063 * sin(2.0 * args[4]));

    eq_eq /= 15.0;

    *dpsi = dp;
    *deps = de;
    *eq = eq_eq;
    *mobl = mean_obliq;
    *tobl = true_obliq;

    return;
}

void starvectors(cat_entry *star, double *pos, double *vel) {
    double paralx, dist, r, d, cra, sra, cdc, sdc, pmr, pmd, rvl;

    paralx = star->parallax;

    if (star->parallax <= 0.0)
        paralx = 1.0e-7;

    dist = RAD2SEC / paralx;
    r = (star->ra) * 15.0 * DEG2RAD;
    d = (star->dec) * DEG2RAD;
    cra = cos(r);
    sra = sin(r);
    cdc = cos(d);
    sdc = sin(d);

    pos[0] = dist * cdc * cra;
    pos[1] = dist * cdc * sra;
    pos[2] = dist * sdc;

    pmr = star->promora * 15.0 * cdc / (paralx * 36525.0);
    pmd = star->promodec / (paralx * 36525.0);
    rvl = star->radialvelocity * 86400.0 / KMAU;
    vel[0] = -pmr * sra - pmd * sdc * cra + rvl * cdc * cra;
    vel[1] = pmr * cra - pmd * sdc * sra + rvl * cdc * sra;
    vel[2] = pmd * cdc + rvl * sdc;

    return;
}

void tdb2tdt(double tdb, double *tdtjd, double *secdiff) {
    double ecc = 0.01671022;
    double rev = 1296000.0;
    double tdays, m, l, lj, e;

    tdays = tdb - T0;
    m = (357.51716 + 0.985599987 * tdays) * 3600.0;
    l = (280.46435 + 0.985609100 * tdays) * 3600.0;
    lj = (34.40438 + 0.083086762 * tdays) * 3600.0;
    m = fmod(m, rev) / RAD2SEC;
    l = fmod(l, rev) / RAD2SEC;
    lj = fmod(lj, rev) / RAD2SEC;
    e = m + ecc * sin(m) + 0.5 * ecc * ecc * sin(2.0 * m);
    *secdiff = 1.658e-3 * sin(e) + 20.73e-6 * sin(l - lj);
    *tdtjd = tdb - *secdiff / 86400.0;

    return;
}

void precession(double tjd1, double *pos, double tjd2, double *pos2);

void transform_cat(cat_entry *incat, cat_entry *newcat) {
    short int j;

    double paralx, dist, r, d, cra, sra, cdc, sdc, pos1[3], term1, pmr, pmd, rvl, vel1[3], pos2[3], vel2[3], xyproj;

    paralx = incat->parallax;
    if (paralx <= 0.0)
        paralx = 1.0e-7;

    dist = RAD2SEC / paralx;
    r = incat->ra * 54000.0 / RAD2SEC;
    d = incat->dec * 3600.0 / RAD2SEC;
    cra = cos(r);
    sra = sin(r);
    cdc = cos(d);
    sdc = sin(d);
    pos1[0] = dist * cdc * cra;
    pos1[1] = dist * cdc * sra;
    pos1[2] = dist * sdc;

    term1 = paralx * 36525.0;
    pmr = incat->promora * 15.0 * cdc / term1;
    pmd = incat->promodec / term1;

    vel1[0] = -pmr * sra - pmd * sdc * cra;
    vel1[1] = pmr * cra - pmd * sdc * sra;
    vel1[2] = pmd * cdc;

    for (j = 0; j < 3; j++) {
        pos2[j] = pos1[j] + vel1[j] * (epoch_fk5 - epoch_hip);
        vel2[j] = vel1[j];
    }

    xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);
    r = atan2(pos2[1], pos2[0]);
    d = atan2(pos2[2], xyproj);
    newcat->ra = r * RAD2SEC / 54000.0;
    newcat->dec = d * RAD2SEC / 3600.0;
    if (newcat->ra < 0.0)
        newcat->ra += 24.0;

    dist = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1] +
                pos2[2] * pos2[2]);
    paralx = RAD2SEC / dist;
    newcat->parallax = paralx;

    cra = cos(r);
    sra = sin(r);
    cdc = cos(d);
    sdc = sin(d);
    pmr = -vel2[0] * sra + vel2[1] * cra;
    pmd = -vel2[0] * cra * sdc - vel2[1] * sra * sdc + vel2[2] * cdc;
    rvl = vel2[0] * cra * cdc + vel2[1] * sra * cdc + vel2[2] * sdc;

    newcat->promora = pmr * paralx * 36525.0 / (15.0 * cdc);
    newcat->promodec = pmd * paralx * 36525.0;
    newcat->radialvelocity = rvl * KMAU / 86400.0;

    if (newcat->parallax <= 1.01e-7) {
        newcat->parallax = 0.0;
        newcat->radialvelocity = incat->radialvelocity;
    }


    return;
}

void transform_hip(cat_entry *hipparcos, cat_entry *fk5) {
    cat_entry scratch;
    scratch.dec = hipparcos->dec;
    scratch.radialvelocity = hipparcos->radialvelocity;

    scratch.ra = hipparcos->ra / 15.0;
    scratch.promora = hipparcos->promora / (150.0 *
                                            cos(hipparcos->dec * DEG2RAD));
    scratch.promodec = hipparcos->promodec / 10.0;
    scratch.parallax = hipparcos->parallax / 1000.0;

    transform_cat(&scratch, fk5);

    return;
}

void sun_eph(double jd, double *ra, double *dec, double *dis) {
    short int i;

    double sum_lon = 0.0;
    double sum_r = 0.0;
    const double factor = 1.0e-07;
    double u, arg, lon, lat, t, t2, emean, sin_lon;

    struct sun_con {
        double l;
        double r;
        double alpha;
        double nu;
    };

    static const struct sun_con con[50] = {
        {403406.0, 0.0, 4.721964, 1.621043},
        {195207.0, -97597.0, 5.937458, 62830.348067},
        {119433.0, -59715.0, 1.115589, 62830.821524},
        {112392.0, -56188.0, 5.781616, 62829.634302},
        {3891.0, -1556.0, 5.5474, 125660.5691},
        {2819.0, -1126.0, 1.5120, 125660.9845},
        {1721.0, -861.0, 4.1897, 62832.4766},
        {0.0, 941.0, 1.163, 0.813},
        {660.0, -264.0, 5.415, 125659.310},
        {350.0, -163.0, 4.315, 57533.850},
        {334.0, 0.0, 4.553, -33.931},
        {314.0, 309.0, 5.198, 777137.715},
        {268.0, -158.0, 5.989, 78604.191},
        {242.0, 0.0, 2.911, 5.412},
        {234.0, -54.0, 1.423, 39302.098},
        {158.0, 0.0, 0.061, -34.861},
        {132.0, -93.0, 2.317, 115067.698},
        {129.0, -20.0, 3.193, 15774.337},
        {114.0, 0.0, 2.828, 5296.670},
        {99.0, -47.0, 0.52, 58849.27},
        {93.0, 0.0, 4.65, 5296.11},
        {86.0, 0.0, 4.35, -3980.70},
        {78.0, -33.0, 2.75, 52237.69},
        {72.0, -32.0, 4.50, 55076.47},
        {68.0, 0.0, 3.23, 261.08},
        {64.0, -10.0, 1.22, 15773.85},
        {46.0, -16.0, 0.14, 188491.03},
        {38.0, 0.0, 3.44, -7756.55},
        {37.0, 0.0, 4.37, 264.89},
        {32.0, -24.0, 1.14, 117906.27},
        {29.0, -13.0, 2.84, 55075.75},
        {28.0, 0.0, 5.96, -7961.39},
        {27.0, -9.0, 5.09, 188489.81},
        {27.0, 0.0, 1.72, 2132.19},
        {25.0, -17.0, 2.56, 109771.03},
        {24.0, -11.0, 1.92, 54868.56},
        {21.0, 0.0, 0.09, 25443.93},
        {21.0, 31.0, 5.98, -55731.43},
        {20.0, -10.0, 4.03, 60697.74},
        {18.0, 0.0, 4.27, 2132.79},
        {17.0, -12.0, 0.79, 109771.63},
        {14.0, 0.0, 4.24, -7752.82},
        {13.0, -5.0, 2.01, 188491.91},
        {13.0, 0.0, 2.65, 207.81},
        {13.0, 0.0, 4.98, 29424.63},
        {12.0, 0.0, 0.93, -7.99},
        {10.0, 0.0, 2.21, 46941.14},
        {10.0, 0.0, 3.59, -68.29},
        {10.0, 0.0, 1.50, 21463.25},
        {10.0, -9.0, 2.55, 157208.40}
    };

    u = (jd - T0) / 3652500.0;

    for (i = 0; i < 50; i++) {
        arg = con[i].alpha + con[i].nu * u;
        sum_lon += con[i].l * sin(arg);
        sum_r += con[i].r * cos(arg);
    }

    lon = 4.9353929 + 62833.1961680 * u + factor * sum_lon;

    lon = fmod(lon, TWOPI);
    if (lon < 0.0)
        lon += TWOPI;

    lat = 0.0;

    *dis = 1.0001026 + factor * sum_r;

    t = u * 100.0;
    t2 = t * t;
    emean = (0.001813 * t2 * t - 0.00059 * t2 - 46.8150 * t + 84381.448) / RAD2SEC;

    sin_lon = sin(lon);
    *ra = atan2((cos(emean) * sin_lon), cos(lon)) * RAD2DEG;
    *ra = fmod(*ra, 360.0);
    if (*ra < 0.0)
        *ra += 360.0;
    *ra = *ra / 15.0;

    *dec = asin(sin(emean) * sin_lon) * RAD2DEG;

    return;
}

void precession(double tjd1, double *pos, double tjd2, double *pos2) {
    double xx, yx, zx, xy, yy, zy, xz, yz, zz, t, t1, t02, t2, t3, zeta0, zee, theta;

    t = (tjd1 - T0) / 36525.0;
    t1 = (tjd2 - tjd1) / 36525.0;
    t02 = t * t;
    t2 = t1 * t1;
    t3 = t2 * t1;
    zeta0 = (2306.2181 + 1.39656 * t - 0.000139 * t02) * t1 + (0.30188 - 0.000344 * t) * t2 + 0.017998 * t3;

    zee = (2306.2181 + 1.39656 * t - 0.000139 * t02) * t1 + (1.09468 + 0.000066 * t) * t2 + 0.018203 * t3;

    theta = (2004.3109 - 0.85330 * t - 0.000217 * t02) * t1 + (-0.42665 - 0.000217 * t) * t2 - 0.041833 * t3;

    zeta0 /= RAD2SEC;
    zee /= RAD2SEC;
    theta /= RAD2SEC;
    xx = cos(zeta0) * cos(theta) * cos(zee) - sin(zeta0) * sin(zee);
    yx = -sin(zeta0) * cos(theta) * cos(zee) - cos(zeta0) *
         sin(zee);
    zx = -sin(theta) * cos(zee);
    xy = cos(zeta0) * cos(theta) * sin(zee) + sin(zeta0) * cos(zee);
    yy = -sin(zeta0) * cos(theta) * sin(zee) + cos(zeta0) *
         cos(zee);
    zy = -sin(theta) * sin(zee);
    xz = cos(zeta0) * sin(theta);
    yz = -sin(zeta0) * sin(theta);
    zz = cos(theta);
    pos2[0] = xx * pos[0] + yx * pos[1] + zx * pos[2];
    pos2[1] = xy * pos[0] + yy * pos[1] + zy * pos[2];
    pos2[2] = xz * pos[0] + yz * pos[1] + zz * pos[2];

    return;
}

void radec2vector(double ra, double dec, double dist, double *vector) {
    vector[0] = dist * cos(DEG2RAD * dec) * cos(DEG2RAD * 15.0 * ra);
    vector[1] = dist * cos(DEG2RAD * dec) * sin(DEG2RAD * 15.0 * ra);
    vector[2] = dist * sin(DEG2RAD * dec);
    return;
}
short int solarsystem(double tjd, short int origin, double *pos, double *vel) {
    short int ierr = 0;
    short int i;
    const double pm[4] = {1047.349, 3497.898, 22903.0, 19412.2};
    const double pa[4] = {5.203363, 9.537070, 19.191264, 30.068963};
    const double pl[4] = {0.600470, 0.871693, 5.466933, 5.321160};
    const double pn[4] = {1.450138e-3, 5.841727e-4, 2.047497e-4, 1.043891e-4};
    const double obl = 23.43929111;

    static double tlast = 0.0;
    static double sine, cose, tmass, pbary[3], vbary[3];

    double oblr, qjd, ras, decs, diss, pos1[3], p[3][3], dlon, sinl, cosl, x, y, z, xdot, ydot, zdot, f;

    if (tlast == 0.0) {
        oblr = obl * TWOPI / 360.0;
        sine = sin(oblr);
        cose = cos(oblr);
        tmass = 1.0;
        for (i = 0; i < 4; i++)
            tmass += 1.0 / pm[i];
        tlast = 1.0;
    }

    if ((tjd < 2340000.5) || (tjd > 2560000.5))
        return (ierr = 1);

    for (i = 0; i < 3; i++) {
        qjd = tjd + (double)(i - 1) * 0.1;
        sun_eph(qjd, &ras, &decs, &diss);
        radec2vector(ras, decs, diss, pos1);
        precession(qjd, pos1, T0, pos);
        p[i][0] = -pos[0];
        p[i][1] = -pos[1];
        p[i][2] = -pos[2];
    }
    for (i = 0; i < 3; i++) {
        pos[i] = p[1][i];
        vel[i] = (p[2][i] - p[0][i]) / 0.2;
    }

    if (origin == 0) {
        if (fabs(tjd - tlast) >= 1.0e-06) {
            for (i = 0; i < 3; i++)
                pbary[i] = vbary[i] = 0.0;

            for (i = 0; i < 4; i++) {
                dlon = pl[i] + pn[i] * (tjd - T0);
                dlon = fmod(dlon, TWOPI);
                sinl = sin(dlon);
                cosl = cos(dlon);

                x = pa[i] * cosl;
                y = pa[i] * sinl * cose;
                z = pa[i] * sinl * sine;
                xdot = -pa[i] * pn[i] * sinl;
                ydot = pa[i] * pn[i] * cosl * cose;
                zdot = pa[i] * pn[i] * cosl * sine;

                f = 1.0 / (pm[i] * tmass);

                pbary[0] += x * f;
                pbary[1] += y * f;
                pbary[2] += z * f;
                vbary[0] += xdot * f;
                vbary[1] += ydot * f;
                vbary[2] += zdot * f;
            }

            tlast = tjd;
        }

        for (i = 0; i < 3; i++) {
            pos[i] -= pbary[i];
            vel[i] -= vbary[i];
        }
    }

    return (ierr);
}

void proper_motion(double tjd1, double *pos, double *vel, double tjd2, double *pos2) {
    short int j;

    for (j = 0; j < 3; j++)
        pos2[j] = pos[j] + (vel[j] * (tjd2 - tjd1));

    return;
}

short int get_earth(double tjd, double *tdb, double *bary_earthp, double *bary_earthv, double *helio_earthp, double *helio_earthv) {
    short int error = 0;

    static double tjd_last = 0.0;
    static double time1, peb[3], veb[3], pes[3], ves[3];
    double dummy, secdiff;

    if (fabs(tjd - tjd_last) > 1.0e-6) {
        tdb2tdt(tjd, &dummy, &secdiff);
        time1 = tjd + secdiff / 86400.0;

        if (error = solarsystem(time1, BARYC, peb, veb)) {
            tjd_last = 0.0;
            return error;
        }

        if (error = solarsystem(time1, HELIOC, pes, ves)) {
            tjd_last = 0.0;
            return error;
        }
        tjd_last = tjd;
    }

    *tdb = time1;
    for (short int i = 0; i < 3; i++) {
        bary_earthp[i] = peb[i];
        bary_earthv[i] = veb[i];
        helio_earthp[i] = pes[i];
        helio_earthv[i] = ves[i];
    }

    return error;
}
void bary_to_geo(double *pos, double *earthvector, double *pos2, double *lighttime)

{
    short int j;

    double sum_of_squares;

    for (j = 0; j < 3; j++)
        pos2[j] = pos[j] - earthvector[j];

    sum_of_squares = pow(pos2[0], 2.0) + pow(pos2[1], 2.0) + pow(pos2[2], 2.0);

    *lighttime = sqrt(sum_of_squares) / C;

    return;
}

short int sun_field(double *pos, double *earthvector, double *pos2)

{
    short int j;

    double f = 0.0;
    double p1mag, pemag, cosd, sind, b, bm, pqmag, zfinl, zinit,
           xifinl, xiinit, delphi, delphp, delp, p1hat[3], pehat[3];

    double c = (C * MAU) / 86400.0;

    p1mag = sqrt(pow(pos[0], 2.0) + pow(pos[1], 2.0) + pow(pos[2], 2.0));
    pemag = sqrt(pow(earthvector[0], 2.0) + pow(earthvector[1], 2.0) + pow(earthvector[2], 2.0));

    for (j = 0; j < 3; j++) {
        p1hat[j] = pos[j] / p1mag;
        pehat[j] = earthvector[j] / pemag;
    }

    cosd = -pehat[0] * p1hat[0] - pehat[1] * p1hat[1] - pehat[2] * p1hat[2];

    if (fabs(cosd) > 0.9999999999) {
        for (j = 0; j < 3; j++)
            pos2[j] = pos[j];
    } else {
        sind = sqrt(1.0 - pow(cosd, 2.0));

        b = pemag * sind;
        bm = b * MAU;

        pqmag = sqrt(pow(p1mag, 2.0) + pow(pemag, 2.0) - 2.0 * p1mag * pemag * cosd);

        zfinl = pemag * cosd;
        zinit = -p1mag + zfinl;
        xifinl = zfinl / b;
        xiinit = zinit / b;

        delphi = 2.0 * GS / (bm * c * c) * (xifinl / sqrt(1.0 + pow(xifinl, 2.0)) - xiinit / sqrt(1.0 + pow(xiinit, 2.0)));

        delphp = delphi / (1.0 + (pemag / pqmag));

        f = delphp * p1mag / sind;

        for (j = 0; j < 3; j++) {
            delp = f * (cosd * p1hat[j] + pehat[j]);
            pos2[j] = pos[j] + delp;
        }
    }

    return 0;
}

short int aberration(double *pos, double *ve, double lighttime, double *pos2) {
    double p1mag, vemag, beta, dot, cosd, gammai, p, q, r;

    if (lighttime == 0.0) {
        p1mag = sqrt(pow(pos[0], 2.0) + pow(pos[1], 2.0) + pow(pos[2], 2.0));
        lighttime = p1mag / C;
    } else
        p1mag = lighttime * C;

    vemag = sqrt(pow(ve[0], 2.0) + pow(ve[1], 2.0) + pow(ve[2], 2.0));
    beta = vemag / C;
    dot = pos[0] * ve[0] + pos[1] * ve[1] + pos[2] * ve[2];

    cosd = dot / (p1mag * vemag);
    gammai = sqrt(1.0 - pow(beta, 2.0));
    p = beta * cosd;
    q = (1.0 + p / (1.0 + gammai)) * lighttime;
    r = 1.0 + p;

    for (short int j = 0; j < 3; j++)
        pos2[j] = (gammai * pos[j] + q * ve[j]) / r;

    return 0;
}

short int nutate(double tjd, short int fn, double *pos, double *pos2) {
    double cobm, sobm, cobt, sobt, cpsi, spsi, xx, yx, zx, xy, yy, zy, xz, yz, zz, oblm, oblt, eqeq, psi, eps;

    earthtilt(tjd, &oblm, &oblt, &eqeq, &psi, &eps);

    cobm = cos(oblm * DEG2RAD);
    sobm = sin(oblm * DEG2RAD);
    cobt = cos(oblt * DEG2RAD);
    sobt = sin(oblt * DEG2RAD);
    cpsi = cos(psi / RAD2SEC);
    spsi = sin(psi / RAD2SEC);

    xx = cpsi;
    yx = -spsi * cobm;
    zx = -spsi * sobm;
    xy = spsi * cobt;
    yy = cpsi * cobm * cobt + sobm * sobt;
    zy = cpsi * sobm * cobt - cobm * sobt;
    xz = spsi * sobt;
    yz = cpsi * cobm * sobt - sobm * cobt;
    zz = cpsi * sobm * sobt + cobm * cobt;

    if (!fn) {

        pos2[0] = xx * pos[0] + yx * pos[1] + zx * pos[2];
        pos2[1] = xy * pos[0] + yy * pos[1] + zy * pos[2];
        pos2[2] = xz * pos[0] + yz * pos[1] + zz * pos[2];
    } else {

        pos2[0] = xx * pos[0] + xy * pos[1] + xz * pos[2];
        pos2[1] = yx * pos[0] + yy * pos[1] + yz * pos[2];
        pos2[2] = zx * pos[0] + zy * pos[1] + zz * pos[2];
    }
    return 0;
}

short int app_star(double tjd, cat_entry *star, double *ra, double *dec) {
    short int error = 0;

    double tdb, time2, peb[3], veb[3], pes[3], ves[3], pos1[3], pos2[3],
           pos3[3], pos4[3], pos5[3], pos6[3], pos7[3], vel1[3];

    if (error = get_earth(tjd, &tdb, peb, veb, pes, ves)) {
        *ra = 0.0;
        *dec = 0.0;
        return error;
    }
    starvectors(star, pos1, vel1);
    proper_motion(T0, pos1, vel1, tdb, pos2);

    bary_to_geo(pos2, peb, pos3, &time2);
    sun_field(pos3, pes, pos4);
    aberration(pos4, veb, time2, pos5);
    precession(T0, pos5, tdb, pos6);
    nutate(tdb, FN0, pos6, pos7);

    vector2radec(pos7, ra, dec);

    return 0;
}

void spin(double st, double *pos1, double *pos2) {
    double str, cosst, sinst, xx, yx, xy, yy;

    str = st * 15.0 * DEG2RAD;
    cosst = cos(str);
    sinst = sin(str);

    xx = cosst;
    yx = -sinst;
    xy = sinst;
    yy = cosst;

    pos2[0] = xx * pos1[0] + yx * pos1[1];
    pos2[1] = xy * pos1[0] + yy * pos1[1];
    pos2[2] = pos1[2];

    return;
}
void pnsw(double gast, double *vece, double *vecs) {
    double v1[3], v2[3], v3[3];

    short int j;

    for (j = 0; j < 3; j++)
        v1[j] = vece[j];

    if (gast == 0.0) {
        for (j = 0; j < 3; j++)
            v2[j] = v1[j];
    } else
        spin(gast, v1, v2);

    for (j = 0; j < 3; j++)
        vecs[j] = v2[j];

    return;
}

void sidereal_time(double jd_high, double ee, double *gst) {
    double t_hi, t_lo, t, t2, t3, st;

    t_hi = (jd_high - T0) / 36525.0;
    t_lo = 0;
    t = t_hi;
    t2 = t * t;
    t3 = t2 * t;

    st = ee - 6.2e-6 * t3 + 0.093104 * t2 + 67310.54841 + 8640184.812866 * t_lo + 3155760000.0 * t_lo + 8640184.812866 * t_hi + 3155760000.0 * t_hi;

    *gst = fmod((st / 3600.0), 24.0);

    if (*gst < 0.0)
        *gst += 24.0;

    return;
}

void equ2hor(double tjd, site_info *location, double ra, double dec, double *zd, double *az) {
    short int j;

    double ujd, dummy, secdiff, tdb, mobl, tobl, ee, dpsi, deps, gast,
           sinlat, coslat, sinlon, coslon, sindc, cosdc, sinra, cosra,
           uze[3], une[3], uwe[3], uz[3], un[3], uw[3], p[3], pz, pn, pw,
           proj, zd0, zd1, refr, cosr, prlen, rlen, pr[3];

    ujd = tjd;

    tdb2tdt(tjd, &dummy, &secdiff);
    tdb = tjd + secdiff / 86400.0;

    earthtilt(tdb, &mobl, &tobl, &ee, &dpsi, &deps);

    sidereal_time(ujd, ee, &gast);

    sinlat = sin(location->latitude * DEG2RAD);
    coslat = cos(location->latitude * DEG2RAD);
    sinlon = sin(location->longitude * DEG2RAD);
    coslon = cos(location->longitude * DEG2RAD);
    sindc = sin(dec * DEG2RAD);
    cosdc = cos(dec * DEG2RAD);
    sinra = sin(ra * 15.0 * DEG2RAD);
    cosra = cos(ra * 15.0 * DEG2RAD);

    uze[0] = coslat * coslon;
    uze[1] = coslat * sinlon;
    uze[2] = sinlat;
    une[0] = -sinlat * coslon;
    une[1] = -sinlat * sinlon;
    une[2] = coslat;

    uwe[0] = sinlon;
    uwe[1] = -coslon;
    uwe[2] = 0.0;

    pnsw(gast, uze, uz);
    pnsw(gast, une, un);
    pnsw(gast, uwe, uw);

    p[0] = cosdc * cosra;
    p[1] = cosdc * sinra;
    p[2] = sindc;

    pz = p[0] * uz[0] + p[1] * uz[1] + p[2] * uz[2];
    pn = p[0] * un[0] + p[1] * un[1] + p[2] * un[2];
    pw = p[0] * uw[0] + p[1] * uw[1] + p[2] * uw[2];

    proj = sqrt(pn * pn + pw * pw);

    if (proj > 0.0)
        *az = -atan2(pw, pn) * RAD2DEG;

    if (*az < 0.0)
        *az += 360.0;

    if (*az >= 360.0)
        *az -= 360.0;

    *zd = atan2(proj, pz) * RAD2DEG;
    return;
}

double julian_date(short int year, short int month, short int day, double hour) {
    long int jd12h = (long)day - 32075L + 1461L * ((long)year + 4800L + ((long)month - 14L) / 12L) / 4L + 367L * ((long)month - 2L - ((long)month - 14L) / 12L * 12L) / 12L - 3L * (((long)year + 4900L + ((long)month - 14L) / 12L) / 100L) / 4L;
    double tjd = (double)jd12h - 0.5 + hour / 24.0;

    return (tjd);
}

// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_hypergeom_gain.c
 \brief \~English function for Estimate Optimam Gain of speech present 
 \brief \~Japanese SN”äEƒQƒCƒ“„’è‚Ì“à•”‚©‚çŒÄ‚Î‚ê‚éŠÖ”
 \author Masatoshi Tsutsumi
 \date 23 Nov 2006
 \version $Id: PF_hypergeom_gain.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <math.h>

#include "PF_hypergeom_gain.h"
// ---------------------------------------------------------------------
/*
function y = hypergeom_gain(x)
% y = hypergeom_gain(x)
% NOTE: x must be greater than 0

table = [
 0.82157, 1.02017, 1.20461, 1.37534, 1.53363, 1.68092, 1.81865, ...
 1.94811, 2.07038, 2.18638, 2.29688, 2.40255, 2.50391, 2.60144, ...
 2.69551, 2.78647, 2.87458, 2.96015, 3.04333, 3.12431, 3.20326]';

y = zeros(size(x));
ind = floor(2*x);
frac = 2*x - ind ;

gt95i = find(x > 9.5);
le95i = find(x <= 9.5);
y(gt95i) = 1 + 0.1296 ./ x(gt95i);
y(le95i) = ((1 - frac(le95i)) .* table(1+ind(le95i)) + frac(le95i) .* table(2+ind(le95i))) ./ sqrt(x(le95i)+0.0001);
*/
// 

int PF_HyperGeomGain(HA_Float x, HA_Float *y)
{
	int	ind;
	double	table[] = {0.82157, 1.02017, 1.20461, 1.37534, 1.53363, 1.68092, 1.81865,
							1.94811, 2.07038, 2.18638, 2.29688, 2.40255, 2.50391, 2.60144,
							2.69551, 2.78647, 2.87458, 2.96015, 3.04333, 3.12431, 3.20326};
	double	frac;
	double	dx;

	dx = (double)x;
	*y = 0.0;

	// ˆø”x‚Ì’l‚Ì”ÍˆÍ‚ğŒŸ¸
	if (dx <= 0.0) {
		return 0;
	}

	if (dx > 9.5) {
		*y = (HA_Float)(1.0 + 0.1296 / dx);
	}
	else {
		ind = (int)(x * 2.0);
		frac = dx * 2.0 - (double)ind;
		*y = (HA_Float)(((1.0 - frac) * table[ind] + frac * table[ind + 1]) / sqrt(dx + 0.0001));
	}

	return 1;
}


extern	double _log();

double LN(statlink, x)
  int *statlink; double x;
  {return(_log(x));}
